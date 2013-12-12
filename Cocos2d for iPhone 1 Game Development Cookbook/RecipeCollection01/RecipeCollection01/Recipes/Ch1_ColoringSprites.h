#import "cocos2d.h"
#import "Recipe.h"

enum
{
	TAG_FADE_TO_BLACK = 0,
	TAG_FADE_TO_WHITE = 1,
	TAG_FADE_SPRITE = 2,
	TAG_MOUNTAIN_BG = 3,
	TAG_SUN_BG = 4,
	TAG_GOOD_SAMURAI = 5,
	TAG_BAD_SAMURAI = 6,
	TAG_GROUND_GRADIENT = 7,
	TAG_RED_GRADIENT = 8
};

//Interface
@interface Ch1_ColoringSprites : Recipe
{
}

-(CCLayer*) runRecipe;
-(void) initButtons;
-(void) fadeToBlackCallback:(id)sender;
-(void) fadeToWhiteCallback:(id)sender;
-(void) drawColoredSpriteAt:(CGPoint)position withRect:(CGRect)rect withColor:(ccColor3B)color withZ:(float)z;
-(void) glowAt:(CGPoint)position withScale:(CGSize)size withColor:(ccColor3B)color withRotation:(float)rotation withSprite:(CCSprite*)sprite;

@end



//Implementation
@implementation Ch1_ColoringSprites

-(CCLayer*) runRecipe
{
	[self initButtons];
		
	//The Fade Scene Sprite
	CCSprite *fadeSprite = [CCSprite spriteWithFile:@"blank.png"];
	[fadeSprite setOpacity:0];
	[fadeSprite setPosition:ccp(240,160)];
	[fadeSprite setTextureRect:CGRectMake(0,0,480,320)];
	[self addChild:fadeSprite z:3 tag:TAG_FADE_SPRITE];
	
	//Draw the sky using blank.png
	[self drawColoredSpriteAt:ccp(240,190) withRect:CGRectMake(0,0,480,260) withColor:ccc3(150,200,200) withZ:0];
	
	//Draw the sun
	CCSprite *sun = [CCSprite spriteWithFile:@"fire.png"];
	[sun setPosition:ccp(50,230)];
	[sun setScale:3.0f];
	[sun setColor:ccc3(255,255,0)];
	[self addChild:sun z:0 tag:TAG_SUN_BG];	
	
	//Draw some mountains in the background
	CCSprite *mountains = [CCSprite spriteWithFile:@"mountains.png"];
	[mountains setPosition:ccp(250,200)];
	[mountains setScale:0.6f];
	[self addChild:mountains z:0 tag:TAG_MOUNTAIN_BG];
	
	//Add a gradient below the mountains
	CCLayerGradient *groundGradient = [CCLayerGradient layerWithColor:ccc4(61,33,62,255) fadingTo:ccc4(65,89,54,255) alongVector:ccp(0,-1)];
	[groundGradient setContentSize:CGSizeMake(480,100)];
	[groundGradient setPosition:ccp(0,50)];
	[self addChild:groundGradient z:0 tag:TAG_GROUND_GRADIENT];
	
	//Add a sinister red glow gradient behind the evil samurai
	CCLayerGradient *redGradient = [CCLayerGradient layerWithColor:ccc4(0,0,0,0) fadingTo:ccc4(255,0,0,100) alongVector:ccp(1,0)];
	[redGradient setContentSize:CGSizeMake(200,200)];
	[redGradient setPosition:ccp(280,60)];
	[self addChild:redGradient z:2 tag:TAG_RED_GRADIENT];
	
	//Draw dramatic movie bars
	[self drawColoredSpriteAt:ccp(240,290) withRect:CGRectMake(0,0,480,60) withColor:ccc3(0,0,0) withZ:2];
	[self drawColoredSpriteAt:ccp(240,30) withRect:CGRectMake(0,0,480,60) withColor:ccc3(0,0,0) withZ:2];

	//Draw the good samurai
	CCSprite *goodSamurai = [CCSprite spriteWithFile:@"samurai_good.png"];
	goodSamurai.anchorPoint = ccp(0.5f,0);
	[goodSamurai setPosition:ccp(100,70)];
	[goodSamurai setScale:0.5f];
	[self addChild:goodSamurai z:1 tag:TAG_GOOD_SAMURAI];
	
	//Draw the evil samurai
	CCSprite *evilSamurai = [CCSprite spriteWithFile:@"samurai_evil.png"];
	evilSamurai.anchorPoint = ccp(0.5f,0);
	[evilSamurai setPosition:ccp(370,70)];
	[evilSamurai setFlipX:YES];
	[evilSamurai setScale:0.5f];
	[self addChild:evilSamurai z:1 tag:TAG_BAD_SAMURAI];
	
	// Make the swords glow
	[self glowAt:ccp(230,280) withScale:CGSizeMake(3.0f, 11.0f) withColor:ccc3(0,230,255) withRotation:45.0f withSprite:goodSamurai];
	[self glowAt:ccp(70,280) withScale:CGSizeMake(3.0f, 11.0f) withColor:ccc3(255,200,2) withRotation:-45.0f withSprite:evilSamurai];
	
	return self;	
}

-(void) initButtons
{
	[CCMenuItemFont setFontSize:16];

	//'Fade to Black' button
	CCMenuItemFont* fadeToBlack = [CCMenuItemFont itemFromString:@"FADE TO BLACK" target:self selector:@selector(fadeToBlackCallback:)];
	CCMenu *fadeToBlackMenu = [CCMenu menuWithItems:fadeToBlack, nil];
    fadeToBlackMenu.position = ccp( 120 , 20 );
    [self addChild:fadeToBlackMenu z:4 tag:TAG_FADE_TO_BLACK];
	
	//'Fade to White' button
	CCMenuItemFont* fadeToWhite = [CCMenuItemFont itemFromString:@"FADE TO WHITE" target:self selector:@selector(fadeToWhiteCallback:)];
	CCMenu *fadeToWhiteMenu = [CCMenu menuWithItems:fadeToWhite, nil];
    fadeToWhiteMenu.position = ccp( 240 , 20 );
    [self addChild:fadeToWhiteMenu z:4 tag:TAG_FADE_TO_WHITE];	
}

/* Fade the scene to black */
-(void) fadeToBlackCallback:(id)sender
{
	CCSprite *fadeSprite = [self getChildByTag:TAG_FADE_SPRITE];
	[fadeSprite stopAllActions];
	[fadeSprite setColor:ccc3(0,0,0)];
	[fadeSprite setOpacity:0.0f];
	[fadeSprite runAction: 
		[CCSequence actions:[CCFadeIn actionWithDuration:2.0f], [CCFadeOut actionWithDuration:2.0f], nil] ];	
}

/* Fade the scene to white */
-(void) fadeToWhiteCallback:(id)sender
{
	CCSprite *fadeSprite = [self getChildByTag:TAG_FADE_SPRITE];
	[fadeSprite stopAllActions];
	[fadeSprite setColor:ccc3(255,255,255)];
	[fadeSprite setOpacity:0.0f];
	[fadeSprite runAction: 
		[CCSequence actions:[CCFadeIn actionWithDuration:2.0f], [CCFadeOut actionWithDuration:2.0f], nil] ];	
}

/* Draw a colored sprite */
-(void) drawColoredSpriteAt:(CGPoint)position withRect:(CGRect)rect withColor:(ccColor3B)color withZ:(float)z
{
	CCSprite *sprite = [CCSprite spriteWithFile:@"blank.png"];
	[sprite setPosition:position];
	[sprite setTextureRect:rect];
	[sprite setColor:color];
	[self addChild:sprite];
	
	//Set Z Order
	[self reorderChild:sprite z:z];
}

/* Create a glow effect */
-(void) glowAt:(CGPoint)position withScale:(CGSize)size withColor:(ccColor3B)color withRotation:(float)rotation withSprite:(CCSprite*)sprite {
	CCSprite *glowSprite = [CCSprite spriteWithFile:@"fire.png"];
	[glowSprite setColor:color];
	[glowSprite setPosition:position];
	[glowSprite setRotation:rotation];
	[glowSprite setBlendFunc: (ccBlendFunc) { GL_ONE, GL_ONE }];
	[glowSprite runAction: [CCRepeatForever actionWithAction:
		[CCSequence actions:[CCScaleTo actionWithDuration:0.9f scaleX:size.width scaleY:size.height], [CCScaleTo actionWithDuration:0.9f scaleX:size.width*0.75f scaleY:size.height*0.75f], nil] ] ];	
	[glowSprite runAction: [CCRepeatForever actionWithAction:
		[CCSequence actions:[CCFadeTo actionWithDuration:0.9f opacity:150], [CCFadeTo actionWithDuration:0.9f opacity:255], nil] ] ];	
		
	[sprite addChild:glowSprite];
}

@end
