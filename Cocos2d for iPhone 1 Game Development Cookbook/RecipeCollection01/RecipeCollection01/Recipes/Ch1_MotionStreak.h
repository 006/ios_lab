#import "cocos2d.h"
#import "Recipe.h"

//Interface
@interface Ch1_MotionStreak : Recipe
{
	CCSprite *rocket;
	CCMotionStreak *streak;
	CGPoint rocketDirection;
	float PI;
}

-(CCLayer*) runRecipe;
-(void)step:(ccTime)delta;

@end

//Implementation
@implementation Ch1_MotionStreak

-(CCLayer*) runRecipe {
	CGSize s = [[CCDirector sharedDirector] winSize];
		
	//Set the rocket initially in a random direction.
	rocketDirection = ccp(arc4random()%4+1,arc4random()%4+1);
		
	//Add the rocket sprite.
	rocket = [CCSprite spriteWithFile:@"rocket.png"];
	[rocket setPosition:ccp(s.width/2, s.height/2)];
	[rocket setScale:0.5f];
	[self addChild:rocket];
		
	//Create the streak object and add it to the scene.
	streak = [CCMotionStreak streakWithFade:1 minSeg:1 image:@"streak.png" width:32 length:32 color:ccc4(255,255,255,255)];
	[self addChild:streak];
	
	streak.position = ccp(s.width/2, s.height/2);

	[self schedule:@selector(step:)];

	return self;
}

-(void)step:(ccTime)delta {
	CGSize s = [[CCDirector sharedDirector] winSize];

	//Make rocket bounce off walls
	if(rocket.position.x > s.width || rocket.position.x < 0){
		rocketDirection = ccp(-rocketDirection.x, rocketDirection.y);
	}
	else if(rocket.position.y > s.height || rocket.position.y < 0){
		rocketDirection = ccp(rocketDirection.x, -rocketDirection.y);
	}

	//Slowly turn the rocket
	rocketDirection = ccp(rocketDirection.x, rocketDirection.y+0.05f);

	//Update rocket position based on direction
	rocket.position = ccp(rocket.position.x + rocketDirection.x, rocket.position.y + rocketDirection.y);
	[streak setPosition:rocket.position];

	//Set the rocket's rotation	
	[rocket setRotation: radiansToDegrees(vectorToRadians(rocketDirection))];
}

@end