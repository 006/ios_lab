#import "Recipe.h"
#import "GameArea2D.h"

@interface Ch8_Tiled : GameArea2D
{
	CCTMXTiledMap *tileMap;
	GameActor *actor;
	DrawLayer *drawLayer;
	DPad *dPad;
	Vector3D *spawnPoint;
}

-(CCLayer*) runRecipe;
-(void) setGameAreaSize;
-(void) step:(ccTime)dt;
-(void) addDPad;
-(void) addActor;
-(void) cameraFollowActor;
-(bool) hudBegan:(NSSet*)touches withEvent:(UIEvent*)event;
-(bool) hudMoved:(NSSet*)touches withEvent:(UIEvent*)event;
-(bool) hudEnded:(NSSet*)touches withEvent:(UIEvent*)event;

@end

//Implementation
@implementation Ch8_Tiled

-(CCLayer*) runRecipe {
	//Load TMX tilemap file
	tileMap = [CCTMXTiledMap tiledMapWithTMXFile:@"tilemap.tmx"];

	//Set game area size based on tilemap size
	[self setGameAreaSize];

	//Superclass initialization and message
	[super runRecipe];
	[self showMessage:@"Use the DPad to move the actor around."];

	//Set initial variables
	spawnPoint = nil;
	cameraZoom = 1.0f;

	//Make sure 2D projection is set
	[[CCDirector sharedDirector] setProjection:CCDirectorProjection2D];

	//Add tile map
	[gameNode addChild:tileMap z:0];

	/* Re-order layers according to their Y value. This creates isometric depth. */
	float mw = tileMap.mapSize.width; float mh = tileMap.mapSize.height;
	float tw = tileMap.tileSize.width; float th = tileMap.tileSize.height;

	//Our layers
	CCTMXLayer *collidableLayer = [tileMap layerNamed:@"Collidable"];	
	CCTMXLayer *ground = [tileMap layerNamed:@"Ground"];
	CCTMXLayer *wall = [tileMap layerNamed:@"Wall"];

	//Gather all the layers into a container
	NSMutableDictionary *layersToReorder = [[[NSMutableDictionary alloc] init] autorelease];
	for( CCTMXLayer* child in [tileMap children] ) {
		//Skip tiles marked "Collidable", "Ground" and "Wall"
		if(child == ground){ continue; }
		else if(child == wall){ continue; }
		else if(child == collidableLayer){ continue; }
		
		//Gather all the layers
		for(float x=0; x<mw; x+=1){
			for(float y=mh-1; y>=0; y-=1){
				CCSprite *childTile = [child tileAt:ccp(x,y)];
				CCSprite *collideTile = [collidableLayer tileAt:ccp(x,y)];
				
				if(childTile && collideTile){
					[layersToReorder setObject:[NSNumber numberWithFloat:y] forKey:[child layerName]];
					x=mw; y=-1;
				}
			}
		}

	}
	
	//Re-order gathered layers
	for(id key in layersToReorder){
		NSString *str = (NSString*)key;
		[tileMap reorderChild:[tileMap layerNamed:str] z:[[layersToReorder objectForKey:key] floatValue]];
	}
	
	//Set the ground to z=0
	[tileMap reorderChild:ground z:0];

	//Add Box2D boxes to represent all layers marked "Collidable"
	for(float x=0; x<mw; x+=1){
		for(float y=0; y<mh; y+=1){
			if([collidableLayer tileAt:ccp(x,y)]){
				[self addBoxAtPoint:ccp(x*tw, mh*th - y*th) size:ccp(tw/2,th/2)]; 
			}
		}
	}
	
	//Remove the "Collidable" layer art as its only an indicator for the level editor
	[tileMap removeChild:collidableLayer cleanup:YES];

	//Add DPad
	[self addDPad];

	[self hideDebugDraw];

	//Create Actor
	[self addActor];
			
	//Create level boundaries
	[self addLevelBoundaries];

	//This sets off a chain reaction
	if(actor){ [actor animateActor]; }

	[self schedule:@selector(step:)];

	return self;
}

-(void) step: (ccTime) dt {	
	[super step:dt];
	
	//Process DPad
	if(dPad.pressed){
		//[actor runWithVector:dPad.pressedVector withSpeedMod:1.0f withConstrain:NO];
		actor.body->ApplyForce(b2Vec2(dPad.pressedVector.x*15.0f, dPad.pressedVector.y*15.0f), actor.body->GetPosition());
		actor.body->SetAngularVelocity(0.0f);
		CGPoint movementVector = ccp(actor.body->GetLinearVelocity().x, actor.body->GetLinearVelocity().y);
		actor.body->SetTransform(actor.body->GetPosition(), -1 * [GameHelper vectorToRadians:movementVector] + PI_CONSTANT/2);
	}else{
		[actor stopRunning];
	}
	
	//Store last gunman direction
	if(dPad.direction != NO_DIRECTION){ 
		actor.direction = dPad.direction;
	}
	
	//Flip the animated sprite if neccessary
	if(dPad.direction == LEFT || dPad.direction == UP_LEFT || dPad.direction == DOWN_LEFT){
		((CCSprite*)[actor.sprite getChildByTag:0]).flipX = YES;	
	}else if(dPad.direction != NO_DIRECTION){
		((CCSprite*)[actor.sprite getChildByTag:0]).flipX = NO;
	}
	
	//Store last velocity
	actor.lastVelocity = ccp(actor.body->GetLinearVelocity().x, actor.body->GetLinearVelocity().y);
	
	//Re-order the actor
	float mh = tileMap.mapSize.height;
	float th = tileMap.tileSize.height;	

	CGPoint p = [actor.sprite position];
	float z = -(p.y/th) + mh;	
	[tileMap reorderChild:actor.sprite z:z ];

	[self cameraFollowActor];
}

-(void) cameraFollowActor {
	//Follow the actor with the camera
	CGPoint actorPosition = ccp(actor.body->GetPosition().x*PTM_RATIO, actor.body->GetPosition().y*PTM_RATIO);
	[self centerCameraOnGameCoord:actorPosition];
}

-(void) setCameraZoom:(float)zoom {
	//Disable zooming
}

-(void) setGameAreaSize {
	//Set gameAreaSize based on tileMap size
	gameAreaSize = ccp((tileMap.mapSize.width * tileMap.tileSize.width)/PTM_RATIO,(tileMap.mapSize.height * tileMap.tileSize.height)/PTM_RATIO);	//Box2d units
}

-(void) addDPad {
	[[CCSpriteFrameCache sharedSpriteFrameCache] addSpriteFramesWithFile:@"dpad_buttons.plist"];
	dPad = [[DPad alloc] init];
	dPad.position = ccp(75,75);
	dPad.opacity = 100;
	[self addChild:dPad z:5];
}

-(void) addActor {
	//Get spawn point from tile object named "SpawnPoint"
	if(!spawnPoint){
		CCTMXObjectGroup *objects = [tileMap objectGroupNamed:@"Objects"];
		NSAssert(objects != nil, @"'Objects' object group not found");
		NSMutableDictionary *sp = [objects objectNamed:@"SpawnPoint"];        
		NSAssert(sp != nil, @"SpawnPoint object not found");
		int x = [[sp valueForKey:@"x"] intValue];
		int y = [[sp valueForKey:@"y"] intValue];
		spawnPoint = [Vector3D x:x y:y z:0];
	}

	//Add actor
	CCSpriteFrameCache *cache = [CCSpriteFrameCache sharedSpriteFrameCache];
	[cache addSpriteFramesWithFile:@"gunman_lightblue.plist"];

	actor = [[GameActor alloc] init];
	actor.gameArea = self;

	actor.sprite = [CCSprite spriteWithFile:@"blank.png"];
	actor.sprite.scale = 0.5f;
	[actor.sprite addChild: [CCSprite spriteWithFile:@"blank.png"] z:1 tag:0];
	
	actor.bodyDef->type = b2_dynamicBody;
	actor.bodyDef->position.Set(spawnPoint.x/PTM_RATIO, spawnPoint.y/PTM_RATIO);
	actor.bodyDef->userData = actor;
				
	actor.body = world->CreateBody(actor.bodyDef);
	
	actor.circleShape = new b2CircleShape();
	actor.circleShape->m_radius = 0.5f;
	actor.fixtureDef->shape = actor.circleShape;
	
	actor.body->CreateFixture(actor.fixtureDef);
	
	actor.body->SetLinearDamping(2.0f);
	
	[tileMap addChild:actor.sprite z:[[tileMap layerNamed:@"0"] vertexZ]];	
}

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
	[super cleanRecipe];
}

@end
