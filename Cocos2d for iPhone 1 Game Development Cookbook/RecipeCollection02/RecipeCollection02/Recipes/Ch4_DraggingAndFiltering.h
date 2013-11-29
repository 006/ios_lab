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
	CB_GROUND = 1<<0,
	CB_FRUIT = 1<<2,
	CB_BOWL = 1<<4
};

@interface Ch4_DraggingAndFiltering : GameArea2D
{
	NSMutableArray *fruitObjects;
	GameMisc *grabbedFruit;
	CCSprite *background;
	b2Vec2 lastFruitVelocity;
	bool fruitGrabbed;
}

-(CCLayer*) runRecipe;
-(void) addFruitBasket;
-(void) ccTouchesBegan:(NSSet *)touches withEvent:(UIEvent *)event;
-(void) ccTouchesMoved:(NSSet *)touches withEvent:(UIEvent *)event;
-(void) ccTouchesEnded:(NSSet *)touches withEvent:(UIEvent *)event;
-(void) addFruit:(NSString*)spriteFrame position:(CGPoint)p shapeType:(NSString*)s;
-(void) step: (ccTime) dt;

@end



//Implementation
@implementation Ch4_DraggingAndFiltering

-(CCLayer*) runRecipe {
	[super runRecipe];
	[message setString:@"Pick up the fruit."];
																																	
	//Create level boundaries
	[self addLevelBoundaries];
			
	//Add fruit bowl
	[self addFruitBasket];
	
	//Initialization of any variables
	fruitGrabbed = NO;

	return self;
}

/* Add basket and fruit objects */
-(void) addFruitBasket {	
	[[CCSpriteFrameCache sharedSpriteFrameCache] addSpriteFramesWithFile:@"fruit_bowl.plist"];
	
	CCSprite *bowlBack = [CCSprite spriteWithSpriteFrameName:@"fruit_bowl_back.png"];
	bowlBack.position = ccp(240,120);
	bowlBack.scale = 0.5f;
	[gameNode addChild:bowlBack z:1];
	
	CCSprite *bowlFront = [CCSprite spriteWithSpriteFrameName:@"fruit_bowl_front.png"];
	bowlFront.position = ccp(240,70);
	bowlFront.scale = 0.5f;
	[gameNode addChild:bowlFront z:3];
		
	//Add physical parts
	b2BodyDef bowlBodyDef;
	bowlBodyDef.position.Set(0, 0);
	bowlBodyDef.type = b2_staticBody;
	b2Body *body = world->CreateBody(&bowlBodyDef);
		
	b2PolygonShape bowlShape;	
		
	b2FixtureDef bowlFixtureDef;
	bowlFixtureDef.restitution = 0.5f;
	bowlFixtureDef.filter.categoryBits = CB_BOWL;
	bowlFixtureDef.filter.maskBits = CB_FRUIT;
	
	//Rim left
	bowlShape.SetAsEdge(b2Vec2(120.0f/PTM_RATIO,120.0f/PTM_RATIO), b2Vec2(180.0f/PTM_RATIO,0.0f/PTM_RATIO));
	bowlFixtureDef.shape = &bowlShape;
	body->CreateFixture(&bowlFixtureDef);	
	
	//Rim right
	bowlShape.SetAsEdge(b2Vec2(300.0f/PTM_RATIO,0.0f/PTM_RATIO), b2Vec2(360.0f/PTM_RATIO,120.0f/PTM_RATIO));
	bowlFixtureDef.shape = &bowlShape;
	body->CreateFixture(&bowlFixtureDef);
	
	//Bottom
	bowlShape.SetAsEdge(b2Vec2(140.0f/PTM_RATIO,70.0f/PTM_RATIO), b2Vec2(340.0f/PTM_RATIO,70.0f/PTM_RATIO));
	bowlFixtureDef.shape = &bowlShape;
	body->CreateFixture(&bowlFixtureDef);
	
	
	/* Add fruit */
	fruitObjects = [[NSMutableArray alloc] init];
	
	[self addFruit:@"fruit_banana.png" position:ccp(210,200) shapeType:@"rect"];
	[self addFruit:@"fruit_apple.png" position:ccp(230,200) shapeType:@"circle"];
	[self addFruit:@"fruit_grapes.png" position:ccp(250,200) shapeType:@"rect"];
	[self addFruit:@"fruit_orange.png" position:ccp(270,200) shapeType:@"circle"];
}

/* Add a fruit object with circle physical properties */
-(void) addFruit:(NSString*)spriteFrame position:(CGPoint)p shapeType:(NSString*)s {
	GameMisc *fruit = [[GameMisc alloc] init];
	fruit.gameArea = self;
	
	fruit.bodyDef->type = b2_dynamicBody;
	fruit.bodyDef->position.Set(p.x/PTM_RATIO, p.y/PTM_RATIO);
	fruit.bodyDef->userData = fruit;
	fruit.body = world->CreateBody(fruit.bodyDef);
	
	fruit.fixtureDef->density = 1.0f;
	fruit.fixtureDef->friction = 0.3f;
	fruit.fixtureDef->restitution = 0.4f;
	fruit.fixtureDef->filter.categoryBits = CB_FRUIT;
	fruit.fixtureDef->filter.maskBits = CB_GROUND | CB_BOWL;	//Fruit does not collide with other fruit
	
	fruit.sprite = [CCSprite spriteWithSpriteFrameName:spriteFrame];
	fruit.sprite.position = ccp(p.x,p.y);
	
	if([s isEqualToString:@"circle"]){
		//Circle
		float textureSize = 160;
		float shapeSize = 40;
			
		fruit.sprite.scale = shapeSize / textureSize * 2;
			
		[gameNode addChild:fruit.sprite z:2];
			
		fruit.circleShape = new b2CircleShape();
		fruit.circleShape->m_radius = shapeSize/PTM_RATIO;
		fruit.fixtureDef->shape = fruit.circleShape;
	}else if([s isEqualToString:@"rect"]){
		//Rectangle
		CGPoint textureSize = ccp(300,100);
		CGPoint shapeSize = ccp(60,20);
	
		fruit.sprite.scaleX = shapeSize.x / textureSize.x * 2;
		fruit.sprite.scaleY = shapeSize.y / textureSize.y * 2;
	
		[gameNode addChild:fruit.sprite z:2];
	
		fruit.polygonShape = new b2PolygonShape();
		fruit.polygonShape->SetAsBox(shapeSize.x/PTM_RATIO, shapeSize.y/PTM_RATIO);
		fruit.fixtureDef->shape = fruit.polygonShape;
	}

	fruit.body->CreateFixture(fruit.fixtureDef);

	[fruitObjects addObject:fruit];
	grabbedFruit = fruit;
}

-(void) ccTouchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {
	UITouch *touch = [touches anyObject];
    CGPoint point = [touch locationInView: [touch view]];
	point = [[CCDirector sharedDirector] convertToGL: point];

	/* Grab the nearest fruit */
	float grabbedDistance = distanceBetweenPoints(point, ccp(grabbedFruit.body->GetPosition().x*PTM_RATIO, grabbedFruit.body->GetPosition().y*PTM_RATIO));
	for(int i=0; i<fruitObjects.count; i++){
		GameMisc *fruit = [fruitObjects objectAtIndex:i];
		float thisDistance = distanceBetweenPoints(ccp(fruit.body->GetPosition().x*PTM_RATIO, fruit.body->GetPosition().y*PTM_RATIO), point);
		if(thisDistance < grabbedDistance){
			grabbedFruit = fruit;
			grabbedDistance = thisDistance;
		}
	}
	
	fruitGrabbed = YES;
	
	[self ccTouchesMoved:touches withEvent:event];
}

-(void) ccTouchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {		
	UITouch *touch = [touches anyObject];
    CGPoint point = [touch locationInView: [touch view]];
	point = [[CCDirector sharedDirector] convertToGL: point];

	/* Reposition the grabbed fruit */
	grabbedFruit.body->SetTransform(b2Vec2(point.x/PTM_RATIO, point.y/PTM_RATIO), grabbedFruit.body->GetAngle());

	b2Vec2 moveDistance = b2Vec2( (point.x/PTM_RATIO - grabbedFruit.sprite.position.x/PTM_RATIO), (point.y/PTM_RATIO - grabbedFruit.sprite.position.y/PTM_RATIO) );
	lastFruitVelocity = b2Vec2(moveDistance.x*20, moveDistance.y*20);
}

-(void) ccTouchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {			
	/* Release the fruit */
	fruitGrabbed = NO;
	grabbedFruit.body->SetLinearVelocity(lastFruitVelocity);
}

-(void) step: (ccTime) dt {	
	[super step:dt];
	
	/* Suspend the fruit in mid-air while it is grabbed */
	if(fruitGrabbed){
		grabbedFruit.body->SetLinearVelocity(b2Vec2_zero);
	}
}

@end
