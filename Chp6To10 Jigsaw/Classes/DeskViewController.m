    //
//  DeskViewController.m
//  Jigsaw
//
//  Created by Chen Li on 10/11/10.
//  Copyright 2010 Chen Li. All rights reserved.
//

#import "DeskViewController.h"
#import "Puzzle.h"
#import "Piece.h"
#import "MyScrollView.h"
#import "InfoViewController.h"

static NSString* kAppId = @"111832852208394";

@implementation DeskViewController

@synthesize curPuzzle, state;
@synthesize secTimer;
@synthesize previewImgView, deskImgView;
@synthesize scrollView, timerLabel;

/*
 // The designated initializer.  Override if you create the controller programmatically and want to perform customization that is not appropriate for viewDidLoad.
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
    if ((self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil])) {
        // Custom initialization
    }
    return self;
}
*/

// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
- (void)viewDidLoad {
    [super viewDidLoad];

	[[NSNotificationCenter defaultCenter] addObserver:self
											 selector:@selector(applicationWillTerminate) name:UIApplicationWillTerminateNotification object:nil];
	// init facebook
	facebook = [[Facebook alloc] init];
	permissions =  [[NSArray arrayWithObjects: 
					 @"read_stream", @"offline_access",nil] retain];
}

-(void) oneSecPassed {
	//NSLog(@"oneSecPassed");
	timePlayed += 1;
	[self displayTime];
}

-(void) displayTime {
	// h, m, s
	int hour = timePlayed/3600;
	int remains = timePlayed - 3600*hour;
	int minute = remains/60;
	int second = remains - 60*minute;
	
	// strings
	NSString* sH;
	NSString* sM;
	NSString* sS;
	if (hour>=10) {
		sH = [NSString stringWithFormat:@"%d",hour];
	}
	else {
		sH = [NSString stringWithFormat:@"0%d",hour];
	}
	if (minute>=10) {
		sM = [NSString stringWithFormat:@"%d",minute];
	}
	else {
		sM = [NSString stringWithFormat:@"0%d",minute];
	}
	if (second>=10) {
		sS = [NSString stringWithFormat:@"%d",second];
	}
	else {
		sS = [NSString stringWithFormat:@"0%d",second];
	}
	NSString* stringTime = [NSString stringWithFormat:@"%@:%@:%@",sH,sM,sS];
	
	// update time display
	timerLabel.text = stringTime;
}

// Use NSLog to help debugging. Remember to remove the NSLog's in release version; they'll affect the performance of the app. 
-(void) setState:(DeskViewState)theState {
	switch (theState) {
		case stateNone:
			NSLog(@"stateNone");
			break;
		case stateSelectedDeskPiece:
			NSLog(@"stateSelectedDeskPiece");
			break;
		case stateSelectedPoolPiece:
			NSLog(@"stateSelectedPoolPiece");
			break;
		case statePoolToDesk:
			NSLog(@"statePoolToDesk");
			break;
		case stateSlidePool:
			NSLog(@"stateSlidePool");
			break;
		default:
			break;
	}
	state = theState;
}

-(void) loadPuzzle:(Puzzle*)aPuzzle {
	NSLog(@"loadPuzzle");
	// remove existing pieces
	for (Piece* aPiece in curPuzzle.poolPieces) {
		[aPiece removeFromSuperview];
	}
	for (Piece* aPiece in curPuzzle.tablePieces) {
		[aPiece removeFromSuperview];
	}
	// set selectedPiece to nil
	selectedPiece = nil;
	// set the state
	state = stateNone;
	// set the deskRect and poolRect
	deskRect = CGRectMake(0, 0, 896, 768);
	poolRect = CGRectMake(1024-128, 128+40, 128, 768-128-40-35);
	
	// add the gestures
	[self createGestureRecognizers];
	
	// load the new puzzle
	self.curPuzzle = aPuzzle;
	// to pool size
	for (Piece* aPiece in curPuzzle.poolPieces) {
		[aPiece toPoolSize];
	}
	NSLog(@"%d pieces", [curPuzzle.poolPieces count]);
	// preview
	previewImgView.image = curPuzzle.image;
	// resize the deskImageView
	deskImgView.frame = CGRectMake(0, 0, aPuzzle.image.size.width, aPuzzle.image.size.height);
	deskImgView.center = CGPointMake((1024-128)/2, 768/2);
	
	// the size of a piece (approximately 1.64*fw)
	d = 53;
	// the gap between two pieces in the pool
	g = 5; 
	// sum of d and g
	H = d+g;
	// offsets
	xOffset = (1024-128-curPuzzle.image.size.width)/2;
	yOffset = (768-curPuzzle.image.size.height)/2;
	
	// scrollView
	if (scrollView==nil) {
		self.scrollView = [[MyScrollView alloc] initWithFrame:CGRectMake(1024-128, 128+40, 128, 600-35)];
		scrollViewOrigin = scrollView.frame.origin;
		[self.view addSubview:scrollView];
		scrollView.delegate = self;
		scrollView.scrollEnabled = YES;
		scrollView.delaysContentTouches = NO;
		scrollView.clipsToBounds = YES;
		scrollView.canCancelContentTouches = YES;
		scrollView.showsVerticalScrollIndicator = YES;
	}
	[self displayTablePieces];
	[self updatePoolDisplay];
	[scrollView flashScrollIndicators];
	
	// get timePlayed
	timePlayed = curPuzzle.timePlayed;
	[self displayTime];
	
	// init timer
	self.secTimer = [NSTimer scheduledTimerWithTimeInterval:1 target:self selector:@selector(oneSecPassed) userInfo:nil repeats:YES];
}

-(void) displayTablePieces {
	NSLog(@"displaying %d table pieces", [curPuzzle.tablePieces count]);
	for (Piece* aPiece in curPuzzle.tablePieces) {
		aPiece.center = CGPointMake((0.5+aPiece.curGrid.x)*aPiece.pieceSize+xOffset, (0.5+aPiece.curGrid.y)*aPiece.pieceSize+yOffset);
		// place the piece
		[aPiece toPieceSize];		// resume pieceSize
		[self.view addSubview:aPiece];		// add it to the table
	}
}

-(void) updatePoolDisplay {
	// make sure the scrollView is scrollable, so that the touches can be passed to other views
	if ([curPuzzle.poolPieces count]*H+g > 600) {
		scrollView.contentSize = CGSizeMake(128, [curPuzzle.poolPieces count]*H+g);	
	}
	else {
		scrollView.contentSize = CGSizeMake(128, 601);
	}

	
	for (int i=0; i<[curPuzzle.poolPieces count]; i++) {
		Piece* curPiece = [curPuzzle.poolPieces objectAtIndex:i];
		[curPiece removeFromSuperview];
		[curPiece toPoolSize];
		curPiece.center = CGPointMake(64, i*H+g+d/2);
		//NSLog(@"%d to scrollView, at center: %@", i, NSStringFromCGPoint(curPiece.center));
		[scrollView addSubview:curPiece];
	}
}

// this happens before any gesture is recognized
-(void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {
	NSLog(@"touchesBegan");
	UITouch* touch = [touches anyObject];
	CGPoint location = [touch locationInView:self.view];
	beginLocation = location;		// record the touch location
	UIView* topView = [self.view hitTest:location withEvent:event];
	NSLog(@"%@",topView);
	// if selected a piece
	if ([topView isKindOfClass:[Piece class]]) {
		selectedPiece = (Piece*)topView;
		pieceOriLocation = selectedPiece.center;
		// show liftSize
		[selectedPiece toLiftSize];
		// if a pool piece
		if ([curPuzzle.poolPieces containsObject:selectedPiece]) {
			self.state = stateSelectedPoolPiece;
		}
		// else if a desk piece
		else {
			self.state = stateSelectedDeskPiece;
		}
	}
	// if didn't select a piece
	else {
		// if touched on the pool area
		if (CGRectContainsPoint(poolRect, location)) {
			self.state = stateSlidePool;
		}
	}

}

-(void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {
	NSLog(@"touchesEnded");
	if (selectedPiece!=nil) {
		// if it's a pool piece, resume pool size
		if ([curPuzzle.poolPieces containsObject:selectedPiece]) {
			[selectedPiece toPoolSize];
		}
		else {
			[selectedPiece toPieceSize];
		}

	}
	selectedPiece = nil;
}

// UIScrollView delegate
-(void)scrollViewWillBeginDragging:(UIScrollView *)scrollView {
	NSLog(@"scrollViewWillBeginDragging");
	if (selectedPiece!=nil) {
		[selectedPiece toPoolSize];
	}
	selectedPiece = nil;
}

- (void)createGestureRecognizers {
	// pan: move the pool; move a piece
    UIPanGestureRecognizer *panGesture = [[UIPanGestureRecognizer alloc]
										  initWithTarget:self action:@selector(handlePanGesture:)];
    [self.view addGestureRecognizer:panGesture];
    [panGesture release];
}

- (IBAction)handlePanGesture:(UIPanGestureRecognizer *)sender {
	NSLog(@"handlePanGesture: %d", sender.state);
	CGPoint pointsMoved = [sender translationInView:self.view];
	CGPoint pieceCurLocation = CGPointAdd(pieceOriLocation, pointsMoved);
	NSLog(@"pointsMoved: %@",NSStringFromCGPoint(pointsMoved));
	NSLog(@"location: %@",NSStringFromCGPoint(pieceCurLocation));
	
	if (sender.state==UIGestureRecognizerStateChanged || sender.state==UIGestureRecognizerStateBegan) {
		if (state==stateSelectedDeskPiece) {
			selectedPiece.center = CGPointAdd(pieceOriLocation, pointsMoved);
		}
		else if (state==stateSelectedPoolPiece) {
			//selectedPiece.center = CGPointAdd(pieceOriLocation, pointsMoved);
			if (fabs(pointsMoved.x)>10) {
				self.state = statePoolToDesk;
				NSLog(@"offset: %@", NSStringFromCGPoint(scrollView.contentOffset));
				selectedPiece.center = CGPointAdd(selectedPiece.center, CGPointSub(scrollViewOrigin, scrollView.contentOffset));
				pieceOriLocation = CGPointAdd(pieceOriLocation, CGPointSub(scrollViewOrigin, scrollView.contentOffset));
				[self.view addSubview:selectedPiece];
			}
			else if (fabs(pointsMoved.y)>10) {
				self.state = stateSlidePool;
				[selectedPiece toPoolSize];
			}
		}
		else if (state==statePoolToDesk) {
			selectedPiece.center = CGPointAdd(pieceOriLocation, pointsMoved);
		}
		else if (state==stateSlidePool) {
			[self updatePoolDisplay];
		}
	}
	else if (sender.state==UIGestureRecognizerStateEnded) {
		// find the closest grid first
		int xGrid, yGrid;				// the grid where the piece should stay
		int W = curPuzzle.pieceSize;		// square size of the piece
		NSLog(@"%d",W);
		int lowRow = floor((pieceCurLocation.y-yOffset)/W);
		int lowCol = floor((pieceCurLocation.x-xOffset)/W);
		NSLog(@"xOffset: %d, yOffset: %d", xOffset, yOffset);
		NSLog(@"lowRow: %d, lowCol: %d",lowRow,lowCol);
		// find yGrid
		if (lowRow>=(curPuzzle.numRow-1)) {
			yGrid = curPuzzle.numRow-1;
		}
		else if (lowRow<0) {
			yGrid = 0;
		}
		else if ((pieceCurLocation.y-lowRow*W-0.5*W-yOffset)<W/2) {
			yGrid = lowRow;
		}
		else {
			yGrid = lowRow + 1;
		}
		// find xGrid
		if (lowCol>=(curPuzzle.numCol-1)) {
			xGrid = curPuzzle.numCol-1;
		}
		else if (lowCol<0) {
			xGrid = 0;
		}
		else if ((pieceCurLocation.x-lowCol*W-0.5*W-xOffset)<W/2) {
			xGrid = lowCol;
		}
		else {
			xGrid = lowCol + 1;
		}

		if (state==statePoolToDesk) {
			NSLog(@"ended at statePoolToDesk");
			// when the piece has been moved from the pool to the table, record the change
			if (CGRectContainsPoint(deskRect, selectedPiece.center)) {
				[curPuzzle.tablePieces addObject:selectedPiece];
				[curPuzzle.poolPieces removeObject:selectedPiece];
				[selectedPiece toPieceSize];		// resume pieceSize
				selectedPiece.curGrid = GridMake(xGrid, yGrid);
				selectedPiece.center = CGPointMake((0.5+selectedPiece.curGrid.x)*selectedPiece.pieceSize+xOffset, (0.5+selectedPiece.curGrid.y)*selectedPiece.pieceSize+yOffset);
				selectedPiece.isPoolPiece = NO;
				
				// check for winning
				if ([curPuzzle isFinished]) {
					NSLog(@"You won the game!");
					[self wonTheGame];
				}
			}
			else {
				selectedPiece.center = CGPointSub(selectedPiece.center, CGPointSub(scrollViewOrigin, scrollView.contentOffset));
				pieceOriLocation = CGPointSub(pieceOriLocation, CGPointSub(scrollViewOrigin, scrollView.contentOffset));
				[scrollView addSubview:selectedPiece];
				
				[selectedPiece toPoolSize];
			}

			[self updatePoolDisplay];
		}
		else if (state==stateSelectedPoolPiece) {
			[selectedPiece toPoolSize];
		}
		else if (state==stateSelectedDeskPiece) {
			// if still in the table
			if (CGRectContainsPoint(deskRect, selectedPiece.center)) {
				selectedPiece.curGrid = GridMake(xGrid, yGrid);
				selectedPiece.center = CGPointMake((0.5+selectedPiece.curGrid.x)*selectedPiece.pieceSize+xOffset, (0.5+selectedPiece.curGrid.y)*selectedPiece.pieceSize+yOffset);
				[selectedPiece toPieceSize];		// resume pieceSize
				
				// check for winning
				if ([curPuzzle isFinished]) {
					NSLog(@"You won the game!");
					[self wonTheGame];
				}
			}
			// else, put it back into the pool
			else {
				[curPuzzle.poolPieces addObject:selectedPiece];
				[curPuzzle.tablePieces removeObject:selectedPiece];
				[selectedPiece toPoolSize];
				[selectedPiece removeFromSuperview];
				selectedPiece.isPoolPiece = YES;
				[self updatePoolDisplay];
			}
		}
		else if (state==stateSlidePool) {
		}
		self.state = stateNone;
		selectedPiece = nil;
	}
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    // Overriden to allow any orientation.
    return (interfaceOrientation==UIInterfaceOrientationLandscapeLeft || interfaceOrientation==UIInterfaceOrientationLandscapeRight);
}


- (void)didReceiveMemoryWarning {
    // Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
    
    // Release any cached data, images, etc that aren't in use.
}


- (void)viewDidUnload {
    [super viewDidUnload];
}

// save puzzle here
-(void) applicationWillTerminate {
	NSLog(@"applicationWillTerminate");
	[secTimer invalidate];
	curPuzzle.timePlayed = timePlayed;
	[self savePuzzle:curPuzzle];
}

// actually, save the dict
-(BOOL) savePuzzle:(Puzzle*)aPuzzle {
	
	NSString* docPath = [NSHomeDirectory() stringByAppendingFormat:@"/Documents"];
	NSString* fileName = [aPuzzle valueForKey:@"fileName"];
	NSString* filePath = [docPath stringByAppendingFormat:@"/%@", fileName];
	BOOL isSuccess = [[aPuzzle dict] writeToFile:filePath atomically:YES];
	
	if (isSuccess) {
		NSLog(@"successfully saved puzzle to %@", filePath);
	}
	else {
		NSLog(@"failed to save puzzle to %@", filePath);
	}
	
	return isSuccess;
}

-(void) wonTheGame {
	NSLog(@"You won the game!");
	UIAlertView* alertView = [[UIAlertView alloc] initWithTitle:nil message:@"Congratulations! You finished this puzzle!" \
													   delegate:self cancelButtonTitle:@"Continue" \
											  otherButtonTitles:@"New puzzle",nil];
	[alertView show];
	[alertView release];
	
}

- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex {
	switch (buttonIndex) {
		case 0:
			NSLog(@"Continue with this puzzle");
			break;
		case 1:
			NSLog(@"Go to main menu");
			curPuzzle.timePlayed = timePlayed;
			[self savePuzzle:curPuzzle];
			[self.view removeFromSuperview];
			break;
		default:
			break;
	}
}

-(IBAction) didClickMenu {
	NSLog(@"didClickMenu");
	UIActionSheet* actionSheet = [[UIActionSheet alloc] initWithTitle:nil delegate:self cancelButtonTitle:@"Cancel" destructiveButtonTitle:nil\
													otherButtonTitles:@"Email", @"Facebook", @"Music", @"Save", @"Info", @"Main Menu", nil];
	[actionSheet showFromRect:CGRectMake(1024-128, 128+20, 1, 1) inView:self.view animated:YES];
	[actionSheet release];
}

- (void)actionSheet:(UIActionSheet *)actionSheet clickedButtonAtIndex:(NSInteger)buttonIndex {
	switch (buttonIndex) {
		case 0:
			NSLog(@"Email");
			[self actionEmail];
			break;
		case 1:
			NSLog(@"Facebook");
			[self actionFacebook];
			break;
		case 2:
			NSLog(@"Music");
			[self actionMusic];
			break;
		case 3:
			NSLog(@"Save");
			[self actionSave];
			break;
		case 4:
			NSLog(@"Info");
			[self actionInfo];
			break;
		case 5:
			NSLog(@"Main Menu");
			[self actionMainMenu];
			break;
		default:
			NSLog(@"un-handled action button");
			break;
	}
}

-(void) actionEmail {
	if ([MFMailComposeViewController canSendMail]) {
		MFMailComposeViewController *picker = [[MFMailComposeViewController alloc] init];
		picker.mailComposeDelegate = self;
		
		// Attach an image to the email
		NSData* imageData = UIImagePNGRepresentation([self screenShot]);
		[picker addAttachmentData:imageData mimeType:@"image/png" fileName:@"JigsawPuzzle.png"];
		
		// Fill out the email body text
		NSString* emailBody = @"Hey, <br /> <br /> \
		check out the \"Photo Jigsaw\" on the iPad!";
		[picker setMessageBody:emailBody isHTML:YES];
		
		[self presentModalViewController:picker animated:YES];
		[picker release];
	}
}

- (void)mailComposeController:(MFMailComposeViewController*)controller didFinishWithResult:(MFMailComposeResult)result error:(NSError*)error {
	[self dismissModalViewControllerAnimated:YES];
}

-(void) actionFacebook {
	[facebook authorize:kAppId permissions:permissions delegate:self];
}

-(void) fbDidLogin {
	NSLog(@"fbDidLogin");
	[self uploadToFacebook];
}

//-(void) fbDidLogout {
//	NSLog(@"fbDidLogout");
//}

-(void) uploadToFacebook {
	NSMutableDictionary * params = [NSMutableDictionary dictionaryWithObjectsAndKeys:
									[self screenShot], @"Photo Jigsaw for iPad",
									nil];
	[facebook requestWithMethodName: @"photos.upload" 
						  andParams: params
					  andHttpMethod: @"POST" 
						andDelegate: self]; 
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// FBRequestDelegate

/**
 * Callback when a request receives Response
 */ 
- (void)request:(FBRequest*)request didReceiveResponse:(NSURLResponse*)response{
	NSLog(@"received response");
//	[activityIndicator stopAnimating];
//	[activityIndicator removeFromSuperview];
};

/**
 * Called when an error prevents the request from completing successfully.
 */
- (void)request:(FBRequest*)request didFailWithError:(NSError*)error{
	NSLog([NSString stringWithFormat:@"%@",[error localizedDescription]]);
};

/**
 * Called when a request returns and its response has been parsed into an object.
 * The resulting object may be a dictionary, an array, a string, or a number, depending
 * on thee format of the API response.
 */
- (void)request:(FBRequest*)request didLoad:(id)result {
	if ([result isKindOfClass:[NSArray class]]) {
		result = [result objectAtIndex:0]; 
	}
	if ([result objectForKey:@"owner"]) {
		NSLog(@"Photo upload Success");
		UIAlertView* alertSheet = [[UIAlertView alloc] initWithTitle:nil message:@"Image Uploaded to Facebook!"\
															delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
		[alertSheet show];
		[alertSheet release];
	} else {
		NSLog([NSString stringWithFormat:@"%@",[result objectForKey:@"name"]]);
	}
};

-(void) actionMusic {
	MPMediaPickerController *picker = [[MPMediaPickerController alloc] initWithMediaTypes: MPMediaTypeMusic];
	picker.delegate = self;                                     
	picker.allowsPickingMultipleItems = YES;					
//	picker.prompt =
//	NSLocalizedString (@"Add songs as background musics",
//					   @"Prompt in media item picker");
	//picker.prompt = @"Add songs as background musics";
	[[UIApplication sharedApplication] setStatusBarStyle: UIStatusBarStyleDefault animated: YES];
	
	//[self presentModalViewController: picker animated: YES];    // 4
	if (!musicPickerPopoverController) {
		musicPickerPopoverController = [[UIPopoverController alloc] initWithContentViewController:picker];
	}
	//musicPickerPopoverController.contentViewController = picker;
	[musicPickerPopoverController presentPopoverFromRect:CGRectMake(400, 300, 1, 1) inView:self.view\
								permittedArrowDirections:UIPopoverArrowDirectionAny animated:YES];
	[picker release];
}

// after chosen a set of songsx
- (void) mediaPicker: (MPMediaPickerController *) mediaPicker
   didPickMediaItems: (MPMediaItemCollection *) collection {
	NSLog(@"didPickMediaItems");
	MPMusicPlayerController* musicPlayer = [MPMusicPlayerController applicationMusicPlayer];
	[musicPlayer setQueueWithItemCollection:collection];
	[musicPlayer play];
	[musicPickerPopoverController dismissPopoverAnimated:YES];
}

-(void) actionSave {
	curPuzzle.timePlayed = timePlayed;
	[self savePuzzle:curPuzzle];
}

-(void) actionInfo {
	
	if (infoVC==nil) {
		infoVC = [[InfoViewController alloc] init];
	}
	if (infoPC==nil) {
		infoPC = [[UIPopoverController alloc] initWithContentViewController:infoVC];
	}
	
	infoPC.popoverContentSize = infoVC.view.frame.size;
	[infoPC presentPopoverFromRect:CGRectMake(300, 200, 20, 20)\
							inView:self.view permittedArrowDirections:UIPopoverArrowDirectionAny animated:YES];
}

-(void) actionMainMenu {
	[secTimer invalidate];
	curPuzzle.timePlayed = timePlayed;
	[self savePuzzle:curPuzzle];
	[self.view removeFromSuperview];
}

-(UIImage*) screenShot {
	// get fullScreenShot first
	UIImage* fullScreenShot;
	UIGraphicsBeginImageContext(self.view.bounds.size);
	[self.view.layer renderInContext:UIGraphicsGetCurrentContext()];
	fullScreenShot = UIGraphicsGetImageFromCurrentImageContext();
	UIGraphicsEndImageContext();
	
	// get the puzzle area's screen shot
	CGImageRef screenShotImageRef = \
	CGImageCreateWithImageInRect([fullScreenShot CGImage], CGRectMake(xOffset, yOffset, curPuzzle.image.size.width, curPuzzle.image.size.height));
	UIImage* screenShot = [UIImage imageWithCGImage:screenShotImageRef];
	CGImageRelease(screenShotImageRef);
	
	return screenShot;
}

- (void)dealloc {
	[curPuzzle release];
	//[slideTimer release];
	[previewImgView release];
	[deskImgView release];
	[[NSNotificationCenter defaultCenter] removeObserver:self];
	[scrollView release];
	[facebook release];
	[permissions release];
	[musicPickerPopoverController release];
	[secTimer invalidate];
	[secTimer release];
	[timerLabel release];
	[infoPC release];
	[infoVC release];
	
    [super dealloc];
}


@end
