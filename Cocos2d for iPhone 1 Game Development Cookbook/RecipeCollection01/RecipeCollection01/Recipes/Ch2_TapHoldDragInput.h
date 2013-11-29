#import "cocos2d.h"
#import "Recipe.h"
#import "ColorTouchSprite.h"

//Interface
@interface Ch2_TapHoldDragInput : Recipe
{
	ColorTouchSprite *colorTouchSprite;
}

-(CCLayer*) runRecipe;
-(void) step;

@end

//Implementation
@implementation Ch2_TapHoldDragInput

-(CCLayer*) runRecipe {
	self.isTouchEnabled = YES;

	//Our message sprite
	message = [CCLabelBMFont labelWithString:@"Tap, hold or drag the square." fntFile:@"eurostile_30.fnt"];
	message.position = ccp(240,260);
	message.scale = 0.75f;
	[self addChild:message];
	
	//Init the ColorTouchSprite
	colorTouchSprite = [ColorTouchSprite spriteWithFile:@"blank.png"];
	colorTouchSprite.position = ccp(240,160);
	[colorTouchSprite setTextureRect:CGRectMake(0,0,100,100)];
	[self addChild:colorTouchSprite];
	
	[self schedule:@selector(step)];
	
	return self;
}

-(void) step {
	//Change color and message depending on ColorTouchSprite state
	if(colorTouchSprite.touchedState == TS_NONE){
		[message setColor:ccc3(255,255,255)];
		[message setString:@"Tap, hold or drag the square."];
		[colorTouchSprite setColor:ccc3(255,255,255)];
	}else if(colorTouchSprite.touchedState == TS_TAP){
		[message setColor:ccc3(255,0,0)];
		[message setString:@"Tap."];
		[colorTouchSprite setColor:ccc3(255,0,0)];
	}else if(colorTouchSprite.touchedState == TS_HOLD){
		[message setColor:ccc3(0,255,0)];
		[message setString:@"Hold."];
		[colorTouchSprite setColor:ccc3(0,255,0)];
	}else if(colorTouchSprite.touchedState == TS_DRAG){
		[message setColor:ccc3(0,0,255)];
		[message setString:@"Drag."];
		[colorTouchSprite setColor:ccc3(0,0,255)];
	}
}

/* Process touch events */
-(void) ccTouchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {	
	UITouch *touch = [touches anyObject];
	CGPoint point = [touch locationInView: [touch view]];
	point = [[CCDirector sharedDirector] convertToGL: point];
	
	if(pointIsInRect(point, [colorTouchSprite rect])){
		[colorTouchSprite ccTouchesBegan:touches withEvent:event];	
	}
}
-(void) ccTouchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {	
	UITouch *touch = [touches anyObject];
	CGPoint point = [touch locationInView: [touch view]];
	point = [[CCDirector sharedDirector] convertToGL: point];
	
	if(pointIsInRect(point, [colorTouchSprite rect])){
		[colorTouchSprite ccTouchesMoved:touches withEvent:event];	
	}
}
-(void) ccTouchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {	
	UITouch *touch = [touches anyObject];
	CGPoint point = [touch locationInView: [touch view]];
	point = [[CCDirector sharedDirector] convertToGL: point];
	
	if(pointIsInRect(point, [colorTouchSprite rect])){
		[colorTouchSprite ccTouchesEnded:touches withEvent:event];	
	}
}

@end