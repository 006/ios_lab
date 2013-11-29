#import "GameArea2D.h"
#import "BasicContactListener.h"

@class GameObject;
@class DebugDrawNode;
@class GameObjectCallback;
@class QueuedAction;

@implementation GameArea2D

@synthesize debugDraw;

-(id)init {
    self = [super init];
    if (self != nil) {
		bodiesToDestroy = [[NSMutableArray alloc] init];
		postDestructionCallbacks = [[NSMutableArray alloc] init];
		bodiesToCreate = [[NSMutableArray alloc] init];
		queuedActions = [[NSMutableArray alloc] init];
    }
    return self;
}

-(void) step: (ccTime) dt
{	
	//Update Physics
	int32 velocityIterations = 8;
	int32 positionIterations = 3;
	
	world->Step(dt, velocityIterations, positionIterations);
	
	//Set sprite positions by body positions
	for (b2Body* b = world->GetBodyList(); b; b = b->GetNext())
	{
		if (b->GetUserData() != NULL) {
			GameObject *obj = (GameObject*)b->GetUserData();
			[obj.sprite setPosition:CGPointMake( b->GetPosition().x * PTM_RATIO, b->GetPosition().y * PTM_RATIO)];
			obj.sprite.rotation = -1 * CC_RADIANS_TO_DEGREES(b->GetAngle());
		}	
	}
	
	//Process body destruction/creation
	[self destroyBodies];
	[self createBodies];
	[self runQueuedActions];
}

-(CCLayer*) runRecipe {
	[super runRecipe];

	//The gameNode is main node we'll attach everything to
	gameNode = [[CCNode alloc] init];
	gameNode.position = ccp(0,0);
	[self addChild:gameNode z:1];

	//Set Y gravity to -20.0f
	b2Vec2 gravity;
	gravity.Set(0.0f, -20.0f);
	
	//Allow objects to sleep to save cycles
	bool doSleep = YES;
	
	//Create our world
	world = new b2World(gravity, doSleep);
	world->SetContinuousPhysics(YES);
	
	//Set contact listener
	world->SetContactListener(new basicContactListener);
	
	//Set up debug drawing routine
	m_debugDraw = new GLESDebugDraw( PTM_RATIO );
	world->SetDebugDraw(m_debugDraw);
	
	//Initialize then hide debug drawing
	[self initDebugDraw];
	[self hideDebugDraw];
	
	//Add button to hide/show debug drawing
	CCMenuItemFont* swapDebugDrawMIF = [CCMenuItemFont itemFromString:@"Debug Draw" target:self selector:@selector(swapDebugDraw)];
	CCMenu *swapDebugDrawMenu = [CCMenu menuWithItems:swapDebugDrawMIF, nil];
    swapDebugDrawMenu.position = ccp( 260 , 20 );
    [self addChild:swapDebugDrawMenu z:5];
	
	//Schedule our every tick method call
	[self schedule:@selector(step:)];	
	
	return self;
}

/* This is called from 'basicContactListener'. It will need to be overridden. */
-(void) handleCollisionWithObjA:(GameObject*)objA withObjB:(GameObject*)objB {
	/** ABSTRACT **/
}

/* Destroy the world upon exit */
- (void) dealloc
{
	delete world;
	world = NULL;
	
	delete m_debugDraw;
	
	[super dealloc];
}

/* Debug information is drawn over everything */
-(void) initDebugDraw {
	DebugDrawNode * ddn = [DebugDrawNode createWithWorld:world];
	[ddn setPosition:ccp(0,0)];
	[gameNode addChild:ddn z:100000];
}

/* When we show debug draw we add a number of flags to show specific information */
-(void) showDebugDraw {
	debugDraw = YES;

	uint32 flags = 0;
	flags += b2DebugDraw::e_shapeBit;
	flags += b2DebugDraw::e_jointBit;
	flags += b2DebugDraw::e_aabbBit;
	flags += b2DebugDraw::e_pairBit;
	flags += b2DebugDraw::e_centerOfMassBit;
	m_debugDraw->SetFlags(flags);
}

/* Minimal debug drawing only shows the shapeBit flag */
-(void) showMinimalDebugDraw {
	debugDraw = YES;

	uint32 flags = 0;
	flags += b2DebugDraw::e_shapeBit;
	m_debugDraw->SetFlags(flags);
}

/* Hide debug drawing */
-(void) hideDebugDraw {
	debugDraw = NO;

	uint32 flags = 0;
	m_debugDraw->SetFlags(flags);
}

/* Swap debug draw callback */
-(void) swapDebugDraw {
	if(debugDraw){
		[self hideDebugDraw];
	}else{
		[self showDebugDraw];
	}
}

/* Add basic level boundary polygon. This is often overridden */
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
		
	groundBox.SetAsEdge(b2Vec2(0,screenSize.height/PTM_RATIO), b2Vec2(screenSize.width/PTM_RATIO,screenSize.height/PTM_RATIO));
	groundFixtureDef.shape = &groundBox;
	body->CreateFixture(&groundFixtureDef);
		
	groundBox.SetAsEdge(b2Vec2(0,screenSize.height/PTM_RATIO), b2Vec2(0,1));
	groundFixtureDef.shape = &groundBox;
	body->CreateFixture(&groundFixtureDef);
		
	groundBox.SetAsEdge(b2Vec2(screenSize.width/PTM_RATIO,screenSize.height/PTM_RATIO), b2Vec2(screenSize.width/PTM_RATIO,1));
	groundFixtureDef.shape = &groundBox;
	body->CreateFixture(&groundFixtureDef);
}

/* Mark a body for destruction */
-(void) markBodyForDestruction:(GameObject*)obj {
	[bodiesToDestroy addObject:[NSValue valueWithPointer:obj]];	
}

/* Destroy queued bodies */

//TODO - Can we limit the number of created and destroyed bodies per tick?
//       This might be messed up by the fact that we are using GameObject pointers. If a GameObject has a new body created before its old
//       body is destroyed this creates a zombie body somewhere in memory.
-(void) destroyBodies {
	for(NSValue *value in bodiesToDestroy){
		GameObject *obj = (GameObject*)[value pointerValue];
		if(obj && obj.body && !obj.markedForDestruction){
			obj.body->SetTransform(b2Vec2(0,0),0);
			world->DestroyBody(obj.body);
			obj.markedForDestruction = YES;
		}
	}
	[bodiesToDestroy removeAllObjects];
	
	//Call all game object callbacks
	for(NSValue *value in postDestructionCallbacks){
		GameObjectCallback  *goc = (GameObjectCallback*)value;		
		[goc.gameObject runAction:[CCCallFunc actionWithTarget:goc.gameObject selector:NSSelectorFromString(goc.callback)]];
	}
	
	[postDestructionCallbacks removeAllObjects];
}

/* Mark a body for creation */
-(void) markBodyForCreation:(GameObject*)obj {
	[bodiesToCreate addObject:[NSValue valueWithPointer:obj]];	
}

/* Create all queued bodies */
-(void) createBodies {
	for(NSValue *value in bodiesToCreate){
		GameObject *obj = (GameObject*)[value pointerValue];
		obj.body = world->CreateBody(obj.bodyDef);		
		obj.body->CreateFixture(obj.fixtureDef);
	}
	[bodiesToCreate removeAllObjects];
}

/* Run any queued actions after creation/destruction */
-(void) runQueuedActions {
	for(NSValue *value in queuedActions){
		QueuedAction *qa = (QueuedAction*)[value pointerValue];
		GameObject *gameObject = (GameObject*)qa.gameObject;
		CCAction *action = (CCAction*)qa.action;
		
		[gameObject runAction:action];
	}
	[queuedActions removeAllObjects];
}

-(void) cleanRecipe {
	[bodiesToDestroy removeAllObjects];
	[postDestructionCallbacks removeAllObjects];
	[bodiesToCreate removeAllObjects];
	[queuedActions removeAllObjects];	
	
	[bodiesToDestroy release];
	[postDestructionCallbacks release];
	[bodiesToCreate release];
	[queuedActions release];	
	
	bodiesToDestroy = nil;
	postDestructionCallbacks = nil;
	bodiesToCreate = nil;
	queuedActions = nil;	

	[super cleanRecipe];
}

@end
