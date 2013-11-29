#import "cocos2d.h"
#import "Recipe.h"
#import "AnalogStick.h"


//Interface
@interface Ch2_AnalogStick : Recipe
{
	AnalogStick *analogStick;
	SimpleAnimObject *gunman;
	int gunmanDirection;
}

-(CCLayer*) runRecipe;
-(void) step:(ccTime)delta;
-(void) animateGunman;
-(void) ccTouchesBegan:(NSSet *)touches withEvent:(UIEvent *)event;
-(void) ccTouchesMoved:(NSSet *)touches withEvent:(UIEvent *)event;
-(void) ccTouchesEnded:(NSSet *)touches withEvent:(UIEvent *)event;

@end

//Implementation
@implementation Ch2_AnalogStick

-(CCLayer*) runRecipe {
	self.isTouchEnabled = YES;

	CCSpriteFrameCache *cache = [CCSpriteFrameCache sharedSpriteFrameCache];
	[cache addSpriteFramesWithFile:@"gunman.plist"];

	//Initialize gunman
	gunman = [SimpleAnimObject spriteWithSpriteFrame:[cache spriteFrameByName:@"gunman_stand_down.png"]];
	gunman.position = ccp(240,160);
	[self addChild:gunman];
	gunman.velocity = ccp(0,0);
	gunmanDirection = AS_DOWN;

	message = [CCLabelBMFont labelWithString:@"Analog Vector:" fntFile:@"eurostile_30.fnt"];
	message.position = ccp(240,260);
	message.scale = 0.75f;
	[self addChild:message];
		
	//Initialize analog stick
	[cache addSpriteFramesWithFile:@"analog_stick.plist"];
	analogStick = [[AnalogStick alloc] init];
	analogStick.position = ccp(100,100);
	[self addChild:analogStick];

	[self schedule:@selector(step:)];
	
	//This sets off a chain reaction.
	[self animateGunman];
	
	return self;
}

-(void) step:(ccTime)delta {
	[message setString:@"Analog Vector:"];
	[message setString:[NSString stringWithFormat:@"%@ %f %f",message.string,analogStick.pressedVector.x,analogStick.pressedVector.y]];

	if(analogStick.direction != AS_NO_DIRECTION){ 
		if(analogStick.direction != gunmanDirection){
			[gunman stopAllActions];
			gunmanDirection = analogStick.direction;
			[self animateGunman];
		}
	}
	gunman.velocity = ccp(analogStick.pressedVector.x*4, analogStick.pressedVector.y*4);	
	
	[gunman update:delta];
}

-(void) animateGunman {
	CCSpriteFrameCache *cache = [CCSpriteFrameCache sharedSpriteFrameCache];
	
	float speed = [analogStick magnitude];
	
	//Animation delay is inverse speed
	float delay = 0.075f/speed;
	if(delay > 0.5f){ delay = 0.5f; }
	CCAnimation *animation = [[CCAnimation alloc] initWithName:@"gunman_anim" delay:delay];	

	NSString *direction;
	bool flipX = NO;
	bool moving = YES;
	if(gunman.velocity.x == 0 && gunman.velocity.y == 0){ moving = NO; }
	
	if(gunmanDirection == AS_LEFT){ direction = @"right"; flipX = YES; }
	else if(gunmanDirection == AS_UP_LEFT){ direction = @"up_right"; flipX = YES; }
	else if(gunmanDirection == AS_UP){ direction = @"up"; }
	else if(gunmanDirection == AS_UP_RIGHT){ direction = @"up_right"; }
	else if(gunmanDirection == AS_RIGHT){ direction = @"right"; }
	else if(gunmanDirection == AS_DOWN_RIGHT){ direction = @"down_right"; }
	else if(gunmanDirection == AS_DOWN){ direction = @"down"; }
	else if(gunmanDirection == AS_DOWN_LEFT){ direction = @"down_right"; flipX = YES; }

	if(moving){
		[animation addFrame:[cache spriteFrameByName:[NSString stringWithFormat:@"gunman_run_%@_01.png",direction]]];
		[animation addFrame:[cache spriteFrameByName:[NSString stringWithFormat:@"gunman_stand_%@.png",direction]]];
		[animation addFrame:[cache spriteFrameByName:[NSString stringWithFormat:@"gunman_run_%@_02.png",direction]]];
		[animation addFrame:[cache spriteFrameByName:[NSString stringWithFormat:@"gunman_stand_%@.png",direction]]];
	}else{
		[animation addFrame:[cache spriteFrameByName:[NSString stringWithFormat:@"gunman_stand_%@.png",direction]]];
	}
	
	gunman.flipX = flipX;
	
	//animateGunman calls itself indefinitely
	[gunman runAction:[CCSequence actions: [CCAnimate actionWithAnimation:animation],
		[CCCallFunc actionWithTarget:self selector:@selector(animateGunman)], nil ]];
}

-(void) ccTouchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {	
	UITouch *touch = [touches anyObject];
    CGPoint point = [touch locationInView: [touch view]];
	point = [[CCDirector sharedDirector] convertToGL: point];
	
	[analogStick ccTouchesBegan:touches withEvent:event];
}
-(void) ccTouchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {	
	UITouch *touch = [touches anyObject];
    CGPoint point = [touch locationInView: [touch view]];
	point = [[CCDirector sharedDirector] convertToGL: point];

	[analogStick ccTouchesMoved:touches withEvent:event];
}
-(void) ccTouchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {	
	UITouch *touch = [touches anyObject];
    CGPoint point = [touch locationInView: [touch view]];
	point = [[CCDirector sharedDirector] convertToGL: point];

	[analogStick ccTouchesEnded:touches withEvent:event];
}

@end