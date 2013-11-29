//  OpenEars version 0.9.02
//  http://www.politepix.com/openears
//
//  AudioSessionManager.m
//  OpenEars
//
//  AudioSessionManager is a class for initializing the Audio Session and forwarding changes in the Audio
//  Session to the OpenEarsEventsObserver class so they can be reacted to when necessary.
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

#import "AudioSessionManager.h"

@implementation AudioSessionManager

void audioSessionInterruptionListener(void *inClientData,
									  UInt32 inInterruptionState) { // Listen for interruptions to the Audio Session.
	
	
	// It's important on the iPhone to have the ability to react to an interruption in app audio such as an incoming or user-initiated phone call.
	// For Pocketsphinx it might be necessary to restart the recognition loop afterwards, or the app's UI might need to be reset or redrawn. 
	// By observing for the AudioSessionInterruptionDidBegin and AudioQueueInterruptionEnded NSNotifications and forwarding them to OpenEarsEventsObserver,
	// the developer using OpenEars can react to an interruption.
	 
	if (inInterruptionState == kAudioSessionBeginInterruption) { // There was an interruption.

#ifdef OPENEARSLOGGING		
		printf("OPENEARSLOGGING: The Audio Session was interrupted.\n");
#endif		
		NSDictionary *userInfoDictionary = [NSDictionary dictionaryWithObject:@"AudioSessionInterruptionDidBegin" forKey:@"OpenEarsNotificationType"]; // Send notification to OpenEarsEventsObserver.
		NSNotification *notification = [NSNotification notificationWithName:@"OpenEarsNotification" object:nil userInfo:userInfoDictionary];
		[[NSNotificationCenter defaultCenter] performSelectorOnMainThread:@selector(postNotification:) withObject:notification waitUntilDone:YES];
		
		
	} else if (inInterruptionState == kAudioSessionEndInterruption) { // The interruption is over.
	
		NSDictionary *userInfoDictionary = [NSDictionary dictionaryWithObject:@"AudioSessionInterruptionDidEnd" forKey:@"OpenEarsNotificationType"]; // Send notification to OpenEarsEventsObserver.
		NSNotification *notification = [NSNotification notificationWithName:@"OpenEarsNotification" object:nil userInfo:userInfoDictionary];
		[[NSNotificationCenter defaultCenter] performSelectorOnMainThread:@selector(postNotification:) withObject:notification waitUntilDone:YES];
#ifdef OPENEARSLOGGING			
		printf("OPENEARSLOGGING: The Audio Session interruption is over.\n");
#endif			
	}
}

void audioSessionPropertyListener(void *inClientData,
								  AudioSessionPropertyID inID,
								  UInt32 inDataSize,
								  const void *inData) { // We also listen to some Audio Session properties so that we can react to changes such as new audio routes (e.g. headphones plugged/unplugged).
	
	 // It may be necessary to react to changes in the audio route; for instance, if the user inserts or removes the headphone mic, 
	 // it's probably necessary to restart a continuous recognition loop in order to calibrate to the changed background levels.
	 
	
	if (inID == kAudioSessionProperty_AudioRouteChange) { // If the property change triggering the function is a change of audio route,
		
		CFNumberRef audioRouteChangeReasonKey = (CFNumberRef)CFDictionaryGetValue((CFDictionaryRef)inData, CFSTR(kAudioSession_AudioRouteChangeKey_Reason));
		SInt32 audioRouteChangeReason;
		CFNumberGetValue(audioRouteChangeReasonKey, kCFNumberSInt32Type, &audioRouteChangeReason); // Get the reason for the route change.
		
		if (audioRouteChangeReason == kAudioSessionRouteChangeReason_OldDeviceUnavailable) { // Route change due to change in device availability.
			
			CFStringRef audioRoute;
			UInt32 size = sizeof(CFStringRef);
			OSStatus getAudioRouteError = AudioSessionGetProperty(kAudioSessionProperty_AudioRoute, &size, &audioRoute);
			
			if (getAudioRouteError != 0) {
#ifdef OPENEARSLOGGING						
				printf("OPENEARSLOGGING: Error %d: Unable to get audio route.\n", (int)getAudioRouteError);
#endif				
			} else {
#ifdef OPENEARSLOGGING						
				printf("OPENEARSLOGGING: Audio route change. The new audio route is: ");
				
				CFShow(audioRoute);
#endif				
								
				NSDictionary *userInfoDictionary = [NSDictionary dictionaryWithObjects:[NSArray arrayWithObjects:@"AudioRouteDidChangeRoute",[NSString stringWithFormat:@"%@",audioRoute],nil] forKeys:[NSArray arrayWithObjects:@"OpenEarsNotificationType",@"AudioRoute",nil]];
				NSNotification *notification = [NSNotification notificationWithName:@"OpenEarsNotification" object:nil userInfo:userInfoDictionary];
				[[NSNotificationCenter defaultCenter] performSelectorOnMainThread:@selector(postNotification:) withObject:notification waitUntilDone:YES]; // Forward the audio route change to OpenEarsEventsObserver.
			}
			
		} else { // Route change not due to device unavailability but most likely as a result of a user-initiated action.
			
			CFStringRef audioRoute;
			UInt32 size = sizeof(CFStringRef);
			OSStatus getAudioRouteError = AudioSessionGetProperty(kAudioSessionProperty_AudioRoute, &size, &audioRoute); /* Get the new route */
						
			if (getAudioRouteError != 0) {
#ifdef OPENEARSLOGGING						
				printf("OPENEARSLOGGING: Error %d: Unable to get new audio route.\n", (int)getAudioRouteError);
#endif					
			} else {
#ifdef OPENEARSLOGGING					
				printf("OPENEARSLOGGING: Audio route change. The new audio route is: ");
				CFShow(audioRoute);
#endif						
				NSDictionary *userInfoDictionary = [NSDictionary dictionaryWithObjects:[NSArray arrayWithObjects:@"AudioRouteDidChangeRoute",[NSString stringWithFormat:@"%@",audioRoute],nil] forKeys:[NSArray arrayWithObjects:@"OpenEarsNotificationType",@"AudioRoute",nil]];
				NSNotification *notification = [NSNotification notificationWithName:@"OpenEarsNotification" object:nil userInfo:userInfoDictionary];
				[[NSNotificationCenter defaultCenter] performSelectorOnMainThread:@selector(postNotification:) withObject:notification waitUntilDone:YES]; // Forward the audio route change to OpenEarsEventsObserver.
			}
			
		}
		
	} else if (inID == kAudioSessionProperty_AudioInputAvailable) {
		
		 // Here we're listening and sending notifications for changes in the availability of the input device.
		 
#ifdef OPENEARSLOGGING			
		printf("OPENEARSLOGGING: There was a change in input device availability: ");
#endif			
		if (inDataSize == sizeof(UInt32)) {
			UInt32 audioInputIsAvailable = *(UInt32*)inData;
			if(audioInputIsAvailable == 0) { // Input became unavailable.
				
				NSDictionary *userInfoDictionary = [NSDictionary dictionaryWithObject:@"AudioInputDidBecomeUnavailable" forKey:@"OpenEarsNotificationType"];
				NSNotification *notification = [NSNotification notificationWithName:@"OpenEarsNotification" object:nil userInfo:userInfoDictionary];
				[[NSNotificationCenter defaultCenter] performSelectorOnMainThread:@selector(postNotification:) withObject:notification waitUntilDone:YES]; // Forward the input availability change to OpenEarsEventsObserver.
#ifdef OPENEARSLOGGING						
				printf("OPENEARSLOGGING: the audio input is now unavailable.\n");
#endif					
			} else if (audioInputIsAvailable == 1) { // Input became available again.
				
#ifdef OPENEARSLOGGING	
				printf("OPENEARSLOGGING: the audio input is now available.\n");
#endif						
				NSDictionary *userInfoDictionary = [NSDictionary dictionaryWithObject:@"AudioInputDidBecomeAvailable" forKey:@"OpenEarsNotificationType"];
				NSNotification *notification = [NSNotification notificationWithName:@"OpenEarsNotification" object:nil userInfo:userInfoDictionary];
				[[NSNotificationCenter defaultCenter] performSelectorOnMainThread:@selector(postNotification:) withObject:notification waitUntilDone:YES]; // Forward the input availability change to OpenEarsEventsObserver.
			}
		}
	}
}


// Here is where we're initiating the audio session.  This should only happen once in an app session.  If a second attempt is made to initiate an audio session using this class, it will hopefully
// receive the error 1768843636 which means that there is already a session, and not try to start a new session here.

- (void) startAudioSession {
	
OSStatus audioSessionInitializationError = AudioSessionInitialize(NULL, NULL, audioSessionInterruptionListener, NULL); // Try to initialize the audio session.

if (audioSessionInitializationError !=0 && ((int)audioSessionInitializationError != 1768843636)) { // There was an error and it wasn't that the audio session is already initialized.
#ifdef OPENEARSLOGGING			
	printf("OPENEARSLOGGING: Error %d: Unable to initialize the audio session.\n", (int)audioSessionInitializationError);
#endif
	
} else { // If there was no error we'll set the properties of the audio session now.
	
	
	// Projects using Pocketsphinx and Flite should use the Audio Session Category kAudioSessionCategory_PlayAndRecord.
	// Using this category routes playback to the ear speaker when the headphones aren't plugged in.
	// This isn't really appropriate for a speech recognition/tts app as far as I can see so I'm re-routing the output to the 
	// main speaker.
	
	UInt32 audioCategory = kAudioSessionCategory_PlayAndRecord; // Set the Audio Session category to kAudioSessionCategory_PlayAndRecord.
	OSStatus audioCategoryError = AudioSessionSetProperty(kAudioSessionProperty_AudioCategory, sizeof(audioCategory), &audioCategory);
	if (audioCategoryError != 0) {
#ifdef OPENEARSLOGGING			
		printf("OPENEARSLOGGING: Error %d: Unable to set audio category.\n", (int)audioCategoryError);
#endif
	}

	UInt32 overrideCategoryDefaultToSpeaker = 1; // Re-route sound output to the main speaker.
	OSStatus overrideCategoryDefaultToSpeakerError = AudioSessionSetProperty (kAudioSessionProperty_OverrideCategoryDefaultToSpeaker, sizeof (overrideCategoryDefaultToSpeaker), &overrideCategoryDefaultToSpeaker);
	if (overrideCategoryDefaultToSpeakerError != 0) printf("Error %d: Unable to override the default speaker.\n", (int)overrideCategoryDefaultToSpeakerError);
	
	OSStatus audioRouteChangeListenerError = AudioSessionAddPropertyListener(kAudioSessionProperty_AudioRouteChange, audioSessionPropertyListener, NULL); // Create listener for changes in the audio route.
	if (audioRouteChangeListenerError != 0) {
#ifdef OPENEARSLOGGING			
		printf("OPENEARSLOGGING: Error %d: Unable to start audio route change listener.\n", (int)audioRouteChangeListenerError);
#endif	
	}

	UInt32 audioInputAvailable = 0;  // Find out if there is an available audio input before we start the session.
	UInt32 size = sizeof(audioInputAvailable);
	OSStatus audioInputAvailableError = AudioSessionGetProperty(kAudioSessionProperty_AudioInputAvailable, &size, &audioInputAvailable);
	if (audioInputAvailableError != 0) {
#ifdef OPENEARSLOGGING			
		printf("OPENEARSLOGGING: Error %d: Unable to get the availability of the audio input.\n", (int)audioInputAvailableError);
#endif
	}
	if(audioInputAvailableError == 0 && audioInputAvailable == 0) {
#ifdef OPENEARSLOGGING			
		printf("OPENEARSLOGGING: There is no audio input available.\n");
#endif
	}

	OSStatus addAvailabilityListenerError = AudioSessionAddPropertyListener(kAudioSessionProperty_AudioInputAvailable, audioSessionPropertyListener, NULL); // Create listener for changes in the Audio Session properties.
	if (addAvailabilityListenerError != 0) {
#ifdef OPENEARSLOGGING			
		printf("OPENEARSLOGGING: Error %d: Unable to add the listener for changes in input availability.\n", (int)addAvailabilityListenerError);
#endif
	}

	OSStatus setAudioSessionActiveError = AudioSessionSetActive(true);  // Finally, start the audio session.
	if (setAudioSessionActiveError != 0) {
#ifdef OPENEARSLOGGING			
		printf("OPENEARSLOGGING: Error %d: Unable to set the audio session active.\n", (int)setAudioSessionActiveError);
#endif
	}
}
}
@end
