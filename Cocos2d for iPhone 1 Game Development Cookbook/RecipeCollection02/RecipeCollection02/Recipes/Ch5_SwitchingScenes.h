#import "Recipe.h"

// TreeSceneMenu
// The node for our binary tree of scenes
@interface TreeSceneMenu : CCLayer {
	NSString *name;
}

+(id) sceneWithString:(NSString*)str;
-(id) initWithString:(NSString*)str;
-(void) goToScene1:(id)sender;
-(void) goToScene2:(id)sender;
-(void) back:(id)sender;

@end

@implementation TreeSceneMenu

+(id) sceneWithString:(NSString*)str {
	//Initialize our scene
	CCScene *s = [CCScene node];	
	TreeSceneMenu *node = [[TreeSceneMenu alloc] initWithString:str];
	[s addChild:node z:0 tag:0];
	return s;
}

-(id) initWithString:(NSString*)str {
	if( (self=[super init] )) {	
		//Set scene name
		name = [NSString stringWithFormat:@"%@",str];
		[name retain];
	
		//Font size/name
		[CCMenuItemFont setFontSize:30];
		[CCMenuItemFont setFontName:@"Marker Felt"];
	
		//Random background color
		CCSprite *bg = [CCSprite spriteWithFile:@"blank.png"];
		bg.position = ccp(240,160);
		[bg setTextureRect:CGRectMake(0,0,480,320)];
		[bg setColor:ccc3(arc4random()%150,arc4random()%150,arc4random()%150)];
		[self addChild:bg];
	
		//Buttons to push new scenes onto the stack
		CCMenuItemFont *scene1Item = [CCMenuItemFont itemFromString:[NSString stringWithFormat:@"Scene %@.1",name] target:self selector:@selector(goToScene1:)];
		CCMenuItemFont *scene2Item = [CCMenuItemFont itemFromString:[NSString stringWithFormat:@"Scene %@.2",name] target:self selector:@selector(goToScene2:)];
		
		//If we are at the root we "Quit" instead of going "Back"
		NSString *backStr = @"Back";
		if([str isEqualToString:@"1"]){
			backStr = @"Quit";
		}
		CCMenuItemFont *backItem = [CCMenuItemFont itemFromString:backStr target:self selector:@selector(back:)];
		
		//Add menu items
		CCMenu *menu = [CCMenu menuWithItems: scene1Item, scene2Item, backItem, nil];
		[menu alignItemsVertically];
		[self addChild:menu];	
	}
	return self;
}

//Push scene 1
-(void) goToScene1:(id)sender {
	[[CCDirector sharedDirector] pushScene:[TreeSceneMenu sceneWithString:[NSString stringWithFormat:@"%@.1",name]]]; 
}

//Push scene 2
-(void) goToScene2:(id)sender {
	[[CCDirector sharedDirector] pushScene:[TreeSceneMenu sceneWithString:[NSString stringWithFormat:@"%@.2",name]]]; 
}

//Pop scene
-(void) back:(id)sender {
	[[CCDirector sharedDirector] popScene];
}

@end


//Our Base Recipe
@interface Ch5_SwitchingScenes : Recipe
{
}

-(CCLayer*) runRecipe;
-(void) goToScene1:(id)sender;

@end

@implementation Ch5_SwitchingScenes

-(CCLayer*) runRecipe {
	[super runRecipe];
    
	//Go to our initial scene
	CCMenuItemFont *goToScene1 = [CCMenuItemFont itemFromString:@"Go To Scene 1" target:self selector:@selector(goToScene1:)];

	CCMenu *menu = [CCMenu menuWithItems: goToScene1, nil];
	[menu alignItemsVertically];
	[self addChild:menu];

	return self;
}

//Push initial scene
-(void) goToScene1:(id)sender {
	[[CCDirector sharedDirector] pushScene:[TreeSceneMenu sceneWithString:@"1"]];
}

@end
