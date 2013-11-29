#import "GameObjectCallback.h"

@implementation GameObjectCallback

@synthesize gameObject, callback;

+(id) createWithObject:(GameObject*)object withCallback:(NSString*)callbackString {
    return [[self alloc] initWithObject:object withCallback:callbackString];
}

-(id) initWithObject:(GameObject*)object withCallback:(NSString*)callbackString{
	if( (self = [self init]) ) {
		gameObject = object;
		callback = callbackString;
	}
	return self;
}

@end