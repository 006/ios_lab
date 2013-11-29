#import "Recipe.h"
#import "GameArea2D.h"
#import "GameObject.h"
#import "GameMisc.h"
#import "GameSensor.h"

@class GameArea2D;
@class GameObject;
@class GameMisc;
@class GameSensor;

@interface Ch4_PhysicalProperties : GameArea2D
{
	NSMutableArray *movableObjects;
	GameMisc *grabbedObject;
	b2Vec2 lastObjectVelocity;
	bool objectGrabbed;
}

-(CCLayer*) runRecipe;
-(void) addIceBlock;
-(void) addStickyBall;
-(void) addBouncyBall;
-(void) ccTouchesBegan:(NSSet *)touches withEvent:(UIEvent *)event;
-(void) ccTouchesMoved:(NSSet *)touches withEvent:(UIEvent *)event;
-(void) ccTouchesEnded:(NSSet *)touches withEvent:(UIEvent *)event;
-(void) handleCollisionWithObjA:(GameObject*)objA withObjB:(GameObject*)objB;
-(void) handleCollisionWithSensor:(GameSensor*)sensor withMisc:(GameMisc*)misc;
-(void) step: (ccTime) dt;
-(void) addBlockWithSpriteFile:(NSString*)file friction:(float)friction textureSize:(float)textureSize shapeSize:(float)shapeSize position:(CGPoint)p;
-(void) addBallWithShapeSize:(float)shapeSize restitution:(float)restitution position:(CGPoint)p color:(ccColor3B)color;
-(void) addLedge;

@end

@implementation Ch4_PhysicalProperties

-(CCLayer*) runRecipe {
	[super runRecipe];
	[message setString:@"Friction and restitution"];
	
	//Variable initialization
	movableObjects = [[NSMutableArray alloc] init];
	objectGrabbed = NO;
																																																					
	//Create level boundaries
	[self addLevelBoundaries];
			
	//Add a crate, a block of ice, bouncing balls and a ledge
	[self addBlockWithSpriteFile:@"crate2.png" friction:0.4f textureSize:64.0f shapeSize:20.0f position:ccp(130,250)];
	[self addBlockWithSpriteFile:@"ice_block.png" friction:0.0f textureSize:70.0f shapeSize:20.0f position:ccp(10,250)];
	[self addBallWithShapeSize:5.0f restitution:0.9f position:ccp(450,200) color:ccc3(255,0,0)];
	[self addBallWithShapeSize:10.0f restitution:0.8f position:ccp(400,200) color:ccc3(255,128,0)];
	[self addBallWithShapeSize:15.0f restitution:0.7f position:ccp(350,200) color:ccc3(255,255,0)];
	[self addBallWithShapeSize:20.0f restitution:0.6f position:ccp(300,200) color:ccc3(0,255,0)];
	[self addLedge];

	return self;
}

/* Add a block with a certain texture, size, position and friction */
-(void) addBlockWithSpriteFile:(NSString*)file friction:(float)friction textureSize:(float)textureSize shapeSize:(float)shapeSize position:(CGPoint)p {
	GameMisc *obj = [[GameMisc alloc] init];
	obj.gameArea = self;
	
	obj.bodyDef->type = b2_dynamicBody;
	obj.bodyDef->position.Set(p.x/PTM_RATIO, p.y/PTM_RATIO);
	obj.bodyDef->userData = obj;
	obj.body = world->CreateBody(obj.bodyDef);
	
	obj.fixtureDef->density = 1.0f;
	obj.fixtureDef->friction = friction;
	obj.fixtureDef->restitution = 0.25f;
	
	obj.sprite = [CCSprite spriteWithFile:file];
	obj.sprite.position = ccp(p.x,p.y);
	obj.sprite.scale = shapeSize / textureSize * 2;
		
	[gameNode addChild:obj.sprite z:2];
					
	obj.polygonShape = new b2PolygonShape();
	obj.polygonShape->SetAsBox(shapeSize/PTM_RATIO, shapeSize/PTM_RATIO);
	obj.fixtureDef->shape = obj.polygonShape;	
		
	obj.body->CreateFixture(obj.fixtureDef);

	[movableObjects addObject:obj];
	grabbedObject = obj;	
}

/* Add a ball with a certain size, position, color and restitution */
-(void) addBallWithShapeSize:(float)shapeSize restitution:(float)restitution position:(CGPoint)p color:(ccColor3B)color {
	GameMisc *obj = [[GameMisc alloc] init];
	obj.gameArea = self;
	
	obj.bodyDef->type = b2_dynamicBody;
	obj.bodyDef->position.Set(p.x/PTM_RATIO, p.y/PTM_RATIO);
	obj.bodyDef->userData = obj;
	obj.body = world->CreateBody(obj.bodyDef);

	obj.fixtureDef->density = 1.0f;
	obj.fixtureDef->friction = 1.0f;
	obj.fixtureDef->restitution = restitution;
	
	obj.sprite = [CCSprite spriteWithFile:@"ball2.png"];
	obj.sprite.position = ccp(p.x,p.y);
	obj.sprite.color = color;
		
	float textureSize = 128.0f;
	obj.sprite.scale = shapeSize / textureSize * 2;
		
	[gameNode addChild:obj.sprite z:2];
		
	obj.circleShape = new b2CircleShape();
	obj.circleShape->m_radius = shapeSize/PTM_RATIO;
	obj.fixtureDef->shape = obj.circleShape;
		
	obj.body->CreateFixture(obj.fixtureDef);

	[movableObjects addObject:obj];
	grabbedObject = obj;
}

/* Add a ledge polygon */
-(void) addLedge {
	GameMisc *obj = [[GameMisc alloc] init];
	obj.gameArea = self;

	obj.bodyDef->position.Set(0,100/PTM_RATIO);
	obj.body = world->CreateBody(obj.bodyDef);
			
	obj.fixtureDef->density = 1.0f;
	obj.fixtureDef->friction = 0.3f;
	obj.fixtureDef->restitution = 0.2f;
			
	float polygonSize = 4;
	CGPoint vertexArr[] = { ccp(0,0.8f), ccp(2,0.5f), ccp(2,0.7f), ccp(0,1) };
	int32 numVerts = 4;
	b2Vec2 vertices[4];
		
	NSMutableArray *vertexArray = [[NSMutableArray alloc] init];
		
	for(int i=0; i<numVerts; i++){
		vertices[i].Set(vertexArr[i].x*polygonSize, vertexArr[i].y*polygonSize);
		[vertexArray addObject:[NSValue valueWithCGPoint:ccp(vertexArr[i].x*PTM_RATIO*polygonSize, 
			vertexArr[i].y*PTM_RATIO*polygonSize)]];
	}
				
	ccTexParams params = {GL_NEAREST,GL_NEAREST_MIPMAP_NEAREST,GL_REPEAT,GL_REPEAT};
	obj.sprite = [TexturedPolygon createWithFile:@"bricks2.png" withVertices:vertexArray];
	[obj.sprite.texture setTexParameters:&params];
	[obj.sprite setPosition:ccp(0,100)];
	[gameNode addChild:obj.sprite z:1];		

	obj.polygonShape = new b2PolygonShape();
	obj.polygonShape->Set(vertices, numVerts);
	obj.fixtureDef->shape = obj.polygonShape;	
	
	obj.body->CreateFixture(obj.fixtureDef);
}

-(void) ccTouchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {
	UITouch *touch = [touches anyObject];
    CGPoint point = [touch locationInView: [touch view]];
	point = [[CCDirector sharedDirector] convertToGL: point];

	/* Grab the nearest object */
	float grabbedDistance = distanceBetweenPoints(point, ccp(grabbedObject.body->GetPosition().x*PTM_RATIO, grabbedObject.body->GetPosition().y*PTM_RATIO));	
	for(int i=0; i<movableObjects.count; i++){
		GameMisc *object = [movableObjects objectAtIndex:i];
		float thisDistance = distanceBetweenPoints(ccp(object.body->GetPosition().x*PTM_RATIO, object.body->GetPosition().y*PTM_RATIO), point);
		if(thisDistance < grabbedDistance){
			grabbedObject = object;
			grabbedDistance = thisDistance;
		}
	}
	
	objectGrabbed = YES;
	
	[self ccTouchesMoved:touches withEvent:event];
}

-(void) ccTouchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {		
	UITouch *touch = [touches anyObject];
    CGPoint point = [touch locationInView: [touch view]];
	point = [[CCDirector sharedDirector] convertToGL: point];
	
	/* Reposition the grabbed object */
	grabbedObject.body->SetTransform(b2Vec2(point.x/PTM_RATIO, point.y/PTM_RATIO), grabbedObject.body->GetAngle());

	b2Vec2 moveDistance = b2Vec2( (point.x/PTM_RATIO - grabbedObject.sprite.position.x/PTM_RATIO), (point.y/PTM_RATIO - grabbedObject.sprite.position.y/PTM_RATIO) );
	lastObjectVelocity = b2Vec2(moveDistance.x*20, moveDistance.y*20);
}

-(void) ccTouchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {		
	/* Release the object */
	objectGrabbed = NO;
	grabbedObject.body->SetLinearVelocity(lastObjectVelocity);
}

-(void) step: (ccTime) dt {	
	[super step:dt];
	
	/* Suspend the grabbed object */
	if(objectGrabbed){
		grabbedObject.body->SetLinearVelocity(b2Vec2_zero);
	}
}

@end
