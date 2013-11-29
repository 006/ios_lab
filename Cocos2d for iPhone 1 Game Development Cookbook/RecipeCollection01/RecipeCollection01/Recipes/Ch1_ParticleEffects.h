#import "cocos2d.h"
#import "Recipe.h"
#import "ParticleWaterfall.h"
#import "ParticleWaterSprinkler.h"

//TODO - Add a custom particle effect.

//Interface

static NSString *particleEffectMethods[] = {
	@"ParticleExplosion",
	@"ParticleFire",
	@"ParticleFireworks",
	@"ParticleFlower",
	@"ParticleGalaxy",
	@"ParticleMeteor",
	@"ParticleRain",
	@"ParticleSmoke",
	@"ParticleSnow",
	@"ParticleSpiral",
	@"ParticleSun",
	@"ParticleWaterfall",
	@"ParticleWaterSprinkler",
};
static int numParticleEffectMethods = 13;

@interface Ch1_ParticleEffects : Recipe
{
	int currentEffect;
}

-(CCLayer*) runRecipe;
-(void) prevEffectCallback:(id)sender;
-(void) reloadEffectCallback:(id)sender;
-(void) nextEffectCallback:(id)sender;
-(void) runCurrentEffect;
-(CCParticleExplosion*) getParticleExplosionEffect;
-(CCParticleFire*) getParticleFireEffect;
-(CCParticleFireworks*) getParticleFireworksEffect;
-(CCParticleFlower*) getParticleFlowerEffect;
-(CCParticleGalaxy*) getParticleGalaxyEffect;
-(CCParticleMeteor*) getParticleMeteorEffect;
-(CCParticleRain*) getParticleRainEffect;
-(CCParticleSmoke*) getParticleSmokeEffect;
-(CCParticleSnow*) getParticleSnowEffect;
-(CCParticleSpiral*) getParticleSpiralEffect;
-(CCParticleSun*) getParticleSunEffect;
-(ParticleWaterfall*) getParticleWaterfallEffect;
-(ParticleWaterSprinkler*) getParticleWaterSprinklerEffect;

@end



//Implementation

@implementation Ch1_ParticleEffects

-(CCLayer*) runRecipe {
	//TO DO - Replace this texture with one that I've drawn myself. Also, one that better illustrates the effects.
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
		currentEffect = numParticleEffectMethods-1;
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
	if(currentEffect >= numParticleEffectMethods){
		currentEffect = 0;
	}
	[self removeChildByTag:TAG_EFFECT_NAME cleanup:YES];
	[self runCurrentEffect];
}
-(void) runCurrentEffect {
	CCNode *node = [self getChildByTag:TAG_EFFECT_NODE];
	[node stopAllActions];
	[self removeChild:node cleanup:YES];
	
	NSString *method = [NSString stringWithFormat:@"get%@Effect",particleEffectMethods[currentEffect]];
	node = [self performSelector:NSSelectorFromString(method)];
	[self addChild:node z:1 tag:TAG_EFFECT_NODE];
	
	[node setPosition:ccp(240,160)];
	if( [particleEffectMethods[currentEffect] isEqualToString:@"ParticleWaterfall"] ||
		[particleEffectMethods[currentEffect] isEqualToString:@"ParticleRain"] ||
		[particleEffectMethods[currentEffect] isEqualToString:@"ParticleSnow"] ){
	  [node setPosition:ccp(240,280)];
	}
		
	CCLabelTTF* name = [CCLabelTTF labelWithString:particleEffectMethods[currentEffect] fontName:@"Arial" fontSize:20];
	name.position = ccp(360,300);
	[self addChild:name z:0 tag:TAG_EFFECT_NAME];
}
-(CCParticleExplosion*) getParticleExplosionEffect {
	return [CCParticleExplosion node];
}
-(CCParticleFire*) getParticleFireEffect {
	return [CCParticleFire node];
}
-(CCParticleFireworks*) getParticleFireworksEffect {
	return [CCParticleFireworks node];
}
-(CCParticleFlower*) getParticleFlowerEffect {
	return [CCParticleFlower node];
}
-(CCParticleGalaxy*) getParticleGalaxyEffect {
	return [CCParticleGalaxy node];
}
-(CCParticleMeteor*) getParticleMeteorEffect {
	return [CCParticleMeteor node];
}
-(CCParticleRain*) getParticleRainEffect {
	return [CCParticleRain node];
}
-(CCParticleSmoke*) getParticleSmokeEffect {
	return [CCParticleSmoke node];
}
-(CCParticleSnow*) getParticleSnowEffect {
	return [CCParticleSnow node];
}
-(CCParticleSpiral*) getParticleSpiralEffect {
	return [CCParticleSpiral node];
}
-(CCParticleSun*) getParticleSunEffect {
	return [CCParticleSun node];
}
-(ParticleWaterfall*) getParticleWaterfallEffect {
	return [ParticleWaterfall node];
}
-(ParticleWaterSprinkler*) getParticleWaterSprinklerEffect {
	return [ParticleWaterSprinkler node];
}

@end
