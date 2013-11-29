//NOTE: Take out the collision filtering here. Use it in its own recipe.


#import "Recipe.h"
#import "GameArea2D.h"
#import "GameObject.h"
#import "GameMisc.h"
#import "GameSensor.h"

@class GameArea2D;
@class GameObject;
@class GameMisc;
@class GameSensor;

enum {
	CB_CAR = 1<<2,
	CB_WHEEL = 1<<4
};

@interface Ch4_Vehicles : GameArea2D
{
	GameMisc *taxi;
	GameMisc *wheel1;
	GameMisc *wheel2;
	bool pressedLeft;
	bool pressedRight;
}

-(CCLayer*) runRecipe;
-(void) createLevel;
-(void) addBrickWallSpriteAtPosition:(CGPoint)p;
-(void) addTaxi;
-(void) ccTouchesBegan:(NSSet *)touches withEvent:(UIEvent *)event;
-(void) ccTouchesMoved:(NSSet *)touches withEvent:(UIEvent *)event;
-(void) ccTouchesEnded:(NSSet *)touches withEvent:(UIEvent *)event;
-(void) step: (ccTime) dt;

@end

@implementation Ch4_Vehicles

-(CCLayer*) runRecipe {
	[super runRecipe];
	[message setString:@"Press and hold to drive car."];	
	
	//Initialization
	pressedLeft = NO;
	pressedRight = NO;
			
	//Create level
	[self createLevel];

	//Add taxi
	[self addTaxi];

	return self;
}

-(void) createLevel {
	/* Create a sine wave road for our car */
	b2BodyDef groundBodyDef;
	groundBodyDef.position.Set(0, 0);
	b2Body *body = world->CreateBody(&groundBodyDef);
		
	b2PolygonShape groundBox;	
	
	b2FixtureDef groundFixtureDef;
	groundFixtureDef.restitution = 0.0f;
	groundFixtureDef.friction = 10.0f;	//The road has a lot of friction
	groundFixtureDef.filter.categoryBits = CB_GROUND;
	groundFixtureDef.filter.maskBits = CB_CAR | CB_WHEEL;

	groundBox.SetAsEdge(b2Vec2(-960/PTM_RATIO,0), b2Vec2(-960/PTM_RATIO,200/PTM_RATIO));
	groundFixtureDef.shape = &groundBox;
	body->CreateFixture(&groundFixtureDef);
	
	groundBox.SetAsEdge(b2Vec2(960/PTM_RATIO,0), b2Vec2(960/PTM_RATIO,200/PTM_RATIO));
	groundFixtureDef.shape = &groundBox;
	body->CreateFixture(&groundFixtureDef);
		
	float32 x1; float32 y1;
	for(int u = -1; u < 2; u++){
		//Add Edge Shapes
		x1 = -15.0f;
		y1 = 2.0f * cosf(x1 / 10.0f * b2_pi);	
		for (int32 i = 0; i < 60; ++i)
		{
			float32 x2 = x1 + 0.5f;
			float32 y2 = 2.0f * cosf(x2 / 10.0f * b2_pi);

			b2PolygonShape shape;
			shape.SetAsEdge(b2Vec2(x1 + u*960/PTM_RATIO, y1), b2Vec2(x2 + u*960/PTM_RATIO, y2));
			body->CreateFixture(&shape, 0.0f);

			x1 = x2;
			y1 = y2;
		}
		
		//Add corresponding graphics
		CCSprite *bg = [CCSprite spriteWithFile:@"road_bg.png"];
		bg.position = ccp(u*960,70);
		[gameNode addChild:bg z:0];
	
		CCSprite *fg = [CCSprite spriteWithFile:@"road_fg.png"];
		fg.position = ccp(u*960,70);
		[gameNode addChild:fg z:2];		
	}
	
	/* Add two bricks walls so you can't drive off the course */
	[self addBrickWallSpriteAtPosition:ccp(970,60)];
	[self addBrickWallSpriteAtPosition:ccp(-970,60)];
}

/* Create a vertical wall at a position */
-(void) addBrickWallSpriteAtPosition:(CGPoint)p  {
	float polygonSize = 4;
	CGPoint vertexArr[] = { ccp(0,0), ccp(0.1f,0), ccp(0.1f,1), ccp(0,1) };
	int32 numVerts = 4;
		
	NSMutableArray *vertexArray = [[NSMutableArray alloc] init];
		
	for(int i=0; i<numVerts; i++){
		[vertexArray addObject:[NSValue valueWithCGPoint:ccp(vertexArr[i].x*PTM_RATIO*polygonSize, 
			vertexArr[i].y*PTM_RATIO*polygonSize)]];
	}
				
	ccTexParams params = {GL_NEAREST,GL_NEAREST_MIPMAP_NEAREST,GL_REPEAT,GL_REPEAT};
	
	TexturedPolygon *sprite = [TexturedPolygon createWithFile:@"bricks2.png" withVertices:vertexArray];
	[sprite.texture setTexParameters:&params];
	[sprite setPosition:p];
	[gameNode addChild:sprite z:1];	
}

-(void) addTaxi {
	// NOTE: In b2Settings.h we increased the b2_maxPolygonVertices definition:
	// #define b2_maxPolygonVertices	16

	[[CCSpriteFrameCache sharedSpriteFrameCache] addSpriteFramesWithFile:@"taxi.plist"];

	CGPoint taxiPosition = ccp(-960,80);
	float taxiScale = 0.2f;
	
	taxi = [[GameMisc alloc] init];
	taxi.gameArea = self;
	
	taxi.bodyDef->type = b2_dynamicBody;
	taxi.bodyDef->position.Set(taxiPosition.x/PTM_RATIO, taxiPosition.y/PTM_RATIO);
	taxi.body = world->CreateBody(taxi.bodyDef);
	
	taxi.fixtureDef->filter.categoryBits = CB_CAR;
	taxi.fixtureDef->filter.maskBits = CB_GROUND;
	taxi.fixtureDef->density = 0.5f;
	taxi.fixtureDef->friction = 0.25f;
	taxi.fixtureDef->restitution = 0.0f;
	
	//Polygon
	CGPoint polygonSize = ccp(8.875f * taxiScale,4.218f * taxiScale);

	int32 numVerts = 11;
	CGPoint vertexArr[] = { ccp(0.95f, 0.08f), ccp(0.98f, 0.18f), ccp(0.94f, 0.38f), ccp(0.81f, 0.67f), ccp(0.66f, 0.83f), 
		ccp(0.53f, 0.95f), ccp(0.38f, 0.94f), ccp(0.24f, 0.8f),  ccp(0.12f, 0.58f), ccp(0.03f, 0.34f),ccp(0.03f, 0.1f)  };
	b2Vec2 vertices[11];
	
	for(int i=0; i<numVerts; i++){
		vertices[i].Set(vertexArr[i].x*polygonSize.x, vertexArr[i].y*polygonSize.y);
	}
			
	taxi.sprite = [CCSprite spriteWithSpriteFrameName:@"taxi_main.png"];
	taxi.sprite.anchorPoint = ccp(0,0);
	[taxi.sprite setPosition:ccp(taxiPosition.x,taxiPosition.y)];
	taxi.sprite.scale = taxiScale;
	[gameNode addChild:taxi.sprite z:1];		
		
	taxi.polygonShape = new b2PolygonShape();
	taxi.polygonShape->Set(vertices, numVerts);
	taxi.fixtureDef->shape = taxi.polygonShape;

	taxi.body->CreateFixture(taxi.fixtureDef);	

	//Wheels
	CGPoint wheelPosition[] = { ccp(taxiPosition.x + 16, taxiPosition.y), ccp(taxiPosition.x + 43, taxiPosition.y) };
	
	for(int i=0; i<2; i++){	
		GameMisc *wheel = [[GameMisc alloc] init];
	
		if(i == 0){
			wheel1 = wheel;
		}else{
			wheel2 = wheel;
		}
			
		wheel.gameArea = self;
		
		wheel.bodyDef->type = b2_dynamicBody;
		wheel.bodyDef->position.Set(wheelPosition[i].x/PTM_RATIO, wheelPosition[i].y/PTM_RATIO);
		wheel.body = world->CreateBody(wheel.bodyDef);	
		
		wheel.body->SetTransform(b2Vec2(wheelPosition[i].x/PTM_RATIO, wheelPosition[i].y/PTM_RATIO),PI/2);
		
		wheel.fixtureDef->filter.categoryBits = CB_WHEEL;
		wheel.fixtureDef->filter.maskBits = CB_GROUND;
		wheel.fixtureDef->density = 10.0f;
		wheel.fixtureDef->friction = 10.0f;
		wheel.fixtureDef->restitution = 0.0f;
		
		CGPoint textureSize = ccp(52,51);
		CGPoint shapeSize = ccp(9,9);
		
		wheel.sprite = [CCSprite spriteWithSpriteFrameName:@"taxi_wheel.png"];
		wheel.sprite.position = ccp(wheelPosition[i].x,wheelPosition[i].y);
		wheel.sprite.scaleX = shapeSize.x / textureSize.x * 2;
		wheel.sprite.scaleY = shapeSize.y / textureSize.y * 2;
			
		[gameNode addChild:wheel.sprite z:1];		
			
		wheel.circleShape = new b2CircleShape();
		wheel.circleShape->m_radius = shapeSize.x/PTM_RATIO;
		wheel.fixtureDef->shape = wheel.circleShape;

		wheel.body->CreateFixture(wheel.fixtureDef);	
		wheel.body->SetAngularDamping(1.0f);
		
		//Add Joint to connect wheel to the taxi
		b2RevoluteJointDef rjd;
		b2RevoluteJoint* joint;
		
		rjd.Initialize(wheel.body, taxi.body, b2Vec2(wheelPosition[i].x/PTM_RATIO, wheelPosition[i].y/PTM_RATIO));

		joint = (b2RevoluteJoint*)world->CreateJoint(&rjd);
	}
}

/* Press the left side of the screen to drive left, the right to drive right */
-(void) ccTouchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {
	UITouch *touch = [touches anyObject];
    CGPoint point = [touch locationInView: [touch view]];
	point = [[CCDirector sharedDirector] convertToGL: point];

	if(point.x < 240){
		pressedLeft = YES; pressedRight = NO;
	}else if(point.x >= 240){
		pressedRight = YES; pressedLeft = NO;
	}
}

-(void) ccTouchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {		
	UITouch *touch = [touches anyObject];
    CGPoint point = [touch locationInView: [touch view]];
	point = [[CCDirector sharedDirector] convertToGL: point];
	
	if(point.x < 240){
		pressedLeft = YES; pressedRight = NO;
	}else if(point.x >= 240){
		pressedRight = YES; pressedLeft = NO;
	}
}

-(void) ccTouchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {			
	UITouch *touch = [touches anyObject];
    CGPoint point = [touch locationInView: [touch view]];
	point = [[CCDirector sharedDirector] convertToGL: point];
	
	pressedLeft = NO;
	pressedRight = NO;
}

-(void) step: (ccTime) dt {	
	[super step:dt];

	gameNode.position = ccp(-taxi.sprite.position.x + 240, -taxi.sprite.position.y + 160);

	//Front wheel drive
	
	//We apply some counter-torque to steady the car
	if(pressedRight){
		wheel2->body->ApplyTorque(-20.0f);
		taxi->body->ApplyTorque(5.0f);
	}else if(pressedLeft){
		wheel1->body->ApplyTorque(20.0f);
		taxi->body->ApplyTorque(-5.0f);
	}
}

@end
