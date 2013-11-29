#import "cocos2d.h"
#import "GameArea2D.h"
#import "DPad.h"
#import "GameButton.h"

enum { //Object type tags
	TYPE_OBJ_BOX = 0
};

enum {	//Collision bits for filtering
	CB_GUNMAN = 1<<0,
	CB_OTHER = 1<<2,
	CB_BULLET = 1<<4,
	CB_SHELL = 1<<8
};

//Interface
@interface SideScrollerRecipe : GameArea2D
{
	DPad *dPad;
	NSMutableArray *buttons;
	
	GameMisc *gunman;
	int gunmanDirection;
	float lastXSpeed;
	float lastYVelocity;
	float jumpCounter;
	bool onGround;

	NSMutableArray *boxes;
}

-(CCLayer*) runRecipe;
-(void) step:(ccTime)delta;
-(void) initGunman;
-(void) animateGunman;
-(void) ccTouchesBegan:(NSSet *)touches withEvent:(UIEvent *)event;
-(void) ccTouchesMoved:(NSSet *)touches withEvent:(UIEvent *)event;
-(void) ccTouchesEnded:(NSSet *)touches withEvent:(UIEvent *)event;
-(void) createButtonWithPosition:(CGPoint)position withUpFrame:(NSString*)upFrame withDownFrame:(NSString*)downFrame withName:(NSString*)name;
-(void) processJump;
-(void) addBoxWithPosition:(CGPoint)p file:(NSString*)file density:(float)density;
-(void) handleCollisionWithObjA:(GameObject*)objA withObjB:(GameObject*)objB;
-(void) handleCollisionWithSensor:(GameSensor*)sensor withMisc:(GameMisc*)misc;
-(void) handleCollisionWithMisc:(GameMisc*)a withMisc:(GameMisc*)b;

@end

//Implementation
@implementation SideScrollerRecipe

-(CCLayer*) runRecipe {
	[super runRecipe];

	//Initialization
	gunmanDirection = DPAD_RIGHT;
	jumpCounter = -10.0f;
	onGround = NO;
	boxes = [[NSMutableArray alloc] init];
	buttons = [[NSMutableArray alloc] init];
	
	//Add level boundaries
	[self addLevelBoundaries];

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
	
	//This sets off an animation chain reaction
	[self animateGunman];
	
	return self;
}

-(void) step:(ccTime)delta {
	[super step:delta];
				
	//Apply gunman running direction
	if(dPad.direction == DPAD_LEFT || dPad.direction == DPAD_UP_LEFT || dPad.direction == DPAD_DOWN_LEFT){
		gunmanDirection = DPAD_LEFT;
		gunman.body->ApplyForce(b2Vec2(-35.0f,0), gunman.body->GetPosition());
		((CCSprite*)[gunman.sprite getChildByTag:0]).flipX = YES;	
	}else if(dPad.direction == DPAD_RIGHT || dPad.direction == DPAD_UP_RIGHT || dPad.direction == DPAD_DOWN_RIGHT){
		gunmanDirection = DPAD_RIGHT;
		gunman.body->ApplyForce(b2Vec2(35.0f,0), gunman.body->GetPosition());
		((CCSprite*)[gunman.sprite getChildByTag:0]).flipX = NO;
	}

	//Decrement jump counter
	jumpCounter -= delta;

	//Did the gunman just hit the ground?
	if(!onGround){
		if((gunman.body->GetLinearVelocity().y - lastYVelocity) > 2 && lastYVelocity < -2){
			gunman.body->SetLinearVelocity(b2Vec2(gunman.body->GetLinearVelocity().x,0));
			onGround = YES;
		}else if(gunman.body->GetLinearVelocity().y == 0 && lastYVelocity == 0){
			gunman.body->SetLinearVelocity(b2Vec2(gunman.body->GetLinearVelocity().x,0));
			onGround = YES;
		}
	}
	
	//Did he just fall off the ground without jumping?
	if(onGround){
		if(gunman.body->GetLinearVelocity().y < -2.0f && lastYVelocity < -2.0f && (gunman.body->GetLinearVelocity().y < lastYVelocity)){
			onGround = NO;
		}
	}

	//Store last velocity
	lastYVelocity = gunman.body->GetLinearVelocity().y;

	//Keep him upright on the ground
	if(onGround){
		gunman.body->SetTransform(gunman.body->GetPosition(),0);
	}
	
	//Animate gunman if his speed changed significantly
	float speed = gunman.body->GetLinearVelocity().x;
	if(speed < 0){ speed *= -1; }
	if(speed > lastXSpeed*2){
		[[gunman.sprite getChildByTag:0] stopAllActions];
		[self animateGunman];
	}
	
	//Keep the gunman in the level
	b2Vec2 gunmanPos = gunman.body->GetPosition();
	if(gunmanPos.x > 530/PTM_RATIO || gunmanPos.x < (-50/PTM_RATIO) || gunmanPos.y < -100/PTM_RATIO){
		gunman.body->SetTransform(b2Vec2(2,10), gunman.body->GetAngle());
	}
}

/* Initialize gunman */
-(void) initGunman {
	gunman = [[GameMisc alloc] init];
	gunman.gameArea = self;

	CGPoint gunmanPosition = ccp(240,250);
	
	gunman.bodyDef->type = b2_dynamicBody;
	gunman.bodyDef->position.Set(gunmanPosition.x/PTM_RATIO, gunmanPosition.y/PTM_RATIO);
	gunman.body = world->CreateBody(gunman.bodyDef);	
	
	CGPoint textureSize = ccp(128,128);
	CGPoint shapeSize = ccp(25,25);
	
	gunman.sprite = [CCSprite spriteWithFile:@"blank.png"];
	gunman.sprite.position = ccp(gunmanPosition.x,gunmanPosition.y);
	gunman.sprite.scaleX = shapeSize.x / textureSize.x * 2.25f;
	gunman.sprite.scaleY = shapeSize.y / textureSize.y * 2.25f;
	
	[gunman.sprite addChild: [CCSprite spriteWithFile:@"blank.png"] z:1 tag:0];
	
	[gameNode addChild:gunman.sprite z:1];		
	
	gunman.polygonShape = new b2PolygonShape();
	gunman.polygonShape->SetAsBox(shapeSize.x/PTM_RATIO/2, shapeSize.y/PTM_RATIO);
	gunman.fixtureDef->shape = gunman.polygonShape;
	
	gunman.fixtureDef->density = 2.0f;
	gunman.fixtureDef->friction = 0.0f;
	gunman.fixtureDef->restitution = 0.0f;
	gunman.fixtureDef->filter.categoryBits = CB_GUNMAN;
	gunman.fixtureDef->filter.maskBits = CB_OTHER;

	gunman.body->CreateFixture(gunman.fixtureDef);
	
	gunman.body->SetLinearDamping(2.0f);
}

/* Process jump */
-(void) processJump {
	if(onGround && jumpCounter < 0){	
		//Start a jump. Starting requires you to not be moving on the Y.
		jumpCounter = 0.4f;
		gunman.body->ApplyLinearImpulse(b2Vec2(0,20.0f), gunman.body->GetPosition());
		onGround = NO;
	}else if(jumpCounter > 0){	
		//Continue a jump
		gunman.body->ApplyForce(b2Vec2(0,65.0f), gunman.body->GetPosition());
	}
}

/* Create a button with a position */
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

/* Animate the gunman */
-(void) animateGunman {
	CCSpriteFrameCache *cache = [CCSpriteFrameCache sharedSpriteFrameCache];
	
	lastXSpeed = gunman.body->GetLinearVelocity().x;
	if(lastXSpeed == 0){ lastXSpeed = 0.0001f; }
	if(lastXSpeed < 0){ lastXSpeed *= -1; }
	
	//Animation delay is inverse speed
	float delay = 0.5f/lastXSpeed;
	if(delay > 0.5f){ delay = 0.5f; }
	CCAnimation *animation = [[CCAnimation alloc] initWithName:@"gunman_anim" delay:delay];	

	NSString *direction;
	bool flipX = NO;
	bool moving = YES;
	
	if( lastXSpeed < 0.2f){ moving = NO; }
		
	if(gunmanDirection == DPAD_LEFT){ direction = @"right"; flipX = YES; }
	else if(gunmanDirection == DPAD_RIGHT){ direction = @"right"; }

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

/* Add a box at a position */
-(void) addBoxWithPosition:(CGPoint)p file:(NSString*)file density:(float)density {
	float textureSize = 64.0f;
	float shapeSize = 20.0f;

	//Initialization
	GameMisc *box = [[GameMisc alloc] init];
	box.typeTag = TYPE_OBJ_BOX;
	box.gameArea = self;
	box.life = 5.0f;
	
	//Physical body
	box.bodyDef->type = b2_dynamicBody;
	box.bodyDef->position.Set(p.x/PTM_RATIO, p.y/PTM_RATIO);
	box.bodyDef->userData = box;
	box.body = world->CreateBody(box.bodyDef);
		
	box.polygonShape = new b2PolygonShape();
	box.polygonShape->SetAsBox(shapeSize/PTM_RATIO, shapeSize/PTM_RATIO);
	box.fixtureDef->shape = box.polygonShape;
		
	box.fixtureDef->density = density;
	box.fixtureDef->friction = 0.5f;
	box.fixtureDef->restitution = 0.0f;
	box.fixtureDef->filter.categoryBits = CB_OTHER;
	box.fixtureDef->filter.maskBits = CB_GUNMAN | CB_BULLET | CB_OTHER | CB_SHELL;
	
	box.body->CreateFixture(box.fixtureDef);
	
	//Sprite
	box.sprite = [CCSprite spriteWithFile:file];
	box.sprite.position = ccp(p.x,p.y);
	box.sprite.scale = shapeSize / textureSize * 2;
	[gameNode addChild:box.sprite z:2];
		
	[boxes addObject:box];
}

/* Collision handling */
-(void) handleCollisionWithObjA:(GameObject*)objA withObjB:(GameObject*)objB {	
	//SENSOR to MISC collision
	if(objA.type == GO_TYPE_SENSOR && objB.type == GO_TYPE_MISC){
		[self handleCollisionWithSensor:(GameSensor*)objA withMisc:(GameMisc*)objB];
	}else if(objA.type == GO_TYPE_MISC && objB.type == GO_TYPE_SENSOR){
		[self handleCollisionWithSensor:(GameSensor*)objB withMisc:(GameMisc*)objA];
	}
	
	//MISC to MISC collision
	else if(objA.type == GO_TYPE_MISC && objB.type == GO_TYPE_MISC){
		[self handleCollisionWithMisc:(GameMisc*)objA withMisc:(GameMisc*)objB];
	}
}

-(void) handleCollisionWithSensor:(GameSensor*)sensor withMisc:(GameMisc*)misc {
	//ABSTRACT
}

-(void) handleCollisionWithMisc:(GameMisc*)a withMisc:(GameMisc*)b {
	//ABSTRACT
}

/* Process dPad and button touches */
-(void) ccTouchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {	
	UITouch *touch = [touches anyObject];
    CGPoint point = [touch locationInView: [touch view]];
	point = [[CCDirector sharedDirector] convertToGL: point];
	
	[dPad ccTouchesBegan:touches withEvent:event];
	for(id b in buttons){
		GameButton *button = (GameButton*)b;
		[button ccTouchesBegan:touches withEvent:event];
	}
}
-(void) ccTouchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {	
	UITouch *touch = [touches anyObject];
    CGPoint point = [touch locationInView: [touch view]];
	point = [[CCDirector sharedDirector] convertToGL: point];

	[dPad ccTouchesMoved:touches withEvent:event];
	for(id b in buttons){
		GameButton *button = (GameButton*)b;
		[button ccTouchesMoved:touches withEvent:event];
	}
}
-(void) ccTouchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {	
	UITouch *touch = [touches anyObject];
    CGPoint point = [touch locationInView: [touch view]];
	point = [[CCDirector sharedDirector] convertToGL: point];

	[dPad ccTouchesEnded:touches withEvent:event];
	for(id b in buttons){
		GameButton *button = (GameButton*)b;
		[button ccTouchesEnded:touches withEvent:event];
	}
}

@end