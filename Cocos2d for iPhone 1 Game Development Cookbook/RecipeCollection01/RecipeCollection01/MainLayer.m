#import "MainLayer.h"
#import "Helpers.h"
#import "Includes.h"

enum {
	TAG_RECIPE = 0,
	TAG_RECIPE_NAME = 1,
	TAG_NEXT_BUTTON = 2,
	TAG_PREV_BUTTON = 3,
	TAG_BG = 4
};

enum {
	Z_BG = 0,
	Z_RECIPE = 1,
	Z_HUD = 2
};

@implementation MainLayer

+(id) scene
{
	CCScene *scene = [CCScene node];
	MainLayer *layer = [MainLayer node];
	[scene addChild:layer];
	return scene;
}

-(id) init
{
	if( (self=[super init] ))
    {
		//Initialization
		currentRecipe = 0;
		recipes = [[NSMutableDictionary alloc] init];		
		[self loadRecipes];
		
		//Add background
		[self addBackground];
		
		//Run current recipe
		[self addRecipe];
		
		//Show buttons
		[self addButtons];
	}
	return self;
}

- (void) dealloc
{
	[recipes release];
	[super dealloc];
}


-(void) loadRecipes
{
	numRecipes = sizeof(recipeNames)/4;
	
	for(int i=0; i<numRecipes; i+=1)
    {
		[recipes setObject:[[NSClassFromString(recipeNames[i]) alloc] init] forKey:recipeNames[i]];
	}
}

-(void) addButtons
{
	CGSize size = [[CCDirector sharedDirector] winSize];
	
	CCMenuItemFont* prev = [CCMenuItemFont itemFromString:@"Prev" target:self selector:@selector(prevCallback:)];
	CCMenu *prevMenu = [CCMenu menuWithItems:prev, nil];
    prevMenu.position = ccp( 40 , 20 );
    [self addChild:prevMenu z:Z_HUD tag:TAG_PREV_BUTTON];	
	
	CCMenuItemFont* next = [CCMenuItemFont itemFromString:@"Next" target:self selector:@selector(nextCallback:)];
	CCMenu *nextMenu = [CCMenu menuWithItems:next, nil];
    nextMenu.position = ccp( size.width-40 , 20 );
    [self addChild:nextMenu z:Z_HUD tag:TAG_NEXT_BUTTON];
}

-(void) prevCallback:(id)sender {
	[self removeRecipe];

	currentRecipe -= 1;
	if(currentRecipe < 0){
		currentRecipe = numRecipes-1;
	}
	
	[self addRecipe];
}

-(void) nextCallback:(id)sender {
	[self removeRecipe];
	
	currentRecipe += 1;
	if(currentRecipe >= numRecipes){
		currentRecipe = 0;
	}
	
	[self addRecipe];
}

-(void) removeRecipe {
	[[self getChildByTag:TAG_RECIPE] cleanRecipe];

	[self removeChildByTag:TAG_RECIPE cleanup:YES];
	[self removeChildByTag:TAG_RECIPE_NAME cleanup:YES];
}

-(void) addRecipe {
	[self addChild: [[recipes objectForKey:recipeNames[currentRecipe]] runRecipe] z:Z_RECIPE tag:TAG_RECIPE ];
	CCLabelTTF *name = [CCLabelTTF labelWithString:recipeNames[currentRecipe] fontName:@"Marker Felt" fontSize:24];
	name.position = ccp(10+name.textureRect.size.width/2,300);
	[self addChild:name z:Z_HUD tag:TAG_RECIPE_NAME];
}

-(void) addBackground {
	CGSize size = [[CCDirector sharedDirector] winSize];
	CCSprite *bg = [CCSprite spriteWithFile:@"blank.png"];
	bg.position = ccp(size.width/2,size.height/2);
	[bg setTextureRect:CGRectMake(0, 0, size.width, size.height)];
	bg.color = ccc3(100,100,100);
	[self addChild:bg z:Z_BG tag:TAG_BG];
}

@end

