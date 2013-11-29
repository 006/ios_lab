#import "cocos2d.h"
#import "Recipe.h"
#import "GameHelper.h"

//Interface
@interface Ch3_MutatingNestedMetadata : Recipe
{
	NSMutableDictionary *dictMutable;
}
-(CCLayer*) runRecipe;
-(void) showJsonData:(NSDictionary*)dict;
-(void) randomizeData;
-(NSMutableDictionary*) makeRecMutableCopy:(NSDictionary*)dict;
-(void) makeRecMutableCopyRec:(id)container;

@end

//Implementation
@implementation Ch3_MutatingNestedMetadata

-(CCLayer*) runRecipe {
	[super runRecipe];

	//Load JSON data
	NSString *fileName = @"data_to_mutate.json";
	NSString *jsonString = [[[NSString alloc] initWithContentsOfFile:getActualPath(fileName) encoding:NSUTF8StringEncoding error:nil] autorelease];
	NSData *jsonData = [jsonString dataUsingEncoding:NSUTF32BigEndianStringEncoding];
	NSDictionary *dict = [[CJSONDeserializer deserializer] deserializeAsDictionary:jsonData error:nil];
	
	//Create deep mutable copy
	dictMutable = [GameHelper makeRecMutableCopy:dict];
	[dictMutable retain];

	//Show JSON data
	[self showJsonData:dictMutable];

	//Add randomize button
	[CCMenuItemFont setFontSize:30];
	CCMenuItemFont *randomizeItem = [CCMenuItemFont itemFromString:@"Randomize Data" target:self selector:@selector(randomizeData)];
	CCMenu *menu = [CCMenu menuWithItems:randomizeItem, nil];
	menu.position = ccp(240,140);
    [self addChild:menu z:1];	

	return self;
}

-(void) showJsonData:(NSDictionary*)dict {
	[self showMessage:@""];
	
	//Loop through all dictionary nodes to process individual types
	NSMutableDictionary *nodes = [dict objectForKey:@"people"];
	for (NSMutableDictionary* node in nodes) {
		float height = [[node objectForKey:@"height"] floatValue];
		float weight = [[node objectForKey:@"weight"] floatValue];
		NSString *name = [node objectForKey:@"name"];

		[self appendMessage:[NSString stringWithFormat:@"%@: %din %dlbs", name, (int)height, (int)weight]];
	}
}

-(void) randomizeData {
	//Randomize some data in 'dictMutable'
	NSMutableArray *nodes = [dictMutable objectForKey:@"people"];
	for (NSMutableDictionary* node in nodes) {	
		[node setObject:[NSNumber numberWithFloat:(float)(arc4random()%48)+30.0f] forKey:@"height"];
		[node setObject:[NSNumber numberWithFloat:(float)(arc4random()%100)+100.0f] forKey:@"weight"];
	}	

	[self showJsonData:dictMutable];
}

@end
