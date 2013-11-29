#import "cocos2d.h"
#import "Recipe.h"
#import "Screenshot.h"

//Interface
@interface Ch1_TakingScreenshots : Recipe {
	float counter;
}

-(CCLayer*) runRecipe;
-(void) step:(ccTime) delta;


@end



//Implementation

@implementation Ch1_TakingScreenshots

-(CCLayer*) runRecipe {
	counter = 0;
	
	CCSprite* sprite = [CCSprite spriteWithTexture:[Screenshot takeAsTexture2D]];
	[sprite setPosition:ccp(240,160)];
	[sprite setScale:0.75f];
	
	[self addChild:sprite z:0 tag:0];

	[self schedule: @selector(step:)];
	
	return self;
}

-(void) step:(ccTime) delta {
	counter += (float)delta;
	
	if(counter > 1.0f){
		[self removeChildByTag:0 cleanup:YES];
		
		CCSprite* sprite = [CCSprite spriteWithTexture:[Screenshot takeAsTexture2D]];
		[sprite setPosition:ccp(240,160)];
		[sprite setScale:0.75f];
		
		[self addChild:sprite z:0 tag:0];
		
		counter = 0;
	}
}

@end
