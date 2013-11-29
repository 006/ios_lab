#import "SideScrollerRecipe.h"

enum { //Object type tags
	TYPE_OBJ_BULLET = 1,
	TYPE_OBJ_SHELL = 2
};

//Interface
@interface Ch4_Bullets : SideScrollerRecipe
{
	CCSprite *muzzleFlash;
	float fireCount;
	
	NSMutableArray *bullets;
	NSMutableArray *explosions;
}

-(CCLayer*) runRecipe;
-(void) step:(ccTime)delta;
-(void) handleCollisionWithMisc:(GameMisc*)a withMisc:(GameMisc*)b;
-(void) fireGun;
-(void) bulletImpactAt:(CGPoint)p onObject:(GameMisc*)obj;
-(void) boxExplosionAt:(CGPoint)p withRotation:(float)rot;

@end

//Implementation
@implementation Ch4_Bullets

-(CCLayer*) runRecipe {
	[super runRecipe];
	[message setString:@"Press 'B' to fire some bullets."];

	//Add PLIST files
	CCSpriteFrameCache *cache = [CCSpriteFrameCache sharedSpriteFrameCache];
	[cache addSpriteFramesWithFile:@"blastmark.plist"];
	[cache addSpriteFramesWithFile:@"muzzleflash.plist"];
	[cache addSpriteFramesWithFile:@"crate_break.plist"];

	//Initialization
	fireCount = 0;
	bullets = [[NSMutableArray alloc] init];
	explosions = [[NSMutableArray alloc] init];
	
	//Initialize muzzleFlash
	muzzleFlash = [CCSprite spriteWithSpriteFrameName:@"muzzleflash_0010.png"];
	muzzleFlash.position = ccp(60,5);
	muzzleFlash.scale = 2.0f;
	muzzleFlash.opacity = 0;
	[gunman.sprite addChild:muzzleFlash z:2];
	
	//Create buttons		
	[self createButtonWithPosition:ccp(340,75) withUpFrame:@"b_button_up.png" withDownFrame:@"b_button_down.png" withName:@"B"];
	[self createButtonWithPosition:ccp(420,75) withUpFrame:@"a_button_up.png" withDownFrame:@"a_button_down.png" withName:@"A"];

	//Add brick ground and crates
	for(float x=0; x<480; x+=40){
		[self addBoxWithPosition:ccp(x,40) file:@"bricks.png" density:5.0f];
	}
	for(int i=0; i<12; i++){
		float x = arc4random()%480;
		float y = arc4random()%200+120;
		[self addBoxWithPosition:ccp(x,y) file:@"crate2.png" density:1.0f];
	}
	
	return self;
}

-(void) step:(ccTime)delta {
	[super step:delta];
				
	//Set muzzleFlash position
	if(dPad.direction == DPAD_LEFT || dPad.direction == DPAD_UP_LEFT || dPad.direction == DPAD_DOWN_LEFT){
		muzzleFlash.position = ccp(-60,5);
	}else if(dPad.direction == DPAD_RIGHT || dPad.direction == DPAD_UP_RIGHT || dPad.direction == DPAD_DOWN_RIGHT){
		muzzleFlash.position = ccp(60,5);
	}

	//Process button input
	for(id b in buttons){
		GameButton *button = (GameButton*)b;
		if(button.pressed && [button.name isEqualToString:@"B"]){
			[self fireGun];
		}else if(button.pressed && [button.name isEqualToString:@"A"]){
			[self processJump];
		}else if(!button.pressed && [button.name isEqualToString:@"A"]){
			jumpCounter = -10.0f;
		}
	}

	//Weapon fire limiter
	fireCount -= delta;
	
	//Bullet life cycle
	NSMutableArray *bulletsToRemove = [[[NSMutableArray alloc] init] autorelease];
	
	for(id b in bullets){
		GameMisc *bullet = (GameMisc*)b;
		bullet.life -= delta;
		if(bullet.life < 0){
			[self markBodyForDestruction:bullet];
			[bulletsToRemove addObject:bullet];
			[gameNode removeChild:bullet.sprite cleanup:NO];
		}
	}
	
	for(id obj in bulletsToRemove){
		[bullets removeObject:obj];
	}
	
	//Box life cycle
	NSMutableArray *boxesToRemove = [[[NSMutableArray alloc] init] autorelease];
	
	for(id b in boxes){
		GameMisc *box = (GameMisc*)b;
		if(box.life < 0){
			[self markBodyForDestruction:box];
			[boxesToRemove addObject:box];
			[self boxExplosionAt:box.sprite.position withRotation:box.sprite.rotation];
			[gameNode removeChild:box.sprite cleanup:NO];
		}
	}
	
	for(id obj in boxesToRemove){
		[boxes removeObject:obj];
	}
}

/* Fire the gun */
-(void) fireGun {
	//Fire 10 bullets per second
	if(fireCount > 0){
		return;
	}
	fireCount = 0.2f;

	CCSpriteFrameCache *cache = [CCSpriteFrameCache sharedSpriteFrameCache];

	//Animate muzzle flash
	CCAnimation *animation = [[CCAnimation alloc] initWithName:@"muzzleflash" delay:0.1f];	
	int num = arc4random()%10+10;
	[animation addFrame:[cache spriteFrameByName:[NSString stringWithFormat:@"muzzleflash_00%i.png",num]]];
	[muzzleFlash runAction:[CCSequence actions: 
		[CCFadeIn actionWithDuration:0.0f], 
		[CCAnimate actionWithAnimation:animation],
		[CCFadeOut actionWithDuration:0.3f], nil ]];	

	//Fire bullet in the correct direction
	float gunAngle = -gunman.body->GetAngle() + PI/2;
	if(gunmanDirection == DPAD_LEFT){ gunAngle += PI; }
	CGPoint bulletVector = ccp( sin(gunAngle), cos(gunAngle) );

	//Create bullet and shell casing
	for(int i=0; i<2; i++){
		GameMisc *bullet = [[GameMisc alloc] init];
		bullet.gameArea = self;
		bullet.typeTag = TYPE_OBJ_BULLET;
		if(i == 1){
			bullet.typeTag = TYPE_OBJ_SHELL;
		}
		bullet.life = 2.0f;
		if(i == 1){
			bullet.life = 5.0f;
		}

		CGPoint bulletPosition = ccp( gunman.sprite.position.x + bulletVector.x*10, gunman.sprite.position.y + bulletVector.y*10 );
		if(i == 1){
			bulletPosition = ccp( gunman.sprite.position.x, gunman.sprite.position.y );
		}
		
		bullet.bodyDef->type = b2_dynamicBody;
		if(i == 0){
			bullet.bodyDef->bullet = YES;
		}
		bullet.bodyDef->position.Set(bulletPosition.x/PTM_RATIO, bulletPosition.y/PTM_RATIO);
		bullet.body = world->CreateBody(bullet.bodyDef);	
		
		bullet.body->SetTransform(bullet.body->GetPosition(), gunAngle);
			
		CGPoint textureSize = ccp(17,17);
		CGPoint shapeSize = ccp(2,2);
		
		bullet.sprite = [CCSprite spriteWithFile:@"bullet.png"];
		bullet.sprite.position = ccp(bulletPosition.x,bulletPosition.y);
		bullet.sprite.scaleX = shapeSize.x / textureSize.x * 2.25f;
		bullet.sprite.scaleY = shapeSize.y / textureSize.y * 2.25f;
		
		if(i == 1){
			bullet.sprite.color = ccc3(255,200,0);
		}
		
		[gameNode addChild:bullet.sprite z:1];		
		
		bullet.polygonShape = new b2PolygonShape();
		bullet.polygonShape->SetAsBox(shapeSize.x/PTM_RATIO/2, shapeSize.y/PTM_RATIO);
		bullet.fixtureDef->shape = bullet.polygonShape;
		
		bullet.fixtureDef->density = 20.0f;
		bullet.fixtureDef->friction = 1.0f;
		bullet.fixtureDef->restitution = 0.0f;
		if(i == 0){
			bullet.fixtureDef->filter.categoryBits = CB_BULLET;
			bullet.fixtureDef->filter.maskBits = CB_OTHER;
		}else{
			bullet.fixtureDef->filter.categoryBits = CB_SHELL;
			bullet.fixtureDef->filter.maskBits = CB_OTHER | CB_SHELL;		
		}

		bullet.body->CreateFixture(bullet.fixtureDef);

		//Add this bullet to our container
		
		[bullets addObject:bullet];
		
		if(i == 0){
			//Fire the bullet by applying an impulse
			bullet.body->ApplyLinearImpulse(b2Vec2(bulletVector.x*50, bulletVector.y*50), bullet.body->GetPosition());
		}else{
			//Eject the shell
			float radians = vectorToRadians(bulletVector);
			radians += 1.85f * PI;
			CGPoint shellVector = radiansToVector(radians);
			if(shellVector.x > 0){ shellVector.y *= -1; }
			
			bullet.body->ApplyLinearImpulse(b2Vec2(shellVector.x, shellVector.y), bullet.body->GetPosition());
		}
	}
}

-(void) handleCollisionWithMisc:(GameMisc*)a withMisc:(GameMisc*)b {
	//If a bullet touches something we set life to 0 and process the impact on that object
	if(a.typeTag == TYPE_OBJ_BULLET && b.typeTag == TYPE_OBJ_BOX && a.life > 0){
		a.life = 0;
		[self bulletImpactAt:a.sprite.position onObject:b];
		[message setString:@"Bullet hit"];
	}else if(b.typeTag == TYPE_OBJ_BULLET && a.typeTag == TYPE_OBJ_BOX && b.life > 0){
		b.life = 0;
		[self bulletImpactAt:b.sprite.position onObject:a];
		[message setString:@"Bullet hit"];
	}
		
	//Reset our message
	[self runAction:[CCSequence actions:[CCDelayTime actionWithDuration:5.0f], 
		[CCCallFunc actionWithTarget:self selector:@selector(resetMessage)], nil]];	
}

/* Process the bullet impact */
-(void) bulletImpactAt:(CGPoint)p onObject:(GameMisc*)obj {
	//Here we use some trigonometry to determine exactly where the bullet impacted on the box.

	float dist = distanceBetweenPoints(p, obj.sprite.position);	//Hypotenuse
	float xDist = obj.sprite.position.x - p.x;	//Opposite side
	float yDist = obj.sprite.position.y - p.y;	//Adjacent side

	float xAngle = asin(xDist/dist);
	float yAngle = acos(yDist/dist);

	float objSize = [obj.sprite contentSize].width/2 * obj.sprite.scale;
		
	float newXDist = xDist - sin(xAngle) * objSize;
	float newYDist = yDist - cos(yAngle) * objSize;
	
	p = ccp( p.x + newXDist, p.y + newYDist );
	
	//Animate bullet impact
	float delay = 0.035f;
	float duration = 8 * delay;

	GameMisc *blastmark = [[GameMisc alloc] init];
	blastmark.sprite = [CCSprite spriteWithSpriteFrameName:@"blastmark_0000.png"];
	blastmark.life = duration;
	blastmark.sprite.position = p;
	blastmark.sprite.scale = 0.2f;
	blastmark.sprite.opacity = 100;
	
	CCSpriteFrameCache *cache = [CCSpriteFrameCache sharedSpriteFrameCache];
	CCAnimation *animation = [[CCAnimation alloc] initWithName:@"blastmark" delay:delay];
	for(int i=0; i<8; i+=1){
		[animation addFrame:[cache spriteFrameByName:[NSString stringWithFormat:@"blastmark_000%i.png",i]]];
	}
	
	[blastmark.sprite stopAllActions];
	[blastmark.sprite runAction:
		[CCSpawn actions:
			[CCFadeOut actionWithDuration:duration],
			[CCAnimate actionWithAnimation:animation],
			nil
		]
	];
	
	[gameNode addChild:blastmark.sprite z:5];
	[explosions addObject:blastmark];
	
	//Decrement the box life
	obj.life -= 1.0f;
}

/* Process a box exploding */
-(void) boxExplosionAt:(CGPoint)p withRotation:(float)rot {
	float delay = 0.035f;
	float duration = 7 * delay;

	GameMisc *box = [[GameMisc alloc] init];
	box.sprite = [CCSprite spriteWithSpriteFrameName:@"crate_break_01.png"];
	box.life = duration;
	box.sprite.position = p;
	box.sprite.rotation = rot;
	
	CCSpriteFrameCache *cache = [CCSpriteFrameCache sharedSpriteFrameCache];
	CCAnimation *animation = [[CCAnimation alloc] initWithName:@"crate_break" delay:delay];
	for(int i=1; i<8; i+=1){
		[animation addFrame:[cache spriteFrameByName:[NSString stringWithFormat:@"crate_break_0%i.png",i]]];
	}
	
	[box.sprite stopAllActions];
	[box.sprite runAction:
		[CCSpawn actions:
			[CCFadeOut actionWithDuration:duration],
			[CCAnimate actionWithAnimation:animation],
			nil
		]
	];
	
	[gameNode addChild:box.sprite z:5];
	[explosions addObject:box];
}

@end