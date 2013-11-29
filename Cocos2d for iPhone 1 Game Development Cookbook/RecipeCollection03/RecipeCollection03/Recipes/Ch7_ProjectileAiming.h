#import "Recipe.h"
#import "GameArea2D.h"
#import "GameObject.h"
#import "GameSensor.h"
#import "GameMisc.h"
#import "GameHelper.h"
#import "Vector3D.h"
#import "TexturedPolygon.h"
#import "GameActor.h"
#import "GameWaypoint.h"
#import "DrawLayer.h"
#import "DPad.h"

@class GameArea2D;
@class GameObject;
@class GameSensor;
@class GameMisc;
@class GameActor;
@class GameWaypoint;
@class DrawLayer;

enum {
	GO_TAG_ENEMY = 2,
	GO_TAG_MISSILE = 3
};

enum {
	CB_EVERYTHING = 0xFFFF,
	CB_WALL = 0x0001,
	CB_ACTOR = 0x0002,
	CB_ENEMY = 0x0004,
	CB_MISSILE = 0x0008
	//0x0010
	//0x0020
	//0x0040
	//0x0080
};

@interface Ch7_ProjectileAiming : GameArea2D
{
	GameActor *actor;
	DrawLayer *drawLayer;
	NSMutableArray *enemies;
	NSMutableArray *missiles;
	DPad *dPad;
	float fireCount;
}

@property (nonatomic, retain) GameActor *actor;

-(CCLayer*) runRecipe;
-(void) step:(ccTime)dt;
-(void) addDPad;
-(void) addActor;
-(void) addEnemies;
-(void) fireMissiles;
-(Vector3D*) interceptSrc:(GameObject*)src dst:(GameObject*)dst projSpeed:(float)projSpeed;
-(bool) hudBegan:(NSSet*)touches withEvent:(UIEvent*)event;
-(bool) hudMoved:(NSSet*)touches withEvent:(UIEvent*)event;
-(bool) hudEnded:(NSSet*)touches withEvent:(UIEvent*)event;
-(void) handleCollisionWithObjA:(GameObject*)objA withObjB:(GameObject*)objB;
-(void) handleCollisionWithSensor:(GameSensor*)sensor withMisc:(GameMisc*)misc;
-(void) handleCollisionWithMisc:(GameMisc*)a withMisc:(GameMisc*)b;

@end

//Implementation
@implementation Ch7_ProjectileAiming

@synthesize actor;

-(CCLayer*) runRecipe {
	//Set game area size
	gameAreaSize = ccp(1000/PTM_RATIO,600/PTM_RATIO);	//Box2d units

	//Superclass initialization and message
	[super runRecipe];
	[message setString:@"Use the DPad to dodge the projectiles."];

	//Add DPad
	[self addDPad];

	//Add some geometry
	[self addRandomPolygons:5];

	//Create Actor
	[self addActor];
	
	//Create enemies
	[self addEnemies];
	
	//Create level boundaries
	[self addLevelBoundaries];
	
	//Initial variables
	cameraZoom = 0.45f;
	fireCount = 0;
	missiles = [[NSMutableArray alloc] init];

	//Step method
	[self schedule:@selector(step:)];

	return self;
}

-(void) step: (ccTime) dt {	
	[super step:dt];
	
	//Process DPad
	if(dPad.pressed){
		actor.body->ApplyForce(b2Vec2(dPad.pressedVector.x*25.0f, dPad.pressedVector.y*25.0f), actor.body->GetPosition());
		actor.body->SetAngularVelocity(0.0f);
		CGPoint movementVector = ccp(actor.body->GetLinearVelocity().x, actor.body->GetLinearVelocity().y);
		actor.body->SetTransform(actor.body->GetPosition(), -1 * [GameHelper vectorToRadians:movementVector] + PI_CONSTANT/2);
	}else{
		[actor stopRunning];
	}

	//Follow the actor with the camera
	CGPoint actorPosition = ccp(actor.body->GetPosition().x*PTM_RATIO, actor.body->GetPosition().y*PTM_RATIO);
	[self centerCameraOnGameCoord:actorPosition];
	
	//Align enemies
	for(int i=0; i<enemies.count; i++){
		GameActor *enemy = [enemies objectAtIndex:i];
		CGPoint directionVector = CGPointMake(actor.body->GetPosition().x - enemy.body->GetPosition().x, actor.body->GetPosition().y - enemy.body->GetPosition().y);
		float radians = [GameHelper vectorToRadians:directionVector];
		enemy.body->SetTransform(enemy.body->GetPosition(), -1 * radians + PI_CONSTANT/2);
	}
	
	//Firing projectiles
	fireCount += dt;
	if(fireCount > 1.0f){ 
		fireCount = 0;
		[self fireMissiles];
	}
}

/* Add DPad */
-(void) addDPad {
	[[CCSpriteFrameCache sharedSpriteFrameCache] addSpriteFramesWithFile:@"dpad_buttons.plist"];
	dPad = [[DPad alloc] init];
	dPad.position = ccp(75,75);
	dPad.opacity = 100;
	[self addChild:dPad z:5];
}

/* Add Actor */
-(void) addActor {
	actor = [[GameActor alloc] init];
	actor.gameArea = self;
	actor.tag = GO_TAG_ACTOR;
	
	actor.bodyDef->type = b2_dynamicBody;
	actor.bodyDef->position.Set(gameAreaSize.x/2,gameAreaSize.y/2);
	actor.bodyDef->userData = actor;
				
	actor.body = world->CreateBody(actor.bodyDef);
	
	actor.circleShape = new b2CircleShape();
	actor.circleShape->m_radius = 0.5f;
	actor.fixtureDef->shape = actor.circleShape;
	actor.fixtureDef->filter.categoryBits = CB_ACTOR;
	actor.fixtureDef->filter.maskBits = CB_EVERYTHING;
	
	actor.body->CreateFixture(actor.fixtureDef);
	
	actor.body->SetLinearDamping(2.0f);
}

/* Add 3 enemies at random positions */
-(void) addEnemies {
	enemies = [[NSMutableArray alloc] init];

	for(int i=0; i<3; i++){
		GameActor *enemy = [[GameActor alloc] init];
		enemy.gameArea = self;
		enemy.tag = GO_TAG_ENEMY;
		
		enemy.bodyDef->type = b2_dynamicBody;
		enemy.bodyDef->position.Set((arc4random()%((int)(gameAreaSize.x-2)))+1.0f,(arc4random()%((int)(gameAreaSize.y-2)))+1.0f);
		enemy.bodyDef->userData = enemy;
					
		enemy.body = world->CreateBody(enemy.bodyDef);
		
		enemy.circleShape = new b2CircleShape();
		enemy.circleShape->m_radius = 0.5f;
		enemy.fixtureDef->shape = enemy.circleShape;
		enemy.fixtureDef->filter.categoryBits = CB_ENEMY;
		enemy.fixtureDef->filter.maskBits = CB_EVERYTHING & ~CB_MISSILE;
		
		enemy.body->CreateFixture(enemy.fixtureDef);
		
		[enemies addObject:enemy];
	}
}

/*  Each enemy fires a missile object */
-(void) fireMissiles {
	for(int i=0; i<enemies.count; i++){		
		GameActor *enemy = [enemies objectAtIndex:i];
		
		//Create missile
		GameMisc *missile = [[GameMisc alloc] init];
		missile.gameArea = self;
		missile.tag = GO_TAG_MISSILE;
		
		missile.bodyDef->type = b2_dynamicBody;
		missile.bodyDef->position.Set( enemy.body->GetPosition().x, enemy.body->GetPosition().y );
		missile.bodyDef->userData = missile;
		
		missile.body = world->CreateBody(missile.bodyDef);
		
		missile.polygonShape = new b2PolygonShape();
		missile.polygonShape->SetAsBox(0.5f, 0.2f);
		missile.fixtureDef->density = 10.0f;
		missile.fixtureDef->shape = missile.polygonShape;
		missile.fixtureDef->filter.categoryBits = CB_MISSILE;
		missile.fixtureDef->filter.maskBits = CB_EVERYTHING & ~CB_MISSILE & ~CB_ENEMY;
		
		missile.body->CreateFixture(missile.fixtureDef);
		
		//Calculate intercept trajectory
		Vector3D *point = [self interceptSrc:missile dst:actor projSpeed:20.0f];
		if(point){			
			//Align missile
			CGPoint pointToFireAt = CGPointMake(point.x, point.y);
			CGPoint directionVector = CGPointMake(pointToFireAt.x - missile.body->GetPosition().x, pointToFireAt.y - missile.body->GetPosition().y);
			float radians = [GameHelper vectorToRadians:directionVector];
			missile.body->SetTransform(missile.body->GetPosition(), -1 * radians + PI_CONSTANT/2);
		
			//Fire missile
			CGPoint normalVector = [GameHelper radiansToVector:radians];
			missile.body->SetLinearVelocity( b2Vec2(normalVector.x*20.0f, normalVector.y*20.0f) );
		}
		
		[missiles addObject:missile];
	}
}

/* Find the intercept angle given projectile speed and a moving target */
-(Vector3D*) interceptSrc:(GameObject*)src dst:(GameObject*)dst projSpeed:(float)projSpeed {
  float tx = dst.body->GetPosition().x - src.body->GetPosition().x;
  float ty = dst.body->GetPosition().y - src.body->GetPosition().y;
  float tvx = dst.body->GetLinearVelocity().x;
  float tvy = dst.body->GetLinearVelocity().y;
  
  //Get quadratic equation components
  float a = tvx*tvx + tvy*tvy - projSpeed*projSpeed;
  float b = 2 * (tvx * tx + tvy * ty);
  float c = tx*tx + ty*ty;

  //Solve quadratic equation
  Vector3D *ts = [GameHelper quadraticA:a B:b C:c];
  
  //Find the smallest positive solution
  Vector3D *solution = nil;
  if(ts){
    float t0 = ts.x;
    float t1 = ts.y;
    float t = MIN(t0,t1);
    if(t < 0){ t = MAX(t0,t1); }
    if(t > 0){
      float x = dst.body->GetPosition().x + dst.body->GetLinearVelocity().x*t;
      float y = dst.body->GetPosition().y + dst.body->GetLinearVelocity().y*t;
      solution = [Vector3D x:x y:y z:0];
    }
  }
  return solution;
}

/* Our base collision handling routine */
-(void) handleCollisionWithObjA:(GameObject*)objA withObjB:(GameObject*)objB {	
	//General collision handling
	if(objA.tag == GO_TAG_MISSILE){
		[self markBodyForDestruction:objA];
	}else if(objB.tag == GO_TAG_MISSILE){
		[self markBodyForDestruction:objB];
	}
}

/* DPad input */
-(bool) hudBegan:(NSSet*)touches withEvent:(UIEvent*)event {
	bool hudTouched = NO;
	[dPad ccTouchesBegan:touches withEvent:event];
	if(dPad.pressed || touches.count == 1){ hudTouched = YES; }
	return hudTouched;
}

-(bool) hudMoved:(NSSet*)touches withEvent:(UIEvent*)event {
	bool hudTouched = NO;
	[dPad ccTouchesMoved:touches withEvent:event];
	if(dPad.pressed || touches.count == 1){ hudTouched = YES; }
	return hudTouched;
}

-(bool) hudEnded:(NSSet*)touches withEvent:(UIEvent*)event {
	bool hudTouched = NO;
	[dPad ccTouchesEnded:touches withEvent:event];
	if(dPad.pressed || touches.count == 1){ hudTouched = YES; }
	return hudTouched;
}

-(void) cleanRecipe {
	//Cleanup missiles
	for(GameMisc *m in missiles){
		[m release];
		m = nil;
	}
	[missiles removeAllObjects];

	[super cleanRecipe];
}

@end
