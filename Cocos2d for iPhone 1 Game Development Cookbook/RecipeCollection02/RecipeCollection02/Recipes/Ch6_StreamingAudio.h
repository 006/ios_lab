#import "Recipe.h"
#import <MediaPlayer/MediaPlayer.h>
#import "AppDelegate.h"

@interface Ch6_StreamingAudio : Recipe
{
	MPMoviePlayerController	*moviePlayer;
	NSMutableArray *streamingSources;
	int sourceIndex;
	bool isPlaying;
}

-(CCLayer*) runRecipe;

-(CCMenuItemSprite*) menuItemFromSpriteFile:(NSString*)fn target:(id)target selector:(SEL)sel;

-(void)playMusic:(id)sender;
-(void)pauseMusic:(id)sender;
-(void)stopMusic:(id)sender;
-(void)nextSong:(id)sender;
-(void)previousSong:(id)sender;

-(void)setIsPlaying;
-(void)setStreamSource;

@end

@implementation Ch6_StreamingAudio

-(CCLayer*) runRecipe {
	[super runRecipe];
	
	//Show message
	[self showMessage:@"Hit the play button to start the stream."];

	//Create music player buttons
	[[CCSpriteFrameCache sharedSpriteFrameCache] addSpriteFramesWithFile:@"music_player.plist"];

	CCMenuItemSprite *prevItem = [self menuItemFromSpriteFile:@"music_player_prev.png" target:self selector:@selector(previousSong:)];
	CCMenuItemSprite *stopItem = [self menuItemFromSpriteFile:@"music_player_stop.png" target:self selector:@selector(stopMusic:)];
	CCMenuItemSprite *playItem = [self menuItemFromSpriteFile:@"music_player_play.png" target:self selector:@selector(playMusic:)];
	CCMenuItemSprite *pauseItem = [self menuItemFromSpriteFile:@"music_player_pause.png" target:self selector:@selector(pauseMusic:)];
	CCMenuItemSprite *nextItem = [self menuItemFromSpriteFile:@"music_player_next.png" target:self selector:@selector(nextSong:)];

	//Create menu
	CCMenu *menu = [CCMenu menuWithItems:prevItem, stopItem, playItem, pauseItem, nextItem, nil];
	[menu alignItemsHorizontally];
	menu.position = ccp(240,160);
	[self addChild:menu];
	
	//Initial variable values
	sourceIndex = 0;
	isPlaying = NO;
	message.scale = 0.45f;
	message.position = ccp(240,220);
	
	//Streaming sources
	streamingSources = [[NSMutableArray alloc] init];
	[streamingSources addObject:@"http://shoutmedia.abc.net.au:10326"];
	[streamingSources addObject:@"http://audioplayer.wunderground.com/drgruver/Philadelphia.mp3.m3u"];
	[streamingSources addObject:@"http://s8.mediastreaming.it:7050/"];
	[streamingSources addObject:@"http://www.radioparadise.com/musiclinks/rp_64aac.m3u"];
	[streamingSources addObject:@"http://streaming.wrek.org:8000/wrek_HD-2.m3u"];
	
	//Init movie playing (music streamer in this case)
	moviePlayer = [[MPMoviePlayerController alloc] init];
	moviePlayer.movieSourceType = MPMovieSourceTypeStreaming;
	moviePlayer.view.hidden = YES;
	//[[AppDelegate sharedAppDelegate].window addSubview:moviePlayer.view];
	
	[((AppDelegate*)[UIApplication sharedApplication].delegate).window addSubview:moviePlayer.view];

	//Set initial stream source
	[self setStreamSource];

	return self;
}

//Helper method to create sprite buttons
-(CCMenuItemSprite*) menuItemFromSpriteFile:(NSString*)fn target:(id)target selector:(SEL)sel {
	CCSprite *normalSprite = [CCSprite spriteWithSpriteFrameName:fn];
	CCSprite *selectedSprite = [CCSprite spriteWithSpriteFrameName:fn];
	selectedSprite.color = ccc3(128,128,180); [selectedSprite setBlendFunc: (ccBlendFunc) { GL_ONE, GL_ONE }];

	CCMenuItemSprite *item = [CCMenuItemSprite itemFromNormalSprite:normalSprite 
		selectedSprite:selectedSprite target:target selector:sel];
	item.scale = 0.5f;
	
	return item;
}

//Play callback
-(void)playMusic:(id)sender {
	[moviePlayer play];
	[self showMessage:[NSString stringWithFormat:@"Now Playing URL: \n%@", 
		[NSURL URLWithString:[streamingSources objectAtIndex:sourceIndex]]]];
}

//Pause callback
-(void)pauseMusic:(id)sender{
	[moviePlayer pause];
}

//Stop callback
-(void)stopMusic:(id)sender{
	[moviePlayer stop];
}

//Next callback
- (void)nextSong:(id)sender {
	[self setIsPlaying];
	
	sourceIndex++;
	if(sourceIndex > [streamingSources count]-1){
		sourceIndex = 0;
	}

	[self setStreamSource];
}

//Previous callback
- (void)previousSong:(id)sender {
	[self setIsPlaying];

	sourceIndex--;
	if(sourceIndex < 0){
		sourceIndex = [streamingSources count]-1;
	}
	
	[self setStreamSource];
}

-(void) setIsPlaying {
	if(moviePlayer.playbackState == MPMoviePlaybackStatePlaying){
		isPlaying = YES;
	}
}

-(void) setStreamSource {
	[moviePlayer stop];
	
	moviePlayer.contentURL = [NSURL URLWithString:[streamingSources objectAtIndex:sourceIndex]];
	
	if(isPlaying){
		[self playMusic:nil];
	}
}

-(void) dealloc {
	//Release music player if necessary
	if(moviePlayer){ 
		[moviePlayer stop];
		[moviePlayer release];
		moviePlayer = nil;
	}

	[super dealloc];
}

-(void) cleanRecipe {
	//Stop player
	[moviePlayer stop];

	//Release player
    [moviePlayer release];
    moviePlayer = nil;

	[super cleanRecipe];
}

@end
