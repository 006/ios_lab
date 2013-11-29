#import "cocos2d.h"
#import "Recipe.h"
#import "Vector3D.h"
#import "TexturedPolygon.h"

//Interface
@interface Ch1_AnimateTexturedPolygon : Recipe
{
}

-(CCLayer*) runRecipe;

@end

@implementation Ch1_AnimateTexturedPolygon

-(CCLayer*) runRecipe {
	CGSize s = [[CCDirector sharedDirector] winSize];
				
	ccTexParams params = {GL_NEAREST,GL_NEAREST_MIPMAP_NEAREST,GL_REPEAT,GL_REPEAT};
	
	//Create grass animated textured polygon
	CGPoint grassVertexArr[] = { ccp(0,0), ccp(480,0), ccp(480,320), ccp(0,320) };
	int grassNumVerts = 4;
	NSMutableArray *grassVertices = [[[NSMutableArray alloc] init] autorelease];
	for(int i=0; i<grassNumVerts; i++){
		[grassVertices addObject:[NSValue valueWithCGPoint:ccp(grassVertexArr[i].x*1, grassVertexArr[i].y*1)]];
	}

	TexturedPolygon *grassPoly = [TexturedPolygon createWithFile:@"grass_tile_01.png" withVertices:grassVertices];
	[grassPoly.texture setTexParameters:&params];
	grassPoly.position = ccp(0,0);
	[self addChild:grassPoly z:1 tag:1];

	//Create swaying grass animation
	NSMutableArray *grassAnimFrames = [NSMutableArray array];
	
	//This is a two part animation with 'back' and 'forth' frames
	for(int i=0; i<=6; i++){
		[grassPoly addAnimFrameWithFile:[NSString stringWithFormat:@"grass_tile_0%d.png",i] toArray:grassAnimFrames];
	}
	for(int i=5; i>0; i--){
		[grassPoly addAnimFrameWithFile:[NSString stringWithFormat:@"grass_tile_0%d.png",i] toArray:grassAnimFrames];
	}

	CCAnimation *grassAnimation = [[CCAnimation alloc] initWithName:@"grass_tile_anim" delay:0.1f];
	for(int i=0; i<[grassAnimFrames count]; i++){
		[grassAnimation addFrame:[grassAnimFrames objectAtIndex:i]];
	}
	
	CCActionInterval *grassAnimate = [CCSequence actions: [CCAnimate actionWithAnimation:grassAnimation restoreOriginalFrame:NO], 
		[CCDelayTime actionWithDuration:0.0f], nil];
	CCActionInterval *grassRepeatAnimation = [CCRepeatForever actionWithAction:grassAnimate];
	[grassPoly runAction:grassRepeatAnimation];
	
	return self;
}

@end