@interface ParticleWaterfall : CCParticleRain {}

-(id)init;
-(id)initWithTotalParticles:(int)p;

@end


@implementation ParticleWaterfall

-(id)init {
	return [self initWithTotalParticles:400];
}
-(id)initWithTotalParticles:(int)p {
	if(self != [super initWithTotalParticles: p])
		return nil;

	// angle
	angle = 270;
	angleVar = 12;

	// emitter position
	self.position = ccp(160, 60);
	posVar = ccp(16, 4);

	// life of particles
	life = 2;
	lifeVar = 0.25f;

	// speed of particles
	self.speed = 100;
	self.speedVar = 20;
	self.gravity = ccp(self.gravity.x, -5);

	// size of particles
	startSize = 35.0f;
	endSize = 100.0f;

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