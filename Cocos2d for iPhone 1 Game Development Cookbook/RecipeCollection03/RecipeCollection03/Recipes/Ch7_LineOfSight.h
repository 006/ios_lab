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
#import "RayCast.h"

@class GameArea2D;
@class GameObject;
@class GameSensor;
@class GameMisc;
@class GameActor;
@class GameWaypoint;
@class DrawLayer;

@interface Ch7_LineOfSight : GameArea2D
{
	GameActor *actor;
	DrawLayer *drawLayer;
	NSMutableArray *enemies;
	DPad *dPad;
}

@property (nonatomic, retain) GameActor *actor;

-(CCLayer*) runRecipe;
-(void) setGameAreaSize;
-(void) step:(ccTime)dt;
-(void) followActorWithEnemies;
-(void) addDPad;
-(void) addActor;
-(void) addEnemies;
-(void) drawLayer;
-(void) addDrawLayer;
-(bool) hudBegan:(NSSet*)touches withEvent:(UIEvent*)event;
-(bool) hudMoved:(NSSet*)touches withEvent:(UIEvent*)event;
-(bool) hudEnded:(NSSet*)touches withEvent:(UIEvent*)event;

@end

//Implementation
@implementation Ch7_LineOfSight

@synthesize actor;

-(CCLayer*) runRecipe {
	//Set game area size
	[self setGameAreaSize];
	
	//Superclass initalization and message
	[super runRecipe];
	[message setString:@"Use the DPad to avoid the AI actors."];

	//Add DPad
	[self addDPad];

	//Add random geometry
	[self addRandomPolygons:5];

	//Create Actor
	[self addActor];
	
	//Create enemies
	[self addEnemies];
	
	//Add draw layer
	[self addDrawLayer];
	
	//Create level boundaries
	[self addLevelBoundaries];
	
	//Initial variables
	cameraZoom = 0.45f;

	//Schedule step method
	[self schedule:@selector(step:)];

	return self;
}

/* Set game area size */
-(void) setGameAreaSize {
	gameAreaSize = ccp(1000/PTM_RATIO,600/PTM_RATIO);	//Box2d units
}

-(void) step: (ccTime) dt {	
	[super step:dt];
	
	//Process DPad
	if(dPad.pressed){
		//[actor runWithVector:dPad.pressedVector withSpeedMod:1.0f withConstrain:NO];
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
	
	//Make the enemies follow the actor
	[self followActorWithEnemies];
}

-(void) followActorWithEnemies {
	//If enemies can see the actor they follow
	for(int i=0; i<enemies.count; i++){
		//Align enemies
		GameActor *enemy = [enemies objectAtIndex:i];
		CGPoint directionVector = CGPointMake(actor.body->GetPosition().x - enemy.body->GetPosition().x, actor.body->GetPosition().y - enemy.body->GetPosition().y);
		float radians = [GameHelper vectorToRadians:directionVector];
		enemy.body->SetTransform(enemy.body->GetPosition(), -1 * radians + PI_CONSTANT/2);
		
		RayCastClosestCallback callback;
		world->RayCast(&callback, enemy.body->GetPosition(), actor.body->GetPosition());
		
		//Did the raycast hit anything?
		enemy.tag = 0;	//Assume we can't
		if(callback.m_hit){
			//Is the closest point the actor?
			if(callback.m_fixture->GetBody() == actor.body){
				//If so, follow the actor
				b2Vec2 normal = b2Vec2( callback.m_normal.x * -5.0f, callback.m_normal.y * -5.0f);
				enemy.body->ApplyForce(normal, actor.body->GetPosition());
				enemy.tag = 1;	//Set seeing flag to true
			}
		}
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

/* Add actor */
-(void) addActor {
	actor = [[GameActor alloc] init];
	actor.gameArea = self;
	
	actor.bodyDef->type = b2_dynamicBody;
	actor.bodyDef->position.Set(gameAreaSize.x/2,gameAreaSize.y/2);
	actor.bodyDef->userData = actor;
				
	actor.body = world->CreateBody(actor.bodyDef);
	
	actor.circleShape = new b2CircleShape();
	actor.circleShape->m_radius = 0.5f;
	actor.fixtureDef->shape = actor.circleShape;
	
	actor.body->CreateFixture(actor.fixtureDef);
	
	actor.body->SetLinearDamping(2.0f);
}

/* Add 3 enemies */
-(void) addEnemies {
	enemies = [[NSMutableArray alloc] init];

	for(int i=0; i<3; i++){
		GameActor *enemy = [[GameActor alloc] init];
		enemy.gameArea = self;
		enemy.tag = 0;	//We use this to store whether or not we can see the actor
		
		enemy.bodyDef->type = b2_dynamicBody;
		enemy.bodyDef->position.Set((arc4random()%((int)(gameAreaSize.x-2)))+1.0f,(arc4random()%((int)(gameAreaSize.y-2)))+1.0f);
		enemy.bodyDef->userData = enemy;
					
		enemy.body = world->CreateBody(enemy.bodyDef);
		
		enemy.circleShape = new b2CircleShape();
		enemy.circleShape->m_radius = 0.5f;
		enemy.fixtureDef->shape = enemy.circleShape;
		
		enemy.body->CreateFixture(enemy.fixtureDef);
		
		[enemies addObject:enemy];
	}
}

/* Draw each enemy 'sight line' if they can see you */
-(void) drawLayer {
	for(int i=0; i<enemies.count; i++){
		GameActor *enemy = [enemies objectAtIndex:i];
		if(enemy.tag == 1){
			glColor4ub(255,255,0,32);
		
			CGPoint actorPosition = ccp(actor.body->GetPosition().x*PTM_RATIO, actor.body->GetPosition().y*PTM_RATIO);
			CGPoint enemyPosition = ccp(enemy.body->GetPosition().x*PTM_RATIO, enemy.body->GetPosition().y*PTM_RATIO);
			
			ccDrawLine(actorPosition, enemyPosition);

			glColor4ub(255,255,255,255);
		}
	}
}

/* Add draw layer */
-(void) addDrawLayer {
	drawLayer = [[DrawLayer alloc] init];
	drawLayer.position = ccp(0,0);
	drawLayer.recipe = self;
	[gameNode addChild:drawLayer z:10];
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

@end