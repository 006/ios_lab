#import "Recipe.h"

@interface FadeWhiteTransition : CCTransitionFade
+(id) transitionWithDuration:(ccTime) t scene:(CCScene*)s;
@end
@interface FlipXLeftOver : CCTransitionFlipX 
+(id) transitionWithDuration:(ccTime) t scene:(CCScene*)s;
@end
@interface FlipXRightOver : CCTransitionFlipX 
+(id) transitionWithDuration:(ccTime) t scene:(CCScene*)s;
@end
@interface FlipYUpOver : CCTransitionFlipY 
+(id) transitionWithDuration:(ccTime) t scene:(CCScene*)s;
@end
@interface FlipYDownOver : CCTransitionFlipY 
+(id) transitionWithDuration:(ccTime) t scene:(CCScene*)s;
@end
@interface FlipAngularLeftOver : CCTransitionFlipAngular 
+(id) transitionWithDuration:(ccTime) t scene:(CCScene*)s;
@end
@interface FlipAngularRightOver : CCTransitionFlipAngular 
+(id) transitionWithDuration:(ccTime) t scene:(CCScene*)s;
@end
@interface ZoomFlipXLeftOver : CCTransitionZoomFlipX 
+(id) transitionWithDuration:(ccTime) t scene:(CCScene*)s;
@end
@interface ZoomFlipXRightOver : CCTransitionZoomFlipX 
+(id) transitionWithDuration:(ccTime) t scene:(CCScene*)s;
@end
@interface ZoomFlipYUpOver : CCTransitionZoomFlipY 
+(id) transitionWithDuration:(ccTime) t scene:(CCScene*)s;
@end
@interface ZoomFlipYDownOver : CCTransitionZoomFlipY 
+(id) transitionWithDuration:(ccTime) t scene:(CCScene*)s;
@end
@interface ZoomFlipAngularLeftOver : CCTransitionZoomFlipAngular 
+(id) transitionWithDuration:(ccTime) t scene:(CCScene*)s;
@end
@interface ZoomFlipAngularRightOver : CCTransitionZoomFlipAngular 
+(id) transitionWithDuration:(ccTime) t scene:(CCScene*)s;
@end
@interface TransitionPageForward : CCTransitionPageTurn
+(id) transitionWithDuration:(ccTime) t scene:(CCScene*)s;
@end
@interface TransitionPageBackward : CCTransitionPageTurn
+(id) transitionWithDuration:(ccTime) t scene:(CCScene*)s;
@end

@implementation FadeWhiteTransition
+(id) transitionWithDuration:(ccTime) t scene:(CCScene*)s {
	return [self transitionWithDuration:t scene:s withColor:ccWHITE];
}
@end

@implementation FlipXLeftOver
+(id) transitionWithDuration:(ccTime) t scene:(CCScene*)s {
	return [self transitionWithDuration:t scene:s orientation:kOrientationLeftOver];
}
@end

@implementation FlipXRightOver
+(id) transitionWithDuration:(ccTime) t scene:(CCScene*)s {
	return [self transitionWithDuration:t scene:s orientation:kOrientationRightOver];
}
@end
@implementation FlipYUpOver
+(id) transitionWithDuration:(ccTime) t scene:(CCScene*)s {
	return [self transitionWithDuration:t scene:s orientation:kOrientationUpOver];
}
@end
@implementation FlipYDownOver
+(id) transitionWithDuration:(ccTime) t scene:(CCScene*)s {
	return [self transitionWithDuration:t scene:s orientation:kOrientationDownOver];
}
@end
@implementation FlipAngularLeftOver
+(id) transitionWithDuration:(ccTime) t scene:(CCScene*)s {
	return [self transitionWithDuration:t scene:s orientation:kOrientationLeftOver];
}
@end
@implementation FlipAngularRightOver
+(id) transitionWithDuration:(ccTime) t scene:(CCScene*)s {
	return [self transitionWithDuration:t scene:s orientation:kOrientationRightOver];
}
@end
@implementation ZoomFlipXLeftOver
+(id) transitionWithDuration:(ccTime) t scene:(CCScene*)s {
	return [self transitionWithDuration:t scene:s orientation:kOrientationLeftOver];
}
@end
@implementation ZoomFlipXRightOver
+(id) transitionWithDuration:(ccTime) t scene:(CCScene*)s {
	return [self transitionWithDuration:t scene:s orientation:kOrientationRightOver];
}
@end
@implementation ZoomFlipYUpOver
+(id) transitionWithDuration:(ccTime) t scene:(CCScene*)s {
	return [self transitionWithDuration:t scene:s orientation:kOrientationUpOver];
}
@end
@implementation ZoomFlipYDownOver
+(id) transitionWithDuration:(ccTime) t scene:(CCScene*)s {
	return [self transitionWithDuration:t scene:s orientation:kOrientationDownOver];
}
@end
@implementation ZoomFlipAngularLeftOver
+(id) transitionWithDuration:(ccTime) t scene:(CCScene*)s {
	return [self transitionWithDuration:t scene:s orientation:kOrientationLeftOver];
}
@end
@implementation ZoomFlipAngularRightOver
+(id) transitionWithDuration:(ccTime) t scene:(CCScene*)s {
	return [self transitionWithDuration:t scene:s orientation:kOrientationRightOver];
}
@end

@implementation TransitionPageForward
+(id) transitionWithDuration:(ccTime) t scene:(CCScene*)s {
	return [self transitionWithDuration:t scene:s backwards:NO];
}
@end

@implementation TransitionPageBackward
+(id) transitionWithDuration:(ccTime) t scene:(CCScene*)s {
	return [self transitionWithDuration:t scene:s backwards:YES];
}
@end

static NSString *transitionTypes[] = {
	@"CCTransitionJumpZoom",
	@"CCTransitionCrossFade",
	@"CCTransitionRadialCCW",
	@"CCTransitionRadialCW",
	@"TransitionPageForward",
	@"TransitionPageBackward",
	@"CCTransitionFadeTR",
	@"CCTransitionFadeBL",
	@"CCTransitionFadeUp",
	@"CCTransitionFadeDown",
	@"CCTransitionTurnOffTiles",
	@"CCTransitionSplitRows",
	@"CCTransitionSplitCols",
	@"CCTransitionFade",
	@"FadeWhiteTransition",
	@"FlipXLeftOver",
	@"FlipXRightOver",
	@"FlipYUpOver",
	@"FlipYDownOver",
	@"FlipAngularLeftOver",
	@"FlipAngularRightOver",
	@"ZoomFlipXLeftOver",
	@"ZoomFlipXRightOver",
	@"ZoomFlipYUpOver",
	@"ZoomFlipYDownOver",
	@"ZoomFlipAngularLeftOver",
	@"ZoomFlipAngularRightOver",
	@"CCTransitionShrinkGrow",
	@"CCTransitionRotoZoom",
	@"CCTransitionMoveInL",
	@"CCTransitionMoveInR",
	@"CCTransitionMoveInT",
	@"CCTransitionMoveInB",
	@"CCTransitionSlideInL",
	@"CCTransitionSlideInR",
	@"CCTransitionSlideInT",
	@"CCTransitionSlideInB",
};
static int numTransitionTypes = 37;

// TransSceneMenu
// Our scene class
@interface TransSceneMenu : CCLayer {
	int currentTransition;
	NSString *name;
}

+(id) sceneWithString:(NSString*)str withCurrentTransition:(int)ct;
-(id) initWithString:(NSString*)str withCurrentTransition:(int)ct;
-(void) prevScene:(id)sender;
-(void) nextScene:(id)sender;
-(void) randomScene:(id)sender;
-(void) loadNewScene;
-(void) quit:(id)sender;

@end

@implementation TransSceneMenu

+(id) sceneWithString:(NSString*)str withCurrentTransition:(int)ct {
	//Create scene
	CCScene *s = [CCScene node];	
	TransSceneMenu *node = [[TransSceneMenu alloc] initWithString:str withCurrentTransition:ct];
	[s addChild:node z:0 tag:0];
	return s;
}

-(id) initWithString:(NSString*)str withCurrentTransition:(int)ct {
	if( (self=[super init] )) {
		name = str;
		currentTransition = ct;
		
		//Font size/name
		[CCMenuItemFont setFontSize:30];
		[CCMenuItemFont setFontName:@"Marker Felt"];
		
		//Random background color
		CCSprite *bg = [CCSprite spriteWithFile:@"blank.png"];
		bg.position = ccp(240,160);
		[bg setTextureRect:CGRectMake(0,0,480,320)];
		[bg setColor:ccc3(arc4random()%150,arc4random()%150,arc4random()%150)];
		[self addChild:bg];
		
		CCLabelBMFont *message = [CCLabelBMFont labelWithString:name fntFile:@"eurostile_30.fnt"];
		message.position = ccp(160,270);
		message.scale = 0.5f;
		[message setColor:ccc3(255,255,255)];
		[self addChild:message z:10];
		
		CCMenuItemFont *prevItem = [CCMenuItemFont itemFromString:@"Next     " target:self selector:@selector(prevScene:)];
		CCMenuItemFont *quitItem = [CCMenuItemFont itemFromString:@"   Quit   " target:self selector:@selector(quit:)];
		CCMenuItemFont *randomItem = [CCMenuItemFont itemFromString:@"   Random   " target:self selector:@selector(randomScene:)];
		CCMenuItemFont *nextItem = [CCMenuItemFont itemFromString:@"     Prev" target:self selector:@selector(nextScene:)];
		
		//Add menu items
		CCMenu *menu = [CCMenu menuWithItems: nextItem, randomItem, quitItem, prevItem, nil];
		[menu alignItemsHorizontally];
		[self addChild:menu];	
	}
	return self;
}

-(void) prevScene:(id)sender {
	currentTransition--;
	if(currentTransition < 0){
		currentTransition = numTransitionTypes-1;
	}
	[self loadNewScene];
}

-(void) nextScene:(id)sender {
	currentTransition++;
	if(currentTransition >= numTransitionTypes){
		currentTransition = 0;
	}
	[self loadNewScene];
}

-(void) randomScene:(id)sender {
	currentTransition = arc4random()%numTransitionTypes;
	[self loadNewScene];
}

-(void) loadNewScene {
	[[CCDirector sharedDirector] popScene];
	NSString *className = [NSString stringWithFormat:@"%@",transitionTypes[currentTransition]];
	Class clazz = NSClassFromString (className);
	[[CCDirector sharedDirector] pushScene: [clazz transitionWithDuration:1.2f scene:[TransSceneMenu sceneWithString:className withCurrentTransition:currentTransition]]]; 
}

-(void) quit:(id)sender {
	[[CCDirector sharedDirector] popScene];
}


@end


//Our Base Recipe
@interface Ch5_SceneTransitions : Recipe
{
}

-(CCLayer*) runRecipe;
-(void) viewTransitions:(id)sender;

@end

@implementation Ch5_SceneTransitions

-(CCLayer*) runRecipe {
	[super runRecipe];
    
	CCMenuItemFont *viewTransitions = [CCMenuItemFont itemFromString:@"View Transitions" target:self selector:@selector(viewTransitions:)];

	CCMenu *menu = [CCMenu menuWithItems: viewTransitions, nil];
	[menu alignItemsVertically];
	[self addChild:menu];

	return self;
}

-(void) viewTransitions:(id)sender {
	[[CCDirector sharedDirector] pushScene:[TransSceneMenu sceneWithString:@"" withCurrentTransition:0]];
}

@end
