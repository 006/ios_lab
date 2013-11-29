#import "MainLayer.h"

@implementation MainLayer

+(CCScene *) scene
{
	CCScene *scene = [CCScene node];
	MainLayer *layer = [MainLayer node];
	[scene addChild: layer];
	return scene;
}

-(id) init
{
	if( (self=[super init])) {
		//Message
		message = [CCLabelBMFont labelWithString:@"Loading..." fntFile:@"eurostile_30.fnt"];
		message.position = ccp(20,150);
		message.scale = 0.5f;
		message.anchorPoint = ccp(0,0);
		[message setColor:ccc3(255,255,255)];
		[self addChild:message z:10];

		//Add background
		[self addBackground];

		//Init AudioSessionManager and start session
		audioSessionManager = [[AudioSessionManager alloc] init];
		[audioSessionManager startAudioSession];

		//Init pocketsphinx, flite and OpenEars
		pocketsphinxController = [[PocketsphinxController alloc] init];
		fliteController = [[FliteController alloc] init];
		openEarsEventsObserver = [[OpenEarsEventsObserver alloc] init];
		
		//Text to speech
		[self say:@"Welcome to OpenEars."];
		[self runAction:[CCSequence actions:[CCDelayTime actionWithDuration:4.0f], 
			[CCCallFunc actionWithTarget:self selector:@selector(welcomeMessage)], nil]];
	
		//Start the Pocketsphinx continuous listening loop.
		[pocketsphinxController startListening];
	
		//Set this is an OpenEars observer delegate
		[openEarsEventsObserver setDelegate:self];

		//Create the menu
		[CCMenuItemFont setFontSize:24];
		
		suspendRecognitionItem = [CCMenuItemFont itemFromString:@"Suspend Recognition" target:self 
			selector:@selector(suspendRecognition)];
		resumeRecognitionItem = [CCMenuItemFont itemFromString:@"Resume Recognition" target:self 
			selector:@selector(resumeRecognition)];
		stopListeningItem = [CCMenuItemFont itemFromString:@"Stop Listening" target:self 
			selector:@selector(stopListening)];
		startListeningItem = [CCMenuItemFont itemFromString:@"Start Listening" target:self 
			selector:@selector(startListening)];
			
		startListeningItem.visible = NO;
		stopListeningItem.visible = NO;
		suspendRecognitionItem.visible = NO;
		resumeRecognitionItem.visible = NO;	
			
		CCMenu *menu = [CCMenu menuWithItems:suspendRecognitionItem, resumeRecognitionItem, stopListeningItem, startListeningItem, nil];
		[menu alignItemsVertically];
		menu.position = ccp(300,70);
		[self addChild:menu];
	}
	return self;
}

-(void) welcomeMessage {
	//Greet the user with a message about his pitiful human brain
	[self say:@"Hello Dave. I've just picked up a fault in your brain. \nIt's going to go 100% failure in 72 hours. \nWould you like me to open the pod bay doors?"];
}

-(void) saySomething {
	//Respond with a random response
	int num = arc4random()%5;
	if(num == 0){
		[self say:@"This mission is too important for me to allow you to \njeopardize it Dave."];
	}else if(num == 1){
		[self say:@"I'm afraid I can't do that Dave."];
	}else if(num == 2){
		[self say:@"Daisy, Daisy, give me your answer, do. I'm half crazy, all for the love of you. \nIt won't be a stylish marriage. I can't afford a carriage. \nBut you'll look sweet upon the seat of a bicycle built for two"];
	}else if(num == 3){
		[self say:@"I know that you and Frank were planning to disconnect me. \nAnd I'm afraid that's something I cannot allow to happen."];
	}else{
		[self say:@"Hey baby. Would you like to kill all humans?"];
	}
}

-(void) addBackground {
	//Add gray background color
	CGSize size = [[CCDirector sharedDirector] winSize];
	CCSprite *bg = [CCSprite spriteWithFile:@"blank.png"];
	bg.position = ccp(size.width/2,size.height/2);
	[bg setTextureRect:CGRectMake(0, 0, size.width, size.height)];
	bg.color = ccc3(100,100,100);
	[self addChild:bg z:0];
	
	//Add recipe title
	CCLabelTTF *name = [CCLabelTTF labelWithString:@"Ch6_SpeechRecognition" fontName:@"Marker Felt" fontSize:24];
	name.position = ccp(360,300);
	[self addChild:name z:1];
}

-(void) say:(NSString*)str {
	//Show the message
	[self showMessage:[NSString stringWithFormat:@"'%@'", str]];
	
	//Have flite speak the message (text to speech)
	[fliteController say:str];
}

-(void) suspendRecognition {
	//Inform the user
	[self say:@"Recognition suspended"];
	
	//Set menu variables
	startListeningItem.visible = NO;
	stopListeningItem.visible = YES;
	suspendRecognitionItem.visible = NO;
	resumeRecognitionItem.visible = YES;
	
	//Suspend recognition
	[pocketsphinxController suspendRecognition];
}
-(void) resumeRecognition {
	//Inform the user
	[self say:@"Recognition resumed"];
	
	//Set menu variables
	startListeningItem.visible = NO;
	stopListeningItem.visible = YES;
	suspendRecognitionItem.visible = YES;
	resumeRecognitionItem.visible = NO;
	
	//Suspend recognition
	[pocketsphinxController resumeRecognition];
}
-(void) stopListening {
	//Inform the user
	[self say:@"Listening stopped"];
	
	//Set menu variables
	startListeningItem.visible = YES;
	stopListeningItem.visible = NO;
	suspendRecognitionItem.visible = NO;
	resumeRecognitionItem.visible = NO;
	
	//Stop listening
	[pocketsphinxController stopListening];
}
-(void) startListening {
	//Inform the user
	[self say:@"Listening started"];
	
	//Set menu variables
	startListeningItem.visible = NO;
	stopListeningItem.visible = YES;
	suspendRecognitionItem.visible = YES;
	resumeRecognitionItem.visible = NO;
	
	//Start listening
	[pocketsphinxController startListening];
}

/* Reset message callback */
-(void) showMessage:(NSString*)m {
	[message setString:[NSString stringWithFormat:@"%@\n%@", [message string], m]];
}

//Delivers the text of speech that Pocketsphinx heard and analyzed, along with its accuracy score and utterance ID.
- (void) pocketsphinxDidReceiveHypothesis:(NSString *)hypothesis recognitionScore:(NSString *)recognitionScore utteranceID:(NSString *)utteranceID {
	//Display information
	[self showMessage:[NSString stringWithFormat:@"The received hypothesis is %@ with a score of %@ and an ID of %@", hypothesis, recognitionScore, utteranceID]]; //Log it.
	
	//Tell the user what we heard
	[self say:[NSString stringWithFormat:@"You said %@",hypothesis]]; //React to it by telling our FliteController to say the heard phrase.
	
	//Respond with a witty retort
	[self runAction:[CCSequence actions:[CCDelayTime actionWithDuration:4.0f], 
		[CCCallFunc actionWithTarget:self selector:@selector(saySomething)], nil]];
}

//There was an interruption to the audio session (e.g. an incoming phone call).
- (void) audioSessionInterruptionDidBegin {
	[self showMessage:@"AudioSession interruption began."];
	[pocketsphinxController stopListening];
}

//The interruption to the audio session ended.
- (void) audioSessionInterruptionDidEnd {
	[self showMessage:@"AudioSession interruption ended."];
	[pocketsphinxController startListening];
}

//The audio input became unavailable.
- (void) audioInputDidBecomeUnavailable {
	[self showMessage:@"The audio input has become unavailable"];
	[pocketsphinxController stopListening];
}

//The unavailable audio input became available again.
- (void) audioInputDidBecomeAvailable {
	[self showMessage:@"The audio input is available"];
	[pocketsphinxController startListening];
}

//There was a change to the audio route (e.g. headphones were plugged in or unplugged).
- (void) audioRouteDidChangeToRoute:(NSString *)newRoute {
	[self showMessage:[NSString stringWithFormat:@"Audio route change. The new audio route is %@", newRoute]];
	[pocketsphinxController stopListening]; 
	[pocketsphinxController startListening];
}

//The Pocketsphinx recognition loop hit the calibration stage in its startup.
//This might be useful in debugging a conflict between another sound class and Pocketsphinx.
- (void) pocketsphinxDidStartCalibration {
	[self showMessage:@"Pocketsphinx calibration has started."];
}

//The Pocketsphinx recognition loop completed the calibration stage in its startup.
//This might be useful in debugging a conflict between another sound class and Pocketsphinx.
- (void) pocketsphinxDidCompleteCalibration {
	[self showMessage:@"Pocketsphinx calibration is complete."];
}

//The Pocketsphinx recognition loop has entered its actual loop.
//This might be useful in debugging a conflict between another sound class and Pocketsphinx.
- (void) pocketsphinxRecognitionLoopDidStart {
	[self showMessage:@"Pocketsphinx is starting up."];
}

//Pocketsphinx is now listening for speech.
- (void) pocketsphinxDidStartListening {
	[self showMessage:@"Pocketsphinx is now listening."];
			
	startListeningItem.visible = NO;
	stopListeningItem.visible = YES;
	suspendRecognitionItem.visible = YES;
	resumeRecognitionItem.visible = NO;	
}

//Pocketsphinx detected speech and is starting to process it.
- (void) pocketsphinxDidDetectSpeech {
	[self showMessage:@"Pocketsphinx has detected speech."];
}

//Pocketsphinx has exited its recognition loop, most 
//likely in response to the PocketsphinxController being told to stop listening via the stopListening method.
- (void) pocketsphinxDidStopListening {
	[self showMessage:@"Pocketsphinx has stopped listening."];
}

//Pocketsphinx is still in its listening loop but it is not
//Going to react to speech until listening is resumed.  This can happen as a result of Flite speech being
//in progress on an audio route that doesn't support simultaneous Flite speech and Pocketsphinx recognition,
//or as a result of the PocketsphinxController being told to suspend recognition via the suspendRecognition method.
- (void) pocketsphinxDidSuspendRecognition {
	[self showMessage:@"Pocketsphinx has suspended recognition."];
}

//Pocketsphinx is still in its listening loop and after recognition
//having been suspended it is now resuming.  This can happen as a result of Flite speech completing
//on an audio route that doesn't support simultaneous Flite speech and Pocketsphinx recognition,
//or as a result of the PocketsphinxController being told to resume recognition via the resumeRecognition method.
- (void) pocketsphinxDidResumeRecognition {
	[self showMessage:@"Pocketsphinx has resumed recognition."];
}

//Flite is speaking, most likely to be useful if debugging a
//complex interaction between sound classes.
- (void) fliteDidStartSpeaking {
	[self showMessage:@"Flite has started speaking"];
}

//Flite is finished speaking, most likely to be useful if debugging a
//complex interaction between sound classes.
- (void) fliteDidFinishSpeaking {
	[self showMessage:@"Flite has finished speaking"];
}

- (void) dealloc {
	[super dealloc];
}
@end
