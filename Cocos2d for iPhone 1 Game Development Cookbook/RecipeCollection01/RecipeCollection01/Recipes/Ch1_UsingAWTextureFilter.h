#import "cocos2d.h"
#import "Recipe.h"
#import "CCTexture2DMutable.h"
#import "AWTextureFilter.h"

//Interface
static NSString *awEffects[] = {
	@"GaussianBlur",
	@"SelectiveBlur",
	@"Shadow",
};
static int numAWEffects = 3;

@interface Ch1_UsingAWTextureFilter : Recipe
{
	int currentEffect;
	CGPoint effectPosition;
}

-(CCLayer*) runRecipe;
-(void) prevEffectCallback;
-(void) reloadEffectCallback;
-(void) nextEffectCallback;
-(void) runCurrentEffect;

-(void) showBlurEffect;
-(void) showSelectiveBlurEffect;
-(void) showShadowEffect;

-(void)ccTouchesBegan:(NSSet *)touches withEvent:(UIEvent *)event;
-(void)ccTouchesMoved:(NSSet *)touches withEvent:(UIEvent *)event;

@end



//Implementation

@implementation Ch1_UsingAWTextureFilter

-(CCLayer*) runRecipe {
	self.isTouchEnabled = YES;

	CGSize winSize = [[CCDirector sharedDirector] winSize];
	effectPosition = ccp(240, 160);
	
	[CCMenuItemFont setFontSize:16];
	
	CCMenuItemFont* prev = [CCMenuItemFont itemFromString:@"<PREV" target:self selector:@selector(prevEffectCallback)];
	CCMenu *prevMenu = [CCMenu menuWithItems:prev, nil];
    prevMenu.position = ccp( 160 , 20 );
    [self addChild:prevMenu z:1 tag:TAG_PREV_EFFECT];	
	
	CCMenuItemFont* reload = [CCMenuItemFont itemFromString:@"<RELOAD>" target:self selector:@selector(reloadEffectCallback)];
	CCMenu *reloadMenu = [CCMenu menuWithItems:reload, nil];
    reloadMenu.position = ccp( 240 , 20 );
    [self addChild:reloadMenu z:1 tag:TAG_RELOAD_EFFECT];		
	
	CCMenuItemFont* next = [CCMenuItemFont itemFromString:@"NEXT>" target:self selector:@selector(nextEffectCallback)];
	CCMenu *nextMenu = [CCMenu menuWithItems:next, nil];
    nextMenu.position = ccp( 320 , 20 );
    [self addChild:nextMenu z:1 tag:TAG_NEXT_EFFECT];
	
	currentEffect = 0;
	
	[self runCurrentEffect];
	
	return self;
}


-(void) prevEffectCallback {
	currentEffect -= 1;
	if(currentEffect < 0){
		currentEffect = numAWEffects-1;
	}
	[self removeChildByTag:TAG_EFFECT_NAME cleanup:YES];
	[self runCurrentEffect];
}
-(void) reloadEffectCallback {
	[self removeChildByTag:TAG_EFFECT_NAME cleanup:YES];
	[self runCurrentEffect];	
}
-(void) nextEffectCallback {
	currentEffect += 1;
	if(currentEffect >= numAWEffects){
		currentEffect = 0;
	}
	[self removeChildByTag:TAG_EFFECT_NAME cleanup:YES];
	[self runCurrentEffect];
}
-(void) runCurrentEffect {
	[self removeChildByTag:TAG_EFFECT_NODE cleanup:YES];
	[self removeChildByTag:TAG_EFFECT_NAME cleanup:YES];
	
	CCLabelTTF* name = [CCLabelTTF labelWithString:awEffects[currentEffect] fontName:@"Arial" fontSize:20];
	name.position = ccp(360,300);
	[self addChild:name z:0 tag:TAG_EFFECT_NAME];
	
	NSString *method = [NSString stringWithFormat:@"show%@Effect",awEffects[currentEffect]];
	[self performSelector:NSSelectorFromString(method)];
}

-(void) showGaussianBlurEffect {
	//Pixel Format RGBA8888 is required for this effect
	[CCTexture2D setDefaultAlphaPixelFormat:kCCTexture2DPixelFormat_RGBA8888];

	//Create the mutable texture
	CCTexture2DMutable *mutableTexture = [[[CCTexture2DMutable alloc] initWithImage:[UIImage imageNamed:@"cocos2d_beginner.png"]] autorelease];

	//Copy the mutable texture as a non mutable texture
	CCTexture2D *noMutableTexture = [[[CCTexture2D alloc] initWithImage:[UIImage imageNamed:@"cocos2d_beginner.png"]] autorelease];

	//Apply blur to the mutable texture
	[AWTextureFilter blur:mutableTexture radius:3];

	CGSize winSize = [[CCDirector sharedDirector] winSize];

	//Create sprites to show the textures
	CCSprite *original = [CCSprite spriteWithTexture:noMutableTexture];
	[original setPosition:ccp(winSize.width/2-original.contentSize.width/2-1, winSize.height/2)];

	CCSprite *blur = [CCSprite spriteWithTexture:mutableTexture];
	[blur setPosition:ccp(winSize.width/2+blur.contentSize.width/2+1, winSize.height/2)];

	CCNode *node = [[CCNode alloc] init];
	[node addChild:original z:0 tag:0];
	[node addChild:blur z:0 tag:1];
	
	[self addChild:node z:-1 tag:TAG_EFFECT_NODE];	
}
-(void) showSelectiveBlurEffect {
	CGSize winSize = [[CCDirector sharedDirector] winSize];	

	[CCTexture2D setDefaultAlphaPixelFormat:kCCTexture2DPixelFormat_RGBA8888];
	
	//Create mutable texture
	CCTexture2DMutable *mutableTexture = [[[CCTexture2DMutable alloc] initWithImage:[UIImage imageNamed:@"cocos2d_beginner.png"]] autorelease];
	
	//Apply blur to the mutable texture
	[AWTextureFilter blur:mutableTexture radius:8 rect:CGRectMake(effectPosition.x-200, (winSize.height-effectPosition.y)-75, 150, 150)];
		
	//Create sprites to show the textures
	CCSprite *blur = [CCSprite spriteWithTexture:mutableTexture];
	[blur setPosition:ccp(winSize.width/2, winSize.height/2)];
	
	[self addChild:blur z:-1 tag:TAG_EFFECT_NODE];
}
-(void) showShadowEffect {
	CGSize winSize = [[CCDirector sharedDirector] winSize];

	//Create a background so we can see the shadow
	CCLayerColor *background = [CCLayerColor layerWithColor:ccc4(200, 100, 100, 255) width:300 height:50];
	[background setIsRelativeAnchorPoint:YES];
	[background setAnchorPoint:ccp(0.5f, 0.5f)];
	[background setPosition:ccp(winSize.width/2, winSize.height/2)];
		
	//Create a sprite for the font label
	CCSprite* labelSprite = [CCSprite node];
	[labelSprite setPosition:ccp(winSize.width/2, winSize.height/2)];
	
	//Create a sprite for the shadow
	CCSprite* shadowSprite = [CCSprite node];
	[shadowSprite setPosition:ccp(winSize.width/2+1, winSize.height/2+1)];
	
	//Color it black
	[shadowSprite setColor:ccBLACK];
	
	//Add does to scene
	CCNode* node = [[CCNode alloc] init];
	[node addChild:background z:-1];
	[node addChild:shadowSprite z:0];
	[node addChild:labelSprite z:1];
	
	[self addChild:node z:-1 tag:TAG_EFFECT_NODE];
		
	//Create a mutable texture with a string (a feature of CCTexture2DMutable)
	CCTexture2DMutable *shadowTexture = [[[CCTexture2DMutable alloc] initWithString:@"Shadowed Text" fontName:@"Arial" fontSize:28] autorelease];
	
	//Copy the mutable texture as non mutable texture
	CCTexture2D *labelTexture = [[shadowTexture copyMutable:NO] autorelease];
	
	//Set the label texture
	[labelSprite setTexture:labelTexture];
	[labelSprite setTextureRect:CGRectMake(0, 0, shadowTexture.contentSize.width, shadowTexture.contentSize.height)];
	
	//Apply blur to the shadow texture
	[AWTextureFilter blur:shadowTexture radius:4];
	
	//Set the shadow texture
	[shadowSprite setTexture:shadowTexture];
	[shadowSprite setTextureRect:CGRectMake(0, 0, shadowTexture.contentSize.width, shadowTexture.contentSize.height)];
}

-(void)ccTouchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {
	UITouch *touch = [touches anyObject];
    CGPoint point = [touch locationInView: [touch view]];
	point = [[CCDirector sharedDirector] convertToGL: point];
	
	effectPosition = point;
	[self reloadEffectCallback];
}


-(void)ccTouchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {
	UITouch *touch = [touches anyObject];
    CGPoint point = [touch locationInView: [touch view]];
	point = [[CCDirector sharedDirector] convertToGL: point];

	effectPosition = point;
	[self reloadEffectCallback];
}

@end
