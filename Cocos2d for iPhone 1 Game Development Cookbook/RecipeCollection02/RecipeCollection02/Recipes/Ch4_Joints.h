#import "Recipe.h"
#import "GameArea2D.h"
#import "GameObject.h"
#import "GameMisc.h"
#import "GameSensor.h"

@class GameArea2D;
@class GameObject;
@class GameMisc;
@class GameSensor;

@interface Ch4_Joints : GameArea2D
{
	NSMutableArray *movableObjects;
	GameMisc *grabbedObject;
	b2Vec2 lastObjectVelocity;
	bool objectGrabbed;
}

-(CCLayer*) runRecipe;
-(void) addSeeSaw;
-(void) addBoxWithPosition:(CGPoint)p file:(NSString*)file density:(float)density;
-(void) ccTouchesBegan:(NSSet *)touches withEvent:(UIEvent *)event;
-(void) ccTouchesMoved:(NSSet *)touches withEvent:(UIEvent *)event;
-(void) ccTouchesEnded:(NSSet *)touches withEvent:(UIEvent *)event;
-(void) step: (ccTime) dt;

@end

@implementation Ch4_Joints

-(CCLayer*) runRecipe {
	[super runRecipe];		
	[message setString:@"Drop the weight on the see-saw"];
	
	//Initialization
	movableObjects = [[NSMutableArray alloc] init];
	objectGrabbed = NO;
																											
	//Create level boundaries
	[self addLevelBoundaries];
			
	//Add objects
	[self addSeeSaw];
	[self addBoxWithPosition:ccp(130,120) file:@"crate2.png" density:1.0f];
	[self addBoxWithPosition:ccp(160,120) file:@"crate2.png" density:1.0f];
	[self addBoxWithPosition:ccp(145,150) file:@"crate2.png" density:1.0f];
	[self addBoxWithPosition:ccp(270,100) file:@"weight.png" density:15.0f];

	return self;
}

/* Create a complex see-saw object */
-(void) addSeeSaw {
	/* The triangle is the static base of the see-saw */
	CGPoint trianglePosition = ccp(240,50);
	
	GameMisc *triangle = [[GameMisc alloc] init];
	triangle.gameArea = self;
	
	triangle.bodyDef->type = b2_staticBody;
	triangle.bodyDef->position.Set(trianglePosition.x/PTM_RATIO, trianglePosition.y/PTM_RATIO);
	triangle.body = world->CreateBody(triangle.bodyDef);
	
	//Our triangle polygon
	float polygonSize = 2.0f;
			
	CGPoint vertexArr[] = { ccp(0,0), ccp(1,0), ccp(0.5f,1) };
	int32 numVerts = 3;
	b2Vec2 vertices[3];
	
	NSMutableArray *vertexArray = [[NSMutableArray alloc] init];
	
	for(int i=0; i<numVerts; i++){
		vertices[i].Set(vertexArr[i].x*polygonSize, vertexArr[i].y*polygonSize);
		[vertexArray addObject:[NSValue valueWithCGPoint:ccp(vertexArr[i].x*PTM_RATIO*polygonSize, 
			vertexArr[i].y*PTM_RATIO*polygonSize)]];
	}
			
	ccTexParams params = {GL_NEAREST,GL_NEAREST_MIPMAP_NEAREST,GL_REPEAT,GL_REPEAT};
	triangle.sprite = [TexturedPolygon createWithFile:@"box.png" withVertices:vertexArray];
	[triangle.sprite.texture setTexParameters:&params];
	[triangle.sprite setPosition:ccp(trianglePosition.x,trianglePosition.y)];
	
	[gameNode addChild:triangle.sprite z:1];		
		
	triangle.polygonShape = new b2PolygonShape();
	triangle.polygonShape->Set(vertices, numVerts);
	triangle.fixtureDef->shape = triangle.polygonShape;

	triangle.body->CreateFixture(triangle.fixtureDef);	
	
	
	/* The plank is the dynamic part of the see-saw */
	CGPoint plankPosition = ccp(270,80);	
	
	GameMisc *plank = [[GameMisc alloc] init];
	plank.gameArea = self;
	
	plank.bodyDef->type = b2_dynamicBody;
	plank.bodyDef->position.Set(plankPosition.x/PTM_RATIO, plankPosition.y/PTM_RATIO);
	plank.body = world->CreateBody(plank.bodyDef);	
	
	plank.body->SetTransform(b2Vec2(plankPosition.x/PTM_RATIO, plankPosition.y/PTM_RATIO),PI/2);
	
	CGPoint textureSize = ccp(54,215);
	CGPoint shapeSize = ccp(12,180);
	
	plank.sprite = [CCSprite spriteWithFile:@"column2.png"];
	plank.sprite.position = ccp(plankPosition.x,plankPosition.y);
	plank.sprite.scaleX = shapeSize.x / textureSize.x * 2;
	plank.sprite.scaleY = shapeSize.y / textureSize.y * 2;
		
	[gameNode addChild:plank.sprite z:1];		
		
	plank.polygonShape = new b2PolygonShape();
	plank.polygonShape->SetAsBox(shapeSize.x/PTM_RATIO, shapeSize.y/PTM_RATIO);
	plank.fixtureDef->shape = plank.polygonShape;

	plank.body->CreateFixture(plank.fixtureDef);
	
	/* We initialize a revolute joint linking the plank to the triangle */
	b2RevoluteJointDef rjd;
	b2RevoluteJoint* joint;
	
	rjd.Initialize(plank.body, triangle.body, b2Vec2(trianglePosition.x/PTM_RATIO + polygonSize/2, trianglePosition.y/PTM_RATIO + polygonSize/2));

	joint = (b2RevoluteJoint*)world->CreateJoint(&rjd);
}

/* Add a box object at a position */
-(void) addBoxWithPosition:(CGPoint)p file:(NSString*)file density:(float)density {
	float textureSize = 64.0f;
	float shapeSize = 20.0f;

	GameMisc *obj = [[GameMisc alloc] init];
	obj.gameArea = self;
	
	obj.bodyDef->type = b2_dynamicBody;
	obj.bodyDef->position.Set(p.x/PTM_RATIO, p.y/PTM_RATIO);
	obj.bodyDef->userData = obj;
	obj.body = world->CreateBody(obj.bodyDef);
	
	obj.fixtureDef->density = density;
	obj.fixtureDef->friction = 0.5f;
	obj.fixtureDef->restitution = 0.0f;
	
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
