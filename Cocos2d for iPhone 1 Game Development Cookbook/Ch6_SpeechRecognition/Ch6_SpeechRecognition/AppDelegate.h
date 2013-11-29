//
//  AppDelegate.h
//  Ch6_SpeechRecognition
//
//  Created by nate on 5/11/11.
//  Copyright Logical Extreme Studios 2011. All rights reserved.
//

#import <UIKit/UIKit.h>

@class RootViewController;

@interface AppDelegate : NSObject <UIApplicationDelegate> {
	UIWindow			*window;
	RootViewController	*viewController;
}

@property (nonatomic, retain) UIWindow *window;

@end
