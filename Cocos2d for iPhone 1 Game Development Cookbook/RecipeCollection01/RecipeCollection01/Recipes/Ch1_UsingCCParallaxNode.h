#import "cocos2d.h"
#import "Recipe.h"

//Interface

@interface Ch1_UsingCCParallaxNode : Recipe
{
}

-(CCLayer*) runRecipe;

@end



//Implementation

@implementation Ch1_UsingCCParallaxNode

-(CCLayer*) runRecipe {
	//Create four parallax sprites, one for each layer
	CCSprite* parallaxLayer01 = [CCSprite spriteWithFile:@"parallax_layer_01.png"];	
	CCSprite* parallaxLayer02 = [CCSprite spriteWithFile:@"parallax_layer_02.png"];
	CCSprite* parallaxLayer03 = [CCSprite spriteWithFile:@"parallax_layer_03.png"];
	CCSprite* parallaxLayer04 = [CCSprite spriteWithFile:@"parallax_layer_04.png"];
	
	//Create a parallax node and add all four sprites
	CCParallaxNode* parallaxNode = [CCParallaxNode node];
	[parallaxNode setPosition:ccp(0,0)];
	[parallaxNode addChild:parallaxLayer01 z:1 parallaxRatio:ccp(0, 0) positionOffset:ccp(240,200)];
	[parallaxNode addChild:parallaxLayer02 z:2 parallaxRatio:ccp(1, 0) positionOffset:ccp(240,100)];
	[parallaxNode addChild:parallaxLayer03 z:3 parallaxRatio:ccp(2, 0) positionOffset:ccp(240,100)];
	[parallaxNode addChild:parallaxLayer04 z:4 parallaxRatio:ccp(3, 0) positionOffset:ccp(240,20)];
	[self addChild:parallaxNode z:0 tag:1];
	
	//Move the node to the left then the right
	//This creates the effect that we are moving to the right then the left
	CCMoveBy* moveRight = [CCMoveBy actionWithDuration:5.0f position:ccp(-80, 0)];
	CCMoveBy* moveLeft = [CCMoveBy actionWithDuration:2.5f position:ccp(80, 0)];
	CCSequence* sequence = [CCSequence actions:moveRight, moveLeft, nil];
	CCRepeatForever* repeat = [CCRepeatForever actionWithAction:sequence];
	[parallaxNode runAction:repeat];
	
	return self;
}

@end
