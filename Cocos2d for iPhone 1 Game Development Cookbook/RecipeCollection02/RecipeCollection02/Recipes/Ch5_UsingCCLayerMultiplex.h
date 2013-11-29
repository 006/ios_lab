#import "Recipe.h"

// MultiplexLayerMenu
// Our switchable layer class
@interface MultiplexLayerMenu : CCLayer {
}

+(id) layerWithLayerNumber:(int)layerNumber;
-(id) initWithLayerNumber:(int)layerNumber;
-(void) goToLayer:(id)sender;

@end

@implementation MultiplexLayerMenu

+(id) layerWithLayerNumber:(int)layerNumber {
	return [[MultiplexLayerMenu alloc] initWithLayerNumber:layerNumber];
}

-(id) initWithLayerNumber:(int)layerNumber {
	if( (self=[super init] )) {	
		//Random background color
		CCSprite *bg = [CCSprite spriteWithFile:@"blank.png"];
		bg.position = ccp(240,160);
		[bg setTextureRect:CGRectMake(0,0,480,320)];
		[bg setColor:ccc3(arc4random()%150,arc4random()%150,arc4random()%150)];
		[self addChild:bg];
	
		//Layer number as message
		CCLabelBMFont *message = [CCLabelBMFont labelWithString:[NSString stringWithFormat:@"Layer %i",layerNumber+1] fntFile:@"eurostile_30.fnt"];
		message.position = ccp(160,270);
		message.scale = 0.75f;
		[message setColor:ccc3(255,255,255)];
		[self addChild:message z:10];		

		//Buttons to go to different layers
		CCMenuItemFont *goToLayer1 = [CCMenuItemFont itemFromString:@"Go To Layer 1" target:self selector:@selector(goToLayer:)];
		CCMenuItemFont *goToLayer2 = [CCMenuItemFont itemFromString:@"Go To Layer 2" target:self selector:@selector(goToLayer:)];
		CCMenuItemFont *goToLayer3 = [CCMenuItemFont itemFromString:@"Go To Layer 3" target:self selector:@selector(goToLayer:)];
		goToLayer1.tag = 0; goToLayer2.tag = 1; goToLayer3.tag = 2;
		
		//Add menu items
		CCMenu *menu = [CCMenu menuWithItems: goToLayer1, goToLayer2, goToLayer3, nil];
		[menu alignItemsVertically];
		[self addChild:menu];	
	}
	return self;
}

//Switch to a different layer
-(void) goToLayer:(id)sender {
	CCMenuItemFont *item = (CCMenuItemFont*)sender;
	[(CCLayerMultiplex*)parent_ switchTo:item.tag];
}

@end


@interface Ch5_UsingCCLayerMultiplex : Recipe
{
}

-(CCLayer*) runRecipe;

@end

@implementation Ch5_UsingCCLayerMultiplex

-(CCLayer*) runRecipe {
	[super runRecipe];
    
	//Create our multiplex layer with three MultiplexLayerMenu objects
	CCLayerMultiplex *layer = [CCLayerMultiplex layerWithLayers: [MultiplexLayerMenu layerWithLayerNumber:0], [MultiplexLayerMenu layerWithLayerNumber:1], 
		[MultiplexLayerMenu layerWithLayerNumber:2], nil];
	[self addChild: layer z:0];	
	
	return self;
}

@end
