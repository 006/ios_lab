#import "GameWaypoint.h"

enum {
	MOVE_TYPE_RUN = 0,
	MOVE_TYPE_JUMP = 1,
	MOVE_TYPE_FLY = 2
};

@interface SSGameWaypoint : GameWaypoint {
    int moveType;	//RUN, JUMP, FLY, etc
	bool active;	//Is the waypoint active?
	CGPoint launchVector;	//If we need to JUMP, the launch vector
}

@property (readwrite, assign) int moveType;
@property (readwrite, assign) bool active;
@property (readwrite, assign) CGPoint launchVector;

+(id) createWithPosition:(CGPoint)p moveType:(int)type speedMod:(float)s;
-(id) initWithPosition:(CGPoint)p moveType:(int)type speedMod:(float)s;

@end

@implementation SSGameWaypoint

@synthesize moveType, active, launchVector;

+(id) createWithPosition:(CGPoint)p moveType:(int)type speedMod:(float)s {
    return [[self alloc] initWithPosition:p moveType:type speedMod:s];
}

-(id) initWithPosition:(CGPoint)p moveType:(int)type speedMod:(float)s {
	if( (self = [self init]) ) {
		position = p;
		speedMod = s;
		lastDistance = 1000000.0f;
		moveType = type;
		active = YES;
		postCallback = nil;
		preCallback = nil;
	}
	return self;
}

@end