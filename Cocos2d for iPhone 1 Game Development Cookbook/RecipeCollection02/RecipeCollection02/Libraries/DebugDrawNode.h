#import <UIKit/UIKit.h>
#import <Foundation/Foundation.h>
#import <stdlib.h>

#import "cocos2d.h"

#import "Box2D.h"
#import "GLES-Render.h"

@interface DebugDrawNode : CCLayer {
	b2World* world;
}

@property (readwrite, assign) b2World *world;

+(id) createWithWorld:(b2World*)worldPtr;
-(id) initWithWorld:(b2World*)worldPtr;

@end