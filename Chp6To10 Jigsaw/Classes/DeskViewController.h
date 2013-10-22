//
//  DeskViewController.h
//  Jigsaw
//
//  Created by Chen Li on 10/11/10.
//  Copyright 2010 Chen Li. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "Definitions.h"
#import <MessageUI/MessageUI.h>
#import <MediaPlayer/MediaPlayer.h>
#import "FBConnect.h"

@class Puzzle;
@class Piece;
@class MyScrollView;
@class InfoViewController;

@interface DeskViewController : UIViewController <UIScrollViewDelegate, UIActionSheetDelegate,\
MFMailComposeViewControllerDelegate, MPMediaPickerControllerDelegate,FBRequestDelegate,FBDialogDelegate,FBSessionDelegate> {
	Puzzle* curPuzzle;
	Piece* selectedPiece;
	CGPoint beginLocation;			// the location where the touch begins
	CGPoint pieceOriLocation;		// the original center 
	DeskViewState state;			
	CGRect poolRect;				// the rect of the pool
	CGRect deskRect;				// the rect of the desk
	int d,g,H;						// the key sizes
	int xOffset, yOffset;			// the offset used on the table
	int timePlayed;
	NSTimer* secTimer;
	IBOutlet UILabel* timerLabel;
	
	IBOutlet UIImageView* previewImgView;
	IBOutlet UIImageView* deskImgView;
	UIView* piecesView;
	
	MyScrollView* scrollView;		// hold the pool pieces
	CGPoint scrollViewOrigin;
	
	Facebook* facebook;
	NSArray* permissions;
	
	UIPopoverController* musicPickerPopoverController;
	UIPopoverController* infoPC;
	InfoViewController* infoVC;
}

@property (retain) Puzzle* curPuzzle;
@property (readwrite) DeskViewState state;
@property (retain) NSTimer* secTimer;
@property (retain,nonatomic) IBOutlet UIImageView* deskImgView;
@property (retain,nonatomic) IBOutlet UIImageView* previewImgView;
@property (retain) UIScrollView* scrollView;
@property (retain,nonatomic) IBOutlet UILabel* timerLabel;

-(void) loadPuzzle:(Puzzle*)aPuzzle;
-(void) displayTablePieces;				// used when a puzzle is loaded
-(void) updatePoolDisplay;				// used whenever the curPuzzle.poolPieces is changed
- (void)createGestureRecognizers;
-(void) applicationWillTerminate;
-(BOOL) savePuzzle:(Puzzle*)aPuzzle;
-(void) wonTheGame;

-(IBAction) didClickMenu;
-(void) actionEmail;
-(void) actionFacebook;
-(void) actionMusic;
-(void) actionSave;
-(void) actionInfo;
-(void) actionMainMenu;

-(UIImage*) screenShot;
-(void) uploadToFacebook;

-(void) displayTime;

@end
