#import "cocos2d.h"
#import "Recipe.h"
#import "CCMoveByCustom.h"

enum {
	TAG_RANDOM_ACTION = 6
};

//Interface
@interface Ch1_2DEasingActions : Recipe
{
	int currentActionX;
	int currentActionY;
	CCActionInterval* actionToEaseX;
	CCActionInterval* actionToEaseY;
}

-(CCLayer*) runRecipe;
-(void) prevActionCallback:(id)sender;
-(void) reloadActionCallback:(id)sender;
-(void) randomActionCallback:(id)sender;
-(void) nextActionCallback:(id)sender;
-(void) runCurrentAction;


-(CCEaseIn*) getEaseInActionX;
-(CCEaseOut*) getEaseOutActionX;
-(CCEaseInOut*) getEaseInOutActionX;
-(CCEaseExponentialIn*) getEaseExponentialInActionX;
-(CCEaseExponentialOut*) getEaseExponentialOutActionX;
-(CCEaseExponentialInOut*) getEaseExponentialInOutActionX;
-(CCEaseSineIn*) getEaseSineInActionX;
-(CCEaseSineOut*) getEaseSineOutActionX;
-(CCEaseSineInOut*) getEaseSineInOutActionX;
-(CCEaseElasticIn*) getEaseElasticInActionX;
-(CCEaseElasticOut*) getEaseElasticOutActionX;
-(CCEaseElasticInOut*) getEaseElasticInOutActionX;
-(CCEaseBounceIn*) getEaseBounceInActionX;
-(CCEaseBounceOut*) getEaseBounceOutActionX;
-(CCEaseBounceInOut*) getEaseBounceInOutActionX;
-(CCEaseBackIn*) getEaseBackInActionX;
-(CCEaseBackOut*) getEaseBackOutActionX;
-(CCEaseBackInOut*) getEaseBackInOutActionX;

-(CCEaseIn*) getEaseInActionY;
-(CCEaseOut*) getEaseOutActionY;
-(CCEaseInOut*) getEaseInOutActionY;
-(CCEaseExponentialIn*) getEaseExponentialInActionY;
-(CCEaseExponentialOut*) getEaseExponentialOutActionY;
-(CCEaseExponentialInOut*) getEaseExponentialInOutActionY;
-(CCEaseSineIn*) getEaseSineInActionY;
-(CCEaseSineOut*) getEaseSineOutActionY;
-(CCEaseSineInOut*) getEaseSineInOutActionY;
-(CCEaseElasticIn*) getEaseElasticInActionY;
-(CCEaseElasticOut*) getEaseElasticOutActionY;
-(CCEaseElasticInOut*) getEaseElasticInOutActionY;
-(CCEaseBounceIn*) getEaseBounceInActionY;
-(CCEaseBounceOut*) getEaseBounceOutActionY;
-(CCEaseBounceInOut*) getEaseBounceInOutActionY;
-(CCEaseBackIn*) getEaseBackInActionY;
-(CCEaseBackOut*) getEaseBackOutActionY;
-(CCEaseBackInOut*) getEaseBackInOutActionY;

@end



//Implementation

@implementation Ch1_2DEasingActions

-(CCLayer*) runRecipe {
	[self schedule: @selector(step:)];

	//TO DO - Replace this texture with one that I've drawn myself. Also, one that better illustrates the actions.

	actionToEaseX = [CCMoveByCustom actionWithDuration:2 position:ccp(200,0)];
	[actionToEaseX retain];
	actionToEaseY = [CCMoveByCustom actionWithDuration:2 position:ccp(0,200)];
	[actionToEaseY retain];

	CCSprite *sprite = [CCSprite spriteWithFile:@"colorable_sprite.png"];
	[sprite setPosition:ccp(150,50)];
	[sprite setScale:0.5f];
	[self addChild:sprite z:1 tag:TAG_SPRITE_TO_MOVE];	
	
	//Add guiding lines
	CCSprite *breadcrumbs = [CCSprite spriteWithFile:@"blank.png"];
	breadcrumbs.position = ccp(0,0);
	[self addChild:breadcrumbs z:0 tag:TAG_BREADCRUMBS];
		
	CCSprite *start = [CCSprite spriteWithFile:@"blank.png"];
	start.position = ccp(150,50);
	start.textureRect = CGRectMake(0,0,8,8);
	start.color = ccc3(0,255,0);
	[self addChild:start z:0];
	
	CCSprite *end = [CCSprite spriteWithFile:@"blank.png"];
	end.position = ccp(350,250);
	end.textureRect = CGRectMake(0,0,8,8);
	end.color = ccc3(255,0,0);
	[self addChild:end z:0];	

	[CCMenuItemFont setFontSize:16];

	CCMenuItemFont* prev = [CCMenuItemFont itemFromString:@"<PREV" target:self selector:@selector(prevActionCallback:)];
	CCMenu *prevMenu = [CCMenu menuWithItems:prev, nil];
    prevMenu.position = ccp( 120 , 20 );
    [self addChild:prevMenu z:0 tag:TAG_PREV_ACTION];	
	
	CCMenuItemFont* reload = [CCMenuItemFont itemFromString:@"<RELOAD>" target:self selector:@selector(reloadActionCallback:)];
	CCMenu *reloadMenu = [CCMenu menuWithItems:reload, nil];
    reloadMenu.position = ccp( 200 , 20 );
    [self addChild:reloadMenu z:0 tag:TAG_RELOAD_ACTION];	
	
	CCMenuItemFont* random = [CCMenuItemFont itemFromString:@"<RANDOM>" target:self selector:@selector(randomActionCallback:)];
	CCMenu *randomMenu = [CCMenu menuWithItems:random, nil];
    randomMenu.position = ccp( 280 , 20 );
    [self addChild:randomMenu z:0 tag:TAG_RELOAD_ACTION];	
	
	CCMenuItemFont* next = [CCMenuItemFont itemFromString:@"NEXT>" target:self selector:@selector(nextActionCallback:)];
	CCMenu *nextMenu = [CCMenu menuWithItems:next, nil];
    nextMenu.position = ccp( 360 , 20 );
    [self addChild:nextMenu z:0 tag:TAG_NEXT_ACTION];
	
	currentActionX = 0;
	currentActionY = 0;
	
	[self runCurrentAction];

	return self;
}


-(void) prevActionCallback:(id)sender {
	currentActionX -= 1;
	if(currentActionX < 0){
		currentActionX = numActionMethods-1;
		currentActionY -= 1;
		if(currentActionY < 0){ currentActionY = numActionMethods-1; }
	}
	[self removeChildByTag:TAG_ACTION_NAME cleanup:YES];
	[self runCurrentAction];
}
-(void) reloadActionCallback:(id)sender {
	[self removeChildByTag:TAG_ACTION_NAME cleanup:YES];
	[self runCurrentAction];	
}
-(void) randomActionCallback:(id)sender {
	currentActionX = arc4random()%numActionMethods;
	currentActionY = arc4random()%numActionMethods;
	[self removeChildByTag:TAG_ACTION_NAME cleanup:YES];
	[self runCurrentAction];
}
-(void) nextActionCallback:(id)sender {
	currentActionX += 1;
	if(currentActionX >= numActionMethods){
		currentActionX = 0;
		currentActionY += 1;
		if(currentActionY > numActionMethods){ currentActionY = 0; }
	}
	[self removeChildByTag:TAG_ACTION_NAME cleanup:YES];
	[self runCurrentAction];
}
-(void) runCurrentAction {	
	//Clean and re-add breadcrumbs
	CCNode *breadcrumbs = [self getChildByTag:TAG_BREADCRUMBS];
	[self removeChild:breadcrumbs cleanup:YES];
	breadcrumbs = [CCSprite spriteWithFile:@"blank.png"];
	breadcrumbs.position = ccp(0,0);
	[self addChild:breadcrumbs z:0 tag:TAG_BREADCRUMBS];

	CCNode *node = [self getChildByTag:TAG_SPRITE_TO_MOVE];
	[node stopAllActions];
	[node setPosition:ccp(150,50)];

	NSString *methodX = [NSString stringWithFormat:@"get%@ActionX",actionMethods[currentActionX]];
	NSString *methodY = [NSString stringWithFormat:@"get%@ActionY",actionMethods[currentActionY]];
	
	[node runAction:[self performSelector:NSSelectorFromString(methodX)]];
	[node runAction:[self performSelector:NSSelectorFromString(methodY)]];
	
	CCLabelTTF* name = [CCLabelTTF labelWithString:[NSString stringWithFormat:@"%@ %@", actionMethods[currentActionX], actionMethods[currentActionY]] fontName:@"Arial" fontSize:14];
	name.position = ccp(360,300);
	[self addChild:name z:0 tag:TAG_ACTION_NAME];
}

-(void) step:(ccTime) delta {
	CCNode *breadcrumbs = [self getChildByTag:TAG_BREADCRUMBS];
	CCNode *node = [self getChildByTag:TAG_SPRITE_TO_MOVE];
	
	CCSprite *crumb = [CCSprite spriteWithFile:@"blank.png"];
	crumb.textureRect = CGRectMake(0, 0, 2, 2);
	crumb.color = ccc3(255, 255, 0);
	crumb.position = node.position;
	[breadcrumbs addChild:crumb];
	
	//TODO - Use the location of this breadcrumb and the last one to apply a rotation to the sprite.
}

-(CCEaseIn*) getEaseInActionX {
	return [CCEaseIn actionWithAction:actionToEaseX rate:2];
}
-(CCEaseOut*) getEaseOutActionX {
	return [CCEaseOut actionWithAction:actionToEaseX rate:2];
}
-(CCEaseInOut*) getEaseInOutActionX {
	return [CCEaseInOut actionWithAction:actionToEaseX rate:2];
}
-(CCEaseExponentialIn*) getEaseExponentialInActionX {
	return [CCEaseExponentialIn actionWithAction:actionToEaseX];
}
-(CCEaseExponentialOut*) getEaseExponentialOutActionX {
	return [CCEaseExponentialOut actionWithAction:actionToEaseX];
}
-(CCEaseExponentialInOut*) getEaseExponentialInOutActionX {
	return [CCEaseExponentialInOut actionWithAction:actionToEaseX];
}
-(CCEaseSineIn*) getEaseSineInActionX {
	return [CCEaseSineIn actionWithAction:actionToEaseX];
}
-(CCEaseSineOut*) getEaseSineOutActionX {
	return [CCEaseSineOut actionWithAction:actionToEaseX];
}
-(CCEaseSineInOut*) getEaseSineInOutActionX {
	return [CCEaseSineInOut actionWithAction:actionToEaseX];
}
-(CCEaseElasticIn*) getEaseElasticInActionX {
	return [CCEaseElasticIn actionWithAction:actionToEaseX period:0.3f];
}
-(CCEaseElasticOut*) getEaseElasticOutActionX {
	return [CCEaseElasticOut actionWithAction:actionToEaseX period:0.3f];
}
-(CCEaseElasticInOut*) getEaseElasticInOutActionX {
	return [CCEaseElasticInOut actionWithAction:actionToEaseX period:0.3f];
}
-(CCEaseBounceIn*) getEaseBounceInActionX {
	return [CCEaseBounceIn actionWithAction:actionToEaseX];
}
-(CCEaseBounceOut*) getEaseBounceOutActionX {
	return [CCEaseBounceOut actionWithAction:actionToEaseX];
}
-(CCEaseBounceInOut*) getEaseBounceInOutActionX { 
	return [CCEaseBounceInOut actionWithAction:actionToEaseX];
}
-(CCEaseBackIn*) getEaseBackInActionX {
	return [CCEaseBackIn actionWithAction:actionToEaseX];
}
-(CCEaseBackOut*) getEaseBackOutActionX {
	return [CCEaseBackOut actionWithAction:actionToEaseX];
}
-(CCEaseBackInOut*) getEaseBackInOutActionX {
	return [CCEaseBackInOut actionWithAction:actionToEaseX];
}


-(CCEaseIn*) getEaseInActionY {
	return [CCEaseIn actionWithAction:actionToEaseY rate:2];
}
-(CCEaseOut*) getEaseOutActionY {
	return [CCEaseOut actionWithAction:actionToEaseY rate:2];
}
-(CCEaseInOut*) getEaseInOutActionY {
	return [CCEaseInOut actionWithAction:actionToEaseY rate:2];
}
-(CCEaseExponentialIn*) getEaseExponentialInActionY {
	return [CCEaseExponentialIn actionWithAction:actionToEaseY];
}
-(CCEaseExponentialOut*) getEaseExponentialOutActionY {
	return [CCEaseExponentialOut actionWithAction:actionToEaseY];
}
-(CCEaseExponentialInOut*) getEaseExponentialInOutActionY {
	return [CCEaseExponentialInOut actionWithAction:actionToEaseY];
}
-(CCEaseSineIn*) getEaseSineInActionY {
	return [CCEaseSineIn actionWithAction:actionToEaseY];
}
-(CCEaseSineOut*) getEaseSineOutActionY {
	return [CCEaseSineOut actionWithAction:actionToEaseY];
}
-(CCEaseSineInOut*) getEaseSineInOutActionY {
	return [CCEaseSineInOut actionWithAction:actionToEaseY];
}
-(CCEaseElasticIn*) getEaseElasticInActionY {
	return [CCEaseElasticIn actionWithAction:actionToEaseY period:0.3f];
}
-(CCEaseElasticOut*) getEaseElasticOutActionY {
	return [CCEaseElasticOut actionWithAction:actionToEaseY period:0.3f];
}
-(CCEaseElasticInOut*) getEaseElasticInOutActionY {
	return [CCEaseElasticInOut actionWithAction:actionToEaseY period:0.3f];
}
-(CCEaseBounceIn*) getEaseBounceInActionY {
	return [CCEaseBounceIn actionWithAction:actionToEaseY];
}
-(CCEaseBounceOut*) getEaseBounceOutActionY {
	return [CCEaseBounceOut actionWithAction:actionToEaseY];
}
-(CCEaseBounceInOut*) getEaseBounceInOutActionY { 
	return [CCEaseBounceInOut actionWithAction:actionToEaseY];
}
-(CCEaseBackIn*) getEaseBackInActionY {
	return [CCEaseBackIn actionWithAction:actionToEaseY];
}
-(CCEaseBackOut*) getEaseBackOutActionY {
	return [CCEaseBackOut actionWithAction:actionToEaseY];
}
-(CCEaseBackInOut*) getEaseBackInOutActionY {
	return [CCEaseBackInOut actionWithAction:actionToEaseY];
}

@end
