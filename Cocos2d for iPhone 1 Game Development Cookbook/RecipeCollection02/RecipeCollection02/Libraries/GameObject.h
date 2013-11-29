#import "cocos2d.h"
#import "Box2D.h"
#import "b2Body.h"
#import "GameObjectTypes.h"

@class GameArea2D;

@interface GameObject : CCNode {
	@public
		GameArea2D *gameArea;
		b2Body *body;
		b2BodyDef *bodyDef;
		b2FixtureDef *fixtureDef;
		b2PolygonShape *polygonShape;
		b2CircleShape *circleShape;
		CCSprite *sprite;
		int typeTag;
		bool markedForDestruction;
}
@property (readwrite, assign) GameArea2D *gameArea;
@property (readwrite, assign) b2Body *body;
@property (nonatomic, assign) b2BodyDef *bodyDef;
@property (nonatomic, assign) b2FixtureDef *fixtureDef;
@property (nonatomic, assign) b2PolygonShape *polygonShape;
@property (nonatomic, assign) b2CircleShape *circleShape;
@property (nonatomic, retain) CCSprite *sprite;
@property (readwrite, assign) int typeTag;
@property (readwrite, assign) bool markedForDestruction;
@property (readonly) int type;

-(id) init;
-(void) initBox2D;
-(int) type;

@end