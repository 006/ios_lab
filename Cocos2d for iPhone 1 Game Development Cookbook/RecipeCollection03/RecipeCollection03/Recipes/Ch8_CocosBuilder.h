#import "Recipe.h"
#import "CCBReader.h"

@interface Ch8_CocosBuilder : Recipe
{}

-(CCLayer*) runRecipe;
-(void) pushScene;
-(void) back;

@end

//Implementation
@implementation Ch8_CocosBuilder

-(CCLayer*) runRecipe {
	//Superclass initialization and message
	[super runRecipe];
	[self showMessage:@"Press the button below to load a \nCocosBuilder created scene."];
	
	//Add button to push CocosBuilder scene
	[CCMenuItemFont setFontSize:32];
	CCMenuItemFont *pushItem = [CCMenuItemFont itemFromString:@"Push CocosBuilder Scene" target:self selector:@selector(pushScene)];
	CCMenu *pushMenu = [CCMenu menuWithItems:pushItem, nil];
	pushMenu.position = ccp(240,160);
	[self addChild:pushMenu];
	
	return self;
}

/* Push scene callback */
-(void) pushScene {
	CCScene* scene = [CCBReader sceneWithNodeGraphFromFile:@"scene.ccb" owner:self];
	[[CCDirector sharedDirector] pushScene:scene];
}

/* Callback called from CocosBuilder scene */
-(void) back {
	[[CCDirector sharedDirector] popScene];
}

@end
