#import "cocos2d.h"
#import "Recipe.h"
#import "IphoneCube.h"

//Interface

@interface Ch2_AccelerometerRotation : Recipe
{
	IphoneCube *iphoneCube;
}

-(CCLayer*) runRecipe;

@end



//Implementation

@implementation Ch2_AccelerometerRotation

-(CCLayer*) runRecipe {
	//Enable the accelerometer and set its updateInterval
	self.isAccelerometerEnabled = YES;
	[[UIAccelerometer sharedAccelerometer] setUpdateInterval:(1.0 / 60)];

	//Init our textured box
	iphoneCube = [[IphoneCube alloc] init];
	iphoneCube.translation3D = [Vector3D x:0.0f y:0.0f z:-2.0f];
	iphoneCube.rotation3DAxis = [Vector3D x:0 y:0.0f z:-90];
		
	[self addChild:iphoneCube z:3 tag:0];
	
	return self;
}

/* Handle accelerometer input */
- (void)accelerometer:(UIAccelerometer*)accelerometer didAccelerate:(UIAcceleration*)acceleration{
	//Set x and y box orientation
	iphoneCube.rotation3DAxis.x = -acceleration.x * 270;
	iphoneCube.rotation3DAxis.y = -acceleration.y * 270;
}

@end
