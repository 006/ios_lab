#import "Box2DBodyInfo.h"

@implementation Box2DBodyInfo

@synthesize bodyDef, fixtureDef, gameObject;

+(id) create {
    return [[self alloc] init];
}

-(id) init {
    if( (self=[super init]) ) {
		//Initialization
    }
    return self;
}

-(void) dealloc {
	delete bodyDef;
	delete fixtureDef;
		
	[super dealloc];
}

@end