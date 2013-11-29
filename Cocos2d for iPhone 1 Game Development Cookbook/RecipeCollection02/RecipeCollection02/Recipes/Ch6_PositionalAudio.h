#import "Recipe.h"
#import "Ch4_TopDownIsometric.h"
#import "SimpleAudioEngine.h"

enum {
	CGROUP_NON_INTERRUPTIBLE = 0
};

@interface Ch6_PositionalAudio : Ch4_TopDownIsometric
{
	SimpleAudioEngine *sae;
	float audibleRange;
	CDSoundSource *ballSource;
	CDSoundSource *forestBirdsSource;
	CDSoundSource *gunmanJumpSource;
	CDSoundSource *fireBallSource;
}

-(CCLayer*) runRecipe;
-(void) step:(ccTime)delta;
-(void) playBallSoundWithGain:(float)gain pan:(float)pan pitch:(float)pitch;

@end

@implementation Ch6_PositionalAudio

-(CCLayer*) runRecipe {
	//Run our top-down isometric game recipe
	[super runRecipe];
	
	//Initialize max audible range
	audibleRange = 20.0f;

	//Initialize the audio engine
	sae = [SimpleAudioEngine sharedEngine];

	//Background music is stopped on resign and resumed on become active
	[[CDAudioManager sharedManager] setResignBehavior:kAMRBStopPlay autoHandle:YES];
		
	//Preload the sounds
	[sae preloadEffect:@"forest_birds_ambience.caf"];
	[sae preloadEffect:@"kick_ball_bounce.caf"];
	[sae preloadEffect:@"gunman_jump.caf"];
	[sae preloadEffect:@"bullet_fire_no_shell.caf"];

	//Non-interruptible ball source group
	[[CDAudioManager sharedManager].soundEngine setSourceGroupNonInterruptible:CGROUP_NON_INTERRUPTIBLE isNonInterruptible:YES];
	
	//Add the sounds
	ballSource = [[sae soundSourceForFile:@"kick_ball_bounce.caf"] retain];
	forestBirdsSource = [[sae soundSourceForFile:@"forest_birds_ambience.caf"] retain];
	gunmanJumpSource = [[sae soundSourceForFile:@"gunman_jump.caf"] retain];
	fireBallSource = [[sae soundSourceForFile:@"bullet_fire_no_shell.caf"] retain];
	
	//Start playing forest bird source
	forestBirdsSource.gain = 0.0f;
	forestBirdsSource.looping = YES;
	[forestBirdsSource play];
	
	//Customize fire ball sound
	fireBallSource.pitch = 2.0f;
	fireBallSource.gain = 0.5f;
	
	return self;
}

-(void) step:(ccTime)delta {
	[super step:delta];
	
	//Play forest bird source with gain based on distance from gunman
	float distance = 10000.0f;
	for(int i=0; i<[trees count]; i++){
		GameObject *tree = [trees objectAtIndex:i];
		
		float thisDistance = distanceBetweenPoints(ccp(tree.body->GetPosition().x,tree.body->GetPosition().y),
			ccp(gunman.body->GetPosition().x, gunman.body->GetPosition().y));		
		if(thisDistance < distance){ distance = thisDistance; }
	}
	
	//If closest tree is outside of audible range we set gain to 0.0f
	if(distance < audibleRange){
		forestBirdsSource.gain = (audibleRange-distance)/audibleRange;
	}else{
		forestBirdsSource.gain = 0.0f;
	}
}

//Fire ball sound override
-(void) fireBall {
	if(fireCount < 0){
		[fireBallSource play];
	}
	[super fireBall];
}

//Jump sound override
-(void) processJump {
	if(gunman.body->GetZPosition() <= 1.0f){
		[gunmanJumpSource play];
	}
	[super processJump];
}


-(void) handleCollisionWithGroundWithObj:(GameObject*)gameObject {
	[super handleCollisionWithGroundWithObj:gameObject];

	//Play ball bounce sound with gain based on distance from gunman
	if(gameObject.typeTag == TYPE_OBJ_BALL){
		float distance = distanceBetweenPoints(ccp(gameObject.body->GetPosition().x,gameObject.body->GetPosition().y),
			ccp(gunman.body->GetPosition().x, gunman.body->GetPosition().y));
			
		if(distance < audibleRange){
			float gain = (audibleRange-distance)/audibleRange;
			float pan = (gameObject.body->GetPosition().x - gunman.body->GetPosition().x)/distance;
			float pitch = ((((GameIsoObject*)gameObject).inGameSize / 10.0f) * -1) + 2;
						
			if(distance < audibleRange){
				[self playBallSoundWithGain:gain pan:pan pitch:pitch];
			}
		}
	}
}

-(void) playBallSoundWithGain:(float)gain pan:(float)pan pitch:(float)pitch {
	//Play the sound using the non-interruptible source group
	[[CDAudioManager sharedManager].soundEngine playSound:ballSource.soundId sourceGroupId:CGROUP_NON_INTERRUPTIBLE pitch:pitch pan:pan gain:gain loop:NO];
}

-(void) cleanRecipe {
	//Release all sounds
	[ballSource release];
	[forestBirdsSource release];
	[gunmanJumpSource release];
	[fireBallSource release];
	
	//End engine
	[SimpleAudioEngine end];
	sae = nil;

	[super cleanRecipe];
}

@end
