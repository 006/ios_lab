//
//  Ch8_Cocos3dAppDelegate.h
//  Ch8_Cocos3d
//
//  Created by nate on 8/24/11.
//  Copyright Logical Extreme Studios 2011. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "CCNodeController.h"
#import "CC3World.h"

@interface Ch8_Cocos3dAppDelegate : NSObject <UIApplicationDelegate> {
	UIWindow* window;
	CCNodeController* viewController;
}

@property (nonatomic, retain) UIWindow* window;

@end
