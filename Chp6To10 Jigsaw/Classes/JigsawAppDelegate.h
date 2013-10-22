//
//  JigsawAppDelegate.h
//  Jigsaw
//
//  Created by Chen Li on 9/23/10.
//  Copyright 2010 Chen Li. All rights reserved.
//

#import <UIKit/UIKit.h>

@class JigsawViewController;

@interface JigsawAppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *window;
    JigsawViewController *viewController;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet JigsawViewController *viewController;

@end

