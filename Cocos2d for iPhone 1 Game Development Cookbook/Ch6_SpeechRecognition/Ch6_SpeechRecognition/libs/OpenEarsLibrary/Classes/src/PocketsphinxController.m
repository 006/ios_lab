
//  OpenEars version 0.9.02
//  http://www.politepix.com/openears
//
//  PocketsphinxController.m
//  OpenEars
//
//  PocketsphinxController is a class which controls the creation and management of
//  a continuous speech recognition loop.
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

#import "PocketsphinxController.h"
#import "ContinuousModel.h"
#import "OpenEarsConfig.h"

@implementation PocketsphinxController

@synthesize voiceRecognitionThread; // A thread so that we can instantiate the continuous recognition loop in the background.
@synthesize continuousModel; // The class containing the actual continuous loop.
@synthesize openEarsEventsObserver; // A class that we'll use to be informed of some important status changes in other parts of OpenEars.

#pragma mark -
#pragma mark Initialization and Memory Management

- (void)dealloc {
	openEarsEventsObserver.delegate = nil; // When releasing a class that uses a delegate of OpenEarsEventsObserver, set its delegate to nil before releasing.
	[openEarsEventsObserver release]; 
	[voiceRecognitionThread release];
	[continuousModel release];
    [super dealloc];
}

- (id) init
{
    if ( self = [super init] )
    {
		[self.openEarsEventsObserver setDelegate:self]; // Before we start we need to sign up for the delegate methods of OpenEarsEventsObserver so we can receive important information about the other OpenEars classes.
		self.continuousModel.exitListeningLoop = 0; // We'll change this when we're ready to exit the loop, for now initialize it to zero.
		self.continuousModel.inMainRecognitionLoop = FALSE; // We aren't in the main recognition loop.
		[self.continuousModel openDevice]; // Let's open the audio device before we do anything else.
    }
    return self;
}

#pragma mark -
#pragma mark Lazy Accessors

// A lazy accessor for the continuous loop.
- (ContinuousModel *)continuousModel {
	if (continuousModel == nil) {
		continuousModel = [[ContinuousModel alloc] init];
	}
	return continuousModel;
}

// A lazy accessor for the OpenEarsEventsObserver.
- (OpenEarsEventsObserver *)openEarsEventsObserver {
	if (openEarsEventsObserver == nil) {
		openEarsEventsObserver = [[OpenEarsEventsObserver alloc] init];
	}
	return openEarsEventsObserver;
}


#pragma mark -
#pragma mark OpenEarsEventsObserver Delegate Methods

// We're just asking for a few delegate methods from OpenEarsEventsObserver so we can react to some specific situations.

- (void) audioRouteDidChangeToRoute:(NSString *)newRoute { // We want to know if the audio route has changed because the ContinuousModel does something different while recording for the headphones route only.
	[self.continuousModel continuousAudioDevice]->currentRoute = (CFStringRef)newRoute;

}

- (void) fliteDidStartSpeaking { // We need to know when Flite is talking because under some circumstances we will suspend recognition at that time.
#if TARGET_IPHONE_SIMULATOR
	if(self.continuousModel.inMainRecognitionLoop == TRUE) { // The simulator will crash if we query the current route
#else
	if(self.continuousModel.inMainRecognitionLoop == TRUE && [[NSString stringWithFormat:@"%@",[self.continuousModel continuousAudioDevice]->currentRoute] isEqualToString:@"HeadsetInOut"]==FALSE) { // Only suspend listening if we aren't using headphones
#endif
		[self suspendRecognitionForFliteSpeech];
	}
}
	
- (void) fliteDidFinishSpeaking { // We need to know when Flite is done talking because under some circumstances we will resume recognition at that time.
#if TARGET_IPHONE_SIMULATOR
	if(self.continuousModel.inMainRecognitionLoop == TRUE) { // The simulator will crash if we query the current route
#else	
	if(self.continuousModel.inMainRecognitionLoop == TRUE && [[NSString stringWithFormat:@"%@",[self.continuousModel continuousAudioDevice]->currentRoute] isEqualToString:@"HeadsetInOut"]==FALSE) { // Only resume listening if we suspended it due to not using headphones
#endif	
		[self resumeRecognitionForFliteSpeech];
	}	
}
		
		
#pragma mark -
#pragma mark Recognition Control Methods

- (void) startListening { // This is an externally-called method that tells this class to detach a new thread and eventually start up the listening loop.
	[self startVoiceRecognitionThread];
}

- (void) stopListening { // This is an externally-called method that tells this class to exit the voice recognition loop and eventually close up the voice recognition thread.
	self.continuousModel.exitListeningLoop = 1;
	[self stopVoiceRecognitionThread];
}

- (void) suspendRecognitionForFliteSpeech { // We will react a little differently to the situation in which Flite is asking for a suspend than when the developer is.
	if(self.continuousModel.inMainRecognitionLoop && [self.continuousModel continuousAudioDevice]->speechIsInProgress == 1) {
		[self.continuousModel continuousAudioDevice]->speechIsInProgress = 0;
	}
}

- (void) resumeRecognitionForFliteSpeech { // We will react a little differently to the situation in which Flite is asking for a resume than when the developer is.
	
	if(self.continuousModel.inMainRecognitionLoop && [self.continuousModel continuousAudioDevice]->speechIsInProgress == 0) {	
		[self.continuousModel continuousAudioDevice]->speechIsInProgress = 1;
	}
}

- (void) suspendRecognition { // This is the externally-called method that tells the class to suspend recognition without exiting the recognition loop.
	if(self.continuousModel.inMainRecognitionLoop && [self.continuousModel continuousAudioDevice]->recordData == 1) { // If it's safe and relevant to try to suspend,
		[self.continuousModel continuousAudioDevice]->recordData = 0; // Tell the driver not to record data.
		NSDictionary *userInfoDictionary = [NSDictionary dictionaryWithObject:@"PocketsphinxDidSuspendRecognition" forKey:@"OpenEarsNotificationType"]; // And tell OpenEarsEventsObserver we've suspended.
		NSNotification *notification = [NSNotification notificationWithName:@"OpenEarsNotification" object:nil userInfo:userInfoDictionary];
		[[NSNotificationCenter defaultCenter] performSelectorOnMainThread:@selector(postNotification:) withObject:notification waitUntilDone:NO];
	}
}

- (void) resumeRecognition { // This is the externally-called method that tells the class to resume recognition after it was suspended without exiting the recognition loop.
	
	if(self.continuousModel.inMainRecognitionLoop && [self.continuousModel continuousAudioDevice]->recordData == 0) {	 // If it's safe and relevant to try to resume,
		[self.continuousModel continuousAudioDevice]->recordData = 1;// Tell the driver to record data.
	
		NSDictionary *userInfoDictionary = [NSDictionary dictionaryWithObject:@"PocketsphinxDidResumeRecognition" forKey:@"OpenEarsNotificationType"]; // And tell OpenEarsEventsObserver we've resumed.
		NSNotification *notification = [NSNotification notificationWithName:@"OpenEarsNotification" object:nil userInfo:userInfoDictionary];
		[[NSNotificationCenter defaultCenter] performSelectorOnMainThread:@selector(postNotification:) withObject:notification waitUntilDone:NO];
	}
}

- (Float32) pocketsphinxInputLevel { // This can only be run in a background thread that you create, otherwise it will block recognition.  It returns the metering level of the Pocketsphinx audio device at the moment it's called.
	
	return pocketsphinxAudioDeviceMeteringLevel([self.continuousModel continuousAudioDevice]);
}

#pragma mark -
#pragma mark Pocketsphinx Threading

- (void) startVoiceRecognitionThreadAutoreleasePool { // This is the autorelease pool in which the actual business of our loop is handled.
	
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init]; // Create the pool.
	
	[NSThread setThreadPriority:.9];     // Give the voice recognition thread high priority for accuracy, though slightly lower than speech (which only occurs rarely, so generally this thread will have the highest priority).
    [self.continuousModel listeningLoop]; // Call the listening loop inside of the autorelease pool.
	[pool drain]; // Drain the pool.
}

- (void)startVoiceRecognitionThread { // Create a new thread for voice recognition.
    if (voiceRecognitionThread != nil) { // If it already exists, stop it.
        [voiceRecognitionThread cancel];
		[self waitForVoiceRecognitionThreadToFinish];
    }
	
    NSThread *voiceRecThread = [[NSThread alloc] initWithTarget:self selector:@selector(startVoiceRecognitionThreadAutoreleasePool) object:nil]; // Then create a thread with the characteristics we want,
    self.voiceRecognitionThread = voiceRecThread; // And give our class thread object those characteristics.
    [voiceRecThread release]; // Get rid of the first thread.
	voiceRecThread = nil; // Set it to nil.
    [self.voiceRecognitionThread start]; // Ask the class voice recognition thread to start up.
}

- (void)waitForVoiceRecognitionThreadToFinish { 
    while (voiceRecognitionThread && ![voiceRecognitionThread isFinished]) { // Wait for the thread to finish.
		[NSThread sleepForTimeInterval:0.1]; // If the thread can't finish yet, sleep.
    }	
}

- (void)stopVoiceRecognitionThread { // This will be called before releasing this class.
    [self.voiceRecognitionThread cancel]; // Ask the thread to stop,
	[self waitForVoiceRecognitionThreadToFinish]; // Wait for it to finish,
    self.voiceRecognitionThread = nil; // Set it to nil if that happens successfully.
	
}

@end
