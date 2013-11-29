#import <UIKit/UIKit.h>
#import <stdlib.h>

@interface ObjectCallback : NSObject {
@public
	NSObject *obj;
	NSString *callback;
}

+(id) createWithObject:(NSObject*)o withCallback:(NSString*)callbackString;
-(id) initWithObject:(NSObject*)o withCallback:(NSString*)callbackString;

@property (nonatomic, retain) NSObject *obj;
@property (nonatomic, retain) NSString *callback;

@end