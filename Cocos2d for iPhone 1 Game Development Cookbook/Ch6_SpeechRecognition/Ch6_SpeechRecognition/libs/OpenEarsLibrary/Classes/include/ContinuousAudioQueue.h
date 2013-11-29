//  OpenEars version 0.9.0.2
//  http://www.politepix.com/openears
//
//  ContinuousAudioQueue.h
//  OpenEars
//
//  ContinuousAudioQueue is a class which handles the interaction between the Pocketsphinx continuous recognition loop and Core Audio.
//
//  Created by Halle Winkler on 8/30/10.
//  This is a sphinx ad header based on modifications to the Sphinxbase file ad.h.
//
//  Copyright Halle Winkler 2010 excepting that which falls under the copyright of Carnegie Mellon University
//  as part of their file ad.h.
//  http://www.politepix.com
//  Contact at http://www.politepix.com/contact
//
//  Excepting that which falls under the license of Carnegie Mellon University as part of their file ad.h, 
//  licensed under the Common Development and Distribution License (CDDL) Version 1.0
//  http://www.opensource.org/licenses/cddl1.txt or see included file license.txt
//  with the single exception to the license that you may distribute executable-only versions
//  of software using OpenEars files without making source code available under the terms of CDDL Version 1.0 
//  paragraph 3.1 if source code to your software isn't otherwise available, and without including a notice in 
//  that case that that source code is available.
//
//  Header for original source file ad.h which I modified to create this header is as follows:
//
/* -*- c-basic-offset: 4; indent-tabs-mode: nil -*- */
/* ====================================================================
 * Copyright (c) 1999-2004 Carnegie Mellon University.  All rights
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
 * ad.h -- generic live audio interface for recording and playback
 * 
 * **********************************************
 * CMU ARPA Speech Project
 *
 * Copyright (c) 1996 Carnegie Mellon University.
 * ALL RIGHTS RESERVED.
 * **********************************************
 * 
 * HISTORY
 * 
 * $Log: ad.h,v $
 * Revision 1.8  2005/06/22 08:00:06  arthchan2003
 * Completed all doxygen documentation on file description for libs3decoder/libutil/libs3audio and programs.
 *
 * Revision 1.7  2004/12/14 00:39:49  arthchan2003
 * add <s3types.h> to the code, change some comments to doxygen style
 *
 * Revision 1.6  2004/12/06 11:17:55  arthchan2003
 * Update the copyright information of ad.h, *sigh* start to feel tired of updating documentation system.  Anyone who has time, please take up libs3audio. That is the last place which is undocumented
 *
 * Revision 1.5  2004/07/23 23:44:46  egouvea
 * Changed the cygwin code to use the same audio files as the MS Visual code, removed unused variables from fe_interface.c
 *
 * Revision 1.4  2004/02/29 23:48:31  egouvea
 * Updated configure.in to the recent automake/autoconf, fixed win32
 * references in audio files.
 *
 * Revision 1.3  2002/11/10 19:27:38  egouvea
 * Fixed references to sun's implementation of audio interface,
 * referring to the correct .h file, and replacing sun4 with sunos.
 *
 * Revision 1.2  2001/12/11 04:40:55  lenzo
 * License cleanup.
 *
 * Revision 1.1.1.1  2001/12/03 16:01:45  egouvea
 * Initial import of sphinx3
 *
 * Revision 1.1.1.1  2001/01/17 05:17:14  ricky
 * Initial Import of the s3.3 decoder, has working decodeaudiofile, s3.3_live
 *
 * 
 * 19-Jan-1999	M K Ravishankar (rkm@cs.cmu.edu) at Carnegie Mellon University
 * 		Added AD_ return codes.  Added ad_open_sps_bufsize(), and
 * 		ad_rec_t.n_buf.
 * 
 * 17-Apr-98	M K Ravishankar (rkm@cs.cmu.edu) at Carnegie Mellon University
 * 		Added ad_open_play_sps().
 * 
 * 07-Mar-98	M K Ravishankar (rkm@cs.cmu.edu) at Carnegie Mellon University
 * 		Added ad_open_sps().
 * 
 * 10-Jun-96	M K Ravishankar (rkm@cs.cmu.edu) at Carnegie Mellon University
 * 		Added ad_wbuf_t, ad_rec_t, and ad_play_t types, and augmented all
 * 		recording functions with ad_rec_t, and playback functions with
 * 		ad_play_t.
 * 
 * 06-Jun-96	M K Ravishankar (rkm@cs.cmu.edu) at Carnegie Mellon University
 *		Created.
 */

/** \file ad.h
 * \brief generic live audio interface for recording and playback
 */

#ifndef _AD_H_
#define _AD_H_
	
#include <AudioToolbox/AudioToolbox.h> 
			
typedef struct { // The audio device struct used by Pocketsphinx.
	AudioQueueRef audioQueue; // The Audio Queue.
	AudioQueueBufferRef audioQueueBuffers[3]; // The buffer array of the Audio Queue, 3 buffers is standard for Core Audio. 
	AudioFileID recordFileID; // The AudioFileID of the record file that is analyzed.
	CFStringRef recordFileName; // The name of the record file.
	CFStringRef currentRoute; // The current Audio Route for the device (e.g. headphone mic or external mic).
	SInt64 recordPacket; // The current packet of the Audio Queue.
	AudioStreamBasicDescription audioQueueRecordFormat; // The recording format of the Audio Queue. 
	AudioQueueLevelMeterState *levelMeterState; // decibel metering of input.
	BOOL recordData; // Should data be recorded?
	BOOL speechIsInProgress; // Is Flite speaking?
	BOOL audioQueueIsRunning; // Is the queue instantiated? 
	BOOL recording; // Is the Audio Queue currently recording sound? 
	SInt32 sps;		// Samples per second.
	SInt32 bps;		// Bytes per sample.
	SInt32 readPosition; // Which packet to start the read on.
} pocketsphinxAudioDevice;	
	
Float32 pocketsphinxAudioDeviceMeteringLevel(pocketsphinxAudioDevice * audioDriver); // Function which returns the metering level of the AudioQueue input.
pocketsphinxAudioDevice *openAudioDevice (const char *audioDevice, SInt32 samplesPerSecond); // Function to open the "audio device" or in this case instantiate a new Audio Queue.
SInt32 startRecordingWithLeader(pocketsphinxAudioDevice * audioDevice); // An optional function that starts Audio Queue recording after a second and a half so that calibration can happen with full buffers.
SInt32 startRecording(pocketsphinxAudioDevice * audioDevice); // Tell the Audio Queue to start recording.
SInt32 stopRecording(pocketsphinxAudioDevice * audioDevice); // Tell the Audio Queue to stop recording.
SInt32 readBufferContents(pocketsphinxAudioDevice * audioDevice, SInt16 * buffer, SInt32 maximum); // Scan the buffer for speech.
SInt32 closeAudioDevice(pocketsphinxAudioDevice * audioDevice); // Close the "audio device" or in this case stop and free the Audio Queue.

#endif


