//  OpenEars version 0.9.02
//  http://www.politepix.com/openears
//
//  OpenEarsConfig.h
//  OpenEars
//
//  OpenEarsConfig.h controls all of the OpenEars settings other than which voice to use for Flite, which is controlled by OpenEarsVoiceConfig.h
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

#import "OpenEarsVoiceConfig.h"

//#define OPENEARSLOGGING // Turn this on to get logging output from audio driver initialization, etc. Please turn on and submit output when reporting a problem.
//#define VERBOSEPOCKETSPHINX // Leave this uncommented to get verbose output from Pocketsphinx, comment it out for quiet operation. I'd recommend starting with it uncommented and then commenting when you're pretty sure things are working.
#define USERCANINTERRUPTSPEECH // Turn this on if you wish to let users cut off Flite speech by talking (only possible when headphones are plugged in). Not sure if this is 100% functional in v 0.9.01.

// Before proceeding, please note that you can only use a jsgf grammar (usually with suffix .gram) or a .languagemodel grammar. 
// If you want to use .languagemodel, replace OpenEars.languagemodel below with the name of your languagemodel file.
// If you want to use a JSGF grammar, uncomment the line #define USEJSGF and replace OpenEars.gram with the name of your jsgf.
// You need to provide a .dic file in either case, so replace OpenEars.dic with the name of your .dic file.

#define kLanguageModelName @"OpenEars.languagemodel" // Enter the name of your language model file here between the quotes, if you are using one. Entries here will be ignored if USEJSGF is uncommented.
#define kDictionaryName @"OpenEars.dic" // Enter the name of your dictionary file here between the quotes.
//#define USEJSGF
//#define kJSGFName @"OpenEars.gram" // Enter the name of your jsgf file here between the quotes, if you are using one. Entries here will be ignored if USEJSGF is commented.