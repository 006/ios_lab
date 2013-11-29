#import "cocos2d.h"
#import "Recipe.h"

//Interface

@interface Ch1_RetinaDisplay : Recipe
{
}

-(CCLayer*) runRecipe;

@end



//Implementation

@implementation Ch1_RetinaDisplay

-(CCLayer*) runRecipe {
	//Switch to Retina mode to see the difference
	CCSprite *sprite = [CCSprite spriteWithFile:@"cocos2d_beginner.png"];
	[sprite setPosition:ccp(240,160)];
	[sprite setScale:1.0f];
	[self addChild:sprite];
	
	return self;
}

@end
