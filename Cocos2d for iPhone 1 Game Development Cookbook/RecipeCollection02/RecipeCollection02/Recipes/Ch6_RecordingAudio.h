#import "Recipe.h"
#import <AVFoundation/AVFoundation.h>
#import <CoreAudio/CoreAudioTypes.h>
#import "CocosDenshion.h"

@interface Ch6_RecordingAudio : Recipe <AVAudioRecorderDelegate>
{
	AVAudioRecorder *recorder;
	NSURL * recordedTmpFile;
	CDSoundEngine *soundEngine;
	
	bool isRecording;
	
	CCMenuItemFont *recordItem;
	CCMenuItemFont *stopRecordingItem;
	CCMenuItemFont *playItem;
	
	CCSprite *slider;
	
	float pitch;
}

-(CCLayer*) runRecipe;

-(void) setSliderByPitch;

-(void) initAudioSession;
-(void) recordAudio;
-(void) playAudio;
-(void) stopRecordingAudio;
-(void) unloadAudioSession;

-(void) record:(id)sender;
-(void) stopRecording:(id)sender;
-(void) play:(id)sender;

@end

@implementation Ch6_RecordingAudio

-(CCLayer*) runRecipe {
	[super runRecipe];
	
	//Show message
	[self showMessage:@"Press the 'Record' button \nthen start speaking."];

	//Set initial pitch and recorded temp file object
	pitch = 1.0f;
	recordedTmpFile = nil;

	//Init audio session
	[self initAudioSession];

	//Create record, stop and play buttons
	[CCMenuItemFont setFontSize:40];
	[CCMenuItemFont setFontName:@"Marker Felt"];
	recordItem = [CCMenuItemFont itemFromString:@"Record" target:self selector:@selector(record:)]; recordItem.color = ccc3(255,0,0);
	stopRecordingItem = [CCMenuItemFont itemFromString:@"Stop Recording" target:self selector:@selector(stopRecording:)]; stopRecordingItem.color = ccc3(0,0,255);
	playItem = [CCMenuItemFont itemFromString:@"Play" target:self selector:@selector(play:)]; playItem.color = ccc3(0,255,0);

	stopRecordingItem.visible = NO;
	playItem.visible = NO;
	isRecording = NO;
	
	//Create our menu
	CCMenu *menu = [CCMenu menuWithItems: recordItem, stopRecordingItem, playItem, nil];
	[menu alignItemsVertically];
	menu.position = ccp(300,150);
	[self addChild:menu];

	//Create pitch label
	CCLabelTTF *label = [CCLabelTTF labelWithString:@"Pitch" fontName:@"Marker Felt" fontSize:30];
	label.position = ccp(100,200);
	[self addChild:label z:1];	
	
	//Create slider
	CCSprite *sliderBg = [CCSprite spriteWithFile:@"blank.png"];
	[sliderBg setTextureRect:CGRectMake(0,0,10,150)];
	sliderBg.color = ccc3(150,150,150);
	sliderBg.position = ccp(100,100);
	[self addChild:sliderBg z:0];
	
	slider = [CCSprite spriteWithFile:@"button_black.png"];
	[self addChild:slider z:1];
	
	//Set slider position
	[self setSliderByPitch];
	
	return self;
}

-(void) setSliderByPitch {
	//Set slider position based on pitch
	slider.position = ccp( 100, 100 + (pitch-1)*75 );
}

-(void) ccTouchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {	
	UITouch *touch = [touches anyObject];
    CGPoint point = [touch locationInView: [touch view]];
	point = [[CCDirector sharedDirector] convertToGL: point];
	
	//Did we touch the slider?
	if(point.x > 80 && point.x < 120 && point.y > 25 && point.y < 175 ){
		//Set pitch based on touch position
		pitch = (point.y-100)/75 + 1;
		if(pitch < 0.1f){ pitch = 0.2f; }
		
		//Set slider position
		[self setSliderByPitch];
	}
}

-(void) ccTouchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {	
	UITouch *touch = [touches anyObject];
    CGPoint point = [touch locationInView: [touch view]];
	point = [[CCDirector sharedDirector] convertToGL: point];
	
	//Did we touch the slider?
	if(point.x > 80 && point.x < 120 && point.y > 25 && point.y < 175 ){
		//Set pitch based on touch position
		pitch = (point.y-100)/75 + 1;
		if(pitch < 0.1f){ pitch = 0.2f; }
		
		//Set slider position
		[self setSliderByPitch];
	}
}

-(void) initAudioSession {
	//Our AVAudioSession singleton pointer
    AVAudioSession * audioSession = [AVAudioSession sharedInstance];
	
    //Setup the audioSession for playback and record.
    [audioSession setCategory:AVAudioSessionCategoryPlayAndRecord error:nil];
	
    //Activate the session
    [audioSession setActive:YES error:nil];
	
	//Init CDSoundEngine
	soundEngine = [[CDSoundEngine alloc] init];

	//Define source groups
	NSArray *defs = [NSArray arrayWithObjects: [NSNumber numberWithInt:1],nil]; 
	[soundEngine defineSourceGroups:defs];
}

-(void) recordAudio {
	//Set settings dictionary: IMA4 format, 44100 sample rate, 2 channels
	NSMutableDictionary* recordSetting = [[[NSMutableDictionary alloc] init] autorelease];
	[recordSetting setValue :[NSNumber numberWithInt:kAudioFormatAppleIMA4] forKey:AVFormatIDKey];
	[recordSetting setValue:[NSNumber numberWithFloat:44100.0] forKey:AVSampleRateKey]; 
	[recordSetting setValue:[NSNumber numberWithInt: 2] forKey:AVNumberOfChannelsKey];
		
	//Set recording temp file location on disk
	recordedTmpFile = [NSURL fileURLWithPath:[NSTemporaryDirectory() stringByAppendingPathComponent: [NSString stringWithString: @"recording.caf"]]];
        
	//Init AVAudioRecorder with location and settings
	recorder = [[AVAudioRecorder alloc] initWithURL:recordedTmpFile settings:recordSetting error:nil];
        
	//Set delegate and start recording
	[recorder setDelegate:self];
	[recorder prepareToRecord];       
	[recorder record];
}   

-(void) playAudio {
	//Override the audio to go back to the speaker
	UInt32 audioRouteOverride = kAudioSessionOverrideAudioRoute_Speaker;
	AudioSessionSetProperty(kAudioSessionProperty_OverrideAudioRoute, sizeof (audioRouteOverride),&audioRouteOverride);
	
	//Get the file path to the recorded audio
	NSString *filePath = [NSTemporaryDirectory() stringByAppendingPathComponent: [NSString stringWithString: @"recording.caf"]];

	//Play our recorded audio
	[soundEngine loadBuffer:0 filePath: filePath];
	[soundEngine playSound:0 sourceGroupId:0 pitch:pitch pan:0.0f gain:10.0f loop: NO];    
}

-(void) stopRecordingAudio {
	//Stop recording
	[recorder stop];
}

- (void) unloadAudioSession {
	//Remove temp file
	NSFileManager * fm = [NSFileManager defaultManager];
	if(recordedTmpFile){ [fm removeItemAtURL:recordedTmpFile error:nil]; }
	
	//Release recorder
	[recorder dealloc];
    recorder = nil;
	
	//Release sound engine
	[soundEngine release];
   
	//Deactivate audio session
	AVAudioSession * audioSession = [AVAudioSession sharedInstance];
	[audioSession setActive:NO error:nil];
}

-(void) record:(id)sender {
	if(!isRecording){
		//Record audio
		[self recordAudio];
		
		//Set variables
		isRecording = YES;
		recordItem.visible = NO;
		stopRecordingItem.visible = YES;
		playItem.visible = NO;
		
		//Show recording message
		[self showMessage:@"Recording..."];
	}
}

-(void) stopRecording:(id)sender {
	//Stop recording audio
	[self stopRecordingAudio];
	
	//Set variables
	isRecording = NO;
	recordItem.visible = YES;
	stopRecordingItem.visible = NO;
	playItem.visible = YES;	
	
}

-(void) play:(id)sender {
	//Play audio
	[self playAudio];
	
	//Set variables
	recordItem.visible = YES;
	stopRecordingItem.visible = NO;
	playItem.visible = YES;
}

-(void) cleanRecipe {
	//Unload audio session
	[self unloadAudioSession];
	[super cleanRecipe];
}

@end
