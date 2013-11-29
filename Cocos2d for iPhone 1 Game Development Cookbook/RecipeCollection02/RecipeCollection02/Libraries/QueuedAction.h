#import <UIKit/UIKit.h>
#import <stdlib.h>
#import "cocos2d.h"

#import "GameObject.h"

@interface QueuedAction : NSObject {
@public
	GameObject* gameObject;
	CCAction* action;
}

+(id) create;
-(id) init;

@property(readwrite, assign) GameObject* gameObject;
@property(readwrite, assign) CCAction* action;

@end