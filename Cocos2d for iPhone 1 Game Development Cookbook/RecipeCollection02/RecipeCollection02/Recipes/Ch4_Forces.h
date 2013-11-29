#import "Recipe.h"
#import "GameArea2D.h"
#import "GameObject.h"
#import "GameMisc.h"
#import "GameSensor.h"

@class GameArea2D;
@class GameObject;
@class GameMisc;
@class GameSensor;

enum {	//Collision bits for filtering
	CB_PLANET = 1<<0,
	CB_NONE = 1<<2
};

@interface Ch4_Forces : GameArea2D
{
	NSMutableDictionary *planets;
}

-(CCLayer*) runRecipe;

-(void) addPlanetWithSpriteFrameName:(NSString*)frameName position:(CGPoint)p;
-(void) step:(ccTime)dt;

@end


//Implementation
@implementation Ch4_Forces

-(CCLayer*) runRecipe {
	[super runRecipe];
	
	//Set our gravity to 0
	world->SetGravity(b2Vec2(0,0));
		
	//Level background
	CCSprite *bg = [CCSprite spriteWithFile:@"solar_system_bg.png"];
	bg.position = ccp(240,160);
	[gameNode addChild:bg z:0];
	
	//Add Planets
	planets = [[NSMutableDictionary alloc] init];
	[[CCSpriteFrameCache sharedSpriteFrameCache] addSpriteFramesWithFile:@"solar_system.plist"];
	
	[self addPlanetWithSpriteFrameName:@"sun.png" position:ccp(240,160)];
	[self addPlanetWithSpriteFrameName:@"mercury.png" position:ccp(210,160)];
	[self addPlanetWithSpriteFrameName:@"venus.png" position:ccp(195,160)];
	[self addPlanetWithSpriteFrameName:@"earth.png" position:ccp(170,160)];
	[self addPlanetWithSpriteFrameName:@"mars.png" position:ccp(150,160)];
	[self addPlanetWithSpriteFrameName:@"jupiter.png" position:ccp(120,160)];
	[self addPlanetWithSpriteFrameName:@"saturn.png" position:ccp(90,160)];
	[self addPlanetWithSpriteFrameName:@"uranus.png" position:ccp(60,160)];
	[self addPlanetWithSpriteFrameName:@"neptune.png" position:ccp(30,160)];
	
	//Apply initial impulses to planets
	[[planets objectForKey:@"mercury.png"] body]->ApplyLinearImpulse(b2Vec2(0,0.075f), [[planets objectForKey:@"mercury.png"] body]->GetPosition());
	[[planets objectForKey:@"venus.png"] body]->ApplyLinearImpulse(b2Vec2(0,0.25f), [[planets objectForKey:@"venus.png"] body]->GetPosition());
	[[planets objectForKey:@"earth.png"] body]->ApplyLinearImpulse(b2Vec2(0,0.45f), [[planets objectForKey:@"earth.png"] body]->GetPosition());
	[[planets objectForKey:@"mars.png"] body]->ApplyLinearImpulse(b2Vec2(0,0.175f), [[planets objectForKey:@"mars.png"] body]->GetPosition());
	[[planets objectForKey:@"jupiter.png"] body]->ApplyLinearImpulse(b2Vec2(0,1.3f), [[planets objectForKey:@"jupiter.png"] body]->GetPosition());
	[[planets objectForKey:@"saturn.png"] body]->ApplyLinearImpulse(b2Vec2(0,4.5f), [[planets objectForKey:@"saturn.png"] body]->GetPosition());
	[[planets objectForKey:@"uranus.png"] body]->ApplyLinearImpulse(b2Vec2(0,0.6f), [[planets objectForKey:@"uranus.png"] body]->GetPosition());
	[[planets objectForKey:@"neptune.png"] body]->ApplyLinearImpulse(b2Vec2(0,0.8f), [[planets objectForKey:@"neptune.png"] body]->GetPosition());
	
	//Fast forward about 16 seconds to create realistic orbits from the start
	for(int i=0; i<1000; i++){
		[self step:0.016666667f];
	}
	
	return self;
}

/*  Every tick apply a force on each planet according to how large it is and how far it is from the sun.
	This simulates heavenly rotation. */
-(void) step:(ccTime)dt {
	[super step:dt];
	
	GameMisc *sun = [planets objectForKey:@"sun.png"];
	
	for(id key in planets){
		GameMisc *planet = [planets objectForKey:key];
		if(![key isEqualToString:@"sun.png"]){
			CGPoint vect = ccp(sun.body->GetPosition().x - planet.body->GetPosition().x, sun.body->GetPosition().y - planet.body->GetPosition().y);
			float planetSize = pow([planet.sprite contentSize].width,2);
			float dist = distanceBetweenPoints(ccp(sun.body->GetPosition().x, sun.body->GetPosition().y), 
				ccp(planet.body->GetPosition().x, planet.body->GetPosition().y));
			
			float mod = dist/planetSize*2000;

			planet.body->ApplyForce(b2Vec2(vect.x/mod, vect.y/mod) , planet.body->GetPosition() );
		}
	}
}

/* Add a planet with a spriteFrame and a position. We determine the shape size from the texture size. */
-(void) addPlanetWithSpriteFrameName:(NSString*)frameName position:(CGPoint)p {
	GameMisc *planet = [[GameMisc alloc] init];
	planet.gameArea = self;
		
	planet.bodyDef->type = b2_dynamicBody;

	planet.bodyDef->position.Set(p.x/PTM_RATIO, p.y/PTM_RATIO);
	planet.bodyDef->userData = planet;
	planet.body = world->CreateBody(planet.bodyDef);

	planet.fixtureDef->density = 1.0f;
	planet.fixtureDef->friction = 0.3f;
	planet.fixtureDef->restitution = 0.6f;
	planet.fixtureDef->filter.categoryBits = CB_PLANET;
	planet.fixtureDef->filter.maskBits = CB_NONE;
		
	planet.sprite = [CCSprite spriteWithSpriteFrameName:frameName];
	planet.sprite.position = p;
	
	float textureSize = [planet.sprite contentSize].width;
	float shapeSize = textureSize/3;
	
	planet.sprite.scale = shapeSize / textureSize * 2;
		
	[gameNode addChild:planet.sprite z:2];
		
	planet.circleShape = new b2CircleShape();
	planet.circleShape->m_radius = shapeSize/PTM_RATIO;
	planet.fixtureDef->shape = planet.circleShape;

	planet.body->CreateFixture(planet.fixtureDef);
	
	[planets setObject:planet forKey:frameName];
	
	planet.body->SetAngularVelocity(-0.25f);
}

-(void) cleanRecipe {
	[planets release];
	[super cleanRecipe];
}

@end
