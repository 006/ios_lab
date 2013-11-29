#import <UIKit/UIKit.h>
#import <stdlib.h>

#import "cocos2d.h"

#import "ObjectCallback.h"

@class ObjectCallback;

@interface GameWaypoint : NSObject {
	@public
		CGPoint position;	//Where you are going to
		float speedMod;	//The relative speed. 0 is not moving. 1 is going as fast as possible.
		float lastDistance;	//How far you were from the waypoint last iteration.
		ObjectCallback *preCallback;		//Call this when we start moving toward the waypoint
		ObjectCallback *postCallback;		//Call this after we reach the waypoint
}

@property (readwrite, assign) CGPoint position;
@property (readwrite, assign) float speedMod;	//The relative speed. 0 is not moving. 1 is going as fast as possible.
@property (readwrite, assign) float lastDistance;
@property (nonatomic, retain) ObjectCallback *preCallback;
@property (nonatomic, retain) ObjectCallback *postCallback;

+(id) createWithPosition:(CGPoint)p withSpeedMod:(float)s;
-(id) initWithPosition:(CGPoint)p withSpeedMod:(float)s;
-(void) processPreCallback;
-(void) processPostCallback;

@end