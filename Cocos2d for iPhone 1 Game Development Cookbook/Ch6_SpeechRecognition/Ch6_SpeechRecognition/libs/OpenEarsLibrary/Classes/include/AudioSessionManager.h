//  OpenEars version 0.9.0.2
//  http://www.politepix.com/openears
//
//  AudioSessionManager.h
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

//  This class creates an Audio Session for your app which uses OpenEars, and forwards important notification about
//  Audio Session status changes to OpenEarsEventsObserver.

#import <Foundation/Foundation.h>
#include <AudioToolbox/AudioToolbox.h> 

@interface AudioSessionManager : NSObject {

}

-(void) startAudioSession; // All that we need to access from outside of this class is the method to start the Audio Session.

@end
