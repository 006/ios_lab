#import "cocos2d.h"
#import "Recipe.h"
#import "SimpleAnimObject.h"

enum {
	TAG_CASTLE = 0,
	TAG_LIGHTNING_BOLT = 1,
	TAG_BATS = 2
};

//Bat animation types
enum {
	BAT_FLYING_UP = 0,
	BAT_GLIDING_DOWN = 1
};

//Interface
@interface Ch1_AnimatingSprites : Recipe
{
	NSMutableArray *bats;
	CCAnimation *batFlyUp;
	CCAnimation *batGlideDown;
	CCSprite *lightningBolt;
	CCSprite *lightningGlow;
	int lightningRemoveCount;
}

-(CCLayer*) runRecipe;
-(float)makeBatFlyUp:(SimpleAnimObject*)bat;
-(void)makeBatGlideDown:(SimpleAnimObject*)bat;

@end

//Implementation
@implementation Ch1_AnimatingSprites

-(CCLayer*) runRecipe {
	//Add our PLIST to the SpriteFrameCache
	[[CCSpriteFrameCache sharedSpriteFrameCache] addSpriteFramesWithFile:@"simple_bat.plist"];
	
	//Add castle background
	CCSprite *castle = [CCSprite spriteWithFile:@"dracula_castle.jpg"];
	[castle setPosition:ccp(240,160)];
	[self addChild: castle z:0 tag:TAG_CASTLE];
	
	//Add a lightning bolt
	lightningBolt = [CCSprite spriteWithFile:@"lightning_bolt.png"];
	[lightningBolt setPosition:ccp(240,160)];
	[lightningBolt setOpacity:64];
	[lightningBolt retain];

	//Add a sprite to make it light up other areas.
	lightningGlow = [CCSprite spriteWithFile:@"lightning_glow.png"];
	[lightningGlow setColor:ccc3(255,255,0)];
	[lightningGlow setPosition:ccp(240,160)];
	[lightningGlow setOpacity:100];
	[lightningGlow setBlendFunc: (ccBlendFunc) { GL_ONE, GL_ONE }];
	[lightningBolt addChild:lightningGlow];
	
	//Set a counter for lightning duration randomization
	lightningRemoveCount = 0;

	//Bats Array Initialization
	bats = [[NSMutableArray alloc] init];
	
	//Add bats using a batch node.
	CCSpriteBatchNode *batch1 = [CCSpriteBatchNode batchNodeWithFile:@"simple_bat.png" capacity:10];
	[self addChild:batch1 z:2 tag:TAG_BATS];
	
	//Make them start flying up.
	for(int x=0; x<10; x++){
		//Create SimpleAnimObject of bat
		SimpleAnimObject *bat = [SimpleAnimObject spriteWithBatchNode:batch1 rect:CGRectMake(0,0,48,48)];
		[batch1 addChild:bat];
		[bat setPosition:ccp(arc4random()%400+40, arc4random()%150+150)];
		
		//Make the bat fly up. Get the animation delay (flappingSpeed).
		float flappingSpeed = [self makeBatFlyUp:bat];
		
		//Base y velocity on flappingSpeed.
		bat.velocity = ccp((arc4random()%1000)/500 + 0.2f, 0.1f/flappingSpeed);
		
		//Add a pointer to this bat object to the NSMutableArray
		[bats addObject:[NSValue valueWithPointer:bat]];
		[bat retain];
		
		//Set the bat's direction based on x velocity.
		if(bat.velocity.x > 0){
			bat.flipX = YES;
		}
	}

	//Schedule physics updates
	[self schedule:@selector(step:)];
	
	return self;
}

-(float)makeBatFlyUp:(SimpleAnimObject*)bat {
	CCSpriteFrameCache * cache = [CCSpriteFrameCache sharedSpriteFrameCache];

	//Randomize animation speed.
	float delay = (float)(arc4random()%5+5)/80;
	CCAnimation *animation = [[CCAnimation alloc] initWithName:@"simply_bat_fly" delay:delay];

	//Randomize animation frame order.
	int num = arc4random()%4+1;
	for(int i=1; i<=4; i+=1){
		[animation addFrame:[cache spriteFrameByName:[NSString stringWithFormat:@"simple_bat_0%i.png",num]]];
		num++;
		if(num > 4){ num = 1; }
	}		
	
	//Stop any running animations and apply this one.
	[bat stopAllActions];
	[bat runAction:[CCRepeatForever actionWithAction: [CCAnimate actionWithAnimation:animation]]];
	
	//Keep track of which animation is running.
	bat.animationType = BAT_FLYING_UP;

	return delay;	//We return how fast the bat is flapping.
}

-(void)makeBatGlideDown:(SimpleAnimObject*)bat {
	CCSpriteFrameCache * cache = [CCSpriteFrameCache sharedSpriteFrameCache];

	//Apply a simple single frame gliding animation.
	CCAnimation *animation = [[CCAnimation alloc] initWithName:@"simple_bat_glide" delay:100.0f];
	[animation addFrame:[cache spriteFrameByName:@"simple_bat_01.png"]];
	
	//Stop any running animations and apply this one.
	[bat stopAllActions];
	[bat runAction:[CCRepeatForever actionWithAction: [CCAnimate actionWithAnimation:animation]]];
	
	//Keep track of which animation is running.
	bat.animationType = BAT_GLIDING_DOWN;
}

-(void)step:(ccTime)delta {
	CGSize s = [[CCDirector sharedDirector] winSize];

	for(id key in bats){
		//Get SimpleAnimObject out of NSArray of NSValue objects.
		SimpleAnimObject *bat = [key pointerValue];
	
		//Make sure bats don't fly off the screen
		if(bat.position.x > s.width){
			bat.velocity = ccp(-bat.velocity.x, bat.velocity.y);
			bat.flipX = NO;
		}else if(bat.position.x < 0){
			bat.velocity = ccp(-bat.velocity.x, bat.velocity.y);
			bat.flipX = YES;
		}else if(bat.position.y > s.height){
			bat.velocity = ccp(bat.velocity.x, -bat.velocity.y);
			[self makeBatGlideDown:bat];
		}else if(bat.position.y < 0){
			bat.velocity = ccp(bat.velocity.x, -bat.velocity.y);
			[self makeBatFlyUp:bat];
		}
		
		//Randomly make them fly back up
		if(arc4random()%100 == 7){
			if(bat.animationType == BAT_GLIDING_DOWN){ [self makeBatFlyUp:bat]; bat.velocity = ccp(bat.velocity.x, -bat.velocity.y); }
			else if(bat.animationType == BAT_FLYING_UP){ [self makeBatGlideDown:bat]; bat.velocity = ccp(bat.velocity.x, -bat.velocity.y); }
		}
		
		//Update bat position based on direction
		bat.position = ccp(bat.position.x + bat.velocity.x, bat.position.y + bat.velocity.y);
	}
	
	//Randomly make lightning strike
	if(arc4random()%70 == 7){
		if(lightningRemoveCount < 0){
			[self addChild:lightningBolt z:1 tag:TAG_LIGHTNING_BOLT];
			lightningRemoveCount = arc4random()%5+5;
		}
	}
	
	//Count down
	lightningRemoveCount -= 1;
	
	//Clean up any old lightning bolts
	if(lightningRemoveCount == 0){
		[self removeChildByTag:TAG_LIGHTNING_BOLT cleanup:NO];
	}
}

-(void) cleanRecipe {
	[bats release];
	[super cleanRecipe];
}

@end