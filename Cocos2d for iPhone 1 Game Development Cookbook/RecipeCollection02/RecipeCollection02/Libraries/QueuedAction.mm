#import "QueuedAction.h"

@implementation QueuedAction

@synthesize gameObject, action;

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
	[super dealloc];
}

@end