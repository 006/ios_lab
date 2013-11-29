//
//  Ch8_Cocos2d_XAppController.h
//  Ch8_Cocos2d-X
//
//  Created by nate on 12/7/11.
//  Copyright Logical Extreme Studios 2011. All rights reserved.
//

@class RootViewController;

@interface AppController : NSObject <UIAccelerometerDelegate, UIAlertViewDelegate, UITextFieldDelegate,UIApplicationDelegate> {
    UIWindow *window;
    RootViewController	*viewController;
}

@end

