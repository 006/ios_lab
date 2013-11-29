/*
 *  LoopingMenu.h
 *  Banzai
 *
 *  Created by João Caxaria on 5/29/09.
 *  Copyright 2009 Imaginary Factory. All rights reserved.
 *
 */
#import "cocos2d.h"
#import "SimpleAudioEngine.h"

/*
typedef enum  {
	kCCMenuStateWaiting,
	kCCMenuStateTrackingTouch
} tCCMenuState;

enum {
	//* priority used by the menu
	kCCMenuTouchPriority = -128,
};
 */

@interface LoopingMenu : CCMenu
{	
	float hPadding;
	float lowerBound;
	float yOffset;
	bool moving;
	bool touchDown;
	float accelerometerVelocity;
	//int state;
	//CCMenuItem *selectedItem;
	
	
	//tCCMenuState state_;
	//CCMenuItem	*selectedItem_;
	//GLubyte		opacity_;
	//ccColor3B	color_;
}

@property float yOffset;

@end
