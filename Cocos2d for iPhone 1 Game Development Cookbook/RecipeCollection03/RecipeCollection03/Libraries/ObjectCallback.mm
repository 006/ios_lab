#import "ObjectCallback.h"

@implementation ObjectCallback

@synthesize obj, callback;

+(id) createWithObject:(NSObject*)o withCallback:(NSString*)callbackString {
    return [[self alloc] initWithObject:o withCallback:callbackString];
}

-(id) initWithObject:(NSObject*)o withCallback:(NSString*)callbackString{
	if( (self = [self init]) ) {
		obj = o;
		callback = callbackString;
	}
	return self;
}

@end