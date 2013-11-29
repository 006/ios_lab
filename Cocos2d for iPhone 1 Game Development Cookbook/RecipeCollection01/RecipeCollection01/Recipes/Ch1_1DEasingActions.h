#import "cocos2d.h"
#import "Recipe.h"

//Interface

enum {
	TAG_SPRITE_TO_MOVE = 0,
	TAG_PREV_ACTION = 1,
	TAG_RELOAD_ACTION = 2,
	TAG_NEXT_ACTION = 3,
	TAG_ACTION_NAME = 4,
	TAG_BREADCRUMBS = 5
};

static NSString *actionMethods[] = {
	@"EaseIn",
	@"EaseOut",
	@"EaseInOut",
	@"EaseExponentialIn",
	@"EaseExponentialOut",
	@"EaseExponentialInOut",
	@"EaseSineIn",
	@"EaseSineOut",
	@"EaseSineInOut",
	@"EaseElasticIn",
	@"EaseElasticOut",
	@"EaseElasticInOut",
	@"EaseBounceIn",
	@"EaseBounceOut",
	@"EaseBounceInOut",
	@"EaseBackIn",
	@"EaseBackOut",
	@"EaseBackInOut",
};
static int numActionMethods = 18;

@interface Ch1_1DEasingActions : Recipe
{
	int currentAction;
	CCActionInterval* actionToEase;
}

-(CCLayer*) runRecipe;
-(void) prevActionCallback:(id)sender;
-(void) reloadActionCallback:(id)sender;
-(void) nextActionCallback:(id)sender;
-(void) runCurrentAction;


-(CCEaseIn*) getEaseInAction;
-(CCEaseOut*) getEaseOutAction;
-(CCEaseInOut*) getEaseInOutAction;
-(CCEaseExponentialIn*) getEaseExponentialInAction;
-(CCEaseExponentialOut*) getEaseExponentialOutAction;
-(CCEaseExponentialInOut*) getEaseExponentialInOutAction;
-(CCEaseSineIn*) getEaseSineInAction;
-(CCEaseSineOut*) getEaseSineOutAction;
-(CCEaseSineInOut*) getEaseSineInOutAction;
-(CCEaseElasticIn*) getEaseElasticInAction;
-(CCEaseElasticOut*) getEaseElasticOutAction;
-(CCEaseElasticInOut*) getEaseElasticInOutAction;
-(CCEaseBounceIn*) getEaseBounceInAction;
-(CCEaseBounceOut*) getEaseBounceOutAction;
-(CCEaseBounceInOut*) getEaseBounceInOutAction;
-(CCEaseBackIn*) getEaseBackInAction;
-(CCEaseBackOut*) getEaseBackOutAction;
-(CCEaseBackInOut*) getEaseBackInOutAction;

@end



//Implementation

@implementation Ch1_1DEasingActions

-(CCLayer*) runRecipe {
	[self schedule: @selector(step:)];

	//TO DO - Replace this texture with one that I've drawn myself. Also, one that better illustrates the actions.

	actionToEase = [CCMoveBy actionWithDuration:2 position:ccp(200,200)];
	[actionToEase retain];

	CCSprite *sprite = [CCSprite spriteWithFile:@"colorable_sprite.png"];
	[sprite setPosition:ccp(150,50)];
	[sprite setScale:0.5f];
	[self addChild:sprite z:1 tag:TAG_SPRITE_TO_MOVE];	
	
	//Add Breadcrumbs
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
    prevMenu.position = ccp( 160 , 20 );
    [self addChild:prevMenu z:0 tag:TAG_PREV_ACTION];	
	
	CCMenuItemFont* reload = [CCMenuItemFont itemFromString:@"<RELOAD>" target:self selector:@selector(reloadActionCallback:)];
	CCMenu *reloadMenu = [CCMenu menuWithItems:reload, nil];
    reloadMenu.position = ccp( 240 , 20 );
    [self addChild:reloadMenu z:0 tag:TAG_RELOAD_ACTION];		
	
	CCMenuItemFont* next = [CCMenuItemFont itemFromString:@"NEXT>" target:self selector:@selector(nextActionCallback:)];
	CCMenu *nextMenu = [CCMenu menuWithItems:next, nil];
    nextMenu.position = ccp( 320 , 20 );
    [self addChild:nextMenu z:0 tag:TAG_NEXT_ACTION];
	
	currentAction = 0;
	
	[self runCurrentAction];

	return self;
}


-(void) prevActionCallback:(id)sender {
	currentAction -= 1;
	if(currentAction < 0){
		currentAction = numActionMethods-1;
	}
	[self removeChildByTag:TAG_ACTION_NAME cleanup:YES];
	[self runCurrentAction];
}
-(void) reloadActionCallback:(id)sender {
	[self removeChildByTag:TAG_ACTION_NAME cleanup:YES];
	[self runCurrentAction];	
}
-(void) nextActionCallback:(id)sender {
	currentAction += 1;
	if(currentAction >= numActionMethods){
		currentAction = 0;
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

	NSString *method = [NSString stringWithFormat:@"get%@Action",actionMethods[currentAction]];
	[node runAction:[self performSelector:NSSelectorFromString(method)]];
	
	CCLabelTTF* name = [CCLabelTTF labelWithString:actionMethods[currentAction] fontName:@"Arial" fontSize:20];
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
}


-(CCEaseIn*) getEaseInAction {
	return [CCEaseIn actionWithAction:actionToEase rate:2];
}
-(CCEaseOut*) getEaseOutAction {
	return [CCEaseOut actionWithAction:actionToEase rate:2];
}
-(CCEaseInOut*) getEaseInOutAction {
	return [CCEaseInOut actionWithAction:actionToEase rate:2];
}
-(CCEaseExponentialIn*) getEaseExponentialInAction {
	return [CCEaseExponentialIn actionWithAction:actionToEase];
}
-(CCEaseExponentialOut*) getEaseExponentialOutAction {
	return [CCEaseExponentialOut actionWithAction:actionToEase];
}
-(CCEaseExponentialInOut*) getEaseExponentialInOutAction {
	return [CCEaseExponentialInOut actionWithAction:actionToEase];
}
-(CCEaseSineIn*) getEaseSineInAction {
	return [CCEaseSineIn actionWithAction:actionToEase];
}
-(CCEaseSineOut*) getEaseSineOutAction {
	return [CCEaseSineOut actionWithAction:actionToEase];
}
-(CCEaseSineInOut*) getEaseSineInOutAction {
	return [CCEaseSineInOut actionWithAction:actionToEase];
}
-(CCEaseElasticIn*) getEaseElasticInAction {
	return [CCEaseElasticIn actionWithAction:actionToEase period:0.3f];
}
-(CCEaseElasticOut*) getEaseElasticOutAction {
	return [CCEaseElasticOut actionWithAction:actionToEase period:0.3f];
}
-(CCEaseElasticInOut*) getEaseElasticInOutAction {
	return [CCEaseElasticInOut actionWithAction:actionToEase period:0.3f];
}
-(CCEaseBounceIn*) getEaseBounceInAction {
	return [CCEaseBounceIn actionWithAction:actionToEase];
}
-(CCEaseBounceOut*) getEaseBounceOutAction {
	return [CCEaseBounceOut actionWithAction:actionToEase];
}
-(CCEaseBounceInOut*) getEaseBounceInOutAction { 
	return [CCEaseBounceInOut actionWithAction:actionToEase];
}
-(CCEaseBackIn*) getEaseBackInAction {
	return [CCEaseBackIn actionWithAction:actionToEase];
}
-(CCEaseBackOut*) getEaseBackOutAction {
	return [CCEaseBackOut actionWithAction:actionToEase];
}
-(CCEaseBackInOut*) getEaseBackInOutAction {
	return [CCEaseBackInOut actionWithAction:actionToEase];
}

@end
