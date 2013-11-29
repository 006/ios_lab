#import "Recipe.h"
#import "ShadowLabel.h"

@interface Ch5_ShadowedLabels : Recipe
{
}

-(CCLayer*) runRecipe;
-(void) label:(NSString*)s at:(CGPoint)p color:(ccColor3B)col activeColor:(ccColor3B)activeCol selector:(SEL)sel tag:(int)tag;
-(void) labelTouched:(id)sender;

@end

@implementation Ch5_ShadowedLabels

-(CCLayer*) runRecipe {
	[super runRecipe];
	
	/* Draw four different shadowed labels using 4 different fonts */
	[CCMenuItemFont setFontSize:47];
	[CCMenuItemFont setFontName:@"Georgia"];
	[self label:@"Label 1" at:ccp(-120,50) color:ccc3(0,50,255) activeColor:ccc3(0,200,255) selector:@selector(labelTouched:) tag:1];

	[CCMenuItemFont setFontSize:40];
	[CCMenuItemFont setFontName:@"Marker Felt"];
	[self label:@"Label 2" at:ccp(120,50) color:ccc3(255,128,0) activeColor:ccc3(255,255,0) selector:@selector(labelTouched:) tag:2];
	
	[CCMenuItemFont setFontSize:45];
	[CCMenuItemFont setFontName:@"Arial"];
	[self label:@"Label 3" at:ccp(-120,-50) color:ccc3(0,128,0) activeColor:ccc3(0,255,0) selector:@selector(labelTouched:) tag:3];
	
	[CCMenuItemFont setFontSize:50];
	[CCMenuItemFont setFontName:@"Courier New"];
	[self label:@"Label 4" at:ccp(120,-50) color:ccc3(255,0,0) activeColor:ccc3(255,255,0) selector:@selector(labelTouched:) tag:4];

	return self;
}

//Label creation helper method
-(void) label:(NSString*)s at:(CGPoint)p color:(ccColor3B)col activeColor:(ccColor3B)activeCol selector:(SEL)sel tag:(int)tag {
	ShadowLabel *label = [ShadowLabel labelFromString:s target:self selector:sel];
	label.position = p;
	label.color = col;
	label.activeColor = activeCol;
	label.tag = tag;
	
	CCMenu *menu = [CCMenu menuWithItems: label.shadow, label, nil];
	[self addChild:menu];
}

//Label touch callback
-(void) labelTouched:(id)sender {
	ShadowLabel *label = (ShadowLabel*)sender;
	[self showMessage:[NSString stringWithFormat:@"Pressed label %d",label.tag]];
}

@end
