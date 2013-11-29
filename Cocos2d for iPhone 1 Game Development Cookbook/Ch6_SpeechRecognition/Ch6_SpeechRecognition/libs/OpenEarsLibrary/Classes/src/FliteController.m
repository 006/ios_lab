//  OpenEars version 0.9.02
//  http://www.politepix.com/openears
//
//  FliteController.m
//  OpenEars
//
//  Created by Halle Winkler on 8/30/10.
//  Copyright Halle Winkler 2010. All rights reserved.
//  http://www.politepix.com
//  Contact at http://www.politepix.com/contact
//
//  This file is licensed under the Common Development and Distribution License (CDDL) Version 1.0
//  http://www.opensource.org/licenses/cddl1.txt or see included file license.txt
//  with the single exception to the license that you may distribute executable-only versions
//  of software using OpenEars files without making source code available under the terms of CDDL Version 1.0 
//  paragraph 3.1 if source code to your software isn't otherwise available, and without including a notice in 
//  that case that that source code is available.

#import "FliteController.h"

#import "OpenEarsConfig.h"

@implementation FliteController

@synthesize audioPlayer; // The audio player used to play back the speech.
@synthesize voice; // Flite makes use of this voice object.
@synthesize openEarsEventsObserver; // We'll use the OpenEarsEventsObserver class to stay informed about important changes in status to other parts of OpenEars.

// Conditionally add some function declarations depending on what voice the developer has chosen to use in OpenEarsVoiceConfig.h.

#ifdef cmu_us_awb8k
void unregister_cmu_us_awb8k(cst_voice *vox);
cst_voice *register_cmu_us_awb8k(const char *voxdir);
#endif

#ifdef cmu_us_rms8k
void unregister_cmu_us_rms8k(cst_voice *vox);
cst_voice *register_cmu_us_rms8k(const char *voxdir);
#endif

#ifdef cmu_us_slt8k
void unregister_cmu_us_slt8k(cst_voice *vox);
cst_voice *register_cmu_us_slt8k(const char *voxdir);
#endif

#ifdef cmu_time_awb
void unregister_cmu_time_awb(cst_voice *vox);
cst_voice *register_cmu_time_awb(const char *voxdir);
#endif

#ifdef cmu_us_awb
void unregister_cmu_us_awb(cst_voice *vox);
cst_voice *register_cmu_us_awb(const char *voxdir);
#endif

#ifdef cmu_us_kal
void unregister_cmu_us_kal(cst_voice *vox);
cst_voice *register_cmu_us_kal(const char *voxdir);
#endif

#ifdef cmu_us_kal16
void unregister_cmu_us_kal16(cst_voice *vox);
cst_voice *register_cmu_us_kal16(const char *voxdir);
#endif

#ifdef cmu_us_rms
void unregister_cmu_us_rms(cst_voice *vox);
cst_voice *register_cmu_us_rms(const char *voxdir);
#endif

#ifdef cmu_us_slt
void unregister_cmu_us_slt(cst_voice *vox);
cst_voice *register_cmu_us_slt(const char *voxdir);
#endif

#pragma mark -
#pragma mark Memory Management and Initialization


- (void)dealloc {
	
// Conditionally unregister the voice that the developer has chosen to use in OpenEarsVoiceConfig.h.
#ifdef cmu_us_awb8k
	unregister_cmu_us_awb8k(voice);
#endif
	
#ifdef cmu_us_rms8k
	unregister_cmu_us_rms8k(voice);
#endif
	
#ifdef cmu_us_slt8k
	unregister_cmu_us_slt8k(voice);
#endif
	
#ifdef cmu_time_awb
	unregister_cmu_time_awb(voice);
#endif
	
#ifdef cmu_us_awb
	unregister_cmu_us_awb(voice);
#endif
	
#ifdef cmu_us_kal
	unregister_cmu_us_kal(voice);
#endif
	
#ifdef cmu_us_kal16
	unregister_cmu_us_kal16(voice);
#endif
	
#ifdef cmu_us_rms
	unregister_cmu_us_rms(voice);
#endif
	
#ifdef cmu_us_slt
	unregister_cmu_us_slt(voice);
#endif
	[audioPlayer release];
	openEarsEventsObserver.delegate = nil; // It's always a good idea to set the OpenEarsEventsObserver delegate to nil before releasing it.
	[openEarsEventsObserver release];
    [super dealloc];
}

- (id) init
{
    if ( self = [super init] )
    {
		[self.openEarsEventsObserver setDelegate:self]; // Sign up for the OpenEarsEventsObserver delegate methods at initialization.

		flite_init(); // Initialize Flite so it's always ready to run.
		
		// Conditionally register the voice that the developer has chosen to use in OpenEarsVoiceConfig.h.
#ifdef cmu_us_awb8k
		voice = register_cmu_us_awb8k(NULL);
#endif
		
#ifdef cmu_us_rms8k
		voice = register_cmu_us_rms8k(NULL);
#endif
		
#ifdef cmu_us_slt8k
		voice = register_cmu_us_slt8k(NULL);
#endif
		
#ifdef cmu_time_awb
		voice = register_cmu_time_awb(NULL);
#endif
		
#ifdef cmu_us_awb
		voice = register_cmu_us_awb(NULL);
#endif
		
#ifdef cmu_us_kal
		voice = register_cmu_us_kal(NULL);
#endif
		
#ifdef cmu_us_kal16
		voice = register_cmu_us_kal16(NULL);
#endif
		
#ifdef cmu_us_rms
		voice = register_cmu_us_rms(NULL);
#endif
		
#ifdef cmu_us_slt
		voice = register_cmu_us_slt(NULL);
#endif
	
    }
    return self;
}

#pragma mark -
#pragma mark Lazy Accessors

// Lazy accessor for the AVAudioPlayer that will play back the file recorded by Flite.
- (AVAudioPlayer *)audioPlayer {
	if (audioPlayer == nil) {
		NSError *urlError = nil;
		audioPlayer = [[AVAudioPlayer alloc] initWithContentsOfURL:[NSURL URLWithString:[NSTemporaryDirectory() stringByAppendingPathComponent: @"statement.wav"]] error:&urlError];
		if(urlError) {
#ifdef OPENEARSLOGGING
			NSLog(@"OPENEARSLOGGING: Error while loading soundfile for Flite: %@", [urlError description]);		
#endif
		}
			
		audioPlayer.meteringEnabled = TRUE; // Enable metering so we can expose the metering function to developers.
		audioPlayer.delegate = self; // I'm not sure if I'm ultimately going to use these AVAudioPlayerDelegate methods since one of them was being a bit weird but this is on for now.
	}
	return audioPlayer;
}

// Lazy accessor for the OpenEarsEventsObserver object.
- (OpenEarsEventsObserver *)openEarsEventsObserver {
	if (openEarsEventsObserver == nil) {
		openEarsEventsObserver = [[OpenEarsEventsObserver alloc] init];
	}
	return openEarsEventsObserver;
}


#pragma mark -
#pragma mark AVAudioPlayer Delegate Methods

// I think I'm just using these for interruption notifications and error logging at the moment.

/* if an error occurs while decoding it will be reported to the delegate. */
- (void)audioPlayerDecodeErrorDidOccur:(AVAudioPlayer *)player error:(NSError *)error {
#ifdef OPENEARSLOGGING

	NSLog(@"OPENEARSLOGGING: AVAudioPlayer Decode Error Did Occur: %@.", [error description]);			
	
#endif
}

/* audioPlayerBeginInterruption: is called when the audio session has been interrupted while the player was playing. The player will have been paused. */
- (void)audioPlayerBeginInterruption:(AVAudioPlayer *)player {
	[self interruptionRoutine:player];
}

/* audioPlayerEndInterruption:withFlags: is called when the audio session interruption has ended and this player had been interrupted while playing. */
/* Currently the only flag is AVAudioSessionInterruptionFlags_ShouldResume. */
#if defined(__MAC_10_7) || defined(__IPHONE_4_0)
- (void)audioPlayerEndInterruption:(AVAudioPlayer *)player withFlags:(NSUInteger)flags {
	[self interruptionOverRoutine:player];
}

#else

/* audioPlayerEndInterruption: is called when the preferred method, audioPlayerEndInterruption:withFlags:, is not implemented. */
- (void)audioPlayerEndInterruption:(AVAudioPlayer *)player {
	[self interruptionOverRoutine:player];
}
#endif

#pragma mark -
#pragma mark Interruption Handling

// If an interruption is received or ended either via the AVAudioPlayer delegate methods or OpenEarsEventsObserver, stop playing.

- (void) interruptionRoutine:(AVAudioPlayer *)player {
	
#ifdef OPENEARSLOGGING
	
	NSLog(@"OPENEARSLOGGING: AVAudioPlayer received an interruption");			
	
#endif
	
	
	if(player.playing == TRUE) {
		[player stop];
		
	}
}

- (void) interruptionOverRoutine:(AVAudioPlayer *)player {
#ifdef OPENEARSLOGGING
	
	NSLog(@"OPENEARSLOGGING: AVAudioPlayer interruption ended");			
	
#endif
	
	if(player.playing == TRUE) {
		[player stop];
	}
}

#pragma mark -
#pragma mark Notification Routing

// Notify OpenEarsEventsObserver that Flite needs recognition suspended due to speech.

- (void) sendSuspendNotificationOnMainThread {
	
	NSDictionary *userInfoDictionary = [NSDictionary dictionaryWithObject:@"FliteDidStartSpeaking" forKey:@"OpenEarsNotificationType"];
	NSNotification *notification = [NSNotification notificationWithName:@"OpenEarsNotification" object:nil userInfo:userInfoDictionary];
	[[NSNotificationCenter defaultCenter] performSelector:@selector(postNotification:) withObject:notification afterDelay:0.0];

}

- (void) sendResumeNotificationOnMainThread {

	NSDictionary *userInfoDictionary = [NSDictionary dictionaryWithObject:@"FliteDidFinishSpeaking" forKey:@"OpenEarsNotificationType"];
	NSNotification *notification = [NSNotification notificationWithName:@"OpenEarsNotification" object:nil userInfo:userInfoDictionary];
	// need delay of length of buffer next (half a sec) since it is the last buffer-full that is being analyzed, not the next one.
	[[NSNotificationCenter defaultCenter] performSelector:@selector(postNotification:) withObject:notification afterDelay:0.5];
	
	
}

#pragma mark -
#pragma mark OpenEarsEventsObserver delegate methods

// If the developer has uncommented the USERCANINTERRUPTSPEECH define in OpenEarsConfig.h, interrupt the playback if a new hypothesis is received via OpenEarsEventsObserver.

- (void) pocketsphinxDidReceiveHypothesis:(NSString *)hypothesis recognitionScore:(NSString *)recognitionScore utteranceID:(NSString *)utteranceID {
#ifdef USERCANINTERRUPTSPEECH
	[self interruptTalking];
#endif
}

#pragma mark -
#pragma mark View Controller Methods

// Externally-accessed method that returns the playback levels of the Flite audio file.

- (Float32) fliteOutputLevel {
	if([self.audioPlayer isPlaying] == TRUE) {
		[self.audioPlayer updateMeters];		
		return [self.audioPlayer averagePowerForChannel:0];
	} return 0.0;
}

- (void) interruptTalking {
	
	/* If there is a statement being made already, let's do some cleanup */

	if([self.audioPlayer isPlaying]) {
		[self.audioPlayer stop];
	}
	
	if(self.audioPlayer != nil) {
		self.audioPlayer = nil;
	}

}

- (void) donePlayingOnMainThread { // Alternate method for avoiding using the AVAudioPlayer delegate method to deliver "done playing" info.
	self.audioPlayer = nil;
	[self performSelectorOnMainThread:@selector(sendResumeNotificationOnMainThread) withObject:nil waitUntilDone:NO];
}

- (void) donePlaying { // Alternate method for avoiding using the AVAudioPlayer delegate method to deliver "done playing" info.
	[self performSelector:@selector(donePlayingOnMainThread) withObject:nil afterDelay:self.audioPlayer.duration];
}

- (void) say:(NSString *)statement {

	// If there is a statement being made already, let's do some cleanup.
	[self interruptTalking];
#ifdef OPENEARSLOGGING
	NSLog(@"I'm running flite"); NSTimeInterval start = [NSDate timeIntervalSinceReferenceDate]; // If logging is on, let's time the Flite processing time so the developer can see if the voice chosen is fast enough.
#endif	
	flite_text_to_speech((char *)[statement UTF8String],voice,(char *)[[NSTemporaryDirectory() stringByAppendingPathComponent: @"statement.wav"] UTF8String]); // The Flite method for turning text into speech.
#ifdef OPENEARSLOGGING
	NSLog(@"I'm done running flite and it took %f seconds", [NSDate timeIntervalSinceReferenceDate] - start); // Deliver the timing info if logging is on.
#endif
	[self performSelectorOnMainThread:@selector(sendSuspendNotificationOnMainThread) withObject:nil waitUntilDone:NO]; // Send a suspend notification so (under some circumstances) recognition isn't on while speaking is in progress.

	[self.audioPlayer play]; // Play the sound created by Flite.
	[self performSelectorOnMainThread:@selector(donePlaying) withObject:nil waitUntilDone:NO]; // Do the done playing routine.
}


@end

