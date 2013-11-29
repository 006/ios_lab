#import "SideScrollerRecipe.h"

@interface Ch4_CharacterMovement : SideScrollerRecipe
{
}

-(CCLayer*) runRecipe;
-(void) step:(ccTime)delta;

@end

//Implementation
@implementation Ch4_CharacterMovement

-(CCLayer*) runRecipe {
	[super runRecipe];
	[message setString:@"Press 'A' to jump."];
	
	//Create buttons
	[self createButtonWithPosition:ccp(420,75) withUpFrame:@"a_button_up.png" withDownFrame:@"a_button_down.png" withName:@"A"];

	//Add some boxes
	for(float x=0; x<480; x+=40){
		[self addBoxWithPosition:ccp(x,40) file:@"bricks.png" density:5.0f];
	}
	for(int i=0; i<12; i++){
		float x = arc4random()%480;
		float y = arc4random()%200+120;
		
		[self addBoxWithPosition:ccp(x,y) file:@"crate2.png" density:1.0f];
	}
	
	return self;
}

-(void) step:(ccTime)delta {
	[super step:delta];

	//Process input for the A button
	for(id b in buttons){
		GameButton *button = (GameButton*)b;
		if(button.pressed && [button.name isEqualToString:@"A"]){
			[self processJump];
		}else{
			jumpCounter = -10.0f;
		}
	}
}

@end