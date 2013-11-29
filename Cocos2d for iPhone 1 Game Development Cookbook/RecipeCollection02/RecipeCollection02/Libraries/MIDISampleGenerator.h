#import "AudioOutput.h"
#import <AudioToolbox/AudioConverter.h>

#include "controller.h"
#include "envelope.h"
#include "modulation.h"
#include "oscillator.h"
#include "filter.h"
#include "parameter.h"

namespace synth { class Controller; }
namespace synth { class Envelope; }
namespace synth { class LFO; }
namespace synth { class Oscillator; }
namespace synth { class Note; }
namespace synth { class LowPass; }

@interface MIDISampleGenerator : NSObject <SampleGenerator>
{
  // Synthesizer components
  AudioOutput* output;
  synth::Controller* controller_;
  
  AudioStreamBasicDescription outputFormat;
}

-(void) initAudioOutput;
-(void) initSynthController;
-(float) randFrom:(float)low to:(float)hi;
-(void) randomize;
-(void) noteOn:(int)note;
-(void) noteOff:(int)note;
-(OSStatus)generateSamples:(AudioBufferList*)buffers;

@end

@implementation MIDISampleGenerator

-(id) init {
	if(self = [super init]){
		controller_ = new synth::Controller;
		[self initAudioOutput];
		[self initSynthController];
	}
		
	return self;
}

-(void) initAudioOutput {
	// Format preferred by the iphone (Fixed 8.24)
	outputFormat.mSampleRate = 44100.0;
	outputFormat.mFormatID = kAudioFormatLinearPCM;
	outputFormat.mFormatFlags  = kAudioFormatFlagsAudioUnitCanonical;
	outputFormat.mBytesPerPacket = sizeof(AudioUnitSampleType);
	outputFormat.mFramesPerPacket = 1;
	outputFormat.mBytesPerFrame = sizeof(AudioUnitSampleType);
	outputFormat.mChannelsPerFrame = 1;
	outputFormat.mBitsPerChannel = 8 * sizeof(AudioUnitSampleType);
	outputFormat.mReserved = 0;

	output = [[AudioOutput alloc] initWithAudioFormat:&outputFormat];
	[output setSampleDelegate:self];
	[output start];  // immediately invokes our callback to generate samples	
}


-(void) initSynthController {	
	//Modulation
	controller_->set_modulation_amount(0.0f); //0 to 1
	controller_->set_modulation_frequency(7.0f); //0 to 15
	controller_->set_modulation_source(synth::Controller::LFO_SRC_SQUARE);
	controller_->set_modulation_destination(synth::Controller::LFO_DEST_PITCH);
	
	//Volume Envelope
	controller_->volume_envelope()->set_attack(0.0f);//0 to 2
	controller_->volume_envelope()->set_decay(0.0f);//0 to 2
	controller_->volume_envelope()->set_sustain(1.0f);//0 to 1
	controller_->volume_envelope()->set_release(0.3f);//0 to 3
	
	//Oscillator
	// OSC 1
	controller_->set_osc1_level(0.5f);	//0 to 1
	controller_->set_osc1_wave_type(synth::Oscillator::SQUARE);
	controller_->set_osc1_octave(synth::Controller::OCTAVE_4);

	// OSC 2
	controller_->set_osc2_level(0.2f);	//0 to 1
	controller_->set_osc2_wave_type(synth::Oscillator::SQUARE);
	controller_->set_osc2_octave(synth::Controller::OCTAVE_4);
	
	controller_->set_glide_samples(0.0f);	//0 to 0.04
	
	//Oscillator Detail
	float semis = 0;	//-12 to 12
	float cents = 0;	//-100 to 100
	int total = (int)semis + 100 * (int)cents;
	controller_->set_osc2_shift(total);
	controller_->set_osc_sync(NO);
	
	//Filter
	controller_->set_filter_cutoff(10000.0f);	//4 to 10000
	controller_->set_filter_resonance(0.0f);	//0 to 0.85
	
	//Filter Envelopee
	controller_->filter_envelope()->set_attack(0.0f);	//0 to 1
	controller_->filter_envelope()->set_decay(0.0f);	//0 to 1
	controller_->filter_envelope()->set_sustain(1.0f);	//0 to 1
	controller_->filter_envelope()->set_release(1.0f);	//0 to 4
	
	//Arpeggio
	controller_->set_arpeggio_enabled(NO);
	controller_->set_arpeggio_octaves(3);	//1, 2, 3, 4, 5
	controller_->set_arpeggio_samples(0.5f);	//0 to 1
	controller_->set_arpeggio_step(synth::Arpeggio::UP_DOWN);
}

-(float) randFrom:(float)low to:(float)hi {
	float num = ((float)(arc4random()%1000)) * (hi-low)/1000 + low;
	return num;
}

-(void) randomize {
	//Modulation
	controller_->set_modulation_amount([self randFrom:0 to:1]); //0 to 1
	controller_->set_modulation_frequency([self randFrom:0 to:15]); //0 to 15
	
	//Oscillator
	// OSC 1
	controller_->set_osc1_level([self randFrom:0 to:1]);	//0 to 1

	// OSC 2
	controller_->set_osc2_level([self randFrom:0 to:1]);	//0 to 1
	
	controller_->set_glide_samples([self randFrom:0 to:0.04f]);	//0 to 0.04
	
	//Oscillator Detail
	float semis = [self randFrom:-12 to:12];	//-12 to 12
	float cents = [self randFrom:-100 to:100];	//-100 to 100
	int total = (int)semis + 100 * (int)cents;
	controller_->set_osc2_shift(total);
	controller_->set_osc_sync((bool)(arc4random()%2));
	
	//Filter
	controller_->set_filter_cutoff([self randFrom:4 to:10000]);	//4 to 10000
	controller_->set_filter_resonance([self randFrom:0 to:0.85f]);	//0 to 0.85
		
	//Arpeggio
	controller_->set_arpeggio_samples([self randFrom:0 to:1]);	//0 to 1
}

-(void) noteOn:(int)note {
	controller_->NoteOn(note);
}
-(void) noteOff:(int)note {
	controller_->NoteOff(note);
}

- (OSStatus)generateSamples:(AudioBufferList*)buffers {
  assert(controller_);
  assert(buffers->mNumberBuffers == 1);  // mono output  
  AudioBuffer* outputBuffer = &buffers->mBuffers[0];
  SInt32* data = (SInt32*)outputBuffer->mData;
  if (controller_->released()) {
    // Silence
    memset(data, 0, outputBuffer->mDataByteSize);
    return noErr;
  }
  int samples = outputBuffer->mDataByteSize / sizeof(SInt32);
  float buffer[samples];
  controller_->GetFloatSamples(buffer, samples);
  for (int i = 0; i < samples; ++i) {
    data[i] = buffer[i] * 16777216L;
  }
  return noErr;
}

-(void) dealloc {
	controller_->NoteOff();
	[super dealloc];
}

@end