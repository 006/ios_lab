#import "Recipe.h"
#import "Ch4_Bullets.h"
#import "SimpleAudioEngine.h"

@interface Ch6_AudioInGame : Ch4_Bullets
{
	SimpleAudioEngine *sae;
	NSMutableDictionary *soundSources;
}

-(CCLayer*) runRecipe;
-(void) step:(ccTime)delta;

//Overridden methods
-(void) processJump;
-(void) fireGun;
-(void) boxExplosionAt:(CGPoint)p withRotation:(float)rot;
-(void) handleCollisionWithMisc:(GameMisc*)a withMisc:(GameMisc*)b;

//Playing Sounds
-(CDSoundSource*) loadSoundEffect:(NSString*)fn gain:(float)gain;
-(void) loadBackgroundMusic:(NSString*)fn;
-(void) playBackgroundMusic:(NSString*)fn;
-(void) playSoundFile:(NSString*)fn;

@end

@implementation Ch6_AudioInGame

-(CCLayer*) runRecipe {
	[super runRecipe];
	
	//Initialize the audio engine
	sae = [SimpleAudioEngine sharedEngine];

	//Background music is stopped on resign and resumed on become active
	[[CDAudioManager sharedManager] setResignBehavior:kAMRBStopPlay autoHandle:YES];
	
	//Initialize source container
	soundSources = [[NSMutableDictionary alloc] init];
	
	//Add the sounds
	[self loadSoundEffect:@"bullet_fire_no_shell.caf" gain:1.0f];
	[self loadSoundEffect:@"bullet_casing_tink.caf" gain:0.25f];
	[self loadSoundEffect:@"gunman_jump.caf" gain:1.5f];
	[self loadSoundEffect:@"box_break.wav" gain:1.5f];
	
	//Add the background music
	[self loadBackgroundMusic:@"hiphop_boss_man_by_p0ss.mp3"];
	sae.backgroundMusicVolume = 0.5f;
	[self playBackgroundMusic:@"hiphop_boss_man_by_p0ss.mp3"];
	
	return self;
}

-(void) step:(ccTime)delta {
	[super step:delta];
}

//Jump sound override
-(void) processJump {
	if(onGround && jumpCounter < 0){	
		[self playSoundFile:@"gunman_jump.caf"];
	}
	
	[super processJump];
}

//Fire gun sound override
-(void) fireGun {
	if(fireCount <= 0){
		[self playSoundFile:@"bullet_fire_no_shell.caf"];
	}
	
	[super fireGun];
}

//Box explosion sound override
-(void) boxExplosionAt:(CGPoint)p withRotation:(float)rot {
	[self playSoundFile:@"box_break.wav"];
	
	[super boxExplosionAt:p withRotation:rot];
}

//Bullet casing sound override
-(void) handleCollisionWithMisc:(GameMisc*)a withMisc:(GameMisc*)b {
	if(a.typeTag == TYPE_OBJ_SHELL || b.typeTag == TYPE_OBJ_SHELL){
		[self playSoundFile:@"bullet_casing_tink.caf"];
	}
	
	[super handleCollisionWithMisc:a withMisc:b];
}


-(void) loadBackgroundMusic:(NSString*)fn {
	//Pre-load background music
	[sae preloadBackgroundMusic:fn];
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

-(CDSoundSource*) loadSoundEffect:(NSString*)fn gain:(float)gain {
	//Pre-load sound
	[sae preloadEffect:fn];

	//Init sound
	CDSoundSource *sound = [[sae soundSourceForFile:fn] retain];
	sound.gain = gain;
	
	//Add sound to container
	[soundSources setObject:sound forKey:fn];
	
	return sound;
}

-(void) playSoundFile:(NSString*)fn {
	//Get sound
	CDSoundSource *sound = [soundSources objectForKey:fn];
	
	//Play sound
	[sound play];
}
-(void) cleanRecipe {
	//Stop background music
	[sae stopBackgroundMusic];
	
	for(id s in soundSources){
		//Release source
		CDSoundSource *source = [soundSources objectForKey:s];
		[source release];
	}
	[soundSources release];
	
	//End engine
	[SimpleAudioEngine end];
	sae = nil;

	[super cleanRecipe];
}

@end
