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

@class GameArea2D;
@class GameObject;
@class GameSensor;
@class GameMisc;
@class GameActor;
@class GameWaypoint;
@class DrawLayer;

@interface Ch7_Waypoints : GameArea2D
{
	GameActor *actor;
	DrawLayer *drawLayer;
}

@property (nonatomic, retain) GameActor *actor;

-(CCLayer*) runRecipe;
-(void) step:(ccTime)dt;
-(void) addActor;
-(void) drawLayer;
-(void) addDrawLayer;
-(void) tapWithPoint:(CGPoint)point;
-(void) reachedWaypoint;
-(void) movingToWaypoint;

@end

//Implementation
@implementation Ch7_Waypoints

@synthesize actor;

-(CCLayer*) runRecipe {
	//Set game area size
	gameAreaSize = ccp(1000/PTM_RATIO,600/PTM_RATIO);	//Box2d units

	//Superclass initialization and message
	[super runRecipe];
	[message setString:@"Tap the screen move the actor to that point.\nTap again to queue another waypoint."];

	//Add polygons
	[self addRandomPolygons:10];
	
	//Create Actor
	[self addActor];
	
	//Add draw layer
	[self addDrawLayer];
	
	//Create level boundaries
	[self addLevelBoundaries];
	
	//Initial variables
	cameraZoom = 0.45f;

	//Step method
	[self schedule:@selector(step:)];

	return self;
}

-(void) step: (ccTime) dt {	
	[super step:dt];
	
	//Process actor waypoints
	[actor processWaypoints];
	
	//Turn actor toward waypoints
	if(actor.waypoints.count > 0){
		CGPoint movementVector = ccp(actor.body->GetLinearVelocity().x, actor.body->GetLinearVelocity().y);
		actor.body->SetTransform(actor.body->GetPosition(), -1 * [GameHelper vectorToRadians:movementVector] + PI_CONSTANT/2);
	}
}

/* Add actor to scene */
-(void) addActor {
	actor = [[GameActor alloc] init];
	actor.gameArea = self;
	
	actor.bodyDef->type = b2_dynamicBody;
	actor.bodyDef->position.Set(240/PTM_RATIO,160/PTM_RATIO);
	actor.bodyDef->userData = actor;
				
	actor.body = world->CreateBody(actor.bodyDef);
	
	actor.circleShape = new b2CircleShape();
	actor.circleShape->m_radius = 0.5f;
	actor.fixtureDef->shape = actor.circleShape;
	actor.fixtureDef->isSensor = NO;
	
	actor.body->CreateFixture(actor.fixtureDef);
}

/* Draw all waypoint lines */
-(void) drawLayer {
	glColor4ub(255,255,0,32);
		
	CGPoint actorPosition = ccp(actor.body->GetPosition().x*PTM_RATIO, actor.body->GetPosition().y*PTM_RATIO);
		
	if(actor.waypoints.count == 1){
		GameWaypoint *gw = (GameWaypoint*)[actor.waypoints objectAtIndex:0];
		ccDrawLine(actorPosition, gw.position);
	}else if(actor.waypoints.count > 1){
		for(int i=0; i<actor.waypoints.count-1; i++){			
			GameWaypoint *gw = (GameWaypoint*)[actor.waypoints objectAtIndex:i];
			GameWaypoint *gwNext = (GameWaypoint*)[actor.waypoints objectAtIndex:i+1];
			
			if(i == 0){
				//From actor to first waypoint
				ccDrawLine(actorPosition, gw.position);
				ccDrawLine(gw.position, gwNext.position);
			}else{
				//From this waypoint to next one
				ccDrawLine(gw.position, gwNext.position);
			}	
		}
	}
	
	glColor4ub(255,255,255,255);
}

/* Add extra drawing layer */
-(void) addDrawLayer {
	drawLayer = [[DrawLayer alloc] init];
	drawLayer.position = ccp(0,0);
	drawLayer.recipe = self;
	[gameNode addChild:drawLayer z:10];
}

/* Add a new waypoint when you touch the screen */
-(void) tapWithPoint:(CGPoint)point {
	ObjectCallback *goc1 = [ObjectCallback createWithObject:self withCallback:@"movingToWaypoint"];
	ObjectCallback *goc2 = [ObjectCallback createWithObject:self withCallback:@"reachedWaypoint"];
	GameWaypoint *wp = [GameWaypoint createWithPosition:[self convertTouchCoord:point] withSpeedMod:1.0f];
	wp.preCallback = goc1;
	wp.postCallback = goc2;
	[actor addWaypoint:wp];
}

/* WP message callbacks */
-(void) movingToWaypoint {
	[self showMessage:@"Moving to WP"];
}

-(void) reachedWaypoint {
	[self showMessage:@"Reached WP"];
}

@end
