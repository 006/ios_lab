#import "Recipe.h"
#import <MediaPlayer/MediaPlayer.h>
#import "AppDelegate.h"

@interface Ch6_iPodLibrary : Recipe <MPMediaPickerControllerDelegate>
{
	MPMusicPlayerController	*musicPlayer;	
	MPMediaItemCollection *userMediaItemCollection;
	
	CCLabelTTF *songLabel;
	CCLabelTTF *artistLabel;
	CCLabelTTF *albumLabel;
	CCSprite *albumArt;
}

-(CCLayer*) runRecipe;

-(CCMenuItemSprite*) menuItemFromSpriteFile:(NSString*)fn target:(id)target selector:(SEL)sel;

-(void)handleNowPlayingItemChanged:(id)notification;

-(void)playMusic:(id)sender;
-(void)pauseMusic:(id)sender;
-(void)stopMusic:(id)sender;
-(void)nextSong:(id)sender;
-(void)previousSong:(id)sender;
-(void)openMediaPicker:(id)sender;

@end

@implementation Ch6_iPodLibrary

-(CCLayer*) runRecipe {
	[super runRecipe];
	
	//Device detection
	NSString *model = [[UIDevice currentDevice] model];
	
	//Show a blank recipe if we use the simulator
	if([model isEqualToString:@"iPhone Simulator"]){
		message.position = ccp(240,250);
		[self showMessage:@"This recipe is not compatible with the Simulator. \nPlease connect a device."];
		return self;
	}

	//Show message
	[self showMessage:@"Press the green button to choose a song."];

	//Create music player buttons
	[[CCSpriteFrameCache sharedSpriteFrameCache] addSpriteFramesWithFile:@"music_player.plist"];

	CCMenuItemSprite *prevItem = [self menuItemFromSpriteFile:@"music_player_prev.png" target:self selector:@selector(previousSong:)];
	CCMenuItemSprite *stopItem = [self menuItemFromSpriteFile:@"music_player_stop.png" target:self selector:@selector(stopMusic:)];
	CCMenuItemSprite *playItem = [self menuItemFromSpriteFile:@"music_player_play.png" target:self selector:@selector(playMusic:)];
	CCMenuItemSprite *pauseItem = [self menuItemFromSpriteFile:@"music_player_pause.png" target:self selector:@selector(pauseMusic:)];
	CCMenuItemSprite *pickerItem = [self menuItemFromSpriteFile:@"music_player_refresh.png" target:self selector:@selector(openMediaPicker:)];
	CCMenuItemSprite *nextItem = [self menuItemFromSpriteFile:@"music_player_next.png" target:self selector:@selector(nextSong:)];

	//Create menu
	CCMenu *menu = [CCMenu menuWithItems:prevItem, stopItem, playItem, pauseItem, pickerItem, nextItem, nil];
	[menu alignItemsHorizontally];
	menu.position = ccp(350,300);
	[self addChild:menu];

	//Add song labels
	songLabel = [CCLabelTTF labelWithString:@"" fontName:@"Marker Felt" fontSize:26];
	artistLabel = [CCLabelTTF labelWithString:@"" fontName:@"Marker Felt" fontSize:20];
	albumLabel = [CCLabelTTF labelWithString:@"" fontName:@"Marker Felt" fontSize:20];
	
	songLabel.position = ccp(240,240);
	artistLabel.position = ccp(240,210);
	albumLabel.position = ccp(240,20);

	[self addChild:songLabel z:2];
	[self addChild:artistLabel z:2];
	[self addChild:albumLabel z:2];

	//Init music player
    musicPlayer = [MPMusicPlayerController iPodMusicPlayer];
	[musicPlayer setRepeatMode:MPMusicRepeatModeAll]; 
    
    //Initial sync of display with music player state
    [self handleNowPlayingItemChanged:nil];
    
    //Register for music player notifications
    NSNotificationCenter *notificationCenter = [NSNotificationCenter defaultCenter];
    [notificationCenter addObserver:self 
                           selector:@selector(handleNowPlayingItemChanged:)
                               name:MPMusicPlayerControllerNowPlayingItemDidChangeNotification 
                             object:musicPlayer];
    [musicPlayer beginGeneratingPlaybackNotifications];

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

- (void)handleNowPlayingItemChanged:(id)notification {
    //Get the current playing item
    MPMediaItem *currentItem = musicPlayer.nowPlayingItem;
	
	//Set labels
	if([currentItem valueForProperty:MPMediaItemPropertyTitle]){
		[songLabel setString: [NSString stringWithFormat:@"%@",[currentItem valueForProperty:MPMediaItemPropertyTitle]]];
		[artistLabel setString: [NSString stringWithFormat:@"%@",[currentItem valueForProperty:MPMediaItemPropertyArtist]]];
		[albumLabel setString: [NSString stringWithFormat:@"%@",[currentItem valueForProperty:MPMediaItemPropertyAlbumTitle]]];
	}

	//Get album artwork
    MPMediaItemArtwork *artwork = [currentItem valueForProperty:MPMediaItemPropertyArtwork];
	UIImage *artworkImage = nil;
    if(artwork) { artworkImage = [artwork imageWithSize:CGSizeMake(100,100)]; }

	//Remove current album art if necessary
	if(albumArt){
		[self removeChild:albumArt cleanup:YES];
		albumArt = nil;
	}

	//Set album art
	if(artworkImage){	
		CCTexture2D *texture = [[[CCTexture2D alloc] initWithImage:artworkImage] autorelease];
		albumArt = [CCSprite spriteWithTexture:texture];
		[self addChild:albumArt z:1];
		albumArt.position = ccp(240,120);
		albumArt.scale = 0.25f;
	}
}

//Play callback
-(void)playMusic:(id)sender {
	[musicPlayer play];
}

//Pause callback
-(void)pauseMusic:(id)sender{
	[musicPlayer pause];
}

//Stop callback
-(void)stopMusic:(id)sender{
	[musicPlayer stop];
}

//Next callback
- (void)nextSong:(id)sender {
    [musicPlayer skipToNextItem];
}

//Previous callback
- (void)previousSong:(id)sender {
	//After 3.5 seconds hitting previous merely rewinds the song
    static NSTimeInterval skipToBeginningOfSongIfElapsedTimeLongerThan = 3.5;

    NSTimeInterval playbackTime = musicPlayer.currentPlaybackTime;
    if (playbackTime <= skipToBeginningOfSongIfElapsedTimeLongerThan) {
		//Previous song
        [musicPlayer skipToPreviousItem];
    } else {
		//Rewind to beginning of current song
        [musicPlayer skipToBeginning];
    }
}

//Add music callback
- (void)openMediaPicker:(id)sender {
	//Unit music MPMediaPickerController
    MPMediaPickerController *mediaPicker = [[MPMediaPickerController alloc] initWithMediaTypes:MPMediaTypeMusic];
    mediaPicker.delegate = self;
    mediaPicker.allowsPickingMultipleItems = YES;
	
	//Present picker as a modal view
    
	[[AppDelegate sharedAppDelegate].viewController presentModalViewController:mediaPicker animated:YES];
    [mediaPicker release];
}

- (void)mediaPicker: (MPMediaPickerController *)mediaPicker didPickMediaItems:(MPMediaItemCollection *)mediaItemCollection {
    //Dismiss the picker
    [[AppDelegate sharedAppDelegate].viewController dismissModalViewControllerAnimated:YES];
    
    //Assign the selected item(s) to the music player and start playback.
    [musicPlayer stop];
    [musicPlayer setQueueWithItemCollection:mediaItemCollection];
    [musicPlayer play];
}

- (void)mediaPickerDidCancel:(MPMediaPickerController *)mediaPicker {
	//User chose no items, dismiss the picker
    [[AppDelegate sharedAppDelegate].viewController dismissModalViewControllerAnimated:YES];
}

-(void) dealloc {
	//Release music player if necessary
	if(musicPlayer){ 
		[musicPlayer stop];
		[musicPlayer release];
		musicPlayer = nil;
	}

	[super dealloc];
}

-(void) cleanRecipe {
	//Stop player
	[musicPlayer stop];

    //Stop music player notifications
    [[NSNotificationCenter defaultCenter] removeObserver:self 
		name:MPMusicPlayerControllerNowPlayingItemDidChangeNotification object:musicPlayer];
    [[NSNotificationCenter defaultCenter] removeObserver:self 
        name:MPMusicPlayerControllerPlaybackStateDidChangeNotification object:musicPlayer];
    [[NSNotificationCenter defaultCenter] removeObserver:self 
        name:MPMusicPlayerControllerVolumeDidChangeNotification object:musicPlayer];
    [musicPlayer endGeneratingPlaybackNotifications];

	//Release player
    [musicPlayer release];
    musicPlayer = nil;

	[super cleanRecipe];
}

@end
