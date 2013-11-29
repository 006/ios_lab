//  OpenEars version 0.9.0.2
//  http://www.politepix.com/openears
//
//  PocketsphinxController.h
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

#import <Foundation/Foundation.h>
#import "OpenEarsEventsObserver.h"
#import "ContinuousModel.h"

@interface PocketsphinxController : NSObject <OpenEarsEventsObserverDelegate> {

	NSThread *voiceRecognitionThread; // The loop would lock if run on the main thread so it has a background thread in which it always runs.
	ContinuousModel *continuousModel; // The continuous model is the actual recognition loop.
	OpenEarsEventsObserver *openEarsEventsObserver; // We use an OpenEarsEventsObserver here to get important information from other objects which may be instantiated.
}

// These are the OpenEars methods for controlling Pocketsphinx. You should use these.

- (void) stopListening; // Exits from the recognition loop.
- (void) startListening;  // Starts the recognition loop.
- (void) suspendRecognition; // Stops interpreting sounds as speech without exiting from the recognition loop. You do not need to call these methods on behalf of Flite.
- (void) resumeRecognition; // Starts interpreting sounds as speech after suspending recognition with the preceding method. You do not need to call these methods on behalf of Flite.

// Here are all the multithreading methods, you should never do anything with any of these.
- (void) startVoiceRecognitionThread;
- (void) stopVoiceRecognitionThread;
- (void) waitForVoiceRecognitionThreadToFinish;
- (void) startVoiceRecognitionThreadAutoreleasePool;

// Suspend and resume that is initiated by Flite. Do not call these directly.
- (void) suspendRecognitionForFliteSpeech;
- (void) resumeRecognitionForFliteSpeech;

- (Float32) pocketsphinxInputLevel; // This gives the input metering levels. This can only be run in a background thread that you create, otherwise it will block recognition

@property (nonatomic, retain) NSThread *voiceRecognitionThread; // The loop would lock if run on the main thread so it has a background thread in which it always runs.
@property (nonatomic, retain) ContinuousModel *continuousModel; // The continuous model is the actual recognition loop.
@property (nonatomic, retain) OpenEarsEventsObserver *openEarsEventsObserver; // We use an OpenEarsEventsObserver here to get important information from other objects which may be instantiated.
@end
