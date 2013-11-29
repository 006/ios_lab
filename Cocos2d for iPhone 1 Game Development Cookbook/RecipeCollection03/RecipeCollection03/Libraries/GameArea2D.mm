#import "GameArea2D.h"
#import "BasicContactFilter.h"
#import "GameObject.h"
#import "GameMisc.h"
#import "TexturedPolygon.h"
#import "GAmeHelper.h"

#define TAP_VS_DRAG_DIST 30.0f

enum {
	GA_CAMERA_STATE_NONE = 0,
	GA_CAMERA_STATE_PANNING = 1,
	GA_CAMERA_STATE_ZOOMING = 2
};

@class GameObject;
@class DebugDrawNode;
@class ObjectCallback;
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
				
		cameraZoom = 1.0f;
		lastMultiTouchZoomDistance = 0.0f;
		draggedToPoint = ccp(0,0);
		lastTouchedPoint = ccp(0,0);
		
		cameraState = GA_CAMERA_STATE_NONE;
		allTouches = [[NSMutableDictionary alloc] init];
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
			//obj.sprite.rotation = -1 * CC_RADIANS_TO_DEGREES(b->GetAngle());
		}	
	}
	
	//Process body destruction/creation
	[self destroyBodies];
	[self createBodies];
	[self runQueuedActions];
	
	/* Process camera */
	[self checkCameraBounds];
	[self processZoomStep];
}

-(CCLayer*) runRecipe {
	[super runRecipe];

	CGSize screenSize = [CCDirector sharedDirector].winSize;

	//The gameNode is main node we'll attach everything to
	gameNode = [[CCNode alloc] init];
	gameNode.position = ccp(screenSize.width/2,screenSize.height/2);
	[self addChild:gameNode z:1];
	
	gameNode.contentSize = CGSizeMake(gameAreaSize.x*PTM_RATIO,gameAreaSize.y*PTM_RATIO);	//Cocos2d units
	gameNode.anchorPoint = ccp(0.5f,0.5f);

	//Set Y gravity to 0
	b2Vec2 gravity;
	gravity.Set(0.0f, 0.0f);
	
	//Allow objects to sleep to save cycles
	bool doSleep = YES;
	
	//Create our world
	world = new b2World(gravity, doSleep);
	world->SetContinuousPhysics(YES);
	
	//Add contact filter and contact listener
	world->SetContactFilter(new basicContactFilter);
	world->SetContactListener(new basicContactListener);
	
	//Set up debug drawing routine
	m_debugDraw = new GLESDebugDraw( PTM_RATIO );
	world->SetDebugDraw(m_debugDraw);
	
	//Initialize then show debug drawing
	[self initDebugDraw];
	[self showMinimalDebugDraw];
/*	
	//Add button to hide/show debug drawing
	CCMenuItemFont* swapDebugDrawMIF = [CCMenuItemFont itemFromString:@"Debug Draw" target:self selector:@selector(swapDebugDraw)];
	CCMenu *swapDebugDrawMenu = [CCMenu menuWithItems:swapDebugDrawMIF, nil];
    swapDebugDrawMenu.position = ccp( 260 , 20 );
    [self addChild:swapDebugDrawMenu z:5];
*/	
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
	
	[allTouches release];
	
	[super dealloc];
}

/* Debug information is drawn over everything */
-(void) initDebugDraw {
	debugDrawNode = [DebugDrawNode createWithWorld:world];
	[debugDrawNode setPosition:ccp(0,0)];
	[gameNode addChild:debugDrawNode z:100000];
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
	GameMisc *obj = [[GameMisc alloc] init];
	obj.gameArea = self;
	obj.tag = GO_TAG_WALL;
			
	obj.bodyDef->type = b2_staticBody;
	obj.bodyDef->position.Set(0,0);
	obj.bodyDef->userData = obj;
	obj.body = world->CreateBody(obj.bodyDef);
	
	obj.polygonShape = new b2PolygonShape();
	obj.fixtureDef->shape = obj.polygonShape;
	obj.fixtureDef->density = 1.0f;
	obj.fixtureDef->friction = 1.0f;
	obj.fixtureDef->restitution = 0.0f;
		
	obj.polygonShape->SetAsEdge(b2Vec2(0,0), b2Vec2(gameAreaSize.x,0));
	obj.body->CreateFixture(obj.fixtureDef);

	obj.polygonShape->SetAsEdge(b2Vec2(0,gameAreaSize.y), b2Vec2(0,0));
	obj.body->CreateFixture(obj.fixtureDef);
	
	obj.polygonShape->SetAsEdge(b2Vec2(0,gameAreaSize.y), b2Vec2(gameAreaSize.x,gameAreaSize.y));
	obj.body->CreateFixture(obj.fixtureDef);
	
	obj.polygonShape->SetAsEdge(b2Vec2(gameAreaSize.x,gameAreaSize.y), b2Vec2(gameAreaSize.x,0));
	obj.body->CreateFixture(obj.fixtureDef);
	
	levelBoundary = obj;
}

-(void) addRandomPolygons:(int)num {
	for(int i=0; i<num; i++){
		float x = (float)(arc4random()%((int)gameAreaSize.x*PTM_RATIO));
		float y = (float)(arc4random()%((int)gameAreaSize.y*PTM_RATIO));	
		
		[self addPolygonAtPoint:ccp(x,y)];
	}
}

-(void) addRandomBoxes:(int)num {
	for(int i=0; i<num; i++){
		float x = (float)(arc4random()%((int)gameAreaSize.x*PTM_RATIO));
		float y = (float)(arc4random()%((int)gameAreaSize.y*PTM_RATIO));	
		
		[self addBoxAtPoint:ccp(x,y) size:ccp((float)(arc4random()%200)+100.0f,(float)(arc4random()%50)+30.0f)];
	}
}

/* Adding a polygon */
-(void) addPolygonAtPoint:(CGPoint)p {
	//Random collection of points
	NSMutableArray *points = [[NSMutableArray alloc] init];
	for(int i=0; i<(arc4random()%5+3); i++){
		float x = (float)(arc4random()%100)+10;
		float y = (float)(arc4random()%100)+10;
		Vector3D *v = [Vector3D x:x y:y z:0];
		[points addObject:v];
	}
	
	//Convex polygon points
	NSMutableArray *convexPolygon = [GameHelper convexHull:points];
	
	//Convex Polygon
	float polygonSize = 0.05f;
				
	int32 numVerts = convexPolygon.count;
	b2Vec2 *vertices;
	vertices = new b2Vec2[convexPolygon.count];
	
	NSMutableArray *vertexArray = [[NSMutableArray alloc] init];
	
	CGPoint maxSize = ccp(0,0);
	for(int i=0; i<convexPolygon.count; i++){
		Vector3D *v = [convexPolygon objectAtIndex:i];
		vertices[i].Set(v.x*polygonSize, v.y*polygonSize);
		[vertexArray addObject:[NSValue valueWithCGPoint:ccp(v.x*PTM_RATIO*polygonSize, v.y*PTM_RATIO*polygonSize)]];
		
		//Figure out max polygon size
		if(maxSize.x < v.x*polygonSize){ maxSize.x = v.x*polygonSize; }
		if(maxSize.y < v.y*polygonSize){ maxSize.y = v.y*polygonSize; }
	}
	
	//Keep polygon in game area
	if(p.x/PTM_RATIO + maxSize.x > gameAreaSize.x){ p.x = (gameAreaSize.x - maxSize.x)*PTM_RATIO; }
	if(p.y/PTM_RATIO + maxSize.y > gameAreaSize.y){ p.y = (gameAreaSize.y - maxSize.y)*PTM_RATIO; }
	if(p.x < 0){ p.x = 0; }
	if(p.y < 0){ p.y = 0; }

	GameMisc *obj = [[GameMisc alloc] init];
	obj.gameArea = self;
	obj.tag = GO_TAG_WALL;
			
	obj.bodyDef->type = b2_staticBody;
	obj.bodyDef->position.Set(p.x/PTM_RATIO, p.y/PTM_RATIO);
	obj.bodyDef->userData = obj;
	obj.body = world->CreateBody(obj.bodyDef);
	
	obj.polygonShape = new b2PolygonShape();
	obj.polygonShape->Set(vertices, numVerts);
	obj.fixtureDef->shape = obj.polygonShape;
	
	obj.body->CreateFixture(obj.fixtureDef);
}

/* Adding a polygon */
-(void) addBoxAtPoint:(CGPoint)p size:(CGPoint)s {
	//Random collection of points
	NSMutableArray *points = [[NSMutableArray alloc] init];
	float x = s.x; float y = s.y;
	
	[points addObject:[Vector3D x:0 y:0 z:0]];
	[points addObject:[Vector3D x:x y:0 z:0]];
	[points addObject:[Vector3D x:x y:y z:0]];
	[points addObject:[Vector3D x:0 y:y z:0]];

	float polygonSize = 0.05f;
				
	int32 numVerts = points.count;
	b2Vec2 *vertices;
	vertices = new b2Vec2[points.count];
	
	NSMutableArray *vertexArray = [[NSMutableArray alloc] init];
	
	CGPoint maxSize = ccp(0,0);
	for(int i=0; i<points.count; i++){
		Vector3D *v = [points objectAtIndex:i];
		vertices[i].Set(v.x*polygonSize, v.y*polygonSize);
		[vertexArray addObject:[NSValue valueWithCGPoint:ccp(v.x*PTM_RATIO*polygonSize, v.y*PTM_RATIO*polygonSize)]];
		
		//Figure out max polygon size
		if(maxSize.x < v.x*polygonSize){ maxSize.x = v.x*polygonSize; }
		if(maxSize.y < v.y*polygonSize){ maxSize.y = v.y*polygonSize; }
	}
	
	//Keep polygon in game area
	if(p.x/PTM_RATIO + maxSize.x > gameAreaSize.x){ p.x = (gameAreaSize.x - maxSize.x)*PTM_RATIO; }
	if(p.y/PTM_RATIO + maxSize.y > gameAreaSize.y){ p.y = (gameAreaSize.y - maxSize.y)*PTM_RATIO; }
	if(p.x < 0){ p.x = 0; }
	if(p.y < 0){ p.y = 0; }

	GameMisc *obj = [[GameMisc alloc] init];
	obj.gameArea = self;
	obj.tag = GO_TAG_WALL;
			
	obj.bodyDef->type = b2_staticBody;
	obj.bodyDef->position.Set(p.x/PTM_RATIO, p.y/PTM_RATIO);
	obj.bodyDef->userData = obj;
	obj.body = world->CreateBody(obj.bodyDef);
	
	obj.polygonShape = new b2PolygonShape();
	obj.polygonShape->Set(vertices, numVerts);
	obj.fixtureDef->shape = obj.polygonShape;
	obj.fixtureDef->restitution = 0.0f;
	obj.fixtureDef->friction = 1.0f;
	
	obj.body->CreateFixture(obj.fixtureDef);
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
		ObjectCallback  *goc = (ObjectCallback*)value;		
		[goc.obj performSelector:NSSelectorFromString(goc.callback)];
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

/* Drawing */
-(void) drawLayer {
	/* ABSTRACT */
}

/* A camera bound limiting routine */
- (bool) checkCameraBoundsWithFailPosition:(CGPoint*)failPosition {
	CGSize screenSize = [CCDirector sharedDirector].winSize;
	
	bool passed = true;
	
	//Return true for now
	return passed;
	
	float fsx = (gameAreaSize.x/2)*cameraZoom;
	float fsy = (gameAreaSize.y/2)*cameraZoom;
	float ssx = screenSize.width;
	float ssy = screenSize.height;
		
	if( [gameNode position].y < -(fsy - ssy) ) {
		(*failPosition).y = -(fsy - ssy);
		passed = false;
	}else if( [gameNode position].y > fsy) {
		(*failPosition).y = fsy;
		passed = false;
	}else{ //Passed
		(*failPosition).y = [gameNode position].y;
	}
	
	if( [gameNode position].x < -(fsx - ssx) ) {
		(*failPosition).x = -(fsx - ssx);
		passed = false;
	}else if( [gameNode position].x > fsx) {
		(*failPosition).x = fsx;
		passed = false;
	}else { //Passed
		(*failPosition).x = [gameNode position].x;
	}
	
	return passed;
}

/* Check camera bounds and reset them if neccessary */
- (bool) checkCameraBounds {
	CGPoint failPosition;
	bool cameraInBounds = [self checkCameraBoundsWithFailPosition:&failPosition];
	
	if(!cameraInBounds){ //Test failed.
		[gameNode setPosition:failPosition];
	}
	
	return cameraInBounds;
}

/* Properly set the camera position using 'gameNode' */
- (void) setCameraPosition:(CGPoint)position {
	[gameNode setPosition:[self convertCamera:position]];
}

/* Convert a camera position */
- (CGPoint) convertCamera:(CGPoint)p {
	CGSize screenSize = [CCDirector sharedDirector].winSize;

	CGPoint newPoint = ccp(p.x*-1, p.y*-1);
	
	//Standard Camera Adjustment
	newPoint = ccp(newPoint.x + (screenSize.width/2) + (gameAreaSize.x/2), newPoint.y + (screenSize.height/2) + (gameAreaSize.y/2));
	
	//Zoom adjustment
	newPoint = ccp(newPoint.x*cameraZoom, newPoint.y*cameraZoom);
	
	return newPoint;
}
/* Set the camera zoom with limits */
-(void) setCameraZoom:(float)zoom {
	if(zoom < 0.1f){ zoom = 0.1f; }	//Lower limit
	if(zoom > 4.0f){ zoom = 4.0f; }	//Upper limit
	gameNode.scale = zoom;
	cameraZoom = zoom;
}
/* Set cameraZoomTo and cameraZoomSpeed */
-(void) zoomTo:(float)zoom withSpeed:(float)speed {	//Speed must be between 0 and 1 exclusive.
	cameraZoomTo = zoom;
	cameraZoomSpeed = speed;
}
/* Process a zoom step */
-(void) processZoomStep {
	float newZoom = cameraZoom;
	if(newZoom != cameraZoomTo){
		if(newZoom < cameraZoomTo){
			newZoom += cameraZoomSpeed;
			if(newZoom > cameraZoomTo){ newZoom = cameraZoomTo; }
		}else if(newZoom > cameraZoomTo){ 
			newZoom -= cameraZoomSpeed;
			if(newZoom < cameraZoomTo){ newZoom = cameraZoomTo; }
		}
		[self setCameraZoom:newZoom];
	}
}

/* Process touches */
-(void) ccTouchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {		
	bool hudTouched = NO;
	hudTouched = [self hudBegan:touches withEvent:event];
		
	for(int i=0; i<[[touches allObjects] count]; i++){
		UITouch *touch = [[touches allObjects] objectAtIndex:i];	
		NSString *hashStr = [NSString stringWithFormat:@"%i",[touch hash]];
		[allTouches setObject:touch forKey:hashStr];
	}
	
	if(!hudTouched){
		if(touches.count == 1 && allTouches.count == 1){
			UITouch *touch = [touches anyObject];
			CGPoint point = [touch locationInView: [touch view]];
			point = [[CCDirector sharedDirector] convertToGL: point];	
			
			draggedToPoint = ccp(0,0);
			lastTouchedPoint = point;
		}else if(touches.count == 2){				
			lastMultiTouchZoomDistance = 0.0f;
		}
	}
}
-(void) ccTouchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {
	CGSize screenSize = [CCDirector sharedDirector].winSize;

	bool hudTouched = NO;
	hudTouched = [self hudMoved:touches withEvent:event];
	
	if(!hudTouched){
		if(touches.count == 1 && allTouches.count == 1){
			UITouch *touch = [touches anyObject];
			CGPoint point = [touch locationInView: [touch view]];
			point = [[CCDirector sharedDirector] convertToGL: point];	
			
			//Process camera movement
			CGPoint lastTouchedDistance = ccp(point.x - lastTouchedPoint.x, point.y - lastTouchedPoint.y);
			draggedToPoint = ccp(draggedToPoint.x + lastTouchedDistance.x, draggedToPoint.y + lastTouchedDistance.y);
			
			//Should we start panning?
			if(cameraState == GA_CAMERA_STATE_NONE && [self distanceP1:ccp(0,0) toP2:draggedToPoint] > TAP_VS_DRAG_DIST){
				cameraState = GA_CAMERA_STATE_PANNING;
			}
			
			//Pan if we are already panning
			if(cameraState == GA_CAMERA_STATE_PANNING){
				//Move camera
				gameNode.anchorPoint = ccp(gameNode.anchorPoint.x - lastTouchedDistance.x/cameraZoom/gameNode.contentSize.width, 
					gameNode.anchorPoint.y - lastTouchedDistance.y/cameraZoom/gameNode.contentSize.height); 
			}
			lastTouchedPoint = point;
		}else if(touches.count == 2){
			UITouch *touch1 = [[touches allObjects] objectAtIndex:0];
			UITouch *touch2 = [[touches allObjects] objectAtIndex:1];
			CGPoint point1 = [touch1 locationInView:[touch1 view]];
			CGPoint point2 = [touch2 locationInView:[touch2 view]];
		
			//Process camera zooming
			cameraState = GA_CAMERA_STATE_ZOOMING;
			float currentDistance = [self distanceP1:point1 toP2:point2];
		
			if(lastMultiTouchZoomDistance == 0){
				lastMultiTouchZoomDistance = currentDistance;
			}else{
				float difference = currentDistance - lastMultiTouchZoomDistance;
				float newZoom = (cameraZoom + (difference/screenSize.height)*cameraZoom);
				[self setCameraZoom:newZoom];
				lastMultiTouchZoomDistance = currentDistance;
			}
		}
	}
}
-(void) ccTouchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {	
	bool hudTouched = NO;
	hudTouched = [self hudEnded:touches withEvent:event];
	
	//Store all touches
	for(int i=0; i<[[touches allObjects] count]; i++){
		UITouch *touch = [[touches allObjects] objectAtIndex:i];	
		NSString *hashStr = [NSString stringWithFormat:@"%i",[touch hash]];
		[allTouches removeObjectForKey:hashStr];
	}
			
	if(!hudTouched){
		UITouch *touch = [touches anyObject];
		CGPoint point = [touch locationInView: [touch view]];
		point = [[CCDirector sharedDirector] convertToGL: point];	
	
		if(cameraState == GA_CAMERA_STATE_NONE){
			[self tapWithPoint:point];
			cameraState = GA_CAMERA_STATE_NONE;
		}else if(cameraState == GA_CAMERA_STATE_PANNING){
			lastTouchedPoint = point;
			draggedToPoint = ccp(0,0);
			cameraState = GA_CAMERA_STATE_NONE;
		}else if(cameraState == GA_CAMERA_STATE_ZOOMING && allTouches.count == 0){
			lastMultiTouchZoomDistance = 0.0f;
			cameraState = GA_CAMERA_STATE_NONE;
		}
	}
}

//Converts a touch coordinate to a game coordinate
-(CGPoint) convertTouchCoord:(CGPoint)touchPoint {
	CGSize screenSize = [CCDirector sharedDirector].winSize;
	
	float cz = cameraZoom;
	CGPoint tp = touchPoint;
	CGPoint ap = gameNode.anchorPoint;
	CGPoint cs = ccp(gameNode.contentSize.width, gameNode.contentSize.height);
	CGPoint ss = ccp(screenSize.width, screenSize.height);
	
	float x = ((tp.x - ss.x/2) / cz) + (ap.x - 0.5f) * cs.x + cs.x/2;
	float y = ((tp.y - ss.y/2) / cz) + (ap.y - 0.5f) * cs.y + cs.y/2;
	
	return ccp(x,y);
}
//Converts a game coordinate to a touch coordinate
-(CGPoint) convertGameCoord:(CGPoint)gamePoint {
	CGSize screenSize = [CCDirector sharedDirector].winSize;
	
	float cz = cameraZoom;
	CGPoint gp = gamePoint;
	CGPoint ap = gameNode.anchorPoint;
	CGPoint cs = ccp(gameNode.contentSize.width, gameNode.contentSize.height);
	CGPoint ss = ccp(screenSize.width, screenSize.height);
	
	float x = gp.x*cz - cs.x*(ap.x-0.5f)*cz - cs.x/2 + ss.x/2;
	float y = gp.y*cz - cs.y*(ap.y-0.5f)*cz - cs.y/2 + ss.y/2;

	return ccp(x,y);
}

-(void) centerCameraOnGameCoord:(CGPoint)gamePoint {	
	gameNode.anchorPoint = ccp(gamePoint.x/gameNode.contentSize.width, gamePoint.y/gameNode.contentSize.height);
}

-(bool) hudBegan:(NSSet*)touches withEvent:(UIEvent*)event {
	/* ABSTRACT */
	return NO;
}

-(bool) hudMoved:(NSSet*)touches withEvent:(UIEvent*)event {
	/* ABSTRACT */
	return NO;
}

-(bool) hudEnded:(NSSet*)touches withEvent:(UIEvent*)event {
	/* ABSTRACT */
	return NO;
}
-(void) tapWithPoint:(CGPoint)point {
	//ABSTRACT
}

-(float) distanceP1:(CGPoint)p1 toP2:(CGPoint)p2 {
	return sqrtf( powf( (p1.x-p2.x) ,2) + powf( (p1.y-p2.y) ,2) );
}

@end
