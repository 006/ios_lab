#import "Recipe.h"
#import "SimpleAudioEngine.h"

@interface Ch6_SoundsAndMusic : Recipe
{
	SimpleAudioEngine *sae;
	NSMutableDictionary *soundSources;
}

-(CCLayer*) runRecipe;
-(void) startRecipe;
-(CCMenuItemSprite*) menuItemFromSpriteFile:(NSString*)fn tag:(int)t;
-(CDSoundSource*) loadSoundEffect:(NSString*)fn;
-(void) loadBackgroundMusic:(NSString*)fn;
-(void) playBackgroundMusic:(NSString*)fn;
-(void) playSoundNumber:(id)sender;
-(void) playSoundFile:(NSString*)fn;

@end

@implementation Ch6_SoundsAndMusic

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
	message.position = ccp(160,275);
	[self showMessage:@"Tap the icons to play sounds and music."];

	//Initialize the audio engine
	sae = [SimpleAudioEngine sharedEngine];

	//Background music is stopped on resign and resumed on become active
	[[CDAudioManager sharedManager] setResignBehavior:kAMRBStopPlay autoHandle:YES];
	
	//Initialize source container
	soundSources = [[NSMutableDictionary alloc] init];
	
	//Add the sounds
	[self loadSoundEffect:@"crazy_chimp.caf"];
	[self loadSoundEffect:@"rapid_gunfire.caf"];
	[self loadSoundEffect:@"howie_scream.caf"];
	[self loadSoundEffect:@"air_horn.caf"];
	[self loadSoundEffect:@"slide_whistle.caf"];
	
	//Add the background music
	[self loadBackgroundMusic:@"hiphop_boss_man_by_p0ss.mp3"];
	
	//Add menu items
	CCMenuItemSprite *musicItem = [self menuItemFromSpriteFile:@"music_note.png" tag:0];
	CCMenuItemSprite *chimpItem = [self menuItemFromSpriteFile:@"you_stupid_monkey.png" tag:1];
	CCMenuItemSprite *gunItem = [self menuItemFromSpriteFile:@"tommy_gun.png" tag:2];
	CCMenuItemSprite *screamItem = [self menuItemFromSpriteFile:@"yaaargh.png" tag:3];
	CCMenuItemSprite *airHornItem = [self menuItemFromSpriteFile:@"air_horn.png" tag:4];
	CCMenuItemSprite *slideWhistleItem = [self menuItemFromSpriteFile:@"slide_whistle.png" tag:5];

	//Create our menu
	CCMenu *menu = [CCMenu menuWithItems: musicItem, chimpItem, gunItem, screamItem, airHornItem, slideWhistleItem, nil];
	[menu alignItemsInColumns: [NSNumber numberWithUnsignedInt:3], [NSNumber numberWithUnsignedInt:3], nil];
	menu.position = ccp(240,140);
	[self addChild:menu];
}

//Helper method to create sprite buttons
-(CCMenuItemSprite*) menuItemFromSpriteFile:(NSString*)fn tag:(int)t {
	CCSprite *normalSprite = [CCSprite spriteWithFile:fn];
	CCSprite *selectedSprite = [CCSprite spriteWithFile:fn];
	selectedSprite.color = ccc3(128,128,180); [selectedSprite setBlendFunc: (ccBlendFunc) { GL_ONE, GL_ONE }];

	CCMenuItemSprite *item = [CCMenuItemSprite itemFromNormalSprite:normalSprite 
		selectedSprite:selectedSprite target:self selector:@selector(playSoundNumber:)];
	item.tag = t;
	item.scale = 0.9f;
	
	return item;
}

//Play sound callback
-(void) playSoundNumber:(id)sender {
	CCMenuItem *item = (CCMenuItem*)sender;
	int number = item.tag;

	if(number == 0){
		[self playBackgroundMusic:@"hiphop_boss_man_by_p0ss.mp3"];
	}else if(number == 1){
		[self playSoundFile:@"crazy_chimp.caf"];
	}else if(number == 2){
		[self playSoundFile:@"rapid_gunfire.caf"];
	}else if(number == 3){
		[self playSoundFile:@"howie_scream.caf"];
	}else if(number == 4){
		[self playSoundFile:@"air_horn.caf"];
	}else if(number == 5){
		[self playSoundFile:@"slide_whistle.caf"];
	}
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
