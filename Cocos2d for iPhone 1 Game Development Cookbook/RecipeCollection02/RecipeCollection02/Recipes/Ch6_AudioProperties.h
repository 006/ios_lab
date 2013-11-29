#import "Recipe.h"
#import "SimpleAudioEngine.h"

@interface Ch6_AudioProperties : Recipe
{
	SimpleAudioEngine *sae;
	NSMutableDictionary *notes;
	NSMutableDictionary *noteSprites;
}

-(CCLayer*) runRecipe;

@end

@implementation Ch6_AudioProperties

-(CCLayer*) runRecipe {
	[super runRecipe];
	
	//Show message
	[self showMessage:@"Drag your fingers across \nthe screen to play a tune."];

	//Enable accelerometer support
	self.isAccelerometerEnabled = YES;
	[[UIAccelerometer sharedAccelerometer] setUpdateInterval:(1.0 / 60)];

	//Add background
	CCSprite *bg = [CCSprite spriteWithFile:@"synth_tone_sheet.png"];
	bg.position = ccp(240,160);
	[self addChild:bg];

	//Initialize the audio engine
	sae = [SimpleAudioEngine sharedEngine];

	//Background music is stopped on resign and resumed on become active
	[[CDAudioManager sharedManager] setResignBehavior:kAMRBStopPlay autoHandle:YES];
	
	//Initialize note container
	notes = [[NSMutableDictionary alloc] init];
	noteSprites = [[NSMutableDictionary alloc] init];
	
	//Preload tone
	[sae preloadEffect:@"synth_tone_mono.caf"];
	
	return self;
}

-(void) ccTouchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {
	//Process multiple touches
	for(int i=0; i<[[touches allObjects] count]; i++){
		UITouch *touch = [[touches allObjects] objectAtIndex:i];
		CGPoint point = [touch locationInView: [touch view]];
		point = [[CCDirector sharedDirector] convertToGL: point];		

		//Use [touch hash] as a key for this sound source
		NSString *key = [NSString stringWithFormat:@"%d",[touch hash]];
		if([notes objectForKey:key]){
			CDSoundSource *sound = [notes objectForKey:key];
			[sound release];
			[notes removeObjectForKey:key];
			
			CCSprite *sprite = [noteSprites objectForKey:key];
			[self removeChild:sprite cleanup:YES];
			[noteSprites removeObjectForKey:key];
		}
		
		//Play our sound with custom pitch and gain
		CDSoundSource *sound = [[sae soundSourceForFile:@"synth_tone_mono.caf"] retain];
		[sound play];
		sound.looping = YES;
		[notes setObject:sound forKey:key];
		sound.pitch = point.x/240.0f;
		sound.gain = point.y/320.0f;
		
		//Show music note where you touched
		CCSprite *sprite = [CCSprite spriteWithFile:@"music_note.png"];
		sprite.position = point;
		[noteSprites setObject:sprite forKey:key];
		sprite.scale = (point.y/320.0f)/2 + 0.25f;
		[self addChild:sprite];
	}
}

-(void) ccTouchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {
	//Adjust sound sources and music note positions
	for(int i=0; i<[[touches allObjects] count]; i++){
		UITouch *touch = [[touches allObjects] objectAtIndex:i];
		CGPoint point = [touch locationInView: [touch view]];
		point = [[CCDirector sharedDirector] convertToGL: point];		

		NSString *key = [NSString stringWithFormat:@"%d",[touch hash]];
		if([notes objectForKey:key]){
			CDSoundSource *sound = [notes objectForKey:key];
			sound.pitch = point.x/240.0f;
			sound.gain = point.y/320.0f;			
			
			CCSprite *sprite = [noteSprites objectForKey:key];
			sprite.position = point;
			sprite.scale = (point.y/320.0f)/2 + 0.25f;
		}
	}
}

-(void) ccTouchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {
	//Stop sounds and remove sprites
	for(int i=0; i<[[touches allObjects] count]; i++){
		UITouch *touch = [[touches allObjects] objectAtIndex:i];
		CGPoint point = [touch locationInView: [touch view]];
		point = [[CCDirector sharedDirector] convertToGL: point];		

		NSString *key = [NSString stringWithFormat:@"%d",[touch hash]];
		if([notes objectForKey:key]){
			//Stop and remove sound source
			CDSoundSource *sound = [notes objectForKey:key];
			[sound stop];
			[sound release];
			[notes removeObjectForKey:key];
			
			//Remove sprite
			CCSprite *sprite = [noteSprites objectForKey:key];
			[self removeChild:sprite cleanup:YES];
			[noteSprites removeObjectForKey:key];			
		}
	}
}

//Adjust sound pan by turning the device sideways
- (void)accelerometer:(UIAccelerometer*)accelerometer didAccelerate:(UIAcceleration*)acceleration{
	for(id s in notes){
		CDSoundSource *sound = [notes objectForKey:s];
		sound.pan = -acceleration.y;	//"Turn" left to pan to the left speaker
	}
}

-(void) cleanRecipe {
	//Stop background music
	[sae stopBackgroundMusic];
	
	//Release sources
	for(id s in notes){
		CDSoundSource *sound = [notes objectForKey:s];
		[sound release];
	}
	
	[notes release];
	[noteSprites release];
	
	//End engine
	[SimpleAudioEngine end];
	sae = nil;

	[super cleanRecipe];
}

@end
