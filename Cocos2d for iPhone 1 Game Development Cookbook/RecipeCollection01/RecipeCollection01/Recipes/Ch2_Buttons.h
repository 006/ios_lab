#import "cocos2d.h"
#import "Recipe.h"
#import "GameButton.h"

//Interface
@interface Ch2_Buttons : Recipe
{
	NSMutableArray *buttons;
}

-(CCLayer*) runRecipe;
-(void) step;
-(void) createButtonWithPosition:(CGPoint)position withUpFrame:(NSString*)upFrame withDownFrame:(NSString*)downFrame withName:(NSString*)name;
-(void) ccTouchesBegan:(NSSet *)touches withEvent:(UIEvent *)event;
-(void) ccTouchesMoved:(NSSet *)touches withEvent:(UIEvent *)event;
-(void) ccTouchesEnded:(NSSet *)touches withEvent:(UIEvent *)event;

@end

//Implementation
@implementation Ch2_Buttons

-(CCLayer*) runRecipe {
	self.isTouchEnabled = YES;

	message = [CCLabelBMFont labelWithString:@"Buttons pressed:" fntFile:@"eurostile_30.fnt"];
	message.position = ccp(240,260);
	message.scale = 0.75f;
	[self addChild:message];

	CGSize s = [[CCDirector sharedDirector] winSize];
	
	CCSpriteFrameCache *cache = [CCSpriteFrameCache sharedSpriteFrameCache];
	
	[cache addSpriteFramesWithFile:@"dpad_buttons.plist"];
		
	buttons = [[NSMutableArray alloc] init];
		
	[self createButtonWithPosition:ccp(350,50) withUpFrame:@"b_button_up.png" withDownFrame:@"b_button_down.png" withName:@"B"];
	[self createButtonWithPosition:ccp(400,100) withUpFrame:@"a_button_up.png" withDownFrame:@"a_button_down.png" withName:@"A"];
	[self createButtonWithPosition:ccp(350,150) withUpFrame:@"x_button_up.png" withDownFrame:@"x_button_down.png" withName:@"X"];
	[self createButtonWithPosition:ccp(300,100) withUpFrame:@"y_button_up.png" withDownFrame:@"y_button_down.png" withName:@"Y"];

	[self schedule:@selector(step)];
	
	return self;
}

-(void) step {
	[message setString:@"Buttons pressed:"];
	for(GameButton *b in buttons){
		if(b.pressed){
			[message setString:[NSString stringWithFormat:@"%@ %@",message.string,b.name]];
		}
	}
}

-(void) createButtonWithPosition:(CGPoint)position withUpFrame:(NSString*)upFrame withDownFrame:(NSString*)downFrame withName:(NSString*)name {
	CCSpriteFrameCache *cache = [CCSpriteFrameCache sharedSpriteFrameCache];
	
	GameButton *button = [[GameButton alloc] init];
	button.position = position;
	[button setUpSpriteFrame:upFrame];
	[button setDownSpriteFrame:downFrame];
	[button setDisplayFrame:[cache spriteFrameByName:[button upSpriteFrame]]];
	button.name = name;
	[self addChild:button];
	[buttons addObject:button];
}

-(void) ccTouchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {	
	UITouch *touch = [touches anyObject];
    CGPoint point = [touch locationInView: [touch view]];
	point = [[CCDirector sharedDirector] convertToGL: point];
	
	for(GameButton *b in buttons){
		[b ccTouchesBegan:touches withEvent:event];
	}
}
-(void) ccTouchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {	
	UITouch *touch = [touches anyObject];
    CGPoint point = [touch locationInView: [touch view]];
	point = [[CCDirector sharedDirector] convertToGL: point];

	for(GameButton *b in buttons){
		[b ccTouchesMoved:touches withEvent:event];
	}
}
-(void) ccTouchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {	
	UITouch *touch = [touches anyObject];
    CGPoint point = [touch locationInView: [touch view]];
	point = [[CCDirector sharedDirector] convertToGL: point];

	for(GameButton *b in buttons){
		[b ccTouchesEnded:touches withEvent:event];
	}
}

-(void) cleanRecipe {
	[buttons release];
	[super cleanRecipe];
}

@end