#import "Recipe.h"
#import "CCUIViewWrapper.h"

@interface Ch5_WrappingUIKit : Recipe
{
}

-(CCLayer*) runRecipe;
-(void) buttonTapped:(id)sender;
-(void) addSpinningButton;
-(void) addScrollView;

@end

@implementation Ch5_WrappingUIKit

-(CCLayer*) runRecipe {
	[super runRecipe];
    
	[self addSpinningButton];
	
	[self addScrollView];
	
	return self;
}

-(void) addSpinningButton {
	//Label
	CCLabelBMFont *label = [CCLabelBMFont labelWithString:@"UIButton" fntFile:@"eurostile_30.fnt"];
	label.position = ccp(350,220);
	label.scale = 0.75f;
	[label setColor:ccc3(255,255,255)];
	[self addChild:label z:10];

	//Our UIButton example
	UIButton *button = [UIButton buttonWithType:UIButtonTypeRoundedRect];
	[button addTarget:self action:@selector(buttonTapped:) forControlEvents:UIControlEventTouchDown];
	[button setTitle:@"Touch Me!" forState:UIControlStateNormal];
	button.frame = CGRectMake(0.0, 0.0, 120.0, 40.0);

	//Wrap the UIButton using CCUIViewWrapper
	CCUIViewWrapper *wrapper = [CCUIViewWrapper wrapperForUIView:button];
	[self addChild:wrapper];
	wrapper.position = ccp(90,140);
	[wrapper runAction:[CCRepeatForever actionWithAction:[CCRotateBy actionWithDuration:5.0f angle:360]]];
}

-(void) addScrollView {
	//Label
	CCLabelBMFont *label = [CCLabelBMFont labelWithString:@"UIScrollView" fntFile:@"eurostile_30.fnt"];
	label.position = ccp(100,220);
	label.scale = 0.75f;
	[label setColor:ccc3(255,255,255)];
	[self addChild:label z:10];	

	//Create a simple UIScrollView with colored UIViews
	CGPoint viewSize = ccp(200.0f,100.0f);
	CGPoint nodeSize = ccp(200.0f,50.0f);
	int nodeCount = 10;

	//Init scrollview
	UIScrollView *scrollview = [[UIScrollView alloc] initWithFrame: CGRectMake(0, 0, viewSize.x, viewSize.y)];

	//Add nodes
	for (int i = 0; i <nodeCount; i++){ 
		CGFloat y = i * nodeSize.y; 
		UIView *view = [[UIView alloc] initWithFrame:CGRectMake(0, y, nodeSize.x, nodeSize.y)];
		view.backgroundColor = [UIColor colorWithRed:(CGFloat)random()/(CGFloat)RAND_MAX green:(CGFloat)random()/(CGFloat)RAND_MAX blue:(CGFloat)random()/(CGFloat)RAND_MAX alpha:1.0];
		[scrollview addSubview:view]; 
		[view release]; 
	}
	scrollview.contentSize = CGSizeMake(viewSize.x, viewSize.y * nodeCount/2); 

	//Wrap the UIScrollView object using CCUIViewWrapper
	CCUIViewWrapper *wrapper = [CCUIViewWrapper wrapperForUIView:scrollview];
	[self addChild:wrapper];
	wrapper.rotation = -90;
	wrapper.position = ccp(50,400);
}

-(void) buttonTapped:(id)sender {
	[self showMessage:@"Button tapped"];
}

@end
