@interface ParticleWaterSprinkler : CCParticleFireworks {}

-(id)init;
-(id)initWithTotalParticles:(int)p;

@end


@implementation ParticleWaterSprinkler

-(id)init {
	return [self initWithTotalParticles:300];
}
-(id)initWithTotalParticles:(int)p {
	if(self != [super initWithTotalParticles: p])
		return nil;

	// color of particles
	startColor.r = 0.4f;
	startColor.g = 0.4f;
	startColor.b = 1.0f;
	startColor.a = 0.6f;
	startColorVar.r = 0.0f;
	startColorVar.g = 0.0f;
	startColorVar.b = 0.0f;
	startColorVar.a = 0.0f;
	endColor.r = 0.5f;
	endColor.g = 0.5f;
	endColor.b = 0.5f;
	endColor.a = 0.0f;
	endColorVar.r = 0.0f;
	endColorVar.g = 0.0f;
	endColorVar.b = 0.0f;
	endColorVar.a = 0.0f;

	// additive
	self.blendAdditive = NO;

	return self;
}

@end