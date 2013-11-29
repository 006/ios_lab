#import <UIKit/UIKit.h>
#import <stdlib.h>
#import "cocos2d.h"

#import "GameObject.h"

@interface GameObjectCallback : NSObject {
@public
	GameObject *gameObject;
	NSString *callback;
}

+(id) createWithObject:(GameObject*)object withCallback:(NSString*)callbackString;
-(id) initWithObject:(GameObject*)object withCallback:(NSString*)callbackString;

@property (nonatomic, retain) GameObject *gameObject;
@property (nonatomic, retain) NSString *callback;

@end