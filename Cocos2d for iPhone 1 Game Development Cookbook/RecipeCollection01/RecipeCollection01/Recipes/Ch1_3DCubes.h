#import "cocos2d.h"
#import "Recipe.h"
#import "Cube3D.h"

//Interface
@interface Ch1_3DCubes : Recipe
{
	Cube3D *cube3d1;
	Cube3D *cube3d2;
}

-(CCLayer*) runRecipe;
-(void) step:(ccTime)delta;

@end



//Implementation

@implementation Ch1_3DCubes

-(CCLayer*) runRecipe {	
	//Load a textured cube and set initial variables
	cube3d1 = [Cube3D spriteWithFile:@"crate.jpg"];
	cube3d1.translation3D = [Vector3D x:2.0f y:0.0f z:-4.0f];
	cube3d1.rotation3DAxis = [Vector3D x:2.0f y:2.0f z:4.0f];
	cube3d1.rotation3DAngle = 0.0f;
	cube3d1.drawTextured = YES;
	[self addChild:cube3d1 z:3 tag:0];
	
	//Load a colored cube and set initial variables
	cube3d2 = [Cube3D spriteWithFile:@"blank.png"];
	cube3d2.translation3D = [Vector3D x:-2.0f y:0.0f z:-4.0f];
	cube3d2.rotation3DAxis = [Vector3D x:2.0f y:2.0f z:4.0f];
	cube3d2.rotation3DAngle = 0.0f;
	cube3d2.drawTextured = NO;
	[self addChild:cube3d2 z:1 tag:1];
	
	//Schedule cube rotation
	[self schedule:@selector(step:)];
	
	return self;
}

-(void) step:(ccTime)delta {
	cube3d1.rotation3DAngle += 0.5f;
	cube3d2.rotation3DAngle -= 0.5f;
}

@end
