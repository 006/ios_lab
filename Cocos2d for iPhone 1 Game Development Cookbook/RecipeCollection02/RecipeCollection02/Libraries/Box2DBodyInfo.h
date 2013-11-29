#import "cocos2d.h"
#import "Box2D.h"
#import "GameObject.h"

@interface Box2DBodyInfo : NSObject {
@public
	b2BodyDef* bodyDef;
	b2FixtureDef* fixtureDef;
	GameObject* gameObject;
}

+(id) create;
-(id) init;

@property(readwrite, assign) b2BodyDef* bodyDef;
@property(readwrite, assign) b2FixtureDef* fixtureDef;
@property(nonatomic, retain) GameObject* gameObject;

@end