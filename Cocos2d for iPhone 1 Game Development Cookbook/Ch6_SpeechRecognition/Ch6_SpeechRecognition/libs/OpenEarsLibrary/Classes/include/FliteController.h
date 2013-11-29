//  OpenEars version 0.9.0.2
//  http://www.politepix.com/openears
//
//  FliteController.h
//  OpenEars
//
//  FliteController is a class which manages text-to-speech
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

#import <AVFoundation/AVFoundation.h>
#import "flite.h"
#import "OpenEarsEventsObserver.h"

@interface FliteController : NSObject <AVAudioPlayerDelegate, OpenEarsEventsObserverDelegate> {

	AVAudioPlayer *audioPlayer; // Plays back the speech
	cst_voice *voice; // Flite uses this to generate speech
	OpenEarsEventsObserver *openEarsEventsObserver; // Observe status changes from audio sessions and Pocketsphinx
}

// These are the only methods to be called directly by an OpenEars project
- (void) say:(NSString *)statement;
- (Float32) fliteOutputLevel;
// End methods to be called directly by an OpenEars project

// Interruption handling
- (void) interruptionRoutine:(AVAudioPlayer *)player;
- (void) interruptionOverRoutine:(AVAudioPlayer *)player;

// Handling not doing voice recognition on Flite speech, do not directly call
- (void) sendResumeNotificationOnMainThread;
- (void) sendSuspendNotificationOnMainThread;
- (void) interruptTalking;

@property (nonatomic, assign) cst_voice *voice;  // Flite uses this to generate speech
@property (nonatomic, retain) AVAudioPlayer *audioPlayer;  // Plays back the speech
@property (nonatomic, retain) OpenEarsEventsObserver *openEarsEventsObserver; // Observe status changes from audio sessions and Pocketsphinx

@end





