//
//  LoadViewController.h
//  Jigsaw
//
//  Created by Chen Li on 10/23/10.
//  Copyright 2010 Chen Li. All rights reserved.
//

#import <UIKit/UIKit.h>

@class JigsawViewController;

@interface LoadViewController : UIViewController <UIActionSheetDelegate,UIAlertViewDelegate> {
	IBOutlet UIScrollView* scrollView;
	NSMutableArray* arrButtons;
	NSMutableArray* arrDict;
	JigsawViewController* owner;
	int iSelectedButton;			// the index of the selected button
	
	id delegate;
}

@property (nonatomic,retain) IBOutlet UIScrollView* scrollView;
@property (retain) NSMutableArray* arrButtons;
@property (retain) NSMutableArray* arrDict;
@property (assign) JigsawViewController* owner;
@property (assign) id delegate;

-(void) loadPuzzle;

@end
