#import "GameIsoObject.h"

@implementation GameIsoObject

@synthesize type, yModifier, actualImageSize, inGameSize, spriteShadow, zModifier, bounceCoefficient, rollCoefficient;

-(id) init {
	if( (self=[super init] )) {
		zModifier = 0;
		bounceCoefficient = [Vector3D x:0.2f y:0.2f z:0.75f];
		rollCoefficient = [Vector3D x:0.01f y:0.01f z:0.8f];
	}
	return self;
}

-(int) type {
	return GO_TYPE_MISC;
}

@end