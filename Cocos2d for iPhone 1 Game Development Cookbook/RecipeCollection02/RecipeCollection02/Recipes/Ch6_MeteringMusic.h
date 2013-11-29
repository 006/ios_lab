#import "Recipe.h"
#import "SimpleAudioEngine.h"

@interface Ch6_MeteringMusic : Recipe
{
	SimpleAudioEngine *sae;
	CCSprite *speakerBase;
	CCSprite *speakerLarge;
	CCSprite *speakerSmall;
	CCSprite *avgMeter;
	CCSprite *peakMeter;
	float peakPower;
	float avgPower;
}

-(CCLayer*) runRecipe;
-(void) startRecipe;
-(void) step:(ccTime)delta;
-(void) setPeakAndAveragePower;
-(void) animateMeterAndSpeaker;

@end

@implementation Ch6_MeteringMusic

-(CCLayer*) runRecipe {
	[super runRecipe];

	//Show loading message
	[self showMessage:@"LOADING..."];

	//Start recipe asynchronously
	[self runAction:[CCSequence actions:[CCDelayTime actionWithDuration:0.5f], 
		[CCCallFunc actionWithTarget:self selector:@selector(startRecipe)], nil]];
		
	return self;
}

-(void) startRecipe {
	//Show message
	[self showMessage:@""];

	//Initialize the audio engine
	sae = [SimpleAudioEngine sharedEngine];

	//Background music is stopped on resign and resumed on become active
	[[CDAudioManager sharedManager] setResignBehavior:kAMRBStopPlay autoHandle:YES];
			
	//Set peak and average power initially
	peakPower = 0;
	avgPower = 0;
			
	//Init Speaker
	[[CCSpriteFrameCache sharedSpriteFrameCache] addSpriteFramesWithFile:@"cartoon_speaker.plist"];
	speakerBase = [CCSprite spriteWithSpriteFrameName:@"cartoon_speaker_base.png"];
	speakerLarge = [CCSprite spriteWithSpriteFrameName:@"cartoon_speaker_big_speaker.png"];
	speakerSmall = [CCSprite spriteWithSpriteFrameName:@"cartoon_speaker_small_speaker.png"];

	speakerBase.position = ccp(340,160);
	speakerLarge.position = ccp(340,160);
	speakerSmall.position = ccp(340,160);
	
	[self addChild:speakerBase z:1];
	[self addChild:speakerLarge z:2];
	[self addChild:speakerSmall z:2];
		
	//Init meter
	avgMeter = [CCSprite spriteWithFile:@"blank.png"];
	[avgMeter setTextureRect:CGRectMake(0,0,10,0)];
	avgMeter.color = ccc3(255,0,0);
	avgMeter.position = ccp(100,20);
	avgMeter.anchorPoint = ccp(0.5f,0);
	[self addChild:avgMeter];
	
	peakMeter = [CCSprite spriteWithFile:@"blank.png"];
	[peakMeter setTextureRect:CGRectMake(0,0,10,5)];
	peakMeter.color = ccc3(255,0,0);
	peakMeter.position = ccp(100,20);
	peakMeter.anchorPoint = ccp(0.5f,0.5f);
	[self addChild:peakMeter];
		
	//Add the background music
	[sae preloadBackgroundMusic:@"technogeek_by_mrpoly.mp3"];
	[sae playBackgroundMusic:@"technogeek_by_mrpoly.mp3"];
	
	//Enable metering
	[CDAudioManager sharedManager].backgroundMusic.audioSourcePlayer.meteringEnabled = YES;
	
	//Schedule step method
	[self schedule:@selector(step:)];
}

-(void) step:(ccTime)delta {
	[self setPeakAndAveragePower];
	[self animateMeterAndSpeaker];
}

-(void) setPeakAndAveragePower {
	//Update meters
	[[CDAudioManager sharedManager].backgroundMusic.audioSourcePlayer updateMeters];

	//Get channels
	int channels = [CDAudioManager sharedManager].backgroundMusic.audioSourcePlayer.numberOfChannels;

	//Average all the channels
	float peakPowerNow = 0;
	float avgPowerNow = 0;

	for(int i=0; i<channels; i++){
		float peak = [[CDAudioManager sharedManager].backgroundMusic.audioSourcePlayer peakPowerForChannel:i];
		float avg = [[CDAudioManager sharedManager].backgroundMusic.audioSourcePlayer averagePowerForChannel:i];
		peakPowerNow += peak/channels;
		avgPowerNow += avg/channels;
	}
	
	//Change from a DB level to a 0 to 1 ratio
	float adjustedPeak = pow(10, (0.05 * peakPowerNow));
	float adjustedAvg = pow(10, (0.05 * avgPowerNow));
		
	//Average it out for smoothing
	peakPower = (peakPower + adjustedPeak)/2;
	avgPower = (avgPower + adjustedAvg)/2;
}

-(void) animateMeterAndSpeaker {
	//Average meter
	[avgMeter setTextureRect:CGRectMake(0,0,10,avgPower*500.0f)];
	
	//Peak meter
	peakMeter.position = ccp(100,20+peakPower*500.0f);
	
	//Animate speaker
	speakerLarge.scale = powf(avgPower,0.4f)*2;
	speakerSmall.scale = powf(avgPower,0.4f)*2;
}

-(void) loadBackgroundMusic:(NSString*)fn {
	//Pre-load background music
	[sae preloadBackgroundMusic:fn];
	
	//Lower the background music volume
	[CDAudioManager sharedManager].backgroundMusic.volume = 0.3f;
}

-(void) playBackgroundMusic:(NSString*)fn {
	if (![sae isBackgroundMusicPlaying]) {
		//Play background music
		[sae playBackgroundMusic:fn];
	}else{
		//Stop music if its currently playing
		[sae stopBackgroundMusic];
	}
}

-(void) cleanRecipe {
	//Stop background music
	[sae stopBackgroundMusic];
	
	//End engine
	[SimpleAudioEngine end];
	sae = nil;

	[super cleanRecipe];
}

@end
