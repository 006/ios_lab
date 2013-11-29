#import "cocos2d.h"
#import "Recipe.h"
#import "ActualPath.h"
#import "GDataXMLNode.h"

//Interface
@interface Ch3_ReadingXmlFiles : Recipe
{
}

-(CCLayer*) runRecipe;
-(void) processMap:(GDataXMLDocument*)doc;
-(void) processSpriteFile:(GDataXMLElement*)node;
-(void) processTexturedPolygon:(GDataXMLElement*)node;
-(float) getZFromY:(float)y;

@end

//Implementation
@implementation Ch3_ReadingXmlFiles

-(CCLayer*) runRecipe {
	//Read our file in as an NSData object
	NSString *fileName = @"scene3.xml";
	NSString *xmlString = [[[NSString alloc] initWithContentsOfFile:getActualPath(fileName) encoding:NSUTF8StringEncoding error:nil] autorelease];
	NSData *xmlData = [xmlString dataUsingEncoding:NSUTF32BigEndianStringEncoding];

	//Initialize a new GDataXMLDocument with our data
    GDataXMLDocument *doc = [[[GDataXMLDocument alloc] initWithData:xmlData options:0 error:nil] autorelease];
	
	//Process that document
	[self processMap:doc];
	
	return self;
}

-(void) processMap:(GDataXMLDocument*)doc {
	//Find all elements of 'node' type
	NSArray *nodes = [doc.rootElement elementsForName:@"node"];

	//Loop through each element
	for (GDataXMLElement *node in nodes) {
		//Find the first (and assumed only) element with the name 'type' in this node
		NSString *type = [[[node elementsForName:@"type"] objectAtIndex:0] stringValue];
		
		//Process specific node types
		if([type isEqualToString:@"spriteFile"]){
			[self processSpriteFile:node];
		}else if([type isEqualToString:@"texturedPolygon"]){
			[self processTexturedPolygon:node];
		}		
	}
}

/* Process the 'spriteFile' type */
-(void) processSpriteFile:(GDataXMLElement*)node {
	//Init the sprite
	NSString *file = [[[node elementsForName:@"file"] objectAtIndex:0] stringValue];
	CCSprite *sprite = [CCSprite spriteWithFile:file];
	
	//Set sprite position
	GDataXMLElement *posElement = [[node elementsForName:@"position"] objectAtIndex:0];
	sprite.position = ccp( [[[[posElement elementsForName:@"x"] objectAtIndex:0] stringValue] floatValue], 
		[[[[posElement elementsForName:@"y"] objectAtIndex:0] stringValue] floatValue]);
	
	//Each element is considered a string first
	sprite.scale = [[[[node elementsForName:@"scale"] objectAtIndex:0] stringValue] floatValue];

	//Set the anchor point
	sprite.anchorPoint = ccp(0.5,0);

	//We set the sprite Z according to its Y to produce an isometric perspective
	float z = [self getZFromY:sprite.position.y];
	if([node elementsForName:@"z"].count > 0){
		z = [[[[node elementsForName:@"z"] objectAtIndex:0] stringValue] floatValue];
	}

	//Finally, add the sprite
	[self addChild:sprite z:z];
}

/* Process the 'texturedPolygon' type */
-(void) processTexturedPolygon:(GDataXMLElement*)node {	
	//Process vertices
	NSMutableArray *vertices = [[[NSMutableArray alloc] init] autorelease];
	
	NSArray *vertexData = [[[node elementsForName:@"vertices"] objectAtIndex:0] elementsForName:@"vertex"];
	
	for(id vData in vertexData){
		GDataXMLElement *vertexElement = (GDataXMLElement*)vData;
		float x = [[[[vertexElement elementsForName:@"x"] objectAtIndex:0] stringValue] floatValue];
		float y = [[[[vertexElement elementsForName:@"y"] objectAtIndex:0] stringValue] floatValue];
	
		[vertices addObject:[NSValue valueWithCGPoint:ccp(x,y)]];
	}
	
	//Init our textured polygon
	NSString *file = [[[node elementsForName:@"file"] objectAtIndex:0] stringValue];
	
	ccTexParams params = {GL_NEAREST,GL_NEAREST_MIPMAP_NEAREST,GL_REPEAT,GL_REPEAT};
	TexturedPolygon *texturedPoly = [TexturedPolygon createWithFile:file withVertices:vertices];
	[texturedPoly.texture setTexParameters:&params];
	
	//Set position
	GDataXMLElement *posElement = [[node elementsForName:@"position"] objectAtIndex:0];
	texturedPoly.position = ccp( [[[[posElement elementsForName:@"x"] objectAtIndex:0] stringValue] floatValue], 
		[[[[posElement elementsForName:@"y"] objectAtIndex:0] stringValue] floatValue]);
		
	//Add the texturedPolygon behind any sprites
	[self addChild:texturedPoly z:0];
}

/* Our simple method used to order sprites by depth */
-(float) getZFromY:(float)y {
	return 320-y;
}

@end