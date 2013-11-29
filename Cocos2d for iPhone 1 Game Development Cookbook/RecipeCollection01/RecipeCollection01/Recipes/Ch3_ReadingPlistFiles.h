#import "cocos2d.h"
#import "Recipe.h"
#import "ActualPath.h"
#import "TexturedPolygon.h"

//Interface
@interface Ch3_ReadingPlistFiles : Recipe
{
}

-(CCLayer*) runRecipe;
-(void) processMap:(NSDictionary*)dict;
-(void) processSpriteFile:(NSDictionary*)node;
-(void) processTexturedPolygon:(NSDictionary*)node;
-(float) getZFromY:(float)y;

@end

//Implementation
@implementation Ch3_ReadingPlistFiles

-(CCLayer*) runRecipe {
	//Initialize a read-only dictionary from our file
	NSString *fileName = @"scene1.plist";
	NSDictionary *dict = [NSDictionary dictionaryWithContentsOfFile:getActualPath(fileName)];
	
	//Process this dictionary
	[self processMap:dict];
	
	return self;
}

-(void) processMap:(NSDictionary*)dict {
	//Loop through all dictionary nodes to process individual types
	NSArray *nodes = [dict objectForKey:@"nodes"];
	for (id node in nodes) {
		if([[node objectForKey:@"type"] isEqualToString:@"spriteFile"]){
			[self processSpriteFile:node];
		}else if([[node objectForKey:@"type"] isEqualToString:@"texturedPolygon"]){
			[self processTexturedPolygon:node];
		}
	}
}

/* Process the 'spriteFile' type */
-(void) processSpriteFile:(NSDictionary*)node {
	//Init the sprite
	NSString *file = [node objectForKey:@"file"];
	CCSprite *sprite = [CCSprite spriteWithFile:file];
	
	//Set sprite position
	NSDictionary *posDict = [node objectForKey:@"position"];
	sprite.position = ccp([[posDict objectForKey:@"x"] floatValue], [[posDict objectForKey:@"y"] floatValue]);
	
	//Each numeric value is an NSString or NSNumber that must be cast into a float
	sprite.scale = [[node objectForKey:@"scale"] floatValue];
	
	//Set the anchor point so objects are positioned from the bottom-up
	sprite.anchorPoint = ccp(0.5,0);
		
	//We set the sprite Z according to its Y to produce an isometric perspective
	float z = [self getZFromY:[[posDict objectForKey:@"y"] floatValue]];
	if([node objectForKey:@"z"]){
		z = [[node objectForKey:@"z"] floatValue];
	}
	
	//Finally, add the sprite	
	[self addChild:sprite z:z];
}

-(void) processTexturedPolygon:(NSDictionary*)node {
	//Process vertices
	NSMutableArray *vertices = [[[NSMutableArray alloc] init] autorelease];
	NSArray *vertexData = [node objectForKey:@"vertices"];
	for(id vData in vertexData){
		float x = [[vData objectForKey:@"x"] floatValue];
		float y = [[vData objectForKey:@"y"] floatValue];
	
		[vertices addObject:[NSValue valueWithCGPoint:ccp(x,y)]];
	}
	
	//Init our textured polygon
	NSString *file = [node objectForKey:@"file"];
	
	ccTexParams params = {GL_NEAREST,GL_NEAREST_MIPMAP_NEAREST,GL_REPEAT,GL_REPEAT};
	TexturedPolygon *texturedPoly = [TexturedPolygon createWithFile:file withVertices:vertices];
	[texturedPoly.texture setTexParameters:&params];
	
	//Set position
	NSDictionary *posDict = [node objectForKey:@"position"];
	texturedPoly.position = ccp([[posDict objectForKey:@"x"] floatValue], [[posDict objectForKey:@"y"] floatValue]);

	//Add the texturedPolygon behind any sprites
	[self addChild:texturedPoly z:0];
}

/* Our simple method used to order sprites by depth */
-(float) getZFromY:(float)y {
	return 320-y;
}

@end