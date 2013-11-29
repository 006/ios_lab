#import "Recipe.h"

@interface DrawLayer : CCLayer
{
	Recipe *recipe;
}

@property (readwrite, assign) Recipe *recipe;

-(void) draw;

@end

@implementation DrawLayer

@synthesize recipe;

-(void) draw {
	[recipe drawLayer];
}

@end