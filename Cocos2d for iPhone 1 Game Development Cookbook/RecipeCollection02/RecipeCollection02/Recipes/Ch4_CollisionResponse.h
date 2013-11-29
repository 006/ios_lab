#import "Recipe.h"
#import "GameArea2D.h"
#import "GameObject.h"
#import "GameSensor.h"
#import "GameMisc.h"

@class GameArea2D;
@class GameObject;
@class GameSensor;
@class GameMisc;

@interface Ch4_CollisionResponse : GameArea2D
{
}

-(CCLayer*) runRecipe;
-(void) handleCollisionWithObjA:(GameObject*)objA withObjB:(GameObject*)objB;
-(void) handleCollisionWithSensor:(GameSensor*)sensor withMisc:(GameMisc*)misc;
-(void) handleCollisionWithMisc:(GameMisc*)a withMisc:(GameMisc*)b;
-(void) addNewObjectWithCoords:(CGPoint)p;
-(void) ccTouchesEnded:(NSSet *)touches withEvent:(UIEvent *)event;

@end



//Implementation
@implementation Ch4_CollisionResponse

-(CCLayer*) runRecipe {
	[super runRecipe];
	[message setString:@"Collision response and sensors."];
	
	//Show debug drawing so we can see the sensor
	[self showDebugDraw];
	
	//Create Sensor
	GameSensor *gameObjSensor = [[GameSensor alloc] init];
	gameObjSensor.gameArea = self;
	
	gameObjSensor.bodyDef->type = b2_staticBody;
	gameObjSensor.bodyDef->position.Set(240/PTM_RATIO,160/PTM_RATIO);
	gameObjSensor.bodyDef->userData = gameObjSensor;
				
	gameObjSensor.body = world->CreateBody(gameObjSensor.bodyDef);
	
	gameObjSensor.circleShape = new b2CircleShape();
	gameObjSensor.circleShape->m_radius = 1.0f;
	gameObjSensor.fixtureDef->shape = gameObjSensor.circleShape;
	gameObjSensor.fixtureDef->isSensor = YES;
	
	gameObjSensor.body->CreateFixture(gameObjSensor.fixtureDef);
	
	//Create level boundaries
	[self addLevelBoundaries];

	//Add block batch sprite
	CCSpriteBatchNode *batch = [CCSpriteBatchNode batchNodeWithFile:@"blocks.png" capacity:150];
	[gameNode addChild:batch z:0 tag:0];

	return self;
}

/* Our base collision handling routine */
-(void) handleCollisionWithObjA:(GameObject*)objA withObjB:(GameObject*)objB {	
	//SENSOR to MISC collision
	if(objA.type == GO_TYPE_SENSOR && objB.type == GO_TYPE_MISC){
		[self handleCollisionWithSensor:(GameSensor*)objA withMisc:(GameMisc*)objB];
	}else if(objA.type == GO_TYPE_MISC && objB.type == GO_TYPE_SENSOR){
		[self handleCollisionWithSensor:(GameSensor*)objB withMisc:(GameMisc*)objA];
	}
	
	//MISC to MISC collision
	else if(objA.type == GO_TYPE_MISC && objB.type == GO_TYPE_MISC){
		[self handleCollisionWithMisc:(GameMisc*)objA withMisc:(GameMisc*)objB];
	}
}

/* Handling collision between specific types of objects */
-(void) handleCollisionWithSensor:(GameSensor*)sensor withMisc:(GameMisc*)misc {
	[message setString:@"Box collided with sensor"];
	
	[self runAction:[CCSequence actions:[CCDelayTime actionWithDuration:0.5f], 
		[CCCallFunc actionWithTarget:self selector:@selector(resetMessage)], nil]];
}

-(void) handleCollisionWithMisc:(GameMisc*)a withMisc:(GameMisc*)b {
	[message setString:@"Box collided with another box"];
	
	[self runAction:[CCSequence actions:[CCDelayTime actionWithDuration:0.5f], 
		[CCCallFunc actionWithTarget:self selector:@selector(resetMessage)], nil]];	
}

/* Adding a new block */
-(void) addNewObjectWithCoords:(CGPoint)p {
	CCSpriteBatchNode *batch = (CCSpriteBatchNode*) [gameNode getChildByTag:0];

	GameMisc *obj = [[GameMisc alloc] init];
	obj.gameArea = self;
		
	int idx = (CCRANDOM_0_1() > .5 ? 0:1);
	int idy = (CCRANDOM_0_1() > .5 ? 0:1);
	obj.sprite = [CCSprite spriteWithBatchNode:batch rect:CGRectMake(32 * idx,32 * idy,32,32)];
	[batch addChild:obj.sprite];
	
	obj.sprite.position = ccp( p.x, p.y);
	
	obj.bodyDef->type = b2_dynamicBody;
	obj.bodyDef->position.Set(p.x/PTM_RATIO, p.y/PTM_RATIO);
	obj.bodyDef->userData = obj;
	obj.body = world->CreateBody(obj.bodyDef);
	
	// Define another box shape for our dynamic body.
	obj.polygonShape = new b2PolygonShape();
	obj.polygonShape->SetAsBox(.5f, .5f);//These are mid points for our 1m box
	obj.fixtureDef->shape = obj.polygonShape;
	
	obj.fixtureDef->density = 1.0f;
	obj.fixtureDef->friction = 0.3f;
	
	obj.body->CreateFixture(obj.fixtureDef);
}

- (void)ccTouchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {
	for( UITouch *touch in touches ) {
		CGPoint location = [touch locationInView: [touch view]];
		location = [[CCDirector sharedDirector] convertToGL: location];
		[self addNewObjectWithCoords: location];
	}
}

@end
