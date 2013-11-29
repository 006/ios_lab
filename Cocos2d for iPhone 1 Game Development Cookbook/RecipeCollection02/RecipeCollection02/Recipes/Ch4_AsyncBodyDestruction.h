#import "Recipe.h"
#import "GameArea2D.h"
#import "GameObject.h"
#import "GameMisc.h"
#import "GameSensor.h"

@class GameArea2D;
@class GameObject;
@class GameMisc;
@class GameSensor;

@interface Ch4_AsyncBodyDestruction : GameArea2D
{
	NSMutableArray *grenades;
	NSMutableArray *explosions;
	CCSprite *gunman;
	CCSprite *gunmanArm;
}

-(CCLayer*) runRecipe;
-(void) addGunman;
-(void) ccTouchesBegan:(NSSet *)touches withEvent:(UIEvent *)event;
-(void) ccTouchesMoved:(NSSet *)touches withEvent:(UIEvent *)event;
-(void) ccTouchesEnded:(NSSet *)touches withEvent:(UIEvent *)event;
-(void) step:(ccTime)dt;
-(void) showArm;
-(void) throwGrenade;
-(void) hideArm;
-(void) explosionAt:(CGPoint)p;

@end



//Implementation
@implementation Ch4_AsyncBodyDestruction

-(CCLayer*) runRecipe {
	[super runRecipe];
	[message setString:@"Tap to throw a grenade."];
													
	//Create level boundaries
	[self addLevelBoundaries];
	
	//Add gunman
	[self addGunman];
	
	//Initialize explosion animation
	[[CCSpriteFrameCache sharedSpriteFrameCache] addSpriteFramesWithFile:@"explosion5.plist"];
	
	//Initialize grenade container
	grenades = [[NSMutableArray alloc] init];

	return self;
}

/* Add a gunman sprite and set his arm position */
-(void) addGunman {
	[[CCSpriteFrameCache sharedSpriteFrameCache] addSpriteFramesWithFile:@"gunman.plist"];

	CGPoint textureSize = ccp(128,128);
	CGPoint shapeSize = ccp(96,96);

	gunmanArm = [CCSprite spriteWithSpriteFrameName:@"gunman_stand_right_arm.png"];
	gunmanArm.position = ccp(65,110);
	gunmanArm.anchorPoint = ccp(0.4f,0.6f);
	gunmanArm.rotation = -90;
	gunmanArm.visible = NO;
	[gameNode addChild:gunmanArm z:1];

	gunman = [CCSprite spriteWithSpriteFrameName:@"gunman_stand_right.png"];
	gunman.position = ccp(70,100);
	
	[gameNode addChild:gunman z:2];
}

/* Animate the arm and launch a grenade */
-(void) ccTouchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {		
	UITouch *touch = [touches anyObject];
    CGPoint point = [touch locationInView: [touch view]];
	point = [[CCDirector sharedDirector] convertToGL: point];

	[gunmanArm runAction:[CCSequence actions: 
		[CCCallFunc actionWithTarget:self selector:@selector(showArm)],
		[CCRotateTo actionWithDuration:0.1f angle:-60],
		[CCCallFunc actionWithTarget:self selector:@selector(throwGrenade)],
		[CCRotateTo actionWithDuration:0.3f angle:30],
		[CCCallFunc actionWithTarget:self selector:@selector(hideArm)],
		nil]];
}

/* Callback for showing the arm using an animation */
-(void) showArm {
	gunmanArm.visible = YES;
	
	CCSpriteFrameCache *cache = [CCSpriteFrameCache sharedSpriteFrameCache];
	
	CCAnimation *animation = [[CCAnimation alloc] initWithName:@"gunmanStandRightNoArm" delay:1.0f];
	[animation addFrame:[cache spriteFrameByName:@"gunman_stand_right_no_arm.png"]];
	[gunman runAction:[CCRepeatForever actionWithAction:[CCAnimate actionWithAnimation:animation]]];
}

/* Callback for throwing the arm. This involves animating the arm and creating a grenade */
-(void) throwGrenade {
	CCSpriteFrameCache *cache = [CCSpriteFrameCache sharedSpriteFrameCache];
	
	CCAnimation *animation = [[CCAnimation alloc] initWithName:@"gunmanStandRightArmEmpty" delay:1.0f];
	[animation addFrame:[cache spriteFrameByName:@"gunman_stand_right_arm_empty.png"]];
	[gunmanArm runAction:[CCRepeatForever actionWithAction:[CCAnimate actionWithAnimation:animation]]];

	GameMisc *grenade = [[GameMisc alloc] init];
	grenade.life = 5.0f;
	grenade.gameArea = self;

	CGPoint grenadePosition = ccp(65,150);
	
	grenade.bodyDef->type = b2_dynamicBody;
	grenade.bodyDef->position.Set(grenadePosition.x/PTM_RATIO, grenadePosition.y/PTM_RATIO);
	grenade.body = world->CreateBody(grenade.bodyDef);	
	
	grenade.body->SetTransform(b2Vec2(grenadePosition.x/PTM_RATIO, grenadePosition.y/PTM_RATIO),PI/2);
	
	CGPoint textureSize = ccp(16,16);
	CGPoint shapeSize = ccp(7,7);
	
	grenade.sprite = [CCSprite spriteWithSpriteFrameName:@"gunman_grenade.png"];
	grenade.sprite.position = ccp(grenadePosition.x,grenadePosition.y);
	grenade.sprite.scaleX = shapeSize.x / textureSize.x * 2;
	grenade.sprite.scaleY = shapeSize.y / textureSize.y * 2;
	
	[gameNode addChild:grenade.sprite z:1];		
	
	grenade.circleShape = new b2CircleShape();
	grenade.circleShape->m_radius = shapeSize.x/PTM_RATIO;
	grenade.fixtureDef->shape = grenade.circleShape;

	grenade.body->CreateFixture(grenade.fixtureDef);
	
	[grenades addObject:grenade];
	
	grenade.body->ApplyLinearImpulse(b2Vec2(1.0f,2.0f) , grenade.body->GetPosition() );
	grenade.body->SetAngularVelocity(PI);
}

/* Hide the arm when finished */
-(void) hideArm {
	gunmanArm.visible = NO;
	gunmanArm.rotation = -90;
	
	CCSpriteFrameCache *cache = [CCSpriteFrameCache sharedSpriteFrameCache];
	
	CCAnimation *animation = [[CCAnimation alloc] initWithName:@"gunmanStandRight" delay:1.0f];
	[animation addFrame:[cache spriteFrameByName:@"gunman_stand_right.png"]];
	[gunman runAction:[CCRepeatForever actionWithAction:[CCAnimate actionWithAnimation:animation]]];
	
	CCAnimation *animationArm = [[CCAnimation alloc] initWithName:@"gunmanStandRightArm" delay:1.0f];
	[animationArm addFrame:[cache spriteFrameByName:@"gunman_stand_right_arm.png"]];
	[gunmanArm runAction:[CCRepeatForever actionWithAction:[CCAnimate actionWithAnimation:animationArm]]];
}

/* Animate an explosion at a position */
-(void) explosionAt:(CGPoint)p {
	float delay = 0.035f;
	float duration = 25 * delay;

	GameMisc *explosion = [[GameMisc alloc] init];
	explosion.sprite = [CCSprite spriteWithSpriteFrameName:@"explosion5_0000.png"];
	explosion.life = duration;
	explosion.sprite.position = p;
	explosion.sprite.scale = 2.0f;
	
	CCSpriteFrameCache *cache = [CCSpriteFrameCache sharedSpriteFrameCache];
	CCAnimation *animation = [[CCAnimation alloc] initWithName:@"explosion5" delay:delay];
	for(int i=0; i<25; i+=1){
		if(i < 10){ 
			[animation addFrame:[cache spriteFrameByName:[NSString stringWithFormat:@"explosion5_000%i.png",i]]];
		}else{
			[animation addFrame:[cache spriteFrameByName:[NSString stringWithFormat:@"explosion5_00%i.png",i]]];
		}
	}
	
	[explosion.sprite stopAllActions];
	[explosion.sprite runAction:
		[CCSpawn actions:
			[CCFadeOut actionWithDuration:duration],
			[CCAnimate actionWithAnimation:animation],
			nil
		]
	];
	
	[gameNode addChild:explosion.sprite z:2];
	[explosions addObject:explosion];
	
	//Grenades that get caught in the blast are thrown in appropriate directions 
	for(id obj in grenades){
		GameMisc *grenade = (GameMisc*)obj;
		
		if(grenade.life > 0 && distanceBetweenPoints(grenade.sprite.position, explosion.sprite.position) < 50){
			CGPoint vect = ccp(grenade.sprite.position.x - explosion.sprite.position.x, 
				grenade.sprite.position.y - explosion.sprite.position.y);
			grenade.body->ApplyLinearImpulse(b2Vec2(vect.x/10, vect.y/10) , grenade.body->GetPosition() );
		}
	}
}

-(void) step:(ccTime)delta {
	[super step:delta];
	
	//Grenade life cycle
	
	//Grenade removal container
	NSMutableArray *grenadesToRemove = [[[NSMutableArray alloc] init] autorelease];
	
	//Explode any grenades
	for(id obj in grenades){
		GameMisc *grenade = (GameMisc*)obj;
		grenade.life -= delta;
		
		//If a grenade is out of life we mark it for destruction, do cleanup and finally animate an explosion
		if(grenade.life < 0){
			[self markBodyForDestruction:grenade];
			[grenadesToRemove addObject:obj];
			[self explosionAt:grenade.sprite.position];
			[gameNode removeChild:grenade.sprite cleanup:NO];
		}
	}
	
	//Remove marked grenades
	for(id obj in grenadesToRemove){
		[grenades removeObject:obj];
	}
	
	
	//Explosion life cycle
	for(id obj in explosions){
		GameMisc *explosion = (GameMisc*)explosion;
		explosion.life -= delta;	
		
		if(explosion.life < 0){
			[explosions removeObject:explosion];
			[gameNode removeChild:explosion.sprite cleanup:YES];
		}
	}
}

-(void) cleanRecipe {
	//Cleanup grenade and explosion containers
	[grenades removeAllObjects];
	[explosions removeAllObjects];
	
	[grenades release];
	[explosions release];
	
	grenades = nil;
	explosions = nil;

	[super cleanRecipe];
}

@end
