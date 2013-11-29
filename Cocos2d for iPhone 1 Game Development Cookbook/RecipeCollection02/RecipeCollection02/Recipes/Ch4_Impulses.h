#import "Recipe.h"
#import "GameArea2D.h"
#import "GameObject.h"
#import "GameMisc.h"
#import "GameSensor.h"

@class GameArea2D;
@class GameObject;
@class GameMisc;
@class GameSensor;

enum {	//Object type tags
	TYPE_OBJ_BASKETBALL = 0,
	TYPE_OBJ_SHOOTER = 1,
	TYPE_OBJ_NET_SENSOR = 2
};

@interface Ch4_Impulses : GameArea2D
{
	GameMisc *basketball;
	CCSprite *bballNet;
	CCSprite *bballShooterArm;
}

-(CCLayer*) runRecipe;
-(void) addBasketball;
-(void) addBasketballNet;
-(void) addShooter;
-(void) ccTouchesBegan:(NSSet *)touches withEvent:(UIEvent *)event;
-(void) handleCollisionWithObjA:(GameObject*)objA withObjB:(GameObject*)objB;
-(void) handleCollisionWithSensor:(GameSensor*)sensor withMisc:(GameMisc*)misc;

@end



//Implementation
@implementation Ch4_Impulses

-(CCLayer*) runRecipe {
	[super runRecipe];
	[message setString:@"Shoot the ball in the hoop."];
													
	//Create level boundaries
	[self addLevelBoundaries];
	
	//Add level background
	CCSprite *bg = [CCSprite spriteWithFile:@"bball_bg.png"];
	bg.position = ccp(240,160);
	[gameNode addChild:bg z:0];
	
	//Add basketball
	[self addBasketball];
	
	//Add basketball net
	[self addBasketballNet];
	
	//Add shooter
	[self addShooter];

	return self;
}

/* Add a basketball net with a sensor */
-(void) addBasketballNet {
	[[CCSpriteFrameCache sharedSpriteFrameCache] addSpriteFramesWithFile:@"bball_net.plist"];

	bballNet = [CCSprite spriteWithSpriteFrameName:@"bball_net_front_01.png"];
	bballNet.position = ccp(375,190);
	bballNet.scale = 0.25f;
	[gameNode addChild:bballNet z:2];
	
	CCSprite *netBack = [CCSprite spriteWithSpriteFrameName:@"bball_net_back.png"];
	netBack.position = ccp(375,190);
	netBack.scale = 0.25f;
	[gameNode addChild:netBack z:1];
	
	b2BodyDef netBodyDef;
	netBodyDef.position.Set(0, 0);
	b2Body *body = world->CreateBody(&netBodyDef);
		
	b2PolygonShape netShape;	
		
	b2FixtureDef netFixtureDef;
	netFixtureDef.restitution = -1.0f;
	
	//Rim front
	netShape.SetAsEdge(b2Vec2(345.0f/PTM_RATIO,220.0f/PTM_RATIO), b2Vec2(355.0f/PTM_RATIO,190.0f/PTM_RATIO));
	netFixtureDef.shape = &netShape;
	body->CreateFixture(&netFixtureDef);	
	
	//Rim back
	netShape.SetAsEdge(b2Vec2(390.0f/PTM_RATIO,190.0f/PTM_RATIO), b2Vec2(410.0f/PTM_RATIO,230.0f/PTM_RATIO));
	netFixtureDef.shape = &netShape;
	body->CreateFixture(&netFixtureDef);
	
	//Backboard
	netShape.SetAsEdge(b2Vec2(410.0f/PTM_RATIO,300.0f/PTM_RATIO), b2Vec2(410.0f/PTM_RATIO,210.0f/PTM_RATIO));
	netFixtureDef.shape = &netShape;
	body->CreateFixture(&netFixtureDef);
	
	//Add net sensor
	GameSensor *gameObjSensor = [[GameSensor alloc] init];
	gameObjSensor.typeTag = TYPE_OBJ_NET_SENSOR;
	gameObjSensor.gameArea = self;

	gameObjSensor.bodyDef->type = b2_staticBody;
	gameObjSensor.bodyDef->position.Set(0,0);
	gameObjSensor.bodyDef->userData = gameObjSensor;
	gameObjSensor.body = world->CreateBody(gameObjSensor.bodyDef);
	
	gameObjSensor.polygonShape = new b2PolygonShape();
	gameObjSensor.polygonShape->SetAsEdge(b2Vec2(370.0f/PTM_RATIO,200.0f/PTM_RATIO), b2Vec2(380.0f/PTM_RATIO,200.0f/PTM_RATIO));
		
	gameObjSensor.fixtureDef->shape = gameObjSensor.polygonShape;
	gameObjSensor.fixtureDef->isSensor = YES;
	
	gameObjSensor.body->CreateFixture(gameObjSensor.fixtureDef);
}

/* Add a basketball */
-(void) addBasketball {
	basketball = [[GameMisc alloc] init];
	basketball.typeTag = TYPE_OBJ_BASKETBALL;
	basketball.gameArea = self;
	
	basketball.bodyDef->type = b2_dynamicBody;

	basketball.bodyDef->position.Set(100/PTM_RATIO, 100/PTM_RATIO);
	basketball.bodyDef->userData = basketball;
	basketball.body = world->CreateBody(basketball.bodyDef);
	
	basketball.fixtureDef->density = 1.0f;
	basketball.fixtureDef->friction = 0.3f;
	basketball.fixtureDef->restitution = 0.6f;
	
	//Circle
	float textureSize = 128;
	float shapeSize = 12;
		
	basketball.sprite = [CCSprite spriteWithFile:@"bball.png"];
	basketball.sprite.position = ccp(100,100);
	basketball.sprite.scale = shapeSize / textureSize * 2;
		
	[gameNode addChild:basketball.sprite];
		
	basketball.circleShape = new b2CircleShape();
	basketball.circleShape->m_radius = shapeSize/PTM_RATIO;
	basketball.fixtureDef->shape = basketball.circleShape;

	basketball.body->CreateFixture(basketball.fixtureDef);
}

/* Add a shooter with reverse karate chop action! */
-(void) addShooter {
	[[CCSpriteFrameCache sharedSpriteFrameCache] addSpriteFramesWithFile:@"bball_shooter.plist"];

	CGPoint textureSize = ccp(128,512);
	CGPoint shapeSize = ccp(24,96);

	bballShooterArm = [CCSprite spriteWithSpriteFrameName:@"bball_shooter_arm.png"];
	bballShooterArm.position = ccp(65,148);
	bballShooterArm.scaleX = shapeSize.x / textureSize.x * 2;
	bballShooterArm.scaleY = shapeSize.y / textureSize.y * 2;
	bballShooterArm.anchorPoint = ccp(0.5,0.8);
	bballShooterArm.rotation = 0;
	[gameNode addChild:bballShooterArm z:2];

	GameSensor *shooter = [[GameSensor alloc] init];
	shooter.typeTag = TYPE_OBJ_SHOOTER;
	shooter.gameArea = self;
	
	shooter.bodyDef->type = b2_staticBody;
	shooter.bodyDef->position.Set(70/PTM_RATIO,100/PTM_RATIO);
	shooter.bodyDef->userData = shooter;
	shooter.body = world->CreateBody(shooter.bodyDef);
		
	shooter.sprite = [CCSprite spriteWithSpriteFrameName:@"bball_shooter_body.png"];
	shooter.sprite.position = ccp(70,100);
	shooter.sprite.scaleX = shapeSize.x / textureSize.x * 2;
	shooter.sprite.scaleY = shapeSize.y / textureSize.y * 2;
	
	[gameNode addChild:shooter.sprite z:1];
	
	shooter.polygonShape = new b2PolygonShape();
	shooter.polygonShape->SetAsBox(shapeSize.x/PTM_RATIO, shapeSize.y/PTM_RATIO);
	
	shooter.fixtureDef->shape = shooter.polygonShape;
	shooter.fixtureDef->isSensor = YES;
	shooter.body->CreateFixture(shooter.fixtureDef);
}

/* Add level boundaries which are higher than normal to allow the shooter to arc the ball */
-(void) addLevelBoundaries {
	CGSize screenSize = [CCDirector sharedDirector].winSize;
		
	b2BodyDef groundBodyDef;
	groundBodyDef.position.Set(0, 0);
	b2Body *body = world->CreateBody(&groundBodyDef);
		
	b2PolygonShape groundBox;	
	
	b2FixtureDef groundFixtureDef;
	groundFixtureDef.density = 1.0f;
	groundFixtureDef.friction = 1.0f;
	groundFixtureDef.restitution = 0.0f;
	
	groundBox.SetAsEdge(b2Vec2(0,1), b2Vec2(screenSize.width/PTM_RATIO,1));
	groundFixtureDef.shape = &groundBox;
	body->CreateFixture(&groundFixtureDef);
		
	groundBox.SetAsEdge(b2Vec2(0,screenSize.height/PTM_RATIO*1.5f), b2Vec2(screenSize.width/PTM_RATIO,screenSize.height/PTM_RATIO*1.5f));
	groundFixtureDef.shape = &groundBox;
	body->CreateFixture(&groundFixtureDef);
		
	groundBox.SetAsEdge(b2Vec2(0,screenSize.height/PTM_RATIO*1.5f), b2Vec2(0,1));
	groundFixtureDef.shape = &groundBox;
	body->CreateFixture(&groundFixtureDef);
		
	groundBox.SetAsEdge(b2Vec2(screenSize.width/PTM_RATIO,screenSize.height/PTM_RATIO*1.5f), b2Vec2(screenSize.width/PTM_RATIO,1));
	groundFixtureDef.shape = &groundBox;
	body->CreateFixture(&groundFixtureDef);
}

-(void) ccTouchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {		
	UITouch *touch = [touches anyObject];
    CGPoint point = [touch locationInView: [touch view]];
	point = [[CCDirector sharedDirector] convertToGL: point];

	/* Apply an impulse when the user touches the screen */
	CGPoint vect = ccp(point.x - basketball.body->GetPosition().x*PTM_RATIO, point.y - basketball.body->GetPosition().y*PTM_RATIO);
	basketball.body->ApplyLinearImpulse(b2Vec2(vect.x/20, vect.y/20) , basketball.body->GetPosition() );
}

/* Main collision handling routine */
-(void) handleCollisionWithObjA:(GameObject*)objA withObjB:(GameObject*)objB {		
	//SENSOR to MISC collision
	if(objA.type == GO_TYPE_SENSOR && objB.type == GO_TYPE_MISC){
		[self handleCollisionWithSensor:(GameSensor*)objA withMisc:(GameMisc*)objB];
	}else if(objA.type == GO_TYPE_MISC && objB.type == GO_TYPE_SENSOR){
		[self handleCollisionWithSensor:(GameSensor*)objB withMisc:(GameMisc*)objA];
	}
}

/* SENSOR to MISC collision */
-(void) handleCollisionWithSensor:(GameSensor*)sensor withMisc:(GameMisc*)misc {
	if(misc.typeTag == TYPE_OBJ_BASKETBALL && sensor.typeTag == TYPE_OBJ_NET_SENSOR){	
		//Animate the net when the shooter makes a basket
		[self runAction:[CCSequence actions:[CCDelayTime actionWithDuration:0.5f], 
			[CCCallFunc actionWithTarget:self selector:@selector(resetMessage)], nil]];
			
		CCAnimation *animation = [[CCAnimation alloc] initWithName:@"swish" delay:0.1f];
		for(int i=1; i<=4; i+=1){
			[animation addFrame:[[CCSpriteFrameCache sharedSpriteFrameCache] 
				spriteFrameByName:[NSString stringWithFormat:@"bball_net_front_0%i.png",i]]];
		}
		for(int i=3; i>=1; i-=1){
			[animation addFrame:[[CCSpriteFrameCache sharedSpriteFrameCache] 
				spriteFrameByName:[NSString stringWithFormat:@"bball_net_front_0%i.png",i]]];
		}		
		[bballNet runAction:[CCAnimate actionWithAnimation:animation]];
	}else if(misc.typeTag == TYPE_OBJ_BASKETBALL && sensor.typeTag == TYPE_OBJ_SHOOTER){
		//Animate the shooter's arm and apply an impulse when he touches the ball */
		[bballShooterArm runAction:[CCSequence actions: 
			[CCRotateBy actionWithDuration:0.1f angle:-90],
			[CCRotateBy actionWithDuration:0.5f angle:90],
			nil]];
	
		basketball.body->SetLinearVelocity(b2Vec2(0,0));
		basketball.body->ApplyLinearImpulse(b2Vec2(3.5f, 7) , basketball.body->GetPosition() );
	}
}

@end
