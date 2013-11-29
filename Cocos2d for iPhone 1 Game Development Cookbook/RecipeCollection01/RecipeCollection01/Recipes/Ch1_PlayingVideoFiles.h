#import "cocos2d.h"
#import "Recipe.h"

#import <MediaPlayer/MediaPlayer.h>

//Interface
@interface Ch1_PlayingVideoFiles : Recipe
{
	MPMoviePlayerController *moviePlayer;
}

-(CCLayer*) runRecipe;
-(void)moviePlayBackDidFinish:(NSNotification*)notification;
-(void)playMovie;
-(void)stopMovie;
-(void)cleanRecipe;

@end



//Implementation
@implementation Ch1_PlayingVideoFiles

-(CCLayer*) runRecipe {
	//Load our video file
	NSURL *url = [NSURL fileURLWithPath:[[NSBundle mainBundle] pathForResource:@"example_vid" ofType:@"mov"]];
	
	//Create a MPMoviePlayerController object
	moviePlayer = [[MPMoviePlayerController alloc] initWithContentURL:url];

	//Register to receive a notification when the movie has finished playing.
	[[NSNotificationCenter defaultCenter] addObserver:self
		selector:@selector(moviePlayBackDidFinish:)
		name:MPMoviePlayerPlaybackDidFinishNotification
		object:moviePlayer];

	//Set the movie's control style and whether or not it should automatically play.
	if ([moviePlayer respondsToSelector:@selector(setFullscreen:animated:)]) {
		//Use the new 3.2 style API.
		moviePlayer.controlStyle = MPMovieControlStyleNone;
		moviePlayer.shouldAutoplay = YES;
				
		CGSize winSize = [[CCDirector sharedDirector] winSize];
		moviePlayer.view.frame = CGRectMake(45, 50, winSize.width-90, winSize.height-100);
		[[[CCDirector sharedDirector] openGLView] addSubview:moviePlayer.view];
	} else {
		//Use the old 2.0 style API.
		moviePlayer.movieControlMode = MPMovieControlModeHidden;
		[self playMovie];
	}
	
	return self;
}

-(void)moviePlayBackDidFinish:(NSNotification*)notification {
	//If playback is finished we stop the movie.
	[self stopMovie];
}

-(void)playMovie {
	//We do not play the movie if it is already playing.
	MPMoviePlaybackState state = moviePlayer.playbackState;
	if(state == MPMoviePlaybackStatePlaying) {
		NSLog(@"Movie is already playing.");
		return;
	}
	
	[moviePlayer play];
}

-(void)stopMovie {
	//We do not stop the movie if it is already stopped.
	MPMoviePlaybackState state = moviePlayer.playbackState;
	if(state == MPMoviePlaybackStateStopped) {
		NSLog(@"Movie is already stopped.");
		return;
	}
	
	//Since playback has finished we remove the observer.
	[[NSNotificationCenter defaultCenter] removeObserver:self
		name:MPMoviePlayerPlaybackDidFinishNotification
        object:moviePlayer];

	//If the moviePlayer.view was added to the openGL view, it needs to be removed.
	if ([moviePlayer respondsToSelector:@selector(setFullscreen:animated:)]) {
		[moviePlayer.view removeFromSuperview];
	}
}

-(void)cleanRecipe {
	[self stopMovie];
	[moviePlayer release];
	[super cleanRecipe];
}

@end