#import "Recipe.h"
#import "SimpleAudioEngine.h"

@interface Ch6_MeteringDialogue : Recipe
{
	SimpleAudioEngine *sae;
	NSMutableDictionary *soundSources;
	CCSprite *claghornBase;
	CCSprite *claghornPupils;
	CCSprite *claghornEyebrows;
	CCSprite *claghornMouth;
	CCSprite *claghornMoustache;
	float lastAudioLevel;
	bool isBlinking;
	float peakPower;
	float avgPower;
}

-(CCLayer*) runRecipe;
-(void) startRecipe;
-(void) step:(ccTime)delta;
-(void) blink;
-(void) unblink;
-(void) resetPupils;
-(CCMenuItemSprite*) menuItemFromSpriteFile:(NSString*)fn tag:(int)t;
-(CDLongAudioSource*) loadLongAudioSource:(NSString*)fn;
-(void) loadBackgroundMusic:(NSString*)fn;
-(void) playBackgroundMusic:(NSString*)fn;
-(void) playSoundNumber:(id)sender;
-(void) playLongAudioSource:(NSString*)fn;
-(void) setPeakAndAveragePower;
-(void) animateClaghorn;

@end

@implementation Ch6_MeteringDialogue

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
	[self showMessage:@"Tap the 'Speak' button to make \nSenator Claghorn speak."];

	//Initial values
	lastAudioLevel = 0;
	isBlinking = NO;
	
	//Initialize the audio engine
	sae = [SimpleAudioEngine sharedEngine];

	//Background music is stopped on resign and resumed on become active
	[[CDAudioManager sharedManager] setResignBehavior:kAMRBStopPlay autoHandle:YES];
	
	//Initialize source container
	soundSources = [[NSMutableDictionary alloc] init];
	
	//Schedule step
	[self schedule:@selector(step:)];
			
	//Add the sounds
	[self loadLongAudioSource:@"claghorn_a_joke_son.caf"];
	[self loadLongAudioSource:@"claghorn_carolina.caf"];
	[self loadLongAudioSource:@"claghorn_eliminate.caf"];
	[self loadLongAudioSource:@"claghorn_howdy.caf"];
	[self loadLongAudioSource:@"claghorn_make_canada_north.caf"];
	[self loadLongAudioSource:@"claghorn_rivers_flow_south.caf"];
	[self loadLongAudioSource:@"claghorn_transfusion.caf"];
	
	//Add the background music
	[self loadBackgroundMusic:@"dixie_1916.mp3"];
	
	//Add menu items
	[CCMenuItemFont setFontSize:30];
	[CCMenuItemFont setFontName:@"Marker Felt"];
	CCMenuItemSprite *musicItem = [self menuItemFromSpriteFile:@"music_note.png" tag:0];
	CCMenuItemFont *talkItem = [CCMenuItemFont itemFromString:@"Speak" target:self selector:@selector(playSoundNumber:)];
	talkItem.tag = 1;

	//Create our menu
	CCMenu *menu = [CCMenu menuWithItems: musicItem, talkItem, nil];
	[menu alignItemsHorizontallyWithPadding:10.0f];
	menu.position = ccp(400,250);
	[self addChild:menu];
		
	//Init Claghorn
	[[CCSpriteFrameCache sharedSpriteFrameCache] addSpriteFramesWithFile:@"claghorn.plist"];
	claghornBase = [CCSprite spriteWithSpriteFrameName:@"claghorn_normal.png"];
	claghornPupils = [CCSprite spriteWithSpriteFrameName:@"claghorn_pupils.png"];
	claghornEyebrows = [CCSprite spriteWithSpriteFrameName:@"claghorn_eyebrows.png"];
	claghornMouth = [CCSprite spriteWithSpriteFrameName:@"claghorn_mouth.png"];
	claghornMoustache = [CCSprite spriteWithSpriteFrameName:@"claghorn_moustache.png"];
	
	claghornBase.position = ccp(240,120);
	claghornPupils.position = ccp(240,120);
	claghornEyebrows.position = ccp(240,120);
	claghornMouth.position = ccp(240,120);
	claghornMoustache.position = ccp(240,120);
	
	[self addChild:claghornBase z:1];
	[self addChild:claghornPupils z:2];
	[self addChild:claghornEyebrows z:2];
	[self addChild:claghornMouth z:2];
	[self addChild:claghornMoustache z:3];
	
	//Play background music
	[self playBackgroundMusic:@"dixie_1916.mp3"];
	
	//Have Claghorn introduce himself
	[self playLongAudioSource:@"claghorn_howdy.caf"];
}

-(void) step:(ccTime)delta {
	//Random blinking
	int rand = arc4random()%200;
	if(rand < 2 && !isBlinking){
		[self blink];
		[self runAction:[CCSequence actions:[CCDelayTime actionWithDuration:0.2f], 
			[CCCallFunc actionWithTarget:self selector:@selector(unblink)], nil]];
	}
	
	//Random looking
	if(rand > 198){
		[claghornPupils stopAllActions];
		[self resetPupils];
		CGPoint moveVect = ccp(240 + arc4random()%10-5.0f, 120 + arc4random()%10-5.0f);
		float duration = ((float)(arc4random()%2000))/1000.0f + 0.5f;
		[claghornPupils runAction:
			[CCSequence actions:
				[CCEaseExponentialOut actionWithAction: [CCMoveTo actionWithDuration:duration position:moveVect] ],
				[CCEaseExponentialOut actionWithAction: [CCMoveTo actionWithDuration:duration position:ccp(240,120)] ],
				nil
			]
		];
	}
	
	[self setPeakAndAveragePower];
	[self animateClaghorn];
}

-(void) setPeakAndAveragePower {
	//Find our playing audio source
	CDLongAudioSource *audioSource = nil;
	for(id s in soundSources){
		CDLongAudioSource *source = [soundSources objectForKey:s];
		if(source.isPlaying){
			audioSource = source;
			break;
		}
	}

	//Update meters
	[audioSource.audioSourcePlayer updateMeters];

	//Get channels
	int channels = audioSource.audioSourcePlayer.numberOfChannels;
	
	//If nothing is playing we set everything to 0 and return
	if(!audioSource){ 
		peakPower = 0;
		avgPower = 0;
		return;
	}

	//Average all the channels
	float peakPowerNow = 0;
	float avgPowerNow = 0;
	
	for(int i=0; i<channels; i++){
		float peak = [audioSource.audioSourcePlayer peakPowerForChannel:i];
		float avg = [audioSource.audioSourcePlayer averagePowerForChannel:i];
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

-(void) animateClaghorn {
	/* Custom mouth animation */
	float level = avgPower;
	
	//Make sure he's actually speaking
	if(level == 0){
		claghornEyebrows.position = ccp(240,120);
		claghornMouth.position = ccp(240,120);
		lastAudioLevel = level;
		return;
	}
	
	//Level bounds
	if(level <= 0){ level = 0.01f; }
	if(level >= 1){ level = 0.99f; }

	//Exaggerate level ebb and flow
	if(level < lastAudioLevel){	
		//Closing mouth
		lastAudioLevel = level;
		level = powf(level,1.5f);
	}else{	
		//Opening mouth
		lastAudioLevel = level;
		level = powf(level,0.75f);
	}

	//If mouth is almost closed, close mouth
	if(level < 0.1f){ level = 0.01f; }

	//Blink if level > 0.8f
	if(level > 0.8f && !isBlinking){
		[self blink];
		[self runAction:[CCSequence actions:[CCDelayTime actionWithDuration:0.5f], 
			[CCCallFunc actionWithTarget:self selector:@selector(unblink)], nil]];
	}
	
	//Raise eyebrows if level > 0.6f
	if(level > 0.6f){
		claghornEyebrows.position = ccp(240,120 + level*5.0f);
	}else{
		claghornEyebrows.position = ccp(240,120);
	}

	//Set mouth position
	claghornMouth.position = ccp(240,120 - level*19.0f);
}

//Blink animation
-(void) blink {
	isBlinking = YES;
	[claghornBase setDisplayFrame:[[CCSpriteFrameCache sharedSpriteFrameCache] spriteFrameByName:@"claghorn_squint.png"]];
	claghornPupils.visible = NO;
}

//Unblink animation
-(void) unblink {
	isBlinking = NO;
	[claghornBase setDisplayFrame:[[CCSpriteFrameCache sharedSpriteFrameCache] spriteFrameByName:@"claghorn_normal.png"]];
	claghornPupils.visible = YES;
}

//Reset pupil position
-(void) resetPupils {
	claghornPupils.position = ccp(240,120);
}

//Helper method to create sprite buttons
-(CCMenuItemSprite*) menuItemFromSpriteFile:(NSString*)fn tag:(int)t {
	CCSprite *normalSprite = [CCSprite spriteWithFile:fn];
	CCSprite *selectedSprite = [CCSprite spriteWithFile:fn];
	selectedSprite.color = ccc3(128,128,180); [selectedSprite setBlendFunc: (ccBlendFunc) { GL_ONE, GL_ONE }];

	CCMenuItemSprite *item = [CCMenuItemSprite itemFromNormalSprite:normalSprite 
		selectedSprite:selectedSprite target:self selector:@selector(playSoundNumber:)];
	item.tag = t;
	item.scale = 0.5f;
	
	return item;
}

//Play voice clip or music callback
-(void) playSoundNumber:(id)sender {
	[self resetPupils];

	CCMenuItem *item = (CCMenuItem*)sender;
	int number = item.tag;

	if(number == 0){
		[self playBackgroundMusic:@"dixie_1916.mp3"];
	}else{
		int num = arc4random()%7;
		if(num == 0){
			[self playLongAudioSource:@"claghorn_a_joke_son.caf"];
		}else if(num == 1){
			[self playLongAudioSource:@"claghorn_carolina.caf"];
		}else if(num == 2){
			[self playLongAudioSource:@"claghorn_eliminate.caf"];
		}else if(num == 3){
			[self playLongAudioSource:@"claghorn_howdy.caf"];
		}else if(num == 4){
			[self playLongAudioSource:@"claghorn_make_canada_north.caf"];
		}else if(num == 5){
			[self playLongAudioSource:@"claghorn_rivers_flow_south.caf"];
		}else if(num == 6){
			[self playLongAudioSource:@"claghorn_transfusion.caf"];
		}
	}
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

-(CDLongAudioSource*) loadLongAudioSource:(NSString*)fn {
	//Init source
	CDLongAudioSource *source = [[CDLongAudioSource alloc] init];
	source.backgroundMusic = NO;
	[source load:fn];
	
	//Enable metering
	source.audioSourcePlayer.meteringEnabled = YES;
	
	//Add sound to container
	[soundSources setObject:source forKey:fn];
	
	return source;
}

-(void) playLongAudioSource:(NSString*)fn {
	//Get sound
	CDLongAudioSource *audioSource = [soundSources objectForKey:fn];
	
	bool aSourceIsPlaying = NO;
	for(id s in soundSources){
		CDLongAudioSource *source = [soundSources objectForKey:s];
		if(source.isPlaying){
			[source stop];
			[source rewind];
			aSourceIsPlaying = YES;
			break;
		}
	}
	
	//Play sound
	if(!aSourceIsPlaying){
		//Play sound
		[audioSource play];
				
		[self runAction: [CCSequence actions: [CCDelayTime actionWithDuration:[audioSource.audioSourcePlayer duration]], 
			[CCCallFunc actionWithTarget:audioSource selector:@selector(stop)],
			[CCCallFunc actionWithTarget:audioSource selector:@selector(rewind)], nil]];
	}
}
-(void) cleanRecipe {
	//Stop background music
	[sae stopBackgroundMusic];
		
	//Remove sound sources
	for(id s in soundSources){
		//Release source
		CDLongAudioSource *source = [soundSources objectForKey:s];
		[source release];
	}
	
	[soundSources release];
	
	//End engine
	[SimpleAudioEngine end];
	sae = nil;

	[super cleanRecipe];
}

@end
