#import <UIKit/UIKit.h>
#import <stdlib.h>
#import "cocos2d.h"

#import "GameObject.h"

#import "Box2D.h"
#import "GLES-Render.h"
#import "Box2DBodyInfo.h"

@class GameWaypoint;

@interface GameActor : GameObject {
	@public	
		NSMutableArray *waypoints;
		int timesBlocked;
		float32 lastAngularVelocity;
		CGPoint lastVelocity;
		NSString *color;
		
		float runSpeed;
		int direction;
		float animCount;
}

@property (nonatomic, retain) NSMutableArray * waypoints;
@property (readwrite, assign) float runSpeed;
@property (readwrite, assign) int direction;
@property (readwrite, assign) CGPoint lastVelocity;
@property (readwrite, assign) NSString *color;
@property (readwrite, assign) float animCount;
	
-(id) init;
-(void) addWaypoint:(GameWaypoint*)waypoint;
-(void) processWaypoints;
-(void) runWithVector:(CGPoint)v withSpeedMod:(float)speedMod withConstrain:(bool)constrain;
-(void) stopRunning;
+(CGPoint) getNormalVectorFromDirection:(int)dir;
-(void) animateActor;
-(void) timeShift:(float)seconds vect:(CGPoint)vect magnitude:(float)magnitude;

@end