#import "cocos2d.h"
#import "Recipe.h"

//Interface

@interface Ch2_AccelerometerSteering : Recipe
{
	CCSprite *steeringWheel;
}

-(CCLayer*) runRecipe;

@end

//Implementation

@implementation Ch2_AccelerometerSteering

-(CCLayer*) runRecipe {
	//Enable the accelerometer and set its updateInterval
	self.isAccelerometerEnabled = YES;
	[[UIAccelerometer sharedAccelerometer] setUpdateInterval:(1.0 / 60)];

	//Init car background
	CCSprite *bg = [CCSprite spriteWithFile:@"car_dash.jpg"];
	bg.position = ccp(240,160);
	bg.opacity = 200;
	[self addChild:bg z:0];

	//Init steeringWheel sprite
	steeringWheel = [CCSprite spriteWithFile:@"car_steering_wheel.png"];
	steeringWheel.position = ccp(230,170);
	[self addChild:steeringWheel z:1];
	
	return self;
}

/* Handle accelerometer input */
- (void)accelerometer:(UIAccelerometer*)accelerometer didAccelerate:(UIAcceleration*)acceleration{
	//Set steeringWheel rotation based on Y plane rotation
	steeringWheel.rotation = -acceleration.y * 180;
}

@end
