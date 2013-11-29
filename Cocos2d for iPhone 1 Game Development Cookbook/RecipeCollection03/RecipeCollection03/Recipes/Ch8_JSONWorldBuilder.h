#import "Recipe.h"
#import "ActualPath.h"
#import "CJSONDeserializer.h"
#import "Vector3D.h"
#import "GameArea2D.h"
#import "DrawLayer.h"

@interface Ch8_JSONWorldBuilder : GameArea2D
{
	NSDictionary *mapData;
	CGPoint canvasSize;
	DrawLayer *drawLayer;
	NSMutableArray *lineVerticesA;
	NSMutableArray *lineVerticesB;
	NSMutableArray *points;
}

-(CCLayer*) runRecipe;
-(void) finishInit;
-(void) step:(ccTime)dt;
-(void) loadMap:(NSString*)mapStr;
-(void) preProcessMapData;
-(void) processSprite:(NSDictionary*)mapNode;
-(void) processTiledSprite:(NSDictionary*)mapNode;
-(void) processLine:(NSDictionary*)mapNode;
-(void) processPoint:(NSDictionary*)mapNode;
-(void) drawLayer;
-(void) addDrawLayer;

@end

//Implementation
@implementation Ch8_JSONWorldBuilder

-(CCLayer*) runRecipe {
	//Load our map file
	[self loadMap:@"world.json"];

	return self;
}

/* Called after the map has been loaded into a container but before assets have been loaded */
-(void) finishInit {
	//Superclass initialization and message
	[super runRecipe];
	[self showMessage:@"Scroll to move the camera around the world.\nPinch to zoom."];

	//Add a draw layer for line drawing
	[self addDrawLayer];
	
	//Initial variables
	cameraZoom = 0.85f;

	//Step method
	[self schedule:@selector(step:)];	
	
	//Init line/point containers
	lineVerticesA = [[NSMutableArray alloc] init];
	lineVerticesB = [[NSMutableArray alloc] init];
	points = [[NSMutableArray alloc] init];
	
	//Center camera
	[self centerCameraOnGameCoord:ccp(0,0)];
}

-(void) step: (ccTime) dt {	
	[super step:dt];
}

/* Our load map method */
-(void) loadMap:(NSString*)mapStr {
	//Retrive the JSON string
	NSString *jsonString = [[NSString alloc] initWithContentsOfFile:getActualPath(mapStr) encoding:NSUTF8StringEncoding error:nil];
	
	//Decode the string into an NSData object
	NSData *jsonData = [jsonString dataUsingEncoding:NSUTF32BigEndianStringEncoding];
	
	//Deserialize the string into a nested NSDictionary
	mapData = [[CJSONDeserializer deserializer] deserializeAsDictionary:jsonData error:nil];
	
	//Add all sprite frames for listed plist files
	NSArray *plistFiles = [mapData objectForKey:@"plistFiles"];
	for (id plistFile in plistFiles) {
		[[CCSpriteFrameCache sharedSpriteFrameCache] addSpriteFramesWithFile:plistFile];
	}
	
	//List of PNG files is also available
	NSArray *pngFiles = [mapData objectForKey:@"pngFiles"];
		
	//Pre process data
	[self preProcessMapData];
	
	//Process map nodes
	NSDictionary *mapNodes = [mapData objectForKey:@"mapNodes"]; 
	for (id mapNodeKey in mapNodes) { 
		NSDictionary *mapNode = [mapNodes objectForKey:mapNodeKey]; 
		
		NSString *nodeType = [mapNode objectForKey:@"type"];
		
		//Process node types
		if([nodeType isEqualToString:@"sprite"]){
			[self processSprite:mapNode];
		}else if([nodeType isEqualToString:@"tiledSprite"]){
			[self processTiledSprite:mapNode];
		}else if([nodeType isEqualToString:@"line"]){
			[self processLine:mapNode];
		}else if([nodeType isEqualToString:@"point"]){
			[self processPoint:mapNode];
		}
	} 
}

-(void) preProcessMapData {
	//Set canvasSize and gameAreaSize from map file
	canvasSize = ccp( [[mapData objectForKey:@"canvasWidth"] floatValue], [[mapData objectForKey:@"canvasHeight"] floatValue] );
	gameAreaSize = ccp( canvasSize.x/PTM_RATIO, canvasSize.y/PTM_RATIO );

	//Finish map initialization
	[self finishInit];
}

/* Process a sprite node. This represents a single sprite onscreen */
-(void) processSprite:(NSDictionary*)mapNode {
	//Get node information
	NSString *texture = [mapNode objectForKey:@"selectedSpriteY"];
	float originX = [[mapNode objectForKey:@"originX"] floatValue];
	float originY = [[mapNode objectForKey:@"originY"] floatValue];
	float zIndex = [[mapNode objectForKey:@"zIndex"] floatValue];
	NSString *flipX = [mapNode objectForKey:@"flipX"];
	NSString *flipY = [mapNode objectForKey:@"flipY"];
	float tag = 0;
	
	//Get metadata
	NSDictionary *metaPairs = [mapNode objectForKey:@"meta"]; 
	for (id metaKey in metaPairs) { 
		NSString* metaValue = [metaPairs objectForKey:metaKey];
		
		//Check for key "tag"
		if([metaKey isEqualToString:@"tag"]){
			tag = ((int)[metaValue dataUsingEncoding:NSUTF8StringEncoding]);
		}
	}
	
	//Either load sprite with file or with sprite frame name
	CCSprite *sprite;
	if(getActualPath(texture) != nil){
		sprite = [CCSprite spriteWithFile:texture];
	}else{
		sprite = [CCSprite spriteWithSpriteFrameName:texture];
	}
	
	//Set sprite size
	float width = [sprite textureRect].size.width;
	float height = [sprite textureRect].size.height;
	
	//Set sprite position (some conversion required)
	float x = originX - (canvasSize.x/2);
	float y = canvasSize.y - originY - (canvasSize.y/2);
	sprite.position = ccp( x+width/2, y-height/2 );
	
	//Flip sprite if necessary
	if([flipX isEqualToString:@"Y"]){ [sprite setFlipX:YES]; }
	if([flipY isEqualToString:@"Y"]){ [sprite setFlipY:YES]; }
	
	//Finally, add the sprite
	[gameNode addChild:sprite z:zIndex-24995 tag:tag];
}

/* Process a tiled sprite. */
-(void) processTiledSprite:(NSDictionary*)mapNode {
	//Get node information
	NSString *texture = [mapNode objectForKey:@"selectedSpriteY"];
	NSMutableDictionary *frames = [[[NSMutableDictionary alloc] init] autorelease];
	float originX = [[mapNode objectForKey:@"originX"] floatValue];
	float originY = [[mapNode objectForKey:@"originY"] floatValue];
	float width = [[mapNode objectForKey:@"width"] floatValue];
	float height = [[mapNode objectForKey:@"height"] floatValue];
	float zIndex = [[mapNode objectForKey:@"zIndex"] floatValue];
	float tag = 0;
	
	//Get metadata
	NSDictionary *metaPairs = [mapNode objectForKey:@"meta"]; 
	for (id metaKey in metaPairs) { 
		NSString* metaValue = [metaPairs objectForKey:metaKey];

		//Check for key "tag" or key "frame" (for animation)
		if([metaKey isEqualToString:@"tag"]){
			tag = ((int)[metaValue dataUsingEncoding:NSUTF8StringEncoding]);
		}else if ([metaKey rangeOfString:@"frame"].location != NSNotFound){
			[frames setObject:metaValue forKey:metaKey];
		}
	}
		
	//Get any masks to be applied to this tiled sprite
	NSArray *masks = [mapNode objectForKey:@"masks"];
	
	//OpenGL texture parameters
	ccTexParams params = {GL_NEAREST,GL_NEAREST_MIPMAP_NEAREST,GL_REPEAT,GL_REPEAT};
	
	//If a mask exists, apply it
	if([masks count] > 0){
		//Create containers
		NSMutableArray *vertices = [[NSMutableArray alloc] init];
		NSMutableArray *triangles = [[NSMutableArray alloc] init];
	
		//Get mask information
		NSDictionary *mask = [masks objectAtIndex:0];
		NSArray *pointsX = [mask objectForKey:@"pointsX"];
		NSArray *pointsY = [mask objectForKey:@"pointsY"];
		NSArray *triPoints = [mask objectForKey:@"triPoints"];
	
		//Set vertices
		for(int i=0; i<[pointsX count]; i+=1){
			CGPoint vertex = ccp([[pointsX objectAtIndex:i] floatValue], height - ([[pointsY objectAtIndex:i] floatValue]));
			[vertices addObject:[NSValue valueWithCGPoint:vertex]];
		}
		
		//Set triangle points
		for(int i=0; i<[triPoints count]; i+=1){
			NSArray *t = [triPoints objectAtIndex:i];

			Vector3D *vect = [Vector3D x:[[t objectAtIndex:0] floatValue] y:[[t objectAtIndex:1] floatValue] z:[[t objectAtIndex:2] floatValue]];
			[triangles addObject:vect];
		}		
			
		//Create TexturedPolygon object
		TexturedPolygon *tp = [TexturedPolygon createWithFile:texture withVertices:vertices withTriangles:triangles];
		[tp.texture setTexParameters:&params];
		
		//Set position
		float x = originX - (canvasSize.x/2);
		float y = canvasSize.y - originY - (canvasSize.y/2);
		tp.position = ccp( x, y-height );
				
		//If meta key "frames" is specified apply some animation
		if([frames count] > 0){
			//Create container for frames
			NSMutableArray *animFrames = [NSMutableArray array];
		
			//Create sprite frames from other textures
			for(int f=0; f<[frames count]; f=f+1){
				NSString* key = [NSString stringWithFormat:@"%@%i", @"frame0", f];
				if(f > 9){
					key = [NSString stringWithFormat:@"%@%i", @"frame", f];
				}
				CCTexture2D *frameTexture = [[CCTextureCache sharedTextureCache] addImage:[frames objectForKey:key]];
				[frameTexture setTexParameters:&params];
				
				CCSpriteFrame *frame = [CCSpriteFrame frameWithTexture:frameTexture rect:tp.textureRect];
				[[CCSpriteFrameCache sharedSpriteFrameCache] addSpriteFrame:frame name:key];
				[animFrames addObject:frame];										
			}
			
			//Create repeated animation with frames
			CCAnimation *animation = [CCAnimation animationWithName:[NSString stringWithFormat:@"%@_anim",texture] delay:0.1f frames:animFrames];
			[tp addAnimation:animation];
			CCActionInterval *animate = [CCAnimate actionWithAnimation:animation restoreOriginalFrame:NO];
			CCActionInterval *repeatAnimation = [CCRepeatForever actionWithAction:animate];
			[tp runAction:repeatAnimation];
		}
				
		//Finally, add the node
		[gameNode addChild:tp z:zIndex-24995];
	}else if([frames count] > 0){
		/* If we have a non-masked tiled animated sprite */
					
		//Create container for frames
		NSMutableArray *animFrames = [NSMutableArray array];
		
		//Create sprite frames from other textures
		for(int f=0; f<[frames count]; f=f+1){
			NSString* key = [NSString stringWithFormat:@"%@%i", @"frame0", f];
			if(f > 9){
				key = [NSString stringWithFormat:@"%@%i", @"frame", f];
			}
			
			CCTexture2D *frameTexture = [[CCTextureCache sharedTextureCache] addImage:[frames objectForKey:key]];
			[frameTexture setTexParameters:&params];
			
			CCSpriteFrame *frame = [CCSpriteFrame frameWithTexture:frameTexture rect:CGRectMake(0,0,width,height)];
			[[CCSpriteFrameCache sharedSpriteFrameCache] addSpriteFrame:frame name:key];
			[animFrames addObject:frame];
		}
		
		//Init our sprite with the first frame and add it
		CCSprite *sprite = [CCSprite spriteWithSpriteFrame:[animFrames objectAtIndex:0]];
		[sprite.texture setTexParameters:&params];
		
		//Set position
		float x = originX - (canvasSize.x/2);
		float y = canvasSize.y - originY - (canvasSize.y/2);
		sprite.position = ccp( x+width/2, y-height/2 );
		
		//Add the node
		[gameNode addChild:sprite z:zIndex-24999];
		
		//Run the repeated animation
		CCAnimation *animation = [CCAnimation animationWithName:[NSString stringWithFormat:@"%@_anim",texture] delay:0.1f frames:animFrames];
		[sprite addAnimation:animation];
		CCActionInterval *animate = [CCAnimate actionWithAnimation: animation restoreOriginalFrame:NO];
		CCActionInterval *repeatAnimation = [CCRepeatForever actionWithAction:animate];
		[sprite runAction:repeatAnimation];
	}else{
		//Use a regular Sprite
		CCSprite *sprite = [CCSprite spriteWithFile:texture rect:CGRectMake(0,0,width,height)];
		[sprite.texture setTexParameters:&params];
		
		//Set position
		float x = originX - (canvasSize.x/2);
		float y = canvasSize.y - originY - (canvasSize.y/2);
		sprite.position = ccp( x+width/2, y-height/2 );
		
		//Add the node
		[gameNode addChild:sprite z:zIndex-24999];
	}
}

/* Process a line */
-(void) processLine:(NSDictionary*)mapNode{
	//Get line information
	NSArray *drawnLines = [mapNode objectForKey:@"drawnLines"];
	NSDictionary *drawnLine = [drawnLines objectAtIndex:0];
	float fromX = [[drawnLine objectForKey:@"fromX"] floatValue];
	float fromY = [[drawnLine objectForKey:@"fromY"] floatValue];
	float toX = [[drawnLine objectForKey:@"toX"] floatValue];
	float toY = [[drawnLine objectForKey:@"toY"] floatValue];

	//Add information to our line containers
	[lineVerticesA addObject:[NSValue valueWithCGPoint:ccp(fromX, canvasSize.y-fromY)]];
	[lineVerticesB addObject:[NSValue valueWithCGPoint:ccp(toX, canvasSize.y-toY)]];
}

/* Process a point */
-(void) processPoint:(NSDictionary*)mapNode{
	//Get point information
	float originX = [[mapNode objectForKey:@"originX"] floatValue];
	float originY = [[mapNode objectForKey:@"originY"] floatValue];
	originY = canvasSize.y - originY;
	
	//If metadata is appropriate, add point to container
	NSDictionary *metaPairs = [mapNode objectForKey:@"meta"]; 
	for (id metaKey in metaPairs) { 
		NSString* metaValue = [metaPairs objectForKey:metaKey];
		
		if([metaKey isEqualToString:@"type"] and [metaValue isEqualToString:@"example"]){
			[points addObject:[NSValue valueWithCGPoint:ccp(originX, originY)]];
		}
	}
}

/* Draw our lines and our points */
-(void) drawLayer {
	//Draw lines
	for(int i=0; i<lineVerticesA.count; i++){
		CGPoint pointA = [[lineVerticesA objectAtIndex:i] CGPointValue];
		CGPoint pointB = [[lineVerticesB objectAtIndex:i] CGPointValue];
		
		CGPoint a = ccp(pointA.x - gameNode.contentSize.width/2, pointA.y - gameNode.contentSize.height/2);
		CGPoint b = ccp(pointB.x - gameNode.contentSize.width/2, pointB.y - gameNode.contentSize.height/2);
		
		glColor4ub(255,0,0,255);
		ccDrawLine(a,b);
		glColor4ub(255,255,255,255);
	}
	
	//Draw points
	for(int i=0; i<points.count; i++){
		CGPoint point = [[points objectAtIndex:i] CGPointValue];
		CGPoint p = ccp(point.x - gameNode.contentSize.width/2, point.y - gameNode.contentSize.height/2);
		
		glColor4ub(255,0,0,255);
		ccDrawCircle(p, 16.0f, 0, 16, NO);
		glColor4ub(255,255,255,255);
	}
}

/* Add draw layer */
-(void) addDrawLayer {
	drawLayer = [[DrawLayer alloc] init];
	drawLayer.position = ccp(0,0);
	drawLayer.recipe = self;
	[gameNode addChild:drawLayer z:10];
}

-(void) cleanRecipe {
	[lineVerticesA release];
	[lineVerticesB release];
	[points release];

	[super cleanRecipe];
}

@end
