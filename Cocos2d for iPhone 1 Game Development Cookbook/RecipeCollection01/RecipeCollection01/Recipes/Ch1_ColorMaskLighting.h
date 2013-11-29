#import "cocos2d.h"
#import "Recipe.h"
#import "SimpleAnimObject.h"

enum {
	TAG_CAVE_BG = 0,
	TAG_DARKNESS_LAYER = 1,
	TAG_BURN_SPRITE = 2,
	TAG_LIGHT_SPRITE = 3,
	TAG_MONK = 4,
	TAG_CAVE_BATS = 5
};

//Interface
@interface Ch1_ColorMaskLighting : Recipe
{
	SimpleAnimObject *burnSprite;
	SimpleAnimObject *lightSprite;
	SimpleAnimObject *monkSprite;
	CCRenderTexture *darknessLayer;

	NSMutableArray *bats;
	CCAnimation *batFlyUp;
	CCAnimation *batGlideDown;
}

-(CCLayer*) runRecipe;
-(float)makeBatFlyUp:(SimpleAnimObject*)bat;
-(void)makeBatGlideDown:(SimpleAnimObject*)bat;

@end

//Implementation
@implementation Ch1_ColorMaskLighting

-(CCLayer*) runRecipe {
	//Add our PLISTs to the SpriteFrameCache singleton
	CCSpriteFrameCache * cache = [CCSpriteFrameCache sharedSpriteFrameCache];
	[cache addSpriteFramesWithFile:@"simple_bat.plist"];
	[cache addSpriteFramesWithFile:@"monk_lantern.plist"];
	
	//Add cave background
	CCSprite *caveBg = [CCSprite spriteWithFile:@"cave.png"];
	[caveBg setPosition:ccp(240,160)];
	[self addChild: caveBg z:0 tag:TAG_CAVE_BG];
	
	//Set up the burn sprite that will "knock out" parts of the darkness layer depending on the alpha value of the pixels in the image.
	burnSprite = [SimpleAnimObject spriteWithFile:@"fire.png"];
	burnSprite.position = ccp(50,50);
	burnSprite.scale = 10.0f;
	[burnSprite setBlendFunc: (ccBlendFunc) { GL_ZERO, GL_ONE_MINUS_SRC_ALPHA }];
	[burnSprite retain];
	burnSprite.velocity = ccp(1,0);
			
	//Add a 'light' sprite which additively blends onto the scene. This represents the cone of light created by the monk's candle.
	lightSprite = [SimpleAnimObject spriteWithFile:@"fire.png"];
	lightSprite.position = ccp(50,50);
	lightSprite.scale = 10.0f;
	[lightSprite setColor:ccc3(100,100,50)];
	[lightSprite setBlendFunc: (ccBlendFunc) { GL_ONE, GL_ONE }];
	lightSprite.velocity = ccp(1,0);
	[self addChild:lightSprite z:4 tag:TAG_LIGHT_SPRITE];
		
	//Add the monk
	monkSprite = [[SimpleAnimObject alloc] init];
	monkSprite.position = ccp(50,50);
	monkSprite.velocity = ccp(1,0);
	[self addChild:monkSprite z:1 tag:TAG_MONK];
	
	//Animate the monk to simulate walking.
	CCAnimation *animation = [[CCAnimation alloc] initWithName:@"monk_lantern_walk" delay:0.1f];
	for(int i=1; i<=5; i+=1){
		[animation addFrame:[cache spriteFrameByName:[NSString stringWithFormat:@"monk_lantern_0%i.png",i]]];
	}
	for(int i=4; i>=2; i-=1){
		[animation addFrame:[cache spriteFrameByName:[NSString stringWithFormat:@"monk_lantern_0%i.png",i]]];
	}
	[monkSprite runAction:[CCRepeatForever actionWithAction: [CCAnimate actionWithAnimation:animation]]];
	
	//Add the 'darkness' layer. This simulates darkness in the cave.
	darknessLayer = [CCRenderTexture renderTextureWithWidth:480 height:320];
	darknessLayer.position =  ccp(240,160);
	[self addChild:darknessLayer z:0 tag:TAG_DARKNESS_LAYER];
		
	//Initialize the array of bats.
	bats = [[NSMutableArray alloc] init];
	
	//Add bats using a batch node.
	CCSpriteBatchNode *batch1 = [CCSpriteBatchNode batchNodeWithFile:@"simple_bat.png" capacity:10];
	[self addChild:batch1 z:2 tag:TAG_BATS];
	
	//Make them start flying up.
	for(int x=0; x<30; x++){
		//Create SimpleAnimObject of bat
		SimpleAnimObject *bat = [SimpleAnimObject spriteWithBatchNode:batch1 rect:CGRectMake(0,0,48,48)];
		bat.scale = 0.5f;
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
	float delay = (float)(arc4random()%5+5)/120;
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
		if(arc4random()%50 == 7){
			if(bat.animationType == BAT_GLIDING_DOWN){ [self makeBatFlyUp:bat]; bat.velocity = ccp(bat.velocity.x, -bat.velocity.y); }
			else if(bat.animationType == BAT_FLYING_UP){ [self makeBatGlideDown:bat]; bat.velocity = ccp(bat.velocity.x, -bat.velocity.y); }
		}
		
		//Update bat position based on direction
		bat.position = ccp(bat.position.x + bat.velocity.x, bat.position.y + bat.velocity.y);
	}
	
	
	//Clear the darkness layer for redrawing. Here we clear it to BLACK with 90% opacity.
	[darknessLayer clear:0.0f g:0.0f b:0.0f a:0.9f];
	
	//Begin the darkness layer drawing routine. This transforms to the proper location, among other things.
	[darknessLayer begin];
	
	//Limit drawing to the alpha channel.
	glColorMask(0.0f, 0.0f, 0.0f, 1.0f);
	
	//Draw the burn sprite only on the alpha channel.
	[burnSprite visit];
	
	//Reset glColorMask to allow drawing of colors.
	glColorMask(1.0f, 1.0f, 1.0f, 1.0f);
	
	//Finish transformation.
	[darknessLayer end];
	
	//Make the monk walk back and forth.
	if(monkSprite.position.x > 480){
		monkSprite.flipX = YES;
		burnSprite.velocity = ccp(-1,0);
		lightSprite.velocity = ccp(-1,0);
		monkSprite.velocity = ccp(-1,0);
	}else if(monkSprite.position.x < 0){
		monkSprite.flipX = NO;
		burnSprite.velocity = ccp(1,0);
		lightSprite.velocity = ccp(1,0);
		monkSprite.velocity = ccp(1,0);
	}
	
	//Update our SimpleAnimObjects
	[burnSprite update:delta];
	[lightSprite update:delta];
	[monkSprite update:delta];
}

-(void) cleanRecipe {
	[bats release];
	[super cleanRecipe];
}

@end