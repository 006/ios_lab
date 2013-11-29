#import "cocos2d.h"
#import "GameArea2D.h"
#import "GameIsoObject.h"
#import "DPad.h"
#import "GameButton.h"
#import "IsometricContactListener.h"
#import "TexturedPolygon.h"
#import "IsoDebugDrawNode.h"

#define PERSPECTIVE_RATIO 0.5f	//This is our viewing angle. 1 would be perfectly top-down. 0 would be ground level.
#define GRAVITY 0.3f	//Our new Z gravity constant
static const float scaleHeightMultiplier = 0.0005f;	//How much larger objects look in the air
static const float zHeightModifier = 1.0f;	//How higher or lower objects look in the air

//NOTE: We modified the Box2D engine for this. I'll need to make a note of that.

enum { //Object type tags
	TYPE_OBJ_GUNMAN = 0,
	TYPE_OBJ_BALL = 1,
	TYPE_OBJ_TREE = 2
};

//Interface
@interface Ch4_TopDownIsometric : GameArea2D
{
	DPad *dPad;
	NSMutableArray *buttons;
	
	GameIsoObject *gunman;
	int gunmanDirection;
	CGPoint lastVelocity;
	float animCount;
	float fireCount;
	
	NSMutableArray *balls;
	NSMutableArray *trees;
	
	float cameraZoom;	//Starts at 1.0f. Smaller means more zoomed out.
	float cameraZoomTo;
	float cameraZoomSpeed; //Must be greater than 0 but less than 1
	float lastMultiTouchZoomDistance;	//How far apart your fingers were last time
	CGPoint lastPressedVector;
	
	CGPoint gameAreaSize;
}

-(CCLayer*) runRecipe;
-(void) initDebugDraw;
-(void) step:(ccTime)delta;
-(void) addGroundTexture;
-(void) initGunman;
-(void) animateGunman;
-(void) ccTouchesBegan:(NSSet *)touches withEvent:(UIEvent *)event;
-(void) ccTouchesMoved:(NSSet *)touches withEvent:(UIEvent *)event;
-(void) ccTouchesEnded:(NSSet *)touches withEvent:(UIEvent *)event;
-(void) createButtonWithPosition:(CGPoint)position withUpFrame:(NSString*)upFrame withDownFrame:(NSString*)downFrame withName:(NSString*)name;
-(void) fireBall;
-(void) processJump;
-(CGPoint) getWorldPosition:(GameIsoObject*)g;
-(void) setZOrderByBodyPosition:(GameObject*)g;
-(bool) checkCameraBoundsWithFailPosition:(CGPoint*)failPosition;
-(bool) checkCameraBounds;
-(void) setCameraPosition:(CGPoint)position;
-(CGPoint) convertCamera:(CGPoint)p;
-(void) setCameraZoom:(float)zoom;
-(void) zoomTo:(float)zoom withSpeed:(float)speed;
-(void) processZoomStep;
-(void) handleCollisionWithGroundWithObj:(GameObject*)gameObject;
-(void) handleZMissWithObjA:(GameObject*)objA withObjB:(GameObject*)objB;
-(void) addBrickWallFrom:(CGPoint)p1 to:(CGPoint)p2 height:(float)height;
-(void) addTreeAtPoint:(CGPoint)treePosition;
-(GameIsoObject*) addBallAtPoint:(CGPoint)ballPosition;

@end

//Implementation
@implementation Ch4_TopDownIsometric

-(CCLayer*) runRecipe {
	[super runRecipe];
	[message setString:@"Press A to jump. \nPress B to fire bouncy balls. \nPinch to zoom."];
	message.position = ccp(message.position.x,message.position.y-20);
			
	//Initialization
	balls = [[NSMutableArray alloc] init];
	trees = [[NSMutableArray alloc] init];
	animCount = 0;
	gameAreaSize = ccp(2000,1200);
	cameraZoom = 1.0f;
	lastMultiTouchZoomDistance = 0.0f;
	lastPressedVector = ccp(1,0);
	fireCount = 0;
	gunmanDirection = DPAD_RIGHT;
	buttons = [[NSMutableArray alloc] init];
	
	//Iso debug drawing
	m_debugDraw = new IsoGLESDebugDraw( PTM_RATIO, PERSPECTIVE_RATIO, gameAreaSize );
	world->SetDebugDraw(m_debugDraw);
	
	//Special isometric gravity and contact listener
	world->SetGravity(b2Vec2(0,0));
	world->SetContactListener(new isometricContactListener);

	//Initialize gunman
	CCSpriteFrameCache *cache = [CCSpriteFrameCache sharedSpriteFrameCache];
	[cache addSpriteFramesWithFile:@"gunman.plist"];
	[self initGunman];

	//Initialize DPad
	[cache addSpriteFramesWithFile:@"dpad_buttons.plist"];
	dPad = [[DPad alloc] init];
	dPad.position = ccp(75,75);
	dPad.opacity = 100;
	[self addChild:dPad z:5];
	
	//Create buttons		
	[self createButtonWithPosition:ccp(340,75) withUpFrame:@"b_button_up.png" withDownFrame:@"b_button_down.png" withName:@"B"];
	[self createButtonWithPosition:ccp(420,75) withUpFrame:@"a_button_up.png" withDownFrame:@"a_button_down.png" withName:@"A"];	
	
	//This sets off a chain reaction
	[self animateGunman];
	
	//Add ground texture
	[self addGroundTexture];
	
	//Add trees
	for(int i=0; i<20; i++){
		[self addTreeAtPoint:ccp(arc4random()%((int)gameAreaSize.x-200)+100,arc4random()%((int)gameAreaSize.y-200)+100)];
	}
	
	/* Add walls */
	
	//Horizontal
	for(int i=0; i<10; i++){
		float length = arc4random()%50+50;
		float x = arc4random()%((int)(gameAreaSize.x-length*8))+length*4;
		float y = arc4random()%((int)gameAreaSize.y-400)+200;
		float height = arc4random()%200;
		[self addBrickWallFrom:ccp(x-length,y) to:ccp(x+length,y) height:height];
	}
	
	//Vertical
	for(int i=0; i<10; i++){
		float length = arc4random()%100;
		float y = arc4random()%((int)(gameAreaSize.y-length*8))+length*4;
		float x = arc4random()%((int)gameAreaSize.x-400)+200;
		float height = arc4random()%50+50;
		[self addBrickWallFrom:ccp(x,y-length) to:ccp(x,y+length) height:height];
	}
	
	//Level Boundaries
	[self addBrickWallFrom:ccp(150,150) to:ccp(gameAreaSize.x-150,150) height:100.0f];
	[self addBrickWallFrom:ccp(150,150) to:ccp(150,gameAreaSize.y-150) height:100.0f];
	[self addBrickWallFrom:ccp(150,gameAreaSize.y-150) to:ccp(gameAreaSize.x-150,gameAreaSize.y-150) height:100.0f];
	[self addBrickWallFrom:ccp(gameAreaSize.x-150,150) to:ccp(gameAreaSize.x-150,gameAreaSize.y-150) height:100.0f];

	return self;
}

/* Init special debug draw node */
-(void) initDebugDraw {
	IsoDebugDrawNode * ddn = [IsoDebugDrawNode createWithWorld:world];
	[ddn setPosition:ccp(0,0)];
	[gameNode addChild:ddn z:100000];
}

/* We override all physical calculations here */
-(void) step: (ccTime) delta {	
	//Update Physics
	
	int32 velocityIterations = 8;
	int32 positionIterations = 3;
	
	world->Step(delta, velocityIterations, positionIterations);
	
	float deltaMod = delta/0.01666666667f;
	
	for (b2Body* b = world->GetBodyList(); b; b = b->GetNext())
	{
		//Z Miss handling allows us to know when an object passes over or under another object
		b->SetHandleZMiss(YES);
		
		if (b->GetUserData() != NULL) {
			//Synchronize the sprites position and rotation with the corresponding body
			GameIsoObject *gameObject = (GameIsoObject*)b->GetUserData();
							
			if(gameObject.sprite){
				if(gameObject.bodyDef->type == b2_dynamicBody){	
					//Process Z velocity and position
					gameObject.body->SetZVelocity( gameObject.body->GetZVelocity() - GRAVITY*deltaMod );
					gameObject.body->SetZPosition( gameObject.body->GetZPosition() + gameObject.body->GetZVelocity()*deltaMod );					
					
					//Process object bouncing and rolling
					if(gameObject.body->GetZPosition() < (-0.01f)){
						gameObject.body->SetZPosition(0.01f);
						gameObject.body->SetZVelocity( gameObject.body->GetZVelocity() * -1 );
						
						b2Vec2 worldVector = gameObject.body->GetLinearVelocityFromLocalPoint(b2Vec2(0,0));
						if(absoluteValue(gameObject.body->GetZVelocity()) > 1.0f){
							[self handleCollisionWithGroundWithObj:gameObject];
							gameObject.body->ApplyLinearImpulse( b2Vec2( gameObject.bounceCoefficient.x*worldVector.x*-1, gameObject.bounceCoefficient.y*worldVector.y*-1 ), gameObject.body->GetPosition() );
							gameObject.body->SetZVelocity( gameObject.body->GetZVelocity() * (1-gameObject.bounceCoefficient.z) );
						}else{
							gameObject.body->ApplyLinearImpulse( b2Vec2( gameObject.rollCoefficient.x*worldVector.x*-1, gameObject.rollCoefficient.y*worldVector.y*-1 ), gameObject.body->GetPosition() );
							gameObject.body->SetZVelocity( gameObject.body->GetZVelocity() * (1-gameObject.rollCoefficient.z) );
						}
					}
				
					//Change sprite positions based on body positions
					gameObject.sprite.position = CGPointMake( convertPositionX(gameAreaSize, b->GetPosition().x * PTM_RATIO), convertPositionY(gameAreaSize, b->GetPosition().y * PTM_RATIO * PERSPECTIVE_RATIO) + gameObject.yModifier + gameObject.body->GetZPosition() * zHeightModifier * PERSPECTIVE_RATIO);
					gameObject.spriteShadow.position = CGPointMake( convertPositionX(gameAreaSize, b->GetPosition().x * PTM_RATIO), convertPositionY(gameAreaSize, b->GetPosition().y * PTM_RATIO * PERSPECTIVE_RATIO));
				
					//Modify sprite scale based on Z (height)
					[gameObject.sprite setScale:( gameObject.body->GetZPosition()*scaleHeightMultiplier + gameObject->inGameSize/gameObject->actualImageSize )];
					gameObject.spriteShadow.scale = gameObject.body->GetZPosition()/100;
					if(gameObject.spriteShadow.scale > 1){ gameObject.spriteShadow.scale = 1; }
					
					//Sprite depth testing based on Y (depth)
					[self setZOrderByBodyPosition:gameObject];
					
				}else if(gameObject.bodyDef->type == b2_staticBody){
					//Static bodies are only positioned and depth tested
					gameObject.sprite.position = CGPointMake( convertPositionX(gameAreaSize, b->GetPosition().x * PTM_RATIO), convertPositionY(gameAreaSize, b->GetPosition().y * PTM_RATIO * PERSPECTIVE_RATIO) + gameObject.yModifier + gameObject.body->GetZPosition() * zHeightModifier * PERSPECTIVE_RATIO);
					[self setZOrderByBodyPosition:gameObject];
					gameObject.spriteShadow.position = CGPointMake( convertPositionX(gameAreaSize, b->GetPosition().x * PTM_RATIO), convertPositionY(gameAreaSize, b->GetPosition().y * PTM_RATIO * PERSPECTIVE_RATIO));
				}
			}
		}	
	}
	
	//Process body creation/destruction
	[self destroyBodies];
	[self createBodies];
	[self runQueuedActions];
	
	
	/* Process gunman */
	
	//Follow gunman with camera
	gameNode.position = ccp((-gunman.spriteShadow.position.x)*cameraZoom + 240, (-gunman.spriteShadow.position.y)*cameraZoom + 160);
	
	//Store last gunman direction
	if(dPad.direction != DPAD_NO_DIRECTION){ 
		gunmanDirection = dPad.direction;
		lastPressedVector = dPad.pressedVector;
	}
	
	//Process gunman running
	gunman.body->ApplyForce(b2Vec2(dPad.pressedVector.x*75.0f, dPad.pressedVector.y*75.0f), gunman.body->GetPosition());
	
	//Flip the animated sprite if neccessary
	if(dPad.direction == DPAD_LEFT || dPad.direction == DPAD_UP_LEFT || dPad.direction == DPAD_DOWN_LEFT){
		((CCSprite*)[gunman.sprite getChildByTag:0]).flipX = YES;	
	}else if(dPad.direction != DPAD_NO_DIRECTION){
		((CCSprite*)[gunman.sprite getChildByTag:0]).flipX = NO;
	}

	//Re-animate the gunman if neccessary
	animCount += delta;
	if(animCount > 1.0f){
		[[gunman.sprite getChildByTag:0] stopAllActions];
		[self animateGunman];
		animCount = 0;
	}
	
	//Decrement fireCount
	fireCount -= delta;
				
	//Store last velocity
	lastVelocity = ccp(gunman.body->GetLinearVelocity().x, gunman.body->GetLinearVelocity().y);
	
	/* Process camera */
	[self checkCameraBounds];
	[self processZoomStep];
	
	/* Process button input */
	for(id b in buttons){
		GameButton *button = (GameButton*)b;
		if(button.pressed && [button.name isEqualToString:@"B"]){
			[self fireBall];
		}else if(button.pressed && [button.name isEqualToString:@"A"]){
			[self processJump];
		}
	}
}

/* Fire a bouncy ball */
-(void) fireBall {
	if(fireCount < 0){
		GameIsoObject *ball = [self addBallAtPoint:ccp(gunman.body->GetPosition().x*PTM_RATIO + lastPressedVector.x*20.0f, gunman.body->GetPosition().y*PTM_RATIO*PERSPECTIVE_RATIO + lastPressedVector.y*20.0f)];
		ball.body->ApplyLinearImpulse(b2Vec2(lastPressedVector.x*1.75f, lastPressedVector.y*1.75f), ball.body->GetPosition());
		ball.body->SetZVelocity( gunman.body->GetZVelocity()*5.0f + 10.0f );
		ball.body->SetZPosition( gunman.body->GetZPosition() + 40.0f);
		fireCount = 10;
	}else{
		fireCount--;
	}

}

/* Process a jump */
-(void) processJump {
	//You can only jump if you are standing or running. You also need to be on the ground.
	if(gunman.body->GetZPosition() > 1.0f){	
		return;
	}
	
	//Make him jump
	[[gunman.sprite getChildByTag:0] stopAllActions];
	gunman.body->SetZVelocity(7.5f);
}

/* Create a button */
-(void) createButtonWithPosition:(CGPoint)position withUpFrame:(NSString*)upFrame withDownFrame:(NSString*)downFrame withName:(NSString*)name {
	CCSpriteFrameCache *cache = [CCSpriteFrameCache sharedSpriteFrameCache];
	
	GameButton *button = [[GameButton alloc] init];
	[buttons addObject:button];
	
	button.position = position;
	button.opacity = 100;
	[button setUpSpriteFrame:upFrame];
	[button setDownSpriteFrame:downFrame];
	[button setDisplayFrame:[cache spriteFrameByName:[button upSpriteFrame]]];
	button.name = name;
	[self addChild:button z:5];
}

/* Init the gunman */
-(void) initGunman {
	gunman = [[GameIsoObject alloc] init];
	gunman.gameArea = self;
	gunman.typeTag = TYPE_OBJ_GUNMAN;

	CGPoint gunmanPosition = ccp(300,500);
	
	gunman.bodyDef->type = b2_dynamicBody;
	gunman.bodyDef->position.Set(gunmanPosition.x/PTM_RATIO, gunmanPosition.y/PTM_RATIO);
	gunman.body = world->CreateBody(gunman.bodyDef);	
	
	CGPoint textureSize = ccp(128,128);
	CGPoint shapeSize = ccp(25,25);

	gunman.yModifier = shapeSize.y;
	gunman.zModifier = -shapeSize.y/2;
	gunman.actualImageSize = textureSize.x;
	gunman.inGameSize = textureSize.x/2;
	
	gunman.sprite = [CCSprite spriteWithFile:@"blank.png"];
	gunman.sprite.position = ccp(gunmanPosition.x,gunmanPosition.y);
	gunman.sprite.scaleX = shapeSize.x / textureSize.x * 2.25f;
	gunman.sprite.scaleY = shapeSize.y / textureSize.y * 2.25f;
	
	[gunman.sprite addChild: [CCSprite spriteWithFile:@"blank.png"] z:1 tag:0];
	
	[gameNode addChild:gunman.sprite z:5];		
	
	gunman.circleShape = new b2CircleShape();
	gunman.circleShape->m_radius = shapeSize.x/PTM_RATIO;
	gunman.fixtureDef->shape = gunman.circleShape;
	
	gunman.fixtureDef->density = 2.0f;
	gunman.fixtureDef->friction = 0.0f;
	gunman.fixtureDef->restitution = 0.0f;

	gunman.body->CreateFixture(gunman.fixtureDef);
	
	gunman.body->SetLinearDamping(2.0f);
	
	//Gunman shadow
	gunman.spriteShadow = [CCSprite spriteWithFile:@"actor_shadow.png"];
	[gameNode addChild:gunman.spriteShadow z:4];
}

/* Add a ground grass texture */
-(void) addGroundTexture {			
	CGPoint grassVertexArr[] = { ccp(0,0), ccp(gameAreaSize.x,0), ccp(gameAreaSize.x,gameAreaSize.y), ccp(0,gameAreaSize.y) };
	int grassNumVerts = 4;
	NSMutableArray *grassVertices = [[NSMutableArray alloc] init];
	for(int i=0; i<grassNumVerts; i++){
		[grassVertices addObject:[NSValue valueWithCGPoint:ccp(grassVertexArr[i].x*1, grassVertexArr[i].y*1)]];
	}

	ccTexParams params = {GL_NEAREST,GL_NEAREST_MIPMAP_NEAREST,GL_REPEAT,GL_REPEAT};
	TexturedPolygon *grassPoly = [TexturedPolygon createWithFile:@"grass_texture.png" withVertices:grassVertices];
	[grassPoly.texture setTexParameters:&params];
	grassPoly.position = ccp(-gameAreaSize.x/2,-gameAreaSize.y/2);
	[gameNode addChild:grassPoly z:0];
}

/* Repeated gunman animation routine */
-(void) animateGunman {
	CCSpriteFrameCache *cache = [CCSpriteFrameCache sharedSpriteFrameCache];

	//Animation delay is inverse speed
	float delay = 0.5f/sqrt(pow(lastVelocity.x,2)+pow(lastVelocity.y,2));
	if(delay > 0.5f){ delay = 0.5f; }
	CCAnimation *animation = [[CCAnimation alloc] initWithName:@"gunman_anim" delay:delay];	

	NSString *direction;
	bool flipX = NO;
	bool moving = YES;
	
	if( sqrt(pow(lastVelocity.x,2)+pow(lastVelocity.y,2)) < 0.2f){ moving = NO; }
		
	if(gunmanDirection == DPAD_LEFT){ direction = @"right"; flipX = YES; }
	else if(gunmanDirection == DPAD_UP_LEFT){ direction = @"up_right"; flipX = YES; }
	else if(gunmanDirection == DPAD_UP){ direction = @"up"; }
	else if(gunmanDirection == DPAD_UP_RIGHT){ direction = @"up_right"; }
	else if(gunmanDirection == DPAD_RIGHT){ direction = @"right"; }
	else if(gunmanDirection == DPAD_DOWN_RIGHT){ direction = @"down_right"; }
	else if(gunmanDirection == DPAD_DOWN){ direction = @"down"; }
	else if(gunmanDirection == DPAD_DOWN_LEFT){ direction = @"down_right"; flipX = YES; }

	if(moving){
		[animation addFrame:[cache spriteFrameByName:[NSString stringWithFormat:@"gunman_run_%@_01.png",direction]]];
		[animation addFrame:[cache spriteFrameByName:[NSString stringWithFormat:@"gunman_stand_%@.png",direction]]];
		[animation addFrame:[cache spriteFrameByName:[NSString stringWithFormat:@"gunman_run_%@_02.png",direction]]];
		[animation addFrame:[cache spriteFrameByName:[NSString stringWithFormat:@"gunman_stand_%@.png",direction]]];
	}else{
		[animation addFrame:[cache spriteFrameByName:[NSString stringWithFormat:@"gunman_stand_%@.png",direction]]];
	}

	//animateGunman calls itself indefinitely
	[[gunman.sprite getChildByTag:0] runAction:[CCSequence actions: 
		[CCAnimate actionWithAnimation:animation],
		[CCCallFunc actionWithTarget:self selector:@selector(animateGunman)], nil ]];
}

/* Convert a body position to a world position */
-(CGPoint) getWorldPosition:(GameIsoObject*)g {
	return CGPointMake(g.body->GetPosition().x * PTM_RATIO, g.body->GetPosition().y * PTM_RATIO * PERSPECTIVE_RATIO);
}

/* Depth testing */
-(void) setZOrderByBodyPosition:(GameIsoObject*)g {	
	float fixedPositionY = gameAreaSize.y - (g.body->GetPosition().y * PTM_RATIO * PERSPECTIVE_RATIO) + g.zModifier;
	[g.sprite.parent reorderChild:g.sprite z:fixedPositionY];
}

/* A camera bound limiting routine */
- (bool) checkCameraBoundsWithFailPosition:(CGPoint*)failPosition {
	CGSize screenSize = [CCDirector sharedDirector].winSize;
	
	bool passed = true;
	
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
	if(zoom < 0.5f){ zoom = 0.5f; }	//Lower limit
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

/* Obligatory isometric collision handling routines */
-(void) handleCollisionWithGroundWithObj:(GameObject*)gameObject {

}

-(void) handleZMissWithObjA:(GameObject*)objA withObjB:(GameObject*)objB {

}

/* Add a tree object */
-(void) addTreeAtPoint:(CGPoint)treePosition {
	GameIsoObject *tree = [[GameIsoObject alloc] init];
	tree.gameArea = self;
	tree.typeTag = TYPE_OBJ_TREE;
	
	tree.bodyDef->type = b2_staticBody;
	tree.bodyDef->position.Set(treePosition.x/PTM_RATIO, treePosition.y/PTM_RATIO/PERSPECTIVE_RATIO);
	tree.body = world->CreateBody(tree.bodyDef);	
	
	CGPoint textureSize = ccp(156,183);
	CGPoint shapeSize = ccp(25,25);

	tree.yModifier = 90.0f;
	tree.actualImageSize = textureSize.x;
	tree.inGameSize = textureSize.x/2;
	tree.body->SetZSize(100.0f);
	tree.body->SetZPosition(0);
	
	tree.sprite = [CCSprite spriteWithFile:@"blank.png"];
	CCSprite *tsprite = [CCSprite spriteWithFile:@"tree.png"];
	tsprite.position = ccp(-25.0f,0);
	[tree.sprite addChild:tsprite];
	
	[gameNode addChild:tree.sprite z:5];		
	
	tree.circleShape = new b2CircleShape();
	tree.circleShape->m_radius = shapeSize.x/PTM_RATIO;
	tree.fixtureDef->shape = tree.circleShape;
	
	tree.fixtureDef->density = 2.0f;
	tree.fixtureDef->friction = 0.0f;
	tree.fixtureDef->restitution = 0.0f;

	tree.body->CreateFixture(tree.fixtureDef);
	
	//Tree shadow
	tree.spriteShadow = [CCSprite spriteWithFile:@"blank.png"];
	[gameNode addChild:tree.spriteShadow z:4];
	CCSprite *tshadow = [CCSprite spriteWithFile:@"tree_shadow.png"];
	tshadow.position = ccp(-90.0f, 50.0f);
	[tree.spriteShadow addChild:tshadow];
	
	tree.tag = [trees count];
	[trees addObject:tree];
}

/* Add a ball with a random size at a position */
-(GameIsoObject*) addBallAtPoint:(CGPoint)ballPosition {
	float scaleMod = ((float)(arc4random()%5))/5.0f + 0.5f;
	
	GameIsoObject *ball = [[GameIsoObject alloc] init];
	ball.gameArea = self;
	ball.typeTag = TYPE_OBJ_BALL;
	
	//Bounce and roll coefficients determine how high the ball boucnes and how fast the ball rolls
	ball.bounceCoefficient = [Vector3D x:0.05f y:0.05f z:0.1f*scaleMod];
	ball.rollCoefficient = [Vector3D x:0.0005f y:0.0005f z:0.5f];
	
	ball.bodyDef->type = b2_dynamicBody;
	ball.bodyDef->position.Set(ballPosition.x/PTM_RATIO, ballPosition.y/PTM_RATIO/PERSPECTIVE_RATIO);
	ball.bodyDef->bullet = YES;
	ball.body = world->CreateBody(ball.bodyDef);	
	
	CGPoint textureSize = ccp(128,99);
	CGPoint shapeSize = ccp(10*scaleMod,10*scaleMod);

	ball.yModifier = 0.0f;
	ball.zModifier = 0.0f;
	ball.actualImageSize = textureSize.x/2;
	ball.inGameSize = shapeSize.x;
	ball.body->SetZSize(5.0f);
	ball.body->SetZPosition(arc4random()%200);
	
	ball.sprite = [CCSprite spriteWithFile:@"blank.png"];
	CCSprite *rsprite = [CCSprite spriteWithFile:@"ball2.png"];
	rsprite.position = ccp(0,0);
	
	//Random blal color
	[rsprite setColor:ccc3(arc4random()%255,arc4random()%255,arc4random()%255)];
	[ball.sprite addChild:rsprite];
	
	[gameNode addChild:ball.sprite z:-1];		
	
	ball.circleShape = new b2CircleShape();
	ball.circleShape->m_radius = shapeSize.x/PTM_RATIO;
	ball.fixtureDef->shape = ball.circleShape;
	
	ball.fixtureDef->density = 1.0f;
	ball.fixtureDef->friction = 0.0f;
	ball.fixtureDef->restitution = 0.95f;

	ball.body->CreateFixture(ball.fixtureDef);
		
	//Ball shadow
	ball.spriteShadow = [CCSprite spriteWithFile:@"actor_shadow.png"];
	ball.spriteShadow.position = ccp(-10000,-10000);
	[gameNode addChild:ball.spriteShadow z:2];
	
	ball.tag = [balls count];
	[balls addObject:ball];
	
	return ball;
}

/* Add a brick wall */
-(void) addBrickWallFrom:(CGPoint)p1 to:(CGPoint)p2 height:(float)height {
	//Walls can only be added at 90 degree angles
	//Depth is only used to give the wall some perceived depth. The wall is always just a line.

	//Determine whether this is a horizontal or vertical wall
	bool horiz = NO;
	if(p1.y == p2.y){ horiz = YES; }

	//Make sure the wall is drawn in 1 direction only
	if(horiz && p1.x > p2.x){ 
		float p1X = p1.x;
		float p2X = p2.x;
		p1 = ccp(p2X,p1.y);
		p2 = ccp(p1X,p2.y);
	}else if(!horiz && p1.y > p2.y){
		float p1Y = p1.y;
		float p2Y = p2.y;
		p1 = ccp(p1.x,p2Y);
		p2 = ccp(p2.x,p1Y);	
	}

	//Convert global to local positions
	CGPoint mp = midPoint(p1, p2);
	p1 = ccp(mp.x-p1.x, mp.y-p1.y);
	p2 = ccp(mp.x-p2.x, mp.y-p2.y);
	
	//Create hte wall
	GameIsoObject *wall = [[GameIsoObject alloc] init];
	wall.gameArea = self;
	
	wall.bodyDef->type = b2_staticBody;
	wall.bodyDef->position.Set(mp.x/PTM_RATIO, mp.y/PTM_RATIO/PERSPECTIVE_RATIO);
	
	wall.body = world->CreateBody(wall.bodyDef);	
	
	CGPoint textureSize = ccp(64,64);
	float depth = 10.0f;

	wall.body->SetZSize(height*2);
	wall.body->SetZPosition(0);
	wall.yModifier = 0;
	wall.actualImageSize = textureSize.x;
	wall.inGameSize = textureSize.x/2;
	
	if(!horiz){
		wall.zModifier = (p2.y-p1.y)/2;
	}
	
	//The base sprite is blank. Then we add everything to that.
	wall.sprite = [CCSprite spriteWithFile:@"blank.png"];
		
	[gameNode addChild:wall.sprite z:5];
	
	//Wall Top
	NSMutableArray *vertexArray = [[NSMutableArray alloc] init];
	int32 numVerts = 4;
	ccTexParams params = {GL_NEAREST,GL_NEAREST_MIPMAP_NEAREST,GL_REPEAT,GL_REPEAT};
	
	if(horiz){
		CGPoint vertexArr[] = { ccp(p1.x,p1.y+height), ccp(p2.x,p2.y+height), ccp(p2.x,p2.y+height+depth/2), ccp(p1.x,p1.y+height+depth/2) };
		for(int i=0; i<numVerts; i++){
			[vertexArray addObject:[NSValue valueWithCGPoint:ccp(vertexArr[i].x, vertexArr[i].y)]];
		}
	}else{
		CGPoint vertexArr[] = { ccp(p1.x-depth/2,p1.y+height), ccp(p2.x-depth/2,p2.y+height), ccp(p2.x+depth/2,p2.y+height), ccp(p1.x+depth/2,p1.y+height) };
		for(int i=0; i<numVerts; i++){
			[vertexArray addObject:[NSValue valueWithCGPoint:ccp(vertexArr[i].x, vertexArr[i].y)]];
		}
	}

	TexturedPolygon *wallTop = [TexturedPolygon createWithFile:@"bricks_top.png" withVertices:vertexArray];
	[wallTop.texture setTexParameters:&params];
	[wallTop setPosition:ccp(0,0)];
	[wall.sprite addChild:wallTop z:2 tag:0];	
		
	//Wall Side
	vertexArray = [[NSMutableArray alloc] init];
	
	if(horiz){
		CGPoint vertexArr[] = { p1, p2, ccp(p2.x,p2.y+height), ccp(p1.x,p1.y+height) };
		for(int i=0; i<numVerts; i++){
			[vertexArray addObject:[NSValue valueWithCGPoint:ccp(vertexArr[i].x, vertexArr[i].y)]];
		}
	}else{
		CGPoint vertexArr[] = { ccp(p2.x-depth/2,p2.y), ccp(p2.x+depth/2,p2.y), ccp(p2.x+depth/2,p2.y+height), ccp(p2.x-depth/2,p2.y+height) };
		for(int i=0; i<numVerts; i++){
			[vertexArray addObject:[NSValue valueWithCGPoint:ccp(vertexArr[i].x, vertexArr[i].y)]];
		}
	}
	
	TexturedPolygon *wallSide = [TexturedPolygon createWithFile:@"bricks.png" withVertices:vertexArray];
	[wallSide.texture setTexParameters:&params];
	[wallSide setPosition:ccp(0,0)];
	[wall.sprite addChild:wallSide z:1 tag:1];	
	
	
	//Wall Shadow
	vertexArray = [[NSMutableArray alloc] init];
	
	if(horiz){
		CGPoint shadowMod = ccp(-height/4,height/4);
		CGPoint vertexArr[] = { p1, p2, ccp(p2.x+shadowMod.x,p2.y+height+shadowMod.y), ccp(p1.x+shadowMod.x,p1.y+height+shadowMod.y) };
		for(int i=0; i<numVerts; i++){
			[vertexArray addObject:[NSValue valueWithCGPoint:ccp(vertexArr[i].x, vertexArr[i].y)]];
		}
	}else{	
		CGPoint shadowMod = ccp(-height/4,height);
		numVerts = 5;
		CGPoint vertexArr[] = { ccp(p1.x-depth/2+shadowMod.x,p1.y+shadowMod.y), ccp(p1.x+shadowMod.x,p1.y+shadowMod.y), ccp(p1.x-depth/2,p1.y), ccp(p2.x-depth/2,p2.y), ccp(p2.x-depth/2+shadowMod.x,p2.y+shadowMod.y) };
		for(int i=0; i<numVerts; i++){
			[vertexArray addObject:[NSValue valueWithCGPoint:ccp(vertexArr[i].x, vertexArr[i].y)]];
		}	
	}
	
	TexturedPolygon *s = [TexturedPolygon createWithFile:@"shadow_texture.png" withVertices:vertexArray];
	[s.texture setTexParameters:&params];
	[s setPosition:ccp(0,0)];
	wall.spriteShadow = s;	
	[gameNode addChild:wall.spriteShadow];
	
	
	//Finally, add the simple shape
	wall.polygonShape = new b2PolygonShape();
	wall.polygonShape->SetAsEdge(b2Vec2(p1.x/PTM_RATIO,p1.y/PTM_RATIO/PERSPECTIVE_RATIO), b2Vec2(p2.x/PTM_RATIO,p2.y/PTM_RATIO/PERSPECTIVE_RATIO));
	wall.fixtureDef->shape = wall.polygonShape;	

	//Wall properties
	wall.fixtureDef->density = 2.0f;
	wall.fixtureDef->friction = 0.0f;
	wall.fixtureDef->restitution = 0.0f;

	//Create the fixture
	wall.body->CreateFixture(wall.fixtureDef);
}

/* Process touches */
-(void) ccTouchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {	
	UITouch *touch = [touches anyObject];
    CGPoint point = [touch locationInView: [touch view]];
	point = [[CCDirector sharedDirector] convertToGL: point];
	
	//Process dPad and button input
	bool hudTouched = NO;
	[dPad ccTouchesBegan:touches withEvent:event];
	if(dPad.pressed){ hudTouched = YES; }
	for(id b in buttons){
		GameButton *button = (GameButton*)b;
		[button ccTouchesBegan:touches withEvent:event];
		if(button.pressed){ hudTouched = YES; }
	}
	
	//Process zooming if we didn't touch the HUD
	if(!hudTouched){
		lastMultiTouchZoomDistance = 0.0f;
	}
}
-(void) ccTouchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {	
	UITouch *touch = [touches anyObject];
    CGPoint point = [touch locationInView: [touch view]];
	point = [[CCDirector sharedDirector] convertToGL: point];

	//Process dPad and button input
	bool hudTouched = NO;
	[dPad ccTouchesMoved:touches withEvent:event];
	if(dPad.pressed){ hudTouched = YES; }
	for(id b in buttons){
		GameButton *button = (GameButton*)b;
		[button ccTouchesMoved:touches withEvent:event];
		if(button.pressed){ hudTouched = YES; }
	}
	
	//Proecss multi-touch zoom input
	CGSize screenSize = [CCDirector sharedDirector].winSize;
	
	if(touches.count == 2){
		NSArray *twoTouch = [touches allObjects];
		
		UITouch *tOne = [twoTouch objectAtIndex:0];
		UITouch *tTwo = [twoTouch objectAtIndex:1];
		CGPoint firstTouch = [tOne locationInView:[tOne view]];
		CGPoint secondTouch = [tTwo locationInView:[tTwo view]];
			
		if(!hudTouched){
			CGFloat currentDistance = distanceBetweenPoints(firstTouch, secondTouch);
		
			if(lastMultiTouchZoomDistance == 0){
				lastMultiTouchZoomDistance = currentDistance;
			}else{
				float difference = currentDistance - lastMultiTouchZoomDistance;
				float newZoom = (cameraZoom + (difference/screenSize.height));
				if(newZoom < 0.4f){ newZoom = 0.4f; }
				if(newZoom > 2.0f){ newZoom = 2.0f; }
				[self setCameraZoom:newZoom];
				lastMultiTouchZoomDistance = currentDistance;
			}
		}
	}
}
-(void) ccTouchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {	
	UITouch *touch = [touches anyObject];
    CGPoint point = [touch locationInView: [touch view]];
	point = [[CCDirector sharedDirector] convertToGL: point];

	//Process dPad and button input
	bool hudTouched = NO;
	[dPad ccTouchesEnded:touches withEvent:event];
	if(dPad.pressed){ hudTouched = YES; }
	for(id b in buttons){
		GameButton *button = (GameButton*)b;
		[button ccTouchesEnded:touches withEvent:event];
		if(button.pressed){ hudTouched = YES; }
	}
	
	//Process zooming if we didn't touch the HUD
	if(!hudTouched){
		lastMultiTouchZoomDistance = 0.0f;
	}
}

@end