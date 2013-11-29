#import "cocos2d.h"
#import "Recipe.h"

//Interface

enum {
	TAG_SPRITE_TO_EFFECT = 0,
	TAG_PREV_EFFECT = 1,
	TAG_RELOAD_EFFECT = 2,
	TAG_NEXT_EFFECT = 3,
	TAG_EFFECT_NAME = 4
};

static NSString *effectMethods[] = {
	@"Shaky3D",
	@"Waves3D",
	@"FlipX3D",
	@"FlipY3D",
	@"Lens3D",
	@"Ripple3D",
	@"Liquid",
	@"Waves",
	@"Twirl",
	@"ShakyTiles3D",
	@"ShatteredTiles3D",
	@"ShuffleTiles",
	@"FadeOutTRTiles",
	@"FadeOutBLTiles",
	@"FadeOutUpTiles",
	@"FadeOutDownTiles",
	@"TurnOffTiles",
	@"WavesTiles3D",
	@"JumpTiles3D",
	@"SplitRows",
	@"SplitCols",
	@"PageTurn3D"
};
static int numEffectMethods = 22;

@interface Ch1_GridEffects : Recipe
{
	int currentEffect;
}

-(CCLayer*) runRecipe;
-(void) prevEffectCallback:(id)sender;
-(void) reloadEffectCallback:(id)sender;
-(void) nextEffectCallback:(id)sender;
-(void) runCurrentEffect;
-(CCShaky3D*) getShaky3DEffect;
-(CCWaves3D*) getWaves3DEffect;
-(CCFlipX3D*) getFlipX3DEffect;
-(CCFlipY3D*) getFlipY3DEffect;
-(CCLens3D*) getLens3DEffect;
-(CCRipple3D*) getRipple3DEffect;
-(CCLiquid*) getLiquidEffect;
-(CCWaves*) getWavesEffect;
-(CCTwirl*) getTwirlEffect;
-(CCShakyTiles3D*) getShakyTiles3DEffect;
-(CCShatteredTiles3D*) getShatteredTiles3DEffect;
-(CCShuffleTiles*) getShuffleTilesEffect;
-(CCFadeOutTRTiles*) getFadeOutTRTilesEffect;
-(CCFadeOutBLTiles*) getFadeOutBLTilesEffect;
-(CCFadeOutUpTiles*) getFadeOutUpTilesEffect;
-(CCFadeOutDownTiles*) getFadeOutDownTilesEffect;
-(CCTurnOffTiles*) getTurnOffTilesEffect;
-(CCWavesTiles3D*) getWavesTiles3DEffect;
-(CCJumpTiles3D*) getJumpTiles3DEffect;
-(CCSplitRows*) getSplitRowsEffect;
-(CCSplitCols*) getSplitColsEffect;
-(CCPageTurn3D*) getPageTurn3DEffect;

@end



//Implementation

@implementation Ch1_GridEffects

-(CCLayer*) runRecipe {
	//TO DO - Replace this texture with one that I've drawn myself. Also, one that better illustrates the effects.

	CCSprite *sprite = [CCSprite spriteWithFile:@"colorable_sprite.png"];
	[sprite setPosition:ccp(240,150)];
	[sprite setScale:2.0f];
	[self addChild:sprite z:0 tag:TAG_SPRITE_TO_EFFECT];	

	[CCMenuItemFont setFontSize:16];

	CCMenuItemFont* prev = [CCMenuItemFont itemFromString:@"<PREV" target:self selector:@selector(prevEffectCallback:)];
	CCMenu *prevMenu = [CCMenu menuWithItems:prev, nil];
    prevMenu.position = ccp( 160 , 20 );
    [self addChild:prevMenu z:0 tag:TAG_PREV_EFFECT];	
	
	CCMenuItemFont* reload = [CCMenuItemFont itemFromString:@"<RELOAD>" target:self selector:@selector(reloadEffectCallback:)];
	CCMenu *reloadMenu = [CCMenu menuWithItems:reload, nil];
    reloadMenu.position = ccp( 240 , 20 );
    [self addChild:reloadMenu z:0 tag:TAG_RELOAD_EFFECT];		
	
	CCMenuItemFont* next = [CCMenuItemFont itemFromString:@"NEXT>" target:self selector:@selector(nextEffectCallback:)];
	CCMenu *nextMenu = [CCMenu menuWithItems:next, nil];
    nextMenu.position = ccp( 320 , 20 );
    [self addChild:nextMenu z:0 tag:TAG_NEXT_EFFECT];
	
	currentEffect = 0;
	
	[self runCurrentEffect];

	return self;
}


-(void) prevEffectCallback:(id)sender {
	currentEffect -= 1;
	if(currentEffect < 0){
		currentEffect = numEffectMethods-1;
	}
	[self removeChildByTag:TAG_EFFECT_NAME cleanup:YES];
	[self runCurrentEffect];
}
-(void) reloadEffectCallback:(id)sender {
	[self removeChildByTag:TAG_EFFECT_NAME cleanup:YES];
	[self runCurrentEffect];	
}
-(void) nextEffectCallback:(id)sender {
	currentEffect += 1;
	if(currentEffect >= numEffectMethods){
		currentEffect = 0;
	}
	[self removeChildByTag:TAG_EFFECT_NAME cleanup:YES];
	[self runCurrentEffect];
}
-(void) runCurrentEffect {
	CCNode *node = [self getChildByTag:TAG_SPRITE_TO_EFFECT];
	[node stopAllActions];

	NSString *method = [NSString stringWithFormat:@"get%@Effect",effectMethods[currentEffect]];
	[node runAction:[self performSelector:NSSelectorFromString(method)]];
	
	CCLabelTTF* name = [CCLabelTTF labelWithString:effectMethods[currentEffect] fontName:@"Arial" fontSize:20];
	name.position = ccp(360,300);
	[self addChild:name z:0 tag:TAG_EFFECT_NAME];
}

-(CCShaky3D*) getShaky3DEffect {
	return [CCShaky3D actionWithRange:5 shakeZ:YES grid:ccg(15,10) duration:10];
}
-(CCWaves3D*) getWaves3DEffect {
	return [CCWaves3D actionWithWaves:5 amplitude:40 grid:ccg(15,10) duration:10];
}
-(CCFlipX3D*) getFlipX3DEffect {
	id flipx  = [CCFlipX3D actionWithDuration:1];
	id flipx_back = [flipx reverse];
	id delay = [CCDelayTime actionWithDuration:0];
	
	return [CCSequence actions: flipx, delay, flipx_back, nil];	
}
-(CCFlipY3D*) getFlipY3DEffect {
	id flipy = [CCFlipY3D actionWithDuration:1];
	id flipy_back = [flipy reverse];
	id delay = [CCDelayTime actionWithDuration:0];
	
	return [CCSequence actions: flipy, delay, flipy_back, nil];
}
-(CCLens3D*) getLens3DEffect {
	CGSize size = [[CCDirector sharedDirector] winSize];
	return [CCLens3D actionWithPosition:ccp(size.width/2,size.height/2) radius:240 grid:ccg(15,10) duration:10];
}
-(CCRipple3D*) getRipple3DEffect {
	CGSize size = [[CCDirector sharedDirector] winSize];
	return [CCRipple3D actionWithPosition:ccp(size.width/2,size.height/2) radius:240 waves:4 amplitude:160 grid:ccg(32,24) duration:10];
}
-(CCLiquid*) getLiquidEffect {
	return [CCLiquid actionWithWaves:4 amplitude:20 grid:ccg(16,12) duration:10];
}
-(CCWaves*) getWavesEffect {
	return [CCWaves actionWithWaves:4 amplitude:20 horizontal:YES vertical:YES grid:ccg(16,12) duration:10];
}
-(CCTwirl*) getTwirlEffect {
	CGSize size = [[CCDirector sharedDirector] winSize];
	return [CCTwirl actionWithPosition:ccp(size.width/2, size.height/2) twirls:1 amplitude:2.5f grid:ccg(12,8) duration:10];
}
-(CCShakyTiles3D*) getShakyTiles3DEffect {
	return [CCShakyTiles3D actionWithRange:5 shakeZ:YES grid:ccg(16,12) duration:10];
}
-(CCShatteredTiles3D*) getShatteredTiles3DEffect {
	return [CCShatteredTiles3D actionWithRange:5 shatterZ:YES grid:ccg(16,12) duration:10];
}
-(CCShuffleTiles*) getShuffleTilesEffect {
	id shuffle = [CCShuffleTiles actionWithSeed:25 grid:ccg(16,12) duration:5];
	id shuffle_back = [shuffle reverse];
	id delay = [CCDelayTime actionWithDuration:2];

	return [CCSequence actions: shuffle, delay, shuffle_back, nil];
}
-(CCFadeOutTRTiles*) getFadeOutTRTilesEffect {
	id fadeout = [CCFadeOutTRTiles actionWithSize:ccg(16,12) duration:2];
	id back = [fadeout reverse];
	id delay = [CCDelayTime actionWithDuration:0.5f];

	return [CCSequence actions: fadeout, delay, back, nil];
}
-(CCFadeOutBLTiles*) getFadeOutBLTilesEffect {
	id fadeout = [CCFadeOutBLTiles actionWithSize:ccg(16,12) duration:2];
	id back = [fadeout reverse];
	id delay = [CCDelayTime actionWithDuration:0.5f];
	
	return [CCSequence actions: fadeout, delay, back, nil];
}
-(CCFadeOutUpTiles*) getFadeOutUpTilesEffect {
	id fadeout = [CCFadeOutUpTiles actionWithSize:ccg(16,12) duration:2];
	id back = [fadeout reverse];
	id delay = [CCDelayTime actionWithDuration:0.5f];
	
	return [CCSequence actions: fadeout, delay, back, nil];
}
-(CCFadeOutDownTiles*) getFadeOutDownTilesEffect {
	id fadeout = [CCFadeOutDownTiles actionWithSize:ccg(16,12) duration:2];
	id back = [fadeout reverse];
	id delay = [CCDelayTime actionWithDuration:0.5f];
	
	return [CCSequence actions: fadeout, delay, back, nil];
}
-(CCTurnOffTiles*) getTurnOffTilesEffect {
	id action = [CCTurnOffTiles actionWithSeed:25 grid:ccg(48,32) duration:2];
	id back = [action reverse];
	id delay = [CCDelayTime actionWithDuration:0.5f];
	
	return [CCSequence actions: action, delay, back, nil];
}

-(CCWavesTiles3D*) getWavesTiles3DEffect {
	return [CCWavesTiles3D actionWithWaves:4 amplitude:120 grid:ccg(15,10) duration:10];
}
-(CCJumpTiles3D*) getJumpTiles3DEffect {
	return [CCJumpTiles3D actionWithJumps:2 amplitude:30 grid:ccg(15,10) duration:10];
}
-(CCSplitRows*) getSplitRowsEffect {
	return [CCSplitRows actionWithRows:9 duration:10];
}
-(CCSplitCols*) getSplitColsEffect {
	return [CCSplitCols actionWithCols:9 duration:10];
}
-(CCPageTurn3D*) getPageTurn3DEffect {
	return [CCPageTurn3D actionWithSize:ccg(15,10) duration:2];
}

@end
