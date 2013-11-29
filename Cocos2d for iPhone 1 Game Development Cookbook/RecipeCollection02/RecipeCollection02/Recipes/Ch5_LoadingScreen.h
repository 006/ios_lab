#import "Recipe.h"
#import "CJSONDeserializer.h"
#import "ActualPath.h"

@interface GameScene : CCLayer {
}

+(id) sceneWithLevel:(NSString*)str;
-(id) initWithLevel:(NSString*)str;
-(void) loadLevel:(NSString*)str;
-(void) processSpriteFile:(NSDictionary*)node;
-(void) quit:(id)sender;

@end

/* The actual 'game' class where we display the textures we loaded asynchronously */
@implementation GameScene

+(id) sceneWithLevel:(NSString*)str {
	//Create our scene
	CCScene *s = [CCScene node];	
	GameScene *node = [[GameScene alloc] initWithLevel:str];
	[s addChild:node z:0 tag:0];
	return s;
}

-(id) initWithLevel:(NSString*)str {
	if( (self=[super init] )) {	
		//Load our level
		[self loadLevel:str];

		//Random colored background
		CCSprite *bg = [CCSprite spriteWithFile:@"blank.png"];
		bg.position = ccp(240,160);
		[bg setTextureRect:CGRectMake(0,0,480,320)];
		[bg setColor:ccc3(arc4random()%150,arc4random()%150,arc4random()%150)];
		[self addChild:bg z:-1];
		
		//Create a label to indicate that this is the loaded level
		CCLabelBMFont *label = [CCLabelBMFont labelWithString:@"The Loaded Level:" fntFile:@"eurostile_30.fnt"];
		label.position = ccp(160,300);
		label.scale = 0.75f;
		[label setColor:ccc3(255,255,255)];
		[self addChild:label z:10];
		
		//Quit button
		CCMenuItemFont *quitItem = [CCMenuItemFont itemFromString:@"Quit" target:self selector:@selector(quit:)];
		CCMenu *menu = [CCMenu menuWithItems: quitItem, nil];
		menu.position = ccp(430,300);
		[self addChild:menu z:10];
	}
	return self;
}

//Quit callback
-(void) quit:(id)sender {
	[[CCDirector sharedDirector] popScene];
	
	//Clear all loaded textures (including ones from other recipes)
	[[CCTextureCache sharedTextureCache] removeAllTextures];
}

//Load level file and process sprites
-(void) loadLevel:(NSString*)str {
	NSString *jsonString = [[NSString alloc] initWithContentsOfFile:getActualPath(str) encoding:NSUTF8StringEncoding error:nil];
	NSData *jsonData = [jsonString dataUsingEncoding:NSUTF32BigEndianStringEncoding];
	NSDictionary *dict = [[CJSONDeserializer deserializer] deserializeAsDictionary:jsonData error:nil];

	NSArray *nodes = [dict objectForKey:@"nodes"];
	for (id node in nodes) {
		if([[node objectForKey:@"type"] isEqualToString:@"spriteFile"]){
			[self processSpriteFile:node];
		}
	}
}

-(void) processSpriteFile:(NSDictionary*)node {
	//Init the sprite
	NSString *file = [node objectForKey:@"file"];
	CCSprite *sprite = [CCSprite spriteWithFile:file];
	
	//Set sprite position
	sprite.position = ccp(arc4random()%480, arc4random()%200);
	
	//Each numeric value is an NSString or NSNumber that must be cast into a float
	sprite.scale = [[node objectForKey:@"scale"] floatValue];
	
	//Set the anchor point so objects are positioned from the bottom-up
	sprite.anchorPoint = ccp(0.5,0);
		
	//Finally, add the sprite
	[self addChild:sprite z:2];
}

@end

@interface LoadingScene : CCLayer {
	NSString *levelStr;	//Our level filename
	float nodesLoaded;	//Number of nodes currently loaded
	float nodesToLoad;	//Total number of nodes to load
	CCLabelBMFont *loadingMessage;	//Message top show the user
	CCSprite *loadingBar;	//Bar indicating loading progress
}

+(id) sceneWithLevel:(NSString*)str;
-(id) initWithLevel:(NSString*)str;
-(void) preloadLevel;
-(void) preloadSpriteFile:(NSDictionary*)node;
-(void) runGame:(id)sender;

@end

@implementation LoadingScene

+(id) sceneWithLevel:(NSString*)str {
	//Create our scene
	CCScene *s = [CCScene node];	
	LoadingScene *node = [[LoadingScene alloc] initWithLevel:str];
	[s addChild:node z:0 tag:0];
	return s;
}

-(id) initWithLevel:(NSString*)str {
	if( (self=[super init] )) {	
		//Set levelStr
		levelStr = str;
		[levelStr retain];	

		//Random colored background
		CCSprite *bg = [CCSprite spriteWithFile:@"blank.png"];
		bg.position = ccp(240,160);
		[bg setTextureRect:CGRectMake(0,0,480,320)];
		[bg setColor:ccc3(arc4random()%150,arc4random()%150,arc4random()%150)];
		[self addChild:bg z:0];
		
		//Set the initial loading message
		loadingMessage = [CCLabelBMFont labelWithString:@"Loading, Please Wait...0%" fntFile:@"eurostile_30.fnt"];
		loadingMessage.position = ccp(160,270);
		loadingMessage.scale = 0.75f;
		[loadingMessage setColor:ccc3(255,255,255)];
		[self addChild:loadingMessage z:10];
		
		//Create an initial '0%' loading bar
		loadingBar = [CCSprite spriteWithFile:@"blank.png"];
		loadingBar.color = ccc3(255,0,0);
		[loadingBar setTextureRect:CGRectMake(0,0,10,25)];
		loadingBar.position = ccp(50,50);
		loadingBar.anchorPoint = ccp(0,0);
		[self addChild:loadingBar z:10];
		
		//Start level pre-load 
		[self preloadLevel];
	}
	return self;
}

//Asynchronously load all required textures
-(void) preloadLevel {
	nodesLoaded = 0;

	NSString *jsonString = [[NSString alloc] initWithContentsOfFile:getActualPath(levelStr) encoding:NSUTF8StringEncoding error:nil];
	NSData *jsonData = [jsonString dataUsingEncoding:NSUTF32BigEndianStringEncoding];
	NSDictionary *dict = [[CJSONDeserializer deserializer] deserializeAsDictionary:jsonData error:nil];

	NSArray *nodes = [dict objectForKey:@"nodes"];
	
	nodesToLoad = [nodes count];
	
	for (id node in nodes) {
		if([[node objectForKey:@"type"] isEqualToString:@"spriteFile"]){
			[self preloadSpriteFile:node];
		}
	}
}

//Asynchronously load a texture and call the specified callback when finished
-(void) preloadSpriteFile:(NSDictionary*)node {
	NSString *file = [node objectForKey:@"file"];
	[[CCTextureCache sharedTextureCache] addImageAsync:file target:self selector:@selector(nodeLoaded:)];
}

//The loading callback
//This increments nodesLoaded and reloads the indicators accordingly
-(void) nodeLoaded:(id)sender {
	nodesLoaded++;
	float percentComplete = 100.0f * (nodesLoaded / nodesToLoad);
	[loadingMessage setString:[NSString stringWithFormat:@"Loading, Please Wait...%d%@", (int)percentComplete, @"%"]];
	
	//When we are 100% complete we run the game
	if(percentComplete >= 100.0f){
		[self runAction:[CCSequence actions: [CCDelayTime actionWithDuration:0.25f], [CCCallFunc actionWithTarget:self selector:@selector(runGame:)], nil]];
	}
	
	//Grow the loading bar
	[loadingBar setTextureRect:CGRectMake(0,0,percentComplete*4,25)];
}

//First pop this scene then load the game scene
-(void) runGame:(id)sender {
	[[CCDirector sharedDirector] popScene];
	[[CCDirector sharedDirector] pushScene:[GameScene sceneWithLevel:@"level1.json"]];
}

@end


@interface Ch5_LoadingScreen : Recipe
{
}

-(CCLayer*) runRecipe;
-(void) loadLevel:(id)sender;

@end

@implementation Ch5_LoadingScreen

-(CCLayer*) runRecipe {
	[super runRecipe];
    
	//The load level button
	CCMenuItemFont *loadLevelItem = [CCMenuItemFont itemFromString:@"Load Level" target:self selector:@selector(loadLevel:)];
	CCMenu *menu = [CCMenu menuWithItems: loadLevelItem, nil];
	menu.position = ccp(240,160);
	[self addChild:menu];

	return self;
}

//Callback to load the level
-(void) loadLevel:(id)sender {
	[[CCDirector sharedDirector] pushScene:[LoadingScene sceneWithLevel:@"level1.json"]];
}

@end