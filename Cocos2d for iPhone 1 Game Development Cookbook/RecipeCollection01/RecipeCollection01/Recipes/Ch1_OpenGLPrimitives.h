#import "cocos2d.h"
#import "Recipe.h"
#import "ShapeLayer.h"

//Interface
@interface Ch1_OpenGLPrimitives : Recipe
{
}

-(CCLayer*) runRecipe;

@end



//Implementation

@implementation Ch1_OpenGLPrimitives

-(CCLayer*) runRecipe {
	ShapeLayer *layer = [[ShapeLayer alloc] init];
	[layer setPosition:ccp(0,0)];
	[self addChild:layer z:2 tag:0];
		
	return self;
}

@end
