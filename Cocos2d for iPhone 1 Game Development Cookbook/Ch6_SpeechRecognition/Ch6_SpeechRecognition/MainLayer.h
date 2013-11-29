#import "cocos2d.h"
#import "AudioSessionManager.h"
#import "OpenEarsEventsObserver.h"
#import "PocketsphinxController.h"
#import "FliteController.h"

@interface MainLayer : CCLayer <OpenEarsEventsObserverDelegate>
{
	CCLabelBMFont *message;

	AudioSessionManager *audioSessionManager;
	PocketsphinxController *pocketsphinxController; 
	FliteController *fliteController;
	OpenEarsEventsObserver *openEarsEventsObserver;

	CCMenuItemFont *suspendRecognitionItem;
	CCMenuItemFont *resumeRecognitionItem;
	CCMenuItemFont *stopListeningItem;
	CCMenuItemFont *startListeningItem;
}

+(CCScene *) scene;

-(void) say:(NSString*)str;
-(void) welcomeMessage;
-(void) saySomething;
-(void) addBackground;
-(void) showMessage:(NSString*)m;

-(void) suspendRecognition;
-(void) resumeRecognition;
-(void) stopListening;
-(void) startListening;

@end
