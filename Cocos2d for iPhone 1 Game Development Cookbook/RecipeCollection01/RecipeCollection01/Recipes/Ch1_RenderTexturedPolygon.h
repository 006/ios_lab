#import "cocos2d.h"
#import "Recipe.h"
#import "Vector3D.h"
#import "TexturedPolygon.h"

//Interface
@interface Ch1_RenderTexturedPolygon : Recipe
{
}

-(CCLayer*) runRecipe;

@end


//Implementation
@implementation Ch1_RenderTexturedPolygon

-(CCLayer*) runRecipe {

	CGSize s = [[CCDirector sharedDirector] winSize];
				
	CGPoint vertexArr[] = { ccp(248,340), ccp(200,226), ccp(62,202), ccp(156,120), ccp(134,2), ccp(250,64), ccp(360,0), 
		ccp(338,128), ccp(434,200), ccp(306,230) };
	int numVerts = 10;
	
	NSMutableArray *vertices = [[[NSMutableArray alloc] init] autorelease];
	
	for(int i=0; i<numVerts; i++){
		[vertices addObject:[NSValue valueWithCGPoint:vertexArr[i]]];
	}
			
	//Note: Your texture MUST be a product of 2 for this to work.
	ccTexParams params = {GL_NEAREST,GL_NEAREST_MIPMAP_NEAREST,GL_REPEAT,GL_REPEAT};
	TexturedPolygon *texturedPoly = [TexturedPolygon createWithFile:@"bricks.jpg" withVertices:vertices];
	[texturedPoly.texture setTexParameters:&params];
	texturedPoly.position = ccp(0,0);
	
	[self addChild:texturedPoly z:1 tag:0];
	
	return self;
}

@end