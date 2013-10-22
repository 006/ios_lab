//
//  JigsawViewController.m
//  Jigsaw
//
//  Created by Chen Li on 9/23/10.
//  Copyright 2010 Chen Li. All rights reserved.
//

#import "JigsawViewController.h"
#import "Piece.h"
#import "Definitions.h"
#import "DeskViewController.h"
#import "LoadViewController.h"
#import "InfoViewController.h"
#import "IndicatorOperation.h"

@implementation JigsawViewController

@synthesize curPuzzle,queue;

/*
// The designated initializer. Override to perform setup that is required before the view is loaded.
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
    if ((self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil])) {
        // Custom initialization
    }
    return self;
}
*/

/*
// Implement loadView to create a view hierarchy programmatically, without using a nib.
- (void)loadView {
}
*/


// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
- (void)viewDidLoad {
    [super viewDidLoad];
	
	self.curPuzzle = nil;
	// init music
	NSString *deviceType = [UIDevice currentDevice].model;
	if ( [deviceType rangeOfString:@"Simulator"].length == 0) {
		MPMusicPlayerController* musicPlayer = [MPMusicPlayerController applicationMusicPlayer];
		[musicPlayer setShuffleMode: MPMusicShuffleModeSongs];
		[musicPlayer setRepeatMode: MPMusicRepeatModeAll];
		// choose the first song
		[musicPlayer setQueueWithQuery: [MPMediaQuery songsQuery]];
		// play it
		[musicPlayer play];
	}
	// init indicator
	activityIndicator = [[UIActivityIndicatorView alloc] initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleWhiteLarge];
	activityIndicator.center = CGPointMake(512, 384);
	// init queue
	queue = [[NSOperationQueue alloc] init];
}

-(IBAction) didClickNewPuzzle {
	NSLog(@"didClickNewPuzzle");
	UIActionSheet* actionSheet = [[UIActionSheet alloc] initWithTitle:@"Select difficulty level" delegate:self cancelButtonTitle:@"Cancel" destructiveButtonTitle:nil\
													otherButtonTitles:@"Easy", @"Normal", @"Hard", nil];
	[actionSheet showFromRect:CGRectMake(350, 200, 0, 0) inView:self.view animated:YES];
	[actionSheet release];
}

-(void) showImagePicker {
	NSLog(@"didClickMenu");
	
	if ([UIImagePickerController isSourceTypeAvailable:UIImagePickerControllerSourceTypePhotoLibrary]) {
		// if the imagePC is not initialized, initialize it
		if (imagePC==nil) {
			// initialize image picker
			UIImagePickerController* imagePicker = [[UIImagePickerController alloc] init];
			imagePicker.delegate = self;
			imagePicker.sourceType = UIImagePickerControllerSourceTypePhotoLibrary;
			// set the imagePicker as the content of the popover controller
			imagePC = [[UIPopoverController alloc] initWithContentViewController:imagePicker];
			// release the imagePicker
			[imagePicker release];
		}
		// present the popover
		[imagePC presentPopoverFromRect:CGRectMake(350, 200, 0, 0) inView:self.view\
			   permittedArrowDirections:UIPopoverArrowDirectionLeft animated:YES];
	}
}

- (void)actionSheet:(UIActionSheet *)actionSheet clickedButtonAtIndex:(NSInteger)buttonIndex {
	switch (buttonIndex) {
		case 0:
			NSLog(@"Easy");
			pieceSize = 128;
			[self showImagePicker];
			break;
		case 1:
			NSLog(@"Normal");
			pieceSize = 64;
			[self showImagePicker];
			break;
		case 2:
			NSLog(@"Hard");
			pieceSize = 32;
			[self showImagePicker];
			break;
		default:
			NSLog(@"un-handled action button");
			break;
	}
}

- (void)imagePickerController:(UIImagePickerController *)picker didFinishPickingMediaWithInfo:(NSDictionary *)info {
	NSLog(@"didFinishPickingMediaWithInfo");
	// the image
	UIImage* oriImage = [info objectForKey:@"UIImagePickerControllerOriginalImage"];
	
	// dismiss the image if it's too small
	if (oriImage.size.width<200 || oriImage.size.height<200) {
		// alert view
		UIAlertView* alertSheet = [[UIAlertView alloc] initWithTitle:nil message:@"Image too small!" delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
		[alertSheet show];
		[alertSheet release];
	}
	// if the image size is ok, generate a puzzle for it and dismiss the popover
	else {
		// original size
		float oriWidth = oriImage.size.width;
		float oriHeight = oriImage.size.height;
		// figure out the correct size of the image (limited to 896*768)
		float targetWidth;
		float targetHeight;
		if (oriWidth/oriHeight>7.0/6.0) {
			targetWidth = 896;
			targetHeight = oriHeight/oriWidth*896;
		}
		else {
			targetHeight = 768;
			targetWidth = oriWidth/oriHeight*768;
		}
		// resize: round to interger multiple of 128
		// so that we can design piece rect to be 128*128 for easy, 64*64 for midium, and 32*32 for hard
		UIGraphicsBeginImageContext(CGSizeMake(128*floor(targetWidth/128),128*floor(targetHeight/128)));
		NSLog(@"Modified image size: %.0f*%.0f",128*floor(targetWidth/128),128*floor(targetHeight/128));
		[oriImage drawInRect:CGRectMake(0, 0, targetWidth, targetHeight)];
		oriImage = UIGraphicsGetImageFromCurrentImageContext();		
		UIGraphicsEndImageContext();
		
		// get documents directory
		NSFileManager* fileManager = [NSFileManager defaultManager];
		NSString* homePath = NSHomeDirectory();
		NSString* docPath = [homePath stringByAppendingFormat:@"/Documents"];
		// check the available file name
		int i=0;
		NSString* imagePath = [docPath stringByAppendingFormat:@"/%d.png",i];
		while ([fileManager fileExistsAtPath:imagePath]) {
			i++;
			imagePath = [docPath stringByAppendingFormat:@"/%d.png",i];
		}
		// save the image
		NSData* imageData = UIImagePNGRepresentation(oriImage);
		[imageData writeToFile:imagePath atomically:YES];
		
		// generate a dictionary for the puzzle
		NSMutableDictionary* dictPuzzle = [Puzzle genDictFromImage:imagePath pieceSize:pieceSize];
		NSLog(@"generated dict for puzzle");
		// path for the dict
		NSString* dictPath = [docPath stringByAppendingFormat:@"/%@",[dictPuzzle valueForKey:@"fileName"]];
		// save the dict
		[dictPuzzle writeToFile:dictPath atomically:YES];
		
		// generate a puzzle from the dictionary
		IndicatorOperation* operation = [[IndicatorOperation alloc] init];
		operation.owner = self;
		[queue addOperation:operation];
		self.curPuzzle = [[[Puzzle alloc] initWithDict:dictPuzzle] autorelease];
		[operation release];
		[self stopIndicator];
		// start the game
		[self enterPuzzle:self.curPuzzle];
	}
}

-(IBAction) didClickLoadPuzzle {
	NSLog(@"didClickLoadPuzzle");
	
	if (loadVC==nil) {
		loadVC = [[LoadViewController alloc] init];
		loadVC.owner = self;
		loadVC.delegate = self;
	}
	if (loadPC==nil) {
		loadPC = [[UIPopoverController alloc] initWithContentViewController:loadVC];
	}
	
	[loadVC loadPuzzle];
	loadPC.popoverContentSize = loadVC.view.frame.size;
	[loadPC presentPopoverFromRect:CGRectMake(350, 300, 0, 0)\
							inView:self.view permittedArrowDirections:UIPopoverArrowDirectionAny animated:YES];
}

-(IBAction) didClickInfo {
	NSLog(@"didClickLoadPuzzle");
	
	if (infoVC==nil) {
		infoVC = [[InfoViewController alloc] init];
	}
	if (infoPC==nil) {
		infoPC = [[UIPopoverController alloc] initWithContentViewController:infoVC];
	}
	
	infoPC.popoverContentSize = infoVC.view.frame.size;
	[infoPC presentPopoverFromRect:CGRectMake(350, 500, 0, 0)\
							inView:self.view permittedArrowDirections:UIPopoverArrowDirectionAny animated:YES];
}

-(IBAction) didClickBgMusic {
	NSLog(@"didClickBgMusic");
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
	[musicPickerPopoverController presentPopoverFromRect:CGRectMake(350, 400, 0, 0) inView:self.view\
								permittedArrowDirections:UIPopoverArrowDirectionAny animated:YES];
	[picker release];
}

// after chosen a set of song
- (void) mediaPicker: (MPMediaPickerController *) mediaPicker
   didPickMediaItems: (MPMediaItemCollection *) collection {
	NSLog(@"didPickMediaItems");
	MPMusicPlayerController* musicPlayer = [MPMusicPlayerController applicationMusicPlayer];
	[musicPlayer setQueueWithItemCollection:collection];
	[musicPlayer play];
	[musicPickerPopoverController dismissPopoverAnimated:YES];
}

-(void) enterPuzzle:(Puzzle*)aPuzzle {
	NSLog(@"enterPuzzle");
	NSLog(@"%@", aPuzzle);
	if (deskVC==nil) {
		deskVC = [[DeskViewController alloc] init];
	}
	
	[deskVC loadPuzzle:aPuzzle];
	[self.view addSubview:deskVC.view];
}

-(void) hidePopovers {
	[imagePC dismissPopoverAnimated:YES];
	[loadPC dismissPopoverAnimated:YES];
}

-(void) startIndicator {
	NSLog(@"startIndicator");
	[self.view addSubview:activityIndicator];
	[activityIndicator startAnimating];
}

-(void) stopIndicator {
	[activityIndicator stopAnimating];
	[activityIndicator removeFromSuperview];
}

// Override to allow orientations other than the default portrait orientation.
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    return (interfaceOrientation==UIInterfaceOrientationLandscapeLeft) || (interfaceOrientation==UIInterfaceOrientationLandscapeRight);
}

- (void)didReceiveMemoryWarning {
	// Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
	
	// Release any cached data, images, etc that aren't in use.
}

- (void)viewDidUnload {
	// Release any retained subviews of the main view.
	// e.g. self.myOutlet = nil;
}


- (void)dealloc {
	[curPuzzle release];
	[deskVC release];
	[loadVC release];
	[loadPC release];
	[infoVC release];
	[infoPC release];
	[queue release];
    [super dealloc];
}

@end
