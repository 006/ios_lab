#import "cocos2d.h"
#import "Recipe.h"
#import "ColorTouchSprite.h"

//Interface
@interface Ch2_DepthTestingInput : Recipe
{
	NSMutableArray *sprites;
}

-(CCLayer*) runRecipe;
-(void) initSprites;

@end

//Implementation
@implementation Ch2_DepthTestingInput

-(CCLayer*) runRecipe {
	self.isTouchEnabled = YES;

	//Our message sprite
	message = [CCLabelBMFont labelWithString:@"Drag the sprites." fntFile:@"eurostile_30.fnt"];
	message.position = ccp(240,260);
	message.scale = 0.75f;
	[self addChild:message];
	
	//Init the ColorTouchSprites
	[self initSprites];
		
	return self;
}

-(void) initSprites {
	sprites = [[NSMutableArray alloc] init];

	//We add 10 randomly colored sprites 
	for(int x=0; x<10; x++){
		CCSprite *sprite = [ColorTouchSprite spriteWithFile:@"blank.png"];
		sprite.position = ccp(x*30+100,x*15+75);
		[sprite setTextureRect:CGRectMake(0,0,75,75)];
		sprite.color = ccc3(arc4random()%255, arc4random()%255, arc4random()%255);
		[self addChild:sprite z:11-x];	
		[sprites addObject:sprite];
	}
}

/* Process touch events */
-(void) ccTouchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {	
	UITouch *touch = [touches anyObject];
	CGPoint point = [touch locationInView: [touch view]];
	point = [[CCDirector sharedDirector] convertToGL: point];
	
	//Process input for all sprites
	for(id sprite in sprites){
		if(pointIsInRect(point, [sprite rect])){
			//Swallow the input
			[sprite ccTouchesBegan:touches withEvent:event];
			return;
		}
	}
}
-(void) ccTouchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {	
	UITouch *touch = [touches anyObject];
	CGPoint point = [touch locationInView: [touch view]];
	point = [[CCDirector sharedDirector] convertToGL: point];
	
	//Process input for all sprites
	for(id sprite in sprites){
		if(pointIsInRect(point, [sprite rect])){
			[sprite ccTouchesMoved:touches withEvent:event];
		}
	}
}
-(void) ccTouchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {	
	UITouch *touch = [touches anyObject];
	CGPoint point = [touch locationInView: [touch view]];
	point = [[CCDirector sharedDirector] convertToGL: point];
	
	//Process input for all sprites
	for(id sprite in sprites){
		//End all input when you lift up your finger
		[sprite ccTouchesEnded:touches withEvent:event];
	}
}

-(void) cleanRecipe {
	[sprites release];
	[super cleanRecipe];
}

@end