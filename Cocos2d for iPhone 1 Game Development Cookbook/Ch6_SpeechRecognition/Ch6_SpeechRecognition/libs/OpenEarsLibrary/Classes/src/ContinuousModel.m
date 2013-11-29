//  OpenEars version 0.9.02
//  http://www.politepix.com/openears
//
//  ContinuousModel.m
//  OpenEars
//
//  ContinuousModel is a class which consists of the continuous listening loop used by Pocketsphinx.
//
//  Created by Halle Winkler on 8/30/10.
//  This is a Pocketsphinx continuous listening loop based on modifications to the Pocketsphinx file continuous.c.
//
//  Copyright Halle Winkler 2010 excepting that which falls under the copyright of Carnegie Mellon University as part
//  of their file continuous.c.
//  http://www.politepix.com
//  Contact at http://www.politepix.com/contact
//
//  Excepting that which falls under the license of Carnegie Mellon University as part of their file continuous.c, 
//  licensed under the Common Development and Distribution License (CDDL) Version 1.0
//  http://www.opensource.org/licenses/cddl1.txt or see included file license.txt
//  with the single exception to the license that you may distribute executable-only versions
//  of software using OpenEars files without making source code available under the terms of CDDL Version 1.0 
//  paragraph 3.1 if source code to your software isn't otherwise available, and without including a notice in 
//  that case that that source code is available.
//
//  Header for original source file continuous.c which I modified to create this file is as follows:
//
/* -*- c-basic-offset: 4; indent-tabs-mode: nil -*- */
/* ====================================================================
 * Copyright (c) 1999-2001 Carnegie Mellon University.  All rights
 * reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer. 
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * This work was supported in part by funding from the Defense Advanced 
 * Research Projects Agency and the National Science Foundation of the 
 * United States of America, and the CMU Sphinx Speech Consortium.
 *
 * THIS SOFTWARE IS PROVIDED BY CARNEGIE MELLON UNIVERSITY ``AS IS'' AND 
 * ANY EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY
 * NOR ITS EMPLOYEES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * ====================================================================
 *
 */
/*
 * demo.c -- An example SphinxII program using continuous listening/silence filtering
 * 		to segment speech into utterances that are then decoded.
 * 
 * HISTORY
 *
 * 15-Jun-99    Kevin A. Lenzo (lenzo@cs.cmu.edu) at Carnegie Mellon University
 *              Added i386_linux and used ad_open_sps instead of ad_open
 * 
 * 14-Jun-96	M K Ravishankar (rkm@cs.cmu.edu) at Carnegie Mellon University.
 * 		Created.
 */

/*
 * This is a simple, tty-based example of a SphinxII client that uses continuous listening
 * with silence filtering to automatically segment a continuous stream of audio input
 * into utterances that are then decoded.
 * 
 * Remarks:
 *   - Each utterance is ended when a silence segment of at least 1 sec is recognized.
 *   - Single-threaded implementation for portability.
 *   - Uses fbs8 audio library; can be replaced with an equivalent custom library.
 */

#import "ContinuousModel.h"
#import "pocketsphinx.h"
#import "ContinuousADModule.h"
#import "unistd.h"
#import "OpenEarsConfig.h"
#import "PocketsphinxRunConfig.h"

@implementation ContinuousModel

@synthesize inMainRecognitionLoop; // Have we entered the main part of the loop yet?
@synthesize exitListeningLoop; // Should we be breaking out of the loop at the nearest opportunity?

static pocketsphinxAudioDevice *audioDevice; // The "device", which is actually a struct containing an Audio Queue.
static ps_decoder_t *pocketSphinxDecoder; // The Pocketsphinx decoder which will perform the actual speech recognition on recorded speech.
FILE *err_set_logfp(FILE *logfp); // This function will allow us to make Pocketsphinx run quietly.

- (pocketsphinxAudioDevice *) continuousAudioDevice { // Return the device to an Objective-C class.
	return audioDevice;	
}

#pragma mark -
#pragma mark Pocketsphinx Listening Loop

- (void) openDevice { // Open the device outside of the main loop.
	if ((audioDevice = openAudioDevice("device",16000)) == NULL) {
#ifdef OPENEARSLOGGING
		printf("OPENEARSLOGGING: openAudioDevice failed\n");
#endif
	}
}

- (void) listeningLoop { // The big recognition loop.
	
	NSDictionary *userInfoDictionaryForStartup = [NSDictionary dictionaryWithObject:@"PocketsphinxRecognitionLoopDidStart" forKey:@"OpenEarsNotificationType"]; // Forward the info that we're starting to OpenEarsEventsObserver.
	NSNotification *notificationForStartup = [NSNotification notificationWithName:@"OpenEarsNotification" object:nil userInfo:userInfoDictionaryForStartup];
	[[NSNotificationCenter defaultCenter] performSelectorOnMainThread:@selector(postNotification:) withObject:notificationForStartup waitUntilDone:NO];
	
	int16 audioDeviceBuffer[4096]; // The following are all used by Pocketsphinx.
    int32 speechData;
	int32 timestamp;
	int32 remainingSpeechData;
	int32 recognitionScore;
    char const *hypothesis;
    char const *utteranceID;
    cont_ad_t *continuousListener;
	
#ifndef VERBOSEPOCKETSPHINX
	err_set_logfp(NULL); // If VERBOSEPOCKETSPHINX isn't defined, this will quiet the output from Pocketsphinx.
#endif


	NSArray *commandArray = [[NSArray alloc] initWithObjects: // This is an array that is used to set up the run arguments for Pocketsphinx. 
							 // Never change any of the values here directly.  They can be changed using the file PocketsphinxRunConfig.h (although you shouldn't 
							 // change anything there unless you are absolutely 100% clear on why you'd want to and what the outcome will be).
							 // See PocketsphinxRunConfig.h for explanations of these constants and the run arguments they correspond to.
					
#ifdef kADCDEV
							 @"-adcdev", kADCDEV,
#endif
							 
#ifdef kAGC
							 @"-agc", kAGC,
#endif
							 
#ifdef kAGCTHRESH
							 @"-agcthresh", kAGCTHRESH,
#endif
							 
#ifdef kALPHA
							 @"-alpha", kALPHA,
#endif
							 
#ifdef kARGFILE
							 @"-argfile", kARGFILE,
#endif
							 
#ifdef kASCALE
							 @"-ascale", kASCALE,
#endif
							 
#ifdef kBACKTRACE
							 @"-backtrace", kBACKTRACE,
#endif
							 
#ifdef kBEAM
							 @"-beam", kBEAM,
#endif
							 
#ifdef kBESTPATH
							 @"-bestpath", kBESTPATH,
#endif
							 
#ifdef kBESTPATHLW
							 @"-bestpathlw", kBESTPATHLW,
#endif
							 
#ifdef kBGHIST
							 @"-bghist", kBGHIST,
#endif
							 
#ifdef kCEPLEN
							 @"-ceplen", kCEPLEN,
#endif
							 
#ifdef kCMN
							 @"-cmn", kCMN,
#endif
							 
#ifdef kCMNINIT
							 @"-cmninit", kCMNINIT,
#endif
							 
#ifdef kCOMPALLSEN
							 @"-compallsen", kCOMPALLSEN,
#endif
							 
#ifdef kDEBUG
							 @"-debug", kDEBUG,
#endif
								 
#ifdef kDICT
							 @"-dict", [NSString stringWithFormat:@"%@/%@",[[NSBundle mainBundle] resourcePath], kDICT],
#endif
							 
#ifdef kDICTCASE
							 @"-dictcase", kDICTCASE,
#endif
							 
#ifdef kDITHER
							 @"-dither", kDITHER,
#endif
							 
#ifdef kDOUBLEBW
							 @"-doublebw", kDOUBLEBW,
#endif
							 
#ifdef kDS
							 @"-ds", kDS,
#endif
							 
#ifdef kFDICT
							 @"-fdict", kFDICT,
#endif
							 
#ifdef kFEAT
							 @"-feat", kFEAT,
#endif
							 
#ifdef kFEATPARAMS
							 @"-featparams", kFEATPARAMS,
#endif
							 
#ifdef kFILLPROB
							 @"-fillprob", kFILLPROB,
#endif
							 
#ifdef kFRATE
							 @"-frate", kFRATE,
#endif
							 
#ifdef kFSG
							 @"-fsg", kFSG,
#endif
							 
#ifdef kFSGUSEALTPRON
							 @"-fsgusealtpron", kFSGUSEALTPRON,
#endif
							 
#ifdef kFSGUSEFILLER
							 @"-fsgusefiller", kFSGUSEFILLER,
#endif
							 
#ifdef kFWDFLAT
							 @"-fwdflat", kFWDFLAT,
#endif
							 
#ifdef kFWDFLATBEAM
							 @"-fwdflatbeam", kFWDFLATBEAM,
#endif
							 
#ifdef kFWDFLATWID
							 @"-fwdflatefwid", kFWDFLATWID,
#endif
							 
#ifdef kFWDFLATLW
							 @"-fwdflatlw", kFWDFLATLW,
#endif
							 
#ifdef kFWDFLATSFWIN
							 @"-fwdflatsfwin", kFWDFLATSFWIN,
#endif
							 
#ifdef kFWDFLATBEAM
							 @"-fwdflatwbeam", kFWDFLATBEAM,
#endif
							 
#ifdef kFWDTREE
							 @"-fwdtree", kFWDTREE,
#endif
							 
#ifdef kHMM
							 @"-hmm", [NSString stringWithFormat:@"%@",[[NSBundle mainBundle] resourcePath]],
#endif
							 
#ifdef kINPUT_ENDIAN
							 @"-input_endian", kINPUT_ENDIAN,
#endif
#ifdef USEJSGF				 
#ifdef kJSGF
							 @"-jsgf",	[NSString stringWithFormat:@"%@/%@",[[NSBundle mainBundle] resourcePath], kJSGF],
#endif
#endif							 
#ifdef kKDMAXBBI
							 @"-kdmaxbbi", kKDMAXBBI,
#endif
							 
#ifdef kKDMAXDEPTH
							 @"-kdmaxdepth", kKDMAXDEPTH,
#endif
							 
#ifdef kKDTREE
							 @"-kdtree", kKDTREE,
#endif
							 
#ifdef kLATSIZE
							 @"-latsize", kLATSIZE,
#endif
							 
#ifdef kLDA
							 @"-lda", kLDA,
#endif
							 
#ifdef kLDADIM
							 @"-ldadim", kLDADIM,
#endif
							 
#ifdef kLEXTREEDUMP
							 @"-lextreedump", kLEXTREEDUMP,
#endif
							 
#ifdef kLIFTER
							 @"-lifter",	kLIFTER,
#endif
#ifdef USEJSGF
#else							 
#ifdef kLM
							 @"-lm",	[NSString stringWithFormat:@"%@/%@",[[NSBundle mainBundle] resourcePath], kLM],
#endif
#endif
#ifdef kLMCTL
							 @"-lmctl",	kLMCTL,
#endif
							 
#ifdef kLMNAME
							 @"-lmname",	kLMNAME,
#endif
							 
#ifdef kLOGBASE
							 @"-logbase", kLOGBASE,
#endif
							 
#ifdef kLOGFN
							 @"-logfn", kLOGFN,
#endif
							 
#ifdef kLOGSPEC
							 @"-logspec", kLOGSPEC,
#endif
							 
#ifdef kLOWERF
							 @"-lowerf", kLOWERF,
#endif
							 
#ifdef kLPBEAM
							 @"-lpbeam", kLPBEAM,
#endif
							 
#ifdef kLPONLYBEAM
							 @"-lponlybeam", kLPONLYBEAM,
#endif
							 
#ifdef kLW
							 @"-lw",	kLW,
#endif
							 
#ifdef kMAXHMMPF
							 @"-maxhmmpf", kMAXHMMPF,
#endif
							 
#ifdef kMAXNEWOOV
							 @"-maxnewoov", kMAXNEWOOV,
#endif
							 
#ifdef kMAXWPF
							 @"-maxwpf", kMAXWPF,
#endif
							 
#ifdef kMDEF
							 @"-mdef", kMDEF,
#endif
							 
#ifdef kMEAN
							 @"-mean", kMEAN,
#endif
							 
#ifdef kMFCLOGDIR
							 @"-mfclogdir", kMFCLOGDIR,
#endif
							 
#ifdef kMIXW
							 @"-mixw", kMIXW,
#endif
							 
#ifdef kMIXWFLOOR
							 @"-mixwfloor", kMIXWFLOOR,
#endif
							 
#ifdef kMLLR
							 @"-mllr", kMLLR,
#endif
							 
#ifdef kMMAP
							 @"-mmap", kMMAP,
#endif
							 
#ifdef kNCEP
							 @"-ncep", kNCEP,
#endif
							 
#ifdef kNFFT
							 @"-nfft", kNFFT,
#endif
							 
#ifdef kNFILT
							 @"-nfilt", kNFILT,
#endif
							 
#ifdef kNWPEN
							 @"-nwpen", kNWPEN,
#endif
							 
#ifdef kPBEAM
							 @"-pbeam", kPBEAM,
#endif
							 
#ifdef kPIP
							 @"-pip", kPIP,
#endif
							 
#ifdef kPL_BEAM
							 @"-pl_beam", kPL_BEAM,
#endif
							 
#ifdef kPL_PBEAM
							 @"-pl_pbeam", kPL_PBEAM,
#endif
							 
#ifdef kPL_WINDOW
							 @"-pl_window", kPL_WINDOW,
#endif
							 
#ifdef kRAWLOGDIR
							 @"-rawlogdir", kRAWLOGDIR,
#endif
							 
#ifdef kREMOVE_DC
							 @"-remove_dc", kREMOVE_DC,
#endif
							 
#ifdef kROUND_FILTERS
							 @"-round_filters", kROUND_FILTERS,
#endif
							 
#ifdef kSAMPRATE
							 @"-samprate", kSAMPRATE,
#endif
							 
#ifdef kSEED
							 @"-seed",kSEED,
#endif
							 
#ifdef kSENDUMP
							 @"-sendump", kSENDUMP,
#endif
							 
#ifdef kSENMGAU
							 @"-senmgau", kSENMGAU,
#endif
							 
#ifdef kSILPROB
							 @"-silprob", kSILPROB,
#endif
							 
#ifdef kSMOOTHSPEC
							 @"-smoothspec", kSMOOTHSPEC,
#endif
							 
#ifdef kSVSPEC
							 @"-svspec", kSVSPEC,
#endif
							 
#ifdef kTMAT
							 @"-tmat", kTMAT,
#endif
							 
#ifdef kTMATFLOOR
							 @"-tmatfloor", kTMATFLOOR,
#endif
							 
#ifdef kTOPN
							 @"-topn", kTOPN,
#endif
							 
#ifdef kTOPN_BEAM
							 @"-topn_beam", kTOPN_BEAM,
#endif
							 
#ifdef kTOPRULE
							 @"-toprule", kTOPRULE,
#endif
							 
#ifdef kTRANSFORM
							 @"-transform", kTRANSFORM,
#endif
							 
#ifdef kUNIT_AREA
							 @"-unit_area", kUNIT_AREA,
#endif
							 
#ifdef kUPPERF
							 @"-upperf", kUPPERF,
#endif
							 
#ifdef kUSEWDPHONES
							 @"-usewdphones", kUSEWDPHONES,
#endif
							 
#ifdef kUW
							 @"-uw", kUW,
#endif
							 
#ifdef kVAR
							 @"-var", kVAR,
#endif
							 
#ifdef kVARFLOOR
							 @"-varfloor", kVARFLOOR,
#endif
							 
#ifdef kVARNORM
							 @"-varnorm", kVARNORM,
#endif
							 
#ifdef kVERBOSE
							 @"-verbose", kVERBOSE,
#endif
							 
#ifdef kWARP_PARAMS
							 @"-warp_params", kWARP_PARAMS,
#endif
							 
#ifdef kWARP_TYPE
							 @"-warp_type", kWARP_TYPE,
#endif
							 
#ifdef kWBEAM
							 @"-wbeam", kWBEAM,
#endif
							 
#ifdef kWIP
							 @"-wip", kWIP,
#endif
							 
#ifdef kWLEN
							 @"-wlen", kWLEN,
#endif
							 nil];
	
	char* argv[[commandArray count]]; // We're simulating the command-line run arguments for Pocketsphinx.
	
	for (int i = 0; i < [commandArray count]; i++ ) { // Grab all the set arguments.
		argv[i] = (char *)[[commandArray objectAtIndex:i]UTF8String];
	}
	
	arg_t cont_args_def[] = { // Grab any extra arguments.
		POCKETSPHINX_OPTIONS,
		{ "-argfile", ARG_STRING, NULL, "Argument file giving extra arguments." },
		CMDLN_EMPTY_OPTION
	};
	
	cmd_ln_t *configuration; // The Pocketsphinx run configuration.
	
    if ([commandArray count] == 2) { // Fail if there aren't really any arguments.
        configuration = cmd_ln_parse_file_r(NULL, cont_args_def, argv[1], TRUE);
    }  else { // Set the Pocketsphinx run configuration to the selected arguments and values.
        configuration = cmd_ln_parse_r(NULL, cont_args_def, [commandArray count], argv, FALSE);
    }
	[commandArray release];
    pocketSphinxDecoder = ps_init(configuration); // Initialize the decoder.
	
    if ((audioDevice = openAudioDevice("device",16000)) == NULL) { // Open the audio device (actually the struct containing the Audio Queue).
#ifdef OPENEARSLOGGING
		printf("OPENEARSLOGGING: openAudioDevice failed\n");
#endif
	}
	
    if ((continuousListener = cont_ad_init(audioDevice, readBufferContents)) == NULL) { // Initialize the continuous recognition module.
#ifdef OPENEARSLOGGING		
        printf("OPENEARSLOGGING: cont_ad_init failed\n");
#endif
	}
	
	audioDevice->recordData = 1; // Set the device to record data rather than ignoring it (it will ignore data when PocketsphinxController receives the suspendRecognition method).
	audioDevice->speechIsInProgress = 1;
	
    if (startRecording(audioDevice) < 0) { // Start recording.
#ifdef OPENEARSLOGGING		
        printf("OPENEARSLOGGING: startRecording failed\n");
#endif
	}

	NSDictionary *userInfoDictionaryForCalibrationStarted = [NSDictionary dictionaryWithObject:@"PocketsphinxDidStartCalibration" forKey:@"OpenEarsNotificationType"];
	NSNotification *notificationForCalibrationStarted = [NSNotification notificationWithName:@"OpenEarsNotification" object:nil userInfo:userInfoDictionaryForCalibrationStarted];
	[[NSNotificationCenter defaultCenter] performSelectorOnMainThread:@selector(postNotification:) withObject:notificationForCalibrationStarted waitUntilDone:NO];
	// Forward notification that calibration is starting to OpenEarsEventsObserver.
	
    if (cont_ad_calib(continuousListener) < 0) { // Start calibration.
#ifdef OPENEARSLOGGING      
		printf("OPENEARSLOGGING: cont_ad_calib failed\n");
#endif
	}
	
	NSDictionary *userInfoDictionaryForCalibrationComplete = [NSDictionary dictionaryWithObject:@"PocketsphinxDidCompleteCalibration" forKey:@"OpenEarsNotificationType"];
	NSNotification *notificationForCalibrationComplete = [NSNotification notificationWithName:@"OpenEarsNotification" object:nil userInfo:userInfoDictionaryForCalibrationComplete];
	[[NSNotificationCenter defaultCenter] performSelectorOnMainThread:@selector(postNotification:) withObject:notificationForCalibrationComplete waitUntilDone:NO];
	// Forward notification that calibration finished to OpenEarsEventsObserver.
	
    for (;;) { // This is the main loop.
		
		self.inMainRecognitionLoop = TRUE; // Note that we're in the main loop.
		
		if(exitListeningLoop == 1) break; // Break if we're trying to exit the loop.
		
    
		// We're now listening for speech.
#ifdef OPENEARSLOGGING			
		printf("OPENEARSLOGGING: Listening.\n");
#endif
		NSDictionary *userInfoDictionaryForListening = [NSDictionary dictionaryWithObject:@"PocketsphinxDidStartListening" forKey:@"OpenEarsNotificationType"];
		NSNotification *notificationForListening = [NSNotification notificationWithName:@"OpenEarsNotification" object:nil userInfo:userInfoDictionaryForListening];
		[[NSNotificationCenter defaultCenter] performSelectorOnMainThread:@selector(postNotification:) withObject:notificationForListening waitUntilDone:NO];
		// Forward notification that we're now listening for speech to OpenEarsEventsObserver.
		
       // Wait for speech and sleep when we don't have any yet.
        while ((speechData = cont_ad_read(continuousListener, audioDeviceBuffer, 4096)) == 0) {
			
            usleep(100000);
			if(exitListeningLoop == 1) break; // Break if we're trying to exit the loop.
		}
		
		if(exitListeningLoop == 1) break; // Break if we're trying to exit the loop.
		
        if (speechData < 0) { // This is an error.
#ifdef OPENEARSLOGGING            
			printf("OPENEARSLOGGING: cont_ad_read failed\n");
#endif
		}

        if (ps_start_utt(pocketSphinxDecoder, NULL) < 0) { // Data has been received and recognition is starting.
#ifdef OPENEARSLOGGING            
			printf("OPENEARSLOGGING: ps_start_utt() failed\n");
#endif
		}
		
		ps_process_raw(pocketSphinxDecoder, audioDeviceBuffer, speechData, FALSE, FALSE); // Process the data.
		
		
#ifdef OPENEARSLOGGING			
		printf("OPENEARSLOGGING: Speech detected...\n");
#endif
		NSDictionary *userInfoDictionaryForSpeechDetection = [NSDictionary dictionaryWithObject:@"PocketsphinxDidDetectSpeech" forKey:@"OpenEarsNotificationType"];
		NSNotification *notificationForSpeechDetection = [NSNotification notificationWithName:@"OpenEarsNotification" object:nil userInfo:userInfoDictionaryForSpeechDetection];
		[[NSNotificationCenter defaultCenter] performSelectorOnMainThread:@selector(postNotification:) withObject:notificationForSpeechDetection waitUntilDone:NO];
		// Forward to OpenEarsEventsObserver than speech has been detected.
		
		timestamp = continuousListener->read_ts;
		
		if(exitListeningLoop == 1) break; // Break if we're trying to exit the loop.
		
        for (;;) { // An inner loop in which the received speech will be decoded up to the point of a silence longer than a second.
			
			if(exitListeningLoop == 1) break; // Break if we're trying to exit the loop.
			
            if ((speechData = cont_ad_read(continuousListener, audioDeviceBuffer, 4096)) < 0) { // Read the available data.
#ifdef OPENEARSLOGGING                
				printf("OPENEARSLOGGING: cont_ad_read failed\n");
#endif
			}
			
			if(exitListeningLoop == 1)  break; // Break if we're trying to exit the loop.
			
            if (speechData == 0) { // No speech data, could be the end of a statement if it's been more than a second sense the last received speech.

                if ((continuousListener->read_ts - timestamp) > 16000) {
                    break;
				}
            } else { // New speech data.

				timestamp = continuousListener->read_ts;
            }
			
			if(exitListeningLoop == 1) break; // Break if we're trying to exit the loop.
			
			// Decode the data.
			remainingSpeechData = ps_process_raw(pocketSphinxDecoder, audioDeviceBuffer, speechData, FALSE, FALSE);
			
            if ((remainingSpeechData == 0) && (speechData == 0)) { // If nothing more to be done for now, sleep.
				usleep(20000);
				if(exitListeningLoop == 1) break; // Break if we're trying to exit the loop.
			}
			
			if(exitListeningLoop == 1) break; // Break if we're trying to exit the loop.
        }

		if(exitListeningLoop == 1) break; // Break if we're trying to exit the loop.
		
        stopRecording(audioDevice); // Stop recording.
        
		while (readBufferContents(audioDevice, audioDeviceBuffer, 4096) >= 0); // Read whatever is left in the buffer.
        cont_ad_reset(continuousListener); // Reset the continuous module.
		
		if(exitListeningLoop == 1) break; // Break if we're trying to exit the loop.
		

#ifdef OPENEARSLOGGING			
		printf("OPENEARSLOGGING: Processing speech, please wait...\n");
#endif

		ps_end_utt(pocketSphinxDecoder); // The utterance is ended,
		hypothesis = ps_get_hyp(pocketSphinxDecoder, &recognitionScore, &utteranceID); // Return the hypothesis.
		int32 probability = ps_get_prob(pocketSphinxDecoder, &utteranceID);

		#ifdef OPENEARSLOGGING
				printf("OPENEARSLOGGING: Pocketsphinx heard \"%s\" with a score of (%d) and an utterance ID of %s.\n", hypothesis, recognitionScore, utteranceID);
		#endif

		NSDictionary *userInfoDictionary = [NSDictionary dictionaryWithObjects:[NSArray arrayWithObjects:@"PocketsphinxDidReceiveHypothesis",[NSString stringWithFormat:@"%s",hypothesis],[NSString stringWithFormat:@"%d",probability],[NSString stringWithFormat:@"%s",utteranceID],nil] forKeys:[NSArray arrayWithObjects:@"OpenEarsNotificationType",@"Hypothesis",@"RecognitionScore",@"UtteranceID",nil]];
		NSNotification *notification = [NSNotification notificationWithName:@"OpenEarsNotification" object:nil userInfo:userInfoDictionary];
		[[NSNotificationCenter defaultCenter] performSelectorOnMainThread:@selector(postNotification:) withObject:notification waitUntilDone:NO];
		// Forward that a hypothesis was received, what it is, and related information to OpenEarsEventsObserver.
		
		if(exitListeningLoop == 1) break; // Break if we're trying to exit the loop.
		
        if (startRecording(audioDevice) < 0) { // Start over.
#ifdef OPENEARSLOGGING            
			printf("OPENEARSLOGGING: startRecording failed\n");
#endif
		}
		
		if(exitListeningLoop == 1) break; // Break if we're trying to exit the loop.
    }
	
	self.inMainRecognitionLoop = FALSE; // We broke out of the loop.
	exitListeningLoop = 0; // We don't want to prompt further exiting attempts since we're out.
	stopRecording(audioDevice); // Stop recording if necessary.
    cont_ad_close(continuousListener); // Close the continuous module.
    ps_free(pocketSphinxDecoder); // Free the decoder.
	closeAudioDevice(audioDevice); // Close the device, i.e. stop and dispose of the Audio Queue.
#ifdef OPENEARSLOGGING	
	printf("OPENEARSLOGGING: No longer listening.\n");
#endif
	
	NSDictionary *userInfoDictionaryForStop = [NSDictionary dictionaryWithObject:@"PocketsphinxDidStopListening" forKey:@"OpenEarsNotificationType"];
	NSNotification *stopNotification = [NSNotification notificationWithName:@"OpenEarsNotification" object:nil userInfo:userInfoDictionaryForStop];
	[[NSNotificationCenter defaultCenter] performSelectorOnMainThread:@selector(postNotification:) withObject:stopNotification waitUntilDone:NO];
	// Forward to OpenEarsEventsObserver that we've made a full stop.
}


@end
