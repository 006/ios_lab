//
//  LoadViewController.m
//  Jigsaw
//
//  Created by Chen Li on 10/23/10.
//  Copyright 2010 Chen Li. All rights reserved.
//

#import "LoadViewController.h"
#import "Puzzle.h"
#import "JigsawViewController.h"
#import "IndicatorOperation.h"

@implementation LoadViewController

@synthesize scrollView, arrButtons, arrDict;
@synthesize owner, delegate;

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
	
	self.scrollView.contentSize = CGSizeMake(480, 1200);
}

-(void) loadPuzzle {
	NSFileManager* fileManager = [NSFileManager defaultManager];
	NSString* homePath = NSHomeDirectory();
	NSString* docPath = [homePath stringByAppendingFormat:@"/Documents"];
	
	// clear all current buttons
	for (UIButton* button in arrButtons) {
		[button removeFromSuperview];
	}
	
	// get all the plist
	NSMutableArray* arrPlist = [[fileManager contentsOfDirectoryAtPath:docPath error:nil] mutableCopy];
	for (int i=0; i<[arrPlist count]; i++) {
		NSString* aFileName = [arrPlist objectAtIndex:i];
		NSLog(@"i:%d,obj:%@",i,aFileName);
		// if not a plist, remove from arrPlist
		if (![[aFileName pathExtension] isEqualToString:@"plist"]) {
			NSLog(@"remove file with name: %@, extension:%@",aFileName,[aFileName pathExtension]);
			NSLog(@"[aFileName pathExtension]!=plist : %d",[aFileName pathExtension]!=@"plist");
			[arrPlist removeObject:aFileName];
		}
	}
	
	// arrButtons
	self.arrButtons = [NSMutableArray array];
	// arrDict
	self.arrDict = [NSMutableArray array];
	
	// read the plist, one by one, and generate a puzzle icon for each one
	// print all the plist
	for (int i=0; i<[arrPlist count]; i++) {
		// get dict
		NSString* aFileName = [arrPlist objectAtIndex:i];
		NSString* aFilePath = [docPath stringByAppendingFormat:@"/%@", aFileName];
		NSDictionary* dictPuzzle = [NSMutableDictionary dictionaryWithContentsOfFile:aFilePath];
		[arrDict addObject:dictPuzzle];
		
		// get image
		NSString* imagePath = [dictPuzzle valueForKey:@"imagePath"];
		NSLog(@"%@", imagePath);
		UIImage* fullImage = [UIImage imageWithContentsOfFile:imagePath];
		// resize image
		// figure out the correct size of the image (limited to 896*768)
		float oriWidth = fullImage.size.width;
		float oriHeight = fullImage.size.height;
		float targetWidth;
		float targetHeight;
		if (oriWidth/oriHeight>60.0/60.0) {
			targetWidth = 60;
			targetHeight = oriHeight/oriWidth*60;
		}
		else {
			targetHeight = 60;
			targetWidth = oriWidth/oriHeight*60;
		}
		// resize: round to interger multiple of 128
		// so that we can design piece rect to be 128*128 for easy, 64*64 for midium, and 32*32 for hard
		UIGraphicsBeginImageContext(CGSizeMake(60,60));
		[fullImage drawInRect:CGRectMake((60-targetWidth)/2, 0, targetWidth, targetHeight)];
		UIImage* smallImage = UIGraphicsGetImageFromCurrentImageContext();		
		UIGraphicsEndImageContext();
		
		// create button
		int row = i/6;
		int col = i-6*row;
		UIButton* button = [UIButton buttonWithType:UIButtonTypeCustom];
		//button.imageView.image = smallImage;
		[button setImage:smallImage forState:UIControlStateNormal];
		button.frame = CGRectMake(20+80*col, 20+80*row, 60, 60);
		[button addTarget:self action:@selector(buttonPressed:) forControlEvents:UIControlEventTouchUpInside];
		
		// add button
		[scrollView addSubview:button];
		[self.arrButtons addObject:button];
	}
}

-(void) buttonPressed:(id)sender {
	for (int i=0; i<[arrButtons count]; i++) {
		if (sender==[arrButtons objectAtIndex:i]) {
			NSLog(@"%d-th button pressed",i);
			iSelectedButton = i;
			
			UIActionSheet* actionSheet = [[UIActionSheet alloc] initWithTitle:nil delegate:self cancelButtonTitle:@"Cancel"\
													   destructiveButtonTitle:@"Delete"\
															otherButtonTitles:@"Load", nil];
			[actionSheet showInView:self.view];
			[actionSheet release];
			
			break;
		}
	}
}

- (void)actionSheet:(UIActionSheet *)actionSheet clickedButtonAtIndex:(NSInteger)buttonIndex {
	switch (buttonIndex) {
		case 0:
			NSLog(@"Delete");
			[actionSheet dismissWithClickedButtonIndex:buttonIndex animated:YES];
			UIAlertView* alertSheet = [[UIAlertView alloc] initWithTitle:nil message:@"Delete the puzzle?" delegate:self \
													   cancelButtonTitle:@"NO" otherButtonTitles:@"Delete",nil];
			alertSheet.transform = CGAffineTransformTranslate(alertSheet.transform, -115, 0);
			[alertSheet show];
			[alertSheet release];
			break;
		case 1:
			NSLog(@"Load");
			// the dict corresponding to the puzzle
			NSDictionary* aDict	= [arrDict objectAtIndex:iSelectedButton];	
			// the puzzle
			IndicatorOperation* operation = [[IndicatorOperation alloc] init];
			operation.owner = owner;
			[owner.queue addOperation:operation];
			Puzzle* aPuzzle = [[[Puzzle alloc] initWithDict:aDict] autorelease];
			[operation release];
			[owner stopIndicator];
			// start the game
			[owner enterPuzzle:aPuzzle];
			break;
		case 2:
			NSLog(@"Cancel");
			break;
		default:
			NSLog(@"un-handled action button");
			break;
	}
}

- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex {
	switch (buttonIndex) {
		case 0:
			NSLog(@"No");
			break;
		case 1:
			NSLog(@"Delete");
			// dismiss the alertView
			// delete the files
			NSFileManager* fileManager = [NSFileManager defaultManager];
			NSString* homePath = NSHomeDirectory();
			NSString* docPath = [homePath stringByAppendingFormat:@"/Documents"];
			NSMutableDictionary* dict = [arrDict objectAtIndex:iSelectedButton];
			NSString* plistName = [dict objectForKey:@"fileName"];
			NSString* imageName = [[plistName stringByDeletingPathExtension] stringByAppendingPathExtension:@"png"];
			NSString* plistPath = [docPath stringByAppendingFormat:@"/%@", plistName];
			NSString* imagePath = [docPath stringByAppendingFormat:@"/%@", imageName];
			[fileManager removeItemAtPath:plistPath error:nil];
			[fileManager removeItemAtPath:imagePath error:nil];
			// reload the puzzles
			[self loadPuzzle];
			break;
		default:
			break;
	}
}

/*
// Override to allow orientations other than the default portrait orientation.
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    // Return YES for supported orientations
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}
*/

- (void)didReceiveMemoryWarning {
    // Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
    
    // Release any cached data, images, etc that aren't in use.
}

- (void)viewDidUnload {
    [super viewDidUnload];
    // Release any retained subviews of the main view.
    // e.g. self.myOutlet = nil;
}


- (void)dealloc {
	[scrollView release];
	[arrButtons release];
	[arrDict release];
    [super dealloc];
}


@end
