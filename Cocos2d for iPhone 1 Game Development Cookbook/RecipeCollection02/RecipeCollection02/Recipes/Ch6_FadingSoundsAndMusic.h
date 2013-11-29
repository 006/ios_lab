#import "Recipe.h"
#import "SimpleAudioEngine.h"
#import "CDXPropertyModifierAction.h"

@interface Ch6_FadingSoundsAndMusic : Recipe
{
	SimpleAudioEngine *sae;
	NSMutableDictionary *soundSources;
	NSMutableDictionary *musicSources;
}

-(CCLayer*) runRecipe;
-(void) startRecipe;
-(CDLongAudioSource*) loadMusic:(NSString*)fn;
-(void) playMusicNumber:(id)sender;
-(void) fadeInMusicFile:(NSString*)fn;

-(CDSoundSource*) loadSoundEffect:(NSString*)fn;
-(void) playSoundNumber:(id)sender;
-(void) playSoundFile:(NSString*)fn;

-(void) fadeOutPlayingMusic;
-(void) fadeUpAllSfx:(id)sender;
-(void) fadeDownAllSfx:(id)sender;
-(void) fadeUpSfxNumber:(id)sender;
-(void) fadeDownSfxNumber:(id)sender;

@end

@implementation Ch6_FadingSoundsAndMusic

-(CCLayer*) runRecipe {
	[super runRecipe];

	//Show loading message
	[self showMessage:@"LOADING..."];

	//Star recipe asynchronously
	[self runAction:[CCSequence actions:[CCDelayTime actionWithDuration:0.5f], 
		[CCCallFunc actionWithTarget:self selector:@selector(startRecipe)], nil]];
		
	return self;
}


-(void) startRecipe {
	//Show message
	[self showMessage:@"Tap a button to hear the specified effect."];

	//Initialize the audio engine
	sae = [SimpleAudioEngine sharedEngine];

	//Background music is stopped on resign and resumed on become active
	[[CDAudioManager sharedManager] setResignBehavior:kAMRBStopPlay autoHandle:YES];
	
	//Initialize source container
	soundSources = [[NSMutableDictionary alloc] init];
	musicSources = [[NSMutableDictionary alloc] init];
	
	//Add music
	[self loadMusic:@"hiphop_boss_man_by_p0ss.mp3"];
	[self loadMusic:@"menu_music_by_mrpoly.mp3"];
	
	//Add sounds
	[self loadSoundEffect:@"gunman_pain.caf"];
	[self loadSoundEffect:@"synth_tone_mono.caf"];
		
	//Add menu items
	[CCMenuItemFont setFontSize:20];
	
	CCMenuItemFont *music0Item = [CCMenuItemFont itemFromString:@"Fade To Song 1" target:self selector:@selector(playMusicNumber:)];
	music0Item.tag = 0;
	CCMenuItemFont *music1Item = [CCMenuItemFont itemFromString:@"Fade To Song 2" target:self selector:@selector(playMusicNumber:)];
	music1Item.tag = 1;
	
	CCMenuItemFont *sfx0Item = [CCMenuItemFont itemFromString:@"SFX 1" target:self selector:@selector(playSoundNumber:)];
	sfx0Item.tag = 0;
	CCMenuItemFont *sfx1Item = [CCMenuItemFont itemFromString:@"SFX 2" target:self selector:@selector(playSoundNumber:)];
	sfx1Item.tag = 1;

	CCMenuItemFont *fadeUpAllSfx = [CCMenuItemFont itemFromString:@"Fade Up All Sfx" target:self selector:@selector(fadeUpAllSfx:)];
	CCMenuItemFont *fadeDownAllSfx = [CCMenuItemFont itemFromString:@"Fade Down All Sfx" target:self selector:@selector(fadeDownAllSfx:)];
	
	CCMenuItemFont *fadeUpSfx0 = [CCMenuItemFont itemFromString:@"Fade Up Sfx 1" target:self selector:@selector(fadeUpSfxNumber:)];
	fadeUpSfx0.tag = 0;
	CCMenuItemFont *fadeUpSfx1 = [CCMenuItemFont itemFromString:@"Fade Up Sfx 2" target:self selector:@selector(fadeUpSfxNumber:)];
	fadeUpSfx1.tag = 1;
	
	CCMenuItemFont *fadeDownSfx0 = [CCMenuItemFont itemFromString:@"Fade Down Sfx 1" target:self selector:@selector(fadeDownSfxNumber:)];
	fadeDownSfx0.tag = 0;
	CCMenuItemFont *fadeDownSfx1 = [CCMenuItemFont itemFromString:@"Fade Down Sfx 2" target:self selector:@selector(fadeDownSfxNumber:)];
	fadeDownSfx1.tag = 1;
	
	//Create our menus
	CCMenu *menu0 = [CCMenu menuWithItems:music0Item, music1Item, nil];
	[menu0 alignItemsInColumns: [NSNumber numberWithUnsignedInt:2], nil];
	menu0.position = ccp(240,240);
	[self addChild:menu0];
	
	CCMenu *menu1 = [CCMenu menuWithItems:sfx0Item, sfx1Item, nil];
	[menu1 alignItemsInColumns: [NSNumber numberWithUnsignedInt:2], nil];
	menu1.position = ccp(240, 180);
	[self addChild:menu1];
	
	CCMenu *menu2 = [CCMenu menuWithItems: fadeUpAllSfx, fadeDownAllSfx, fadeDownSfx0, fadeDownSfx1, fadeUpSfx0, fadeUpSfx1, nil];
	[menu2 alignItemsInColumns: [NSNumber numberWithUnsignedInt:1], [NSNumber numberWithUnsignedInt:1], 
		[NSNumber numberWithUnsignedInt:2], [NSNumber numberWithUnsignedInt:2], nil];
	menu2.position = ccp(240,80);
	[self addChild:menu2];
}

//Play sound callback
-(void) playSoundNumber:(id)sender {
	CCMenuItem *item = (CCMenuItem*)sender;
	int number = item.tag;

	if(number == 0){
		[self playSoundFile:@"gunman_pain.caf"];
	}else if(number == 1){
		[self playSoundFile:@"synth_tone_mono.caf"];
	}
}

-(CDSoundSource*) loadSoundEffect:(NSString*)fn {
	//Pre-load sound
	[sae preloadEffect:fn];

	//Init sound
	CDSoundSource *sound = [[sae soundSourceForFile:fn] retain];
	
	//Add sound to container
	[soundSources setObject:sound forKey:fn];
	
	return sound;
}

-(void) playSoundFile:(NSString*)fn {
	//Get sound
	CDSoundSource *sound = [soundSources objectForKey:fn];
	sound.looping = YES;
	
	//Play sound
	if(sound.isPlaying){
		[sound stop];
	}else{
		[sound play];
	}
}

//Play music callback
-(void) playMusicNumber:(id)sender {
	CCMenuItem *item = (CCMenuItem*)sender;
	int number = item.tag;

	if(number == 0){
		[self fadeOutPlayingMusic];
		[self fadeInMusicFile:@"hiphop_boss_man_by_p0ss.mp3"];
	}else if(number == 1){
		[self fadeOutPlayingMusic];
		[self fadeInMusicFile:@"menu_music_by_mrpoly.mp3"];
	}
}

-(CDLongAudioSource*) loadMusic:(NSString*)fn {
	//Init source
	CDLongAudioSource *source = [[CDLongAudioSource alloc] init];
	source.backgroundMusic = NO;
	[source load:fn];
	
	//Add sound to container
	[musicSources setObject:source forKey:fn];
	
	return source;
}

//Fade out any music sources currently playing
-(void) fadeOutPlayingMusic {
	for(id m in musicSources){
		//Release source
		CDLongAudioSource *source = [musicSources objectForKey:m];
		if(source.isPlaying){
			//Create fader
			CDLongAudioSourceFader* fader = [[CDLongAudioSourceFader alloc] init:source interpolationType:kIT_Linear startVal:source.volume endVal:0.0f];
			[fader setStopTargetWhenComplete:NO];
		
			//Create a property modifier action to wrap the fader
			CDXPropertyModifierAction* fadeAction = [CDXPropertyModifierAction actionWithDuration:3.0f modifier:fader];
			[fader release];//Action will retain
			CCCallFuncN* stopAction = [CCCallFuncN actionWithTarget:source selector:@selector(stop)];
			[[CCActionManager sharedManager] addAction:[CCSequence actions:fadeAction, stopAction, nil] target:source paused:NO];
		}
	}
}

//Fade in a specific music file
-(void) fadeInMusicFile:(NSString*)fn {
	//Stop music if its playing and return
	CDLongAudioSource *source = [musicSources objectForKey:fn];
	if(source.isPlaying){
		[source stop];
		return;
	}
	
	//Set volume to zero and play
	source.volume = 0.0f;
	[source play];
	
	//Create fader
	CDLongAudioSourceFader* fader = [[CDLongAudioSourceFader alloc] init:source interpolationType:kIT_Linear startVal:source.volume endVal:1.0f];
	[fader setStopTargetWhenComplete:NO];
		
	//Create a property modifier action to wrap the fader
	CDXPropertyModifierAction* fadeAction = [CDXPropertyModifierAction actionWithDuration:1.5f modifier:fader];
	[fader release];//Action will retain
	[[CCActionManager sharedManager] addAction:[CCSequence actions:fadeAction, nil] target:source paused:NO];
}

-(void) fadeUpAllSfx:(id)sender {
	//Fade up all sound effects
	[CDXPropertyModifierAction fadeSoundEffects:2.0f finalVolume:1.0f curveType:kIT_Linear shouldStop:NO];
}
-(void) fadeDownAllSfx:(id)sender {
	//Fade down all sound effects
	[CDXPropertyModifierAction fadeSoundEffects:2.0f finalVolume:0.0f curveType:kIT_Linear shouldStop:NO];
}
-(void) fadeUpSfxNumber:(id)sender {
	//Fade up a specific sound effect
	CCMenuItem *item = (CCMenuItem*)sender;
	int number = item.tag;

	CDSoundSource *source;
	if(number == 0){
		source = [soundSources objectForKey:@"gunman_pain.caf"];
	}else if(number == 1){
		source = [soundSources objectForKey:@"synth_tone_mono.caf"];
	}	
	source.gain = 0.0f;
	[CDXPropertyModifierAction fadeSoundEffect:2.0f finalVolume:1.0f curveType:kIT_Linear shouldStop:NO effect:source];
}
-(void) fadeDownSfxNumber:(id)sender {
	//Fade down a specific sound effect
	CCMenuItem *item = (CCMenuItem*)sender;
	int number = item.tag;

	CDSoundSource *source;
	if(number == 0){
		source = [soundSources objectForKey:@"gunman_pain.caf"];
	}else if(number == 1){
		source = [soundSources objectForKey:@"synth_tone_mono.caf"];
	}	
	source.gain = 1.0f;
	[CDXPropertyModifierAction fadeSoundEffect:2.0f finalVolume:0.0f curveType:kIT_Linear shouldStop:NO effect:source];
}

-(void) cleanRecipe {
	//Stop background music
	[sae stopBackgroundMusic];
	
	for(id s in soundSources){
		//Release source
		CDSoundSource *source = [soundSources objectForKey:s];
		if(source.isPlaying){ [source stop]; }
		[source release];
	}
	
	[soundSources release];
	
	for(id m in musicSources){
		//Release source
		CDLongAudioSource *source = [musicSources objectForKey:m];
		if(source.isPlaying){ [source stop]; }
		[source release];
	}
	
	[musicSources release];
	
	//End engine
	[SimpleAudioEngine end];
	sae = nil;

	[super cleanRecipe];
}

@end
