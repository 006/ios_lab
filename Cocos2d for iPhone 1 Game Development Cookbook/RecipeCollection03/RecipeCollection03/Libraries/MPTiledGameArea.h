#import "GameArea2D.h"

typedef enum {
	PRE_GAME = 0,
	GAME_RUNNING = 1,
	GAME_PAUSED = 2
} GameState;

@interface MPTiledGameArea : GameArea2D {
	CCTMXTiledMap *tileMap;
	Vector3D *spawnPoint;	

	NSMutableDictionary *actors;
	NSMutableDictionary *dPads;
	NSString *playerKey;
	
	int gameState;
}

-(id) runRecipe;
-(void) step:(ccTime)dt;
-(void) addTileMap;
-(void) addDPad:(NSString*)key visible:(bool)visible;
-(void) addActor:(NSString*)key position:(CGPoint)position color:(NSString*)color;
-(void) cameraFollowActor;
-(void) setCameraZoom:(float)zoom;
-(void) setGameAreaSize;
-(bool) hudBegan:(NSSet*)touches withEvent:(UIEvent*)event;
-(bool) hudMoved:(NSSet*)touches withEvent:(UIEvent*)event;
-(bool) hudEnded:(NSSet*)touches withEvent:(UIEvent*)event;

@end


@implementation MPTiledGameArea

-(CCLayer*) runRecipe {
	tileMap = [CCTMXTiledMap tiledMapWithTMXFile:@"tilemap.tmx"];
	[self setGameAreaSize];

	[super runRecipe];

	//Initialization stuff
	actors = [[NSMutableDictionary alloc] init];
	dPads = [[NSMutableDictionary alloc] init];
	
	[[CCSpriteFrameCache sharedSpriteFrameCache] addSpriteFramesWithFile:@"dpad_buttons.plist"];
	[[CCDirector sharedDirector] setProjection:CCDirectorProjection2D];
	[self hideDebugDraw];
	cameraZoom = 1.0f;
	gameState = PRE_GAME;
	
	[self addTileMap];
			
	NSLog(@"Spawn point: (%f,%f)", spawnPoint.x, spawnPoint.y);
			
	//Create level boundaries
	[self addLevelBoundaries];

	[self schedule:@selector(step:)];
	
	return self;
}

-(void) step:(ccTime)dt {
	[super step:dt];

	for(int i=0; i<actors.count; i++){
		NSString *key = [NSString stringWithFormat:@"%i",i];
	
		GameActor *actor = [actors objectForKey:key];
		DPad *dPad = [dPads objectForKey:key];
	
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
		[tileMap reorderChild:actor.sprite z:z];
	}
	
	[self cameraFollowActor];
}

-(void) addTileMap {
	[gameNode addChild:tileMap z:0];

	float mw = tileMap.mapSize.width;
	float mh = tileMap.mapSize.height;
	float tw = tileMap.tileSize.width;
	float th = tileMap.tileSize.height;

	CCTMXLayer *collidableLayer = [tileMap layerNamed:@"Collidable"];	
	CCTMXLayer *ground = [tileMap layerNamed:@"Ground"];
	CCTMXLayer *wall = [tileMap layerNamed:@"Wall"];

	NSMutableDictionary *layersToReorder = [[[NSMutableDictionary alloc] init] autorelease];
	for( CCTMXLayer* child in [tileMap children] ) {
		if(child == ground){ continue; }
		else if(child == wall){ continue; }
		else if(child == collidableLayer){ continue; }
		
		for(float x=0; x<mw; x+=1){
			for(float y=mh-1; y>=0; y-=1){
				CCSprite *childTile = [child tileAt:ccp(x,y)];
				CCSprite *collideTile = [collidableLayer tileAt:ccp(x,y)];
				
				if(childTile && !collideTile){
					NSLog(@"Found non-collidable at: (%f,%f)",x,y);
				}else if(childTile && collideTile){
					NSLog(@"Reoder '%@': %f ", [child layerName], y);
					[layersToReorder setObject:[NSNumber numberWithFloat:y] forKey:[child layerName]];
					x=mw; y=-1;
				}
			}
		}

	}
	
	for(id key in layersToReorder){
		NSString *str = (NSString*)key;
		NSLog(@"Reordering: %@ to: %f", str, [[layersToReorder objectForKey:key] floatValue]);
		[tileMap reorderChild:[tileMap layerNamed:str] z:[[layersToReorder objectForKey:key] floatValue]];
	}
	
	
	[tileMap reorderChild:ground z:0];

	for(float x=0; x<mw; x+=1){
		for(float y=0; y<mh; y+=1){
			if([collidableLayer tileAt:ccp(x,y)]){
				[self addBoxAtPoint:ccp(x*tw, mh*th - y*th) size:ccp(tw/2,th/2)]; 
			}
		}
	}
	[tileMap removeChild:collidableLayer cleanup:YES];
	
	//Init Spawn Point
	CCTMXObjectGroup *objects = [tileMap objectGroupNamed:@"Objects"];
	NSAssert(objects != nil, @"'Objects' object group not found");
	NSMutableDictionary *sp = [objects objectNamed:@"SpawnPoint"];        
	NSAssert(sp != nil, @"SpawnPoint object not found");
	int x = [[sp valueForKey:@"x"] intValue];
	int y = [[sp valueForKey:@"y"] intValue];
	spawnPoint = [Vector3D x:x y:y z:0];
	[spawnPoint retain];
}

-(void) addDPad:(NSString*)key visible:(bool)visible {
	DPad *dPad = [[DPad alloc] init];
	
	if(visible){
		dPad.position = ccp(75,75);
		dPad.opacity = 100;
		[self addChild:dPad z:5];
	}else{
		dPad.visible = NO;
		//[dPad retain];
	}
	
	//Add it to the container
	[dPads setObject:dPad forKey:key];
}

-(void) addActor:(NSString*)key position:(CGPoint)position color:(NSString*)color {
	CCSpriteFrameCache *cache = [CCSpriteFrameCache sharedSpriteFrameCache];
	[cache addSpriteFramesWithFile:[NSString stringWithFormat:@"gunman_%@.plist",color]];

	GameActor *actor = [[GameActor alloc] init];
	actor.gameArea = self;
	actor.color = color;

	actor.sprite = [CCSprite spriteWithFile:@"blank.png"];
	actor.sprite.scale = 0.5f;
	[actor.sprite addChild: [CCSprite spriteWithFile:@"blank.png"] z:1 tag:0];
	
	actor.bodyDef->type = b2_dynamicBody;
	actor.bodyDef->position.Set(position.x,position.y);
	actor.bodyDef->userData = actor;
				
	actor.body = world->CreateBody(actor.bodyDef);
	
	actor.circleShape = new b2CircleShape();
	actor.circleShape->m_radius = 0.5f;
	actor.fixtureDef->shape = actor.circleShape;
	
	actor.body->CreateFixture(actor.fixtureDef);
	
	actor.body->SetLinearDamping(2.0f);
	
	[tileMap addChild:actor.sprite z:[[tileMap layerNamed:@"0"] vertexZ]];	
	
	//Add it to the container
	[actors setObject:actor forKey:key];
	
	//This sets off a chain reaction
	[actor animateActor];
}

-(void) cameraFollowActor {
	if(gameState != GAME_RUNNING){
		return;
	}

	//Follow the actor with the camera
	GameActor *actor = [actors objectForKey:playerKey];
	if(!actor){ return; }
	CGPoint actorPosition = ccp(actor.body->GetPosition().x*PTM_RATIO, actor.body->GetPosition().y*PTM_RATIO);
	[self centerCameraOnGameCoord:actorPosition];
}

-(void) setCameraZoom:(float)zoom {
	//Disable zooming
}

-(void) setGameAreaSize {
	gameAreaSize = ccp((tileMap.mapSize.width * tileMap.tileSize.width)/PTM_RATIO,(tileMap.mapSize.height * tileMap.tileSize.height)/PTM_RATIO);	//Box2d units
}

-(bool) hudBegan:(NSSet*)touches withEvent:(UIEvent*)event {
	bool hudTouched = NO;

	DPad *dPad = [dPads objectForKey:playerKey];
	[dPad ccTouchesBegan:touches withEvent:event];
	if(dPad.pressed || touches.count == 1){ hudTouched = YES; }
	return hudTouched;
}

-(bool) hudMoved:(NSSet*)touches withEvent:(UIEvent*)event {
	bool hudTouched = NO;
	
	DPad *dPad = [dPads objectForKey:playerKey];
	[dPad ccTouchesMoved:touches withEvent:event];
	if(dPad.pressed || touches.count == 1){ hudTouched = YES; }
	return hudTouched;
}

-(bool) hudEnded:(NSSet*)touches withEvent:(UIEvent*)event {
	bool hudTouched = NO;
	
	DPad *dPad = [dPads objectForKey:playerKey];
	[dPad ccTouchesEnded:touches withEvent:event];
	if(dPad.pressed || touches.count == 1){ hudTouched = YES; }
	return hudTouched;
}

-(void) cleanRecipe {
	[actors release];
	[dPads release];
	
	[super cleanRecipe];
}

@end