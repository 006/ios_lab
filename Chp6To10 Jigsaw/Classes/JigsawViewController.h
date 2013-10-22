//
//  JigsawViewController.h
//  Jigsaw
//
//  Created by Chen Li on 9/23/10.
//  Copyright 2010 Chen Li. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "LoadViewController.h"
#import <MediaPlayer/MediaPlayer.h>
#import "Puzzle.h"

@class DeskViewController;
@class LoadViewController;
@class InfoViewController;

@interface JigsawViewController : UIViewController\
<UIImagePickerControllerDelegate,UINavigationControllerDelegate,\
UIActionSheetDelegate,MPMediaPickerControllerDelegate> {
	UIPopoverController* imagePC;
	UIPopoverController* musicPC;
	Puzzle* curPuzzle;
	DeskViewController* deskVC;
	UIActivityIndicatorView* activityIndicator;
	NSOperationQueue* queue;
	
	UIPopoverController* loadPC;
	LoadViewController* loadVC;
	UIPopoverController* infoPC;
	InfoViewController* infoVC;
	UIPopoverController* musicPickerPopoverController;
	
	int pieceSize;
}

@property (retain) Puzzle* curPuzzle;
@property (retain) NSOperationQueue* queue;

-(IBAction) didClickNewPuzzle;
-(void) showImagePicker;
-(IBAction) didClickLoadPuzzle;
-(IBAction) didClickBgMusic;
-(IBAction) didClickInfo;
-(void) startIndicator;
-(void) stopIndicator;
-(void) hidePopovers;

-(void) enterPuzzle:(Puzzle*)aPuzzle;

@end

