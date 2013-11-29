#import "cocos2d.h"
#import "Recipe.h"
#import "DPad.h"


//Interface
@interface Ch2_DPad : Recipe
{
	DPad *dPad;
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
@implementation Ch2_DPad

-(CCLayer*) runRecipe {
	self.isTouchEnabled = YES;

	//Add gunman sprites
	CCSpriteFrameCache *cache = [CCSpriteFrameCache sharedSpriteFrameCache];
	[cache addSpriteFramesWithFile:@"gunman.plist"];

	//Initialize gunman
	gunman = [SimpleAnimObject spriteWithSpriteFrame:[cache spriteFrameByName:@"gunman_stand_down.png"]];
	gunman.position = ccp(240,160);
	[self addChild:gunman];
	gunmanDirection = DPAD_DOWN;

	//Initialize message
	message = [CCLabelBMFont labelWithString:@"DPad Vector:" fntFile:@"eurostile_30.fnt"];
	message.position = ccp(240,260);
	message.scale = 0.75f;
	[self addChild:message];
		
	//Initialize DPad
	[cache addSpriteFramesWithFile:@"dpad_buttons.plist"];
	dPad = [[DPad alloc] init];
	dPad.position = ccp(100,100);
	[self addChild:dPad];

	[self schedule:@selector(step:)];
	
	return self;
}

-(void) step:(ccTime)delta {
	//Tell the user our DPad vector
	[message setString:@"DPad Vector:"];
	[message setString:[NSString stringWithFormat:@"%@ %f %f",message.string,dPad.pressedVector.x,dPad.pressedVector.y]];
	
	bool resetAnimation = NO;
	
	//We reset the animation if the gunman changes direction
	if(dPad.direction != DPAD_NO_DIRECTION){ 
		if(gunmanDirection != dPad.direction){
			resetAnimation = YES;
			gunmanDirection = dPad.direction;
		}
	}
	if(gunman.velocity.x != dPad.pressedVector.x*2 || gunman.velocity.y != dPad.pressedVector.y*2){
		gunman.velocity = ccp(dPad.pressedVector.x*2, dPad.pressedVector.y*2);
		resetAnimation = YES;
	}
	
	//Update gunman position
	[gunman update:delta];

	//Re-animate if necessary
	if(resetAnimation){
		[self animateGunman];
	}
}
-(void) animateGunman {
 	CCSpriteFrameCache *cache = [CCSpriteFrameCache sharedSpriteFrameCache];

	/* Animate our gunman */
	CCAnimation *animation = [[CCAnimation alloc] initWithName:@"gunman_anim" delay:0.15f];	

	NSString *direction;
	bool flipX = NO;
	bool moving = YES;
	if(gunman.velocity.x == 0 && gunman.velocity.y == 0){ moving = NO; }
	
	if(gunmanDirection == DPAD_LEFT){ direction = @"right"; flipX = YES; }
	else if(gunmanDirection == DPAD_UP_LEFT){ direction = @"up_right"; flipX = YES; }
	else if(gunmanDirection == DPAD_UP){ direction = @"up"; }
	else if(gunmanDirection == DPAD_UP_RIGHT){ direction = @"up_right"; }
	else if(gunmanDirection == DPAD_RIGHT){ direction = @"right"; }
	else if(gunmanDirection == DPAD_DOWN_RIGHT){ direction = @"down_right"; }
	else if(gunmanDirection == DPAD_DOWN){ direction = @"down"; }
	else if(gunmanDirection == DPAD_DOWN_LEFT){ direction = @"down_right"; flipX = YES; }

	//Our simple running loop
	if(moving){
		[animation addFrame:[cache spriteFrameByName:[NSString stringWithFormat:@"gunman_run_%@_01.png",direction]]];
		[animation addFrame:[cache spriteFrameByName:[NSString stringWithFormat:@"gunman_stand_%@.png",direction]]];
		[animation addFrame:[cache spriteFrameByName:[NSString stringWithFormat:@"gunman_run_%@_02.png",direction]]];
		[animation addFrame:[cache spriteFrameByName:[NSString stringWithFormat:@"gunman_stand_%@.png",direction]]];
	}else{
		[animation addFrame:[cache spriteFrameByName:[NSString stringWithFormat:@"gunman_stand_%@.png",direction]]];
	}
	
	gunman.flipX = flipX;
	[gunman runAction:[CCRepeatForever actionWithAction: [CCAnimate actionWithAnimation:animation]]];
}

/* Process touches */
-(void) ccTouchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {	
	UITouch *touch = [touches anyObject];
    CGPoint point = [touch locationInView: [touch view]];
	point = [[CCDirector sharedDirector] convertToGL: point];
	
	[dPad ccTouchesBegan:touches withEvent:event];
}
-(void) ccTouchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {	
	UITouch *touch = [touches anyObject];
    CGPoint point = [touch locationInView: [touch view]];
	point = [[CCDirector sharedDirector] convertToGL: point];

	[dPad ccTouchesMoved:touches withEvent:event];
}
-(void) ccTouchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {	
	UITouch *touch = [touches anyObject];
    CGPoint point = [touch locationInView: [touch view]];
	point = [[CCDirector sharedDirector] convertToGL: point];

	[dPad ccTouchesEnded:touches withEvent:event];
}

@end