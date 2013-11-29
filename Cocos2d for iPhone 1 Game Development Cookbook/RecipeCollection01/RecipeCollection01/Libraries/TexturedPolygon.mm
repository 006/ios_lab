#import "TexturedPolygon.h"

#import "Vector3D.h"

//Included for CPP polygon triangulation
#import "triangulate.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

@implementation TexturedPolygon
@synthesize vertices, triangles;

-(id)init {
    self = [super init];
    if (self != nil) {
		self.anchorPoint = ccp([self contentSize].width/2,[self contentSize].height/2);
    }
    return self;
}

+(id) createWithFile:(NSString*)file withVertices:(NSArray*)verts {
	/*** Create a TexturedPolygon with vertices only. ***/
	/*** Perform polygon trianglulation to get triangles. ***/
	
	//Initialization
	TexturedPolygon *tp = [TexturedPolygon spriteWithFile:file];
	tp.vertices = [[NSMutableArray alloc] init];
	tp.triangles = [[NSMutableArray alloc] init];
	
	//Polygon Triangulation
	Vector2dVector a;
	
	for(int i=0; i<[verts count];i+=1){
		//Add polygon vertices
		[tp.vertices addObject:[verts objectAtIndex:i]];
		
		//Add polygon vertices to triangulation container
		CGPoint vert = [[verts objectAtIndex:i] CGPointValue];
		a.push_back( Vector2d(vert.x, vert.y) );
	}

	//Run triangulation algorithm
	Vector2dVector result;
	Triangulate::Process(a,result);
	
	//Gather all triangles from result container
	int tcount = result.size()/3;
	for (int i=0; i<tcount; i++) {
		const Vector2d &p1 = result[i*3+0];
		const Vector2d &p2 = result[i*3+1];
		const Vector2d &p3 = result[i*3+2];
		
		NSLog(@"Triangle %d => (%f,%f) (%f,%f) (%f,%f)",i+1,p1.GetX(),p1.GetY(),p2.GetX(),p2.GetY(),p3.GetX(),p3.GetY());
		
		//Add triangle index
		[tp.triangles addObject: [tp getTriangleIndicesFromPoint1:ccp(p1.GetX(),p1.GetY()) point2:ccp(p2.GetX(),p2.GetY()) point3:ccp(p3.GetX(), p3.GetY())] ];
	}	
	
	//Set texture coordinate information
	[tp setCoordInfo];
	
	return tp;
}

+(id) createWithFile:(NSString*)file withVertices:(NSArray*)verts withTriangles:(NSArray*)tris {
	/*** Create a TexturedPolygon with vertices and triangles given. ***/
	
	//Initialization
	TexturedPolygon *tp = [TexturedPolygon spriteWithFile:file];
	tp.vertices = [[NSMutableArray alloc] init];
	tp.triangles = [[NSMutableArray alloc] init];
		
	//Set polygon vertices
	for(int i=0; i<[verts count];i+=1){
		[tp.vertices addObject:[verts objectAtIndex:i]];
	}

	//Set triangle indices
	for(int i=0; i<[tris count];i+=1){
		[tp.triangles addObject:[tris objectAtIndex:i]];
	}

	//Set texture coordinate information
	[tp setCoordInfo];
	
	return tp;
}

-(Vector3D*) getTriangleIndicesFromPoint1:(CGPoint)p1 point2:(CGPoint)p2 point3:(CGPoint)p3 {
	/*** Convert three polygon vertices to triangle indices ***/
	
	Vector3D* indices = [Vector3D x:-1 y:-1 z:-1];
	
	for(int i=0; i< [vertices count]; i++){
		CGPoint vert = [[vertices objectAtIndex:i] CGPointValue];
		if(p1.x == vert.x and p1.y == vert.y){
			indices.x = i;
		}else if(p2.x == vert.x and p2.y == vert.y){
			indices.y = i;
		}else if(p3.x == vert.x and p3.y == vert.y){
			indices.z = i;
		}
	}

	return indices;
}

-(void) addAnimFrameWithFile:(NSString*)file toArray:(NSMutableArray*)arr {
	/*** For textured polygon animation ***/
	
	ccTexParams params = {GL_NEAREST,GL_NEAREST_MIPMAP_NEAREST,GL_REPEAT,GL_REPEAT};
	CCTexture2D *frameTexture = [[CCTextureCache sharedTextureCache] addImage:file];
	[frameTexture setTexParameters:&params];
	CCSpriteFrame *frame = [CCSpriteFrame frameWithTexture:frameTexture rect:self.textureRect];
	[[CCSpriteFrameCache sharedSpriteFrameCache] addSpriteFrame:frame name:file];
	[arr addObject:frame];
}

-(void) setCoordInfo {
	/*** Set texture coordinates for each vertex ***/

	if(coords){ free(coords); }
	coords = (ccV2F_T2F*)malloc(sizeof(ccV2F_T2F)*[vertices count]);
	
	for(int i=0;i<[vertices count];i++) {	
		coords[i].vertices.x = [[vertices objectAtIndex:i] CGPointValue].x;
		coords[i].vertices.y = [[vertices objectAtIndex:i] CGPointValue].y;
		
		float atlasWidth = texture_.pixelsWide;
		float atlasHeight = texture_.pixelsHigh;
		
		coords[i].texCoords.u = (coords[i].vertices.x + rect_.origin.x)/ atlasWidth;
		coords[i].texCoords.v = (contentSize_.height - coords[i].vertices.y + rect_.origin.y)/ atlasHeight ;
	}
}

-(void) dealloc {
	if(coords) free(coords);
	
	[vertices release];
	[triangles release];
	
	[super dealloc];
}

-(void) draw
{
	/*** This is where the magic happens. Texture and draw all triangles. ***/

	glDisableClientState(GL_COLOR_ARRAY);
	
	glColor4ub( color_.r, color_.g, color_.b, quad_.bl.colors.a);
	
	BOOL newBlend = NO;
	if( blendFunc_.src != CC_BLEND_SRC || blendFunc_.dst != CC_BLEND_DST ) {
		newBlend = YES;
		glBlendFunc( blendFunc_.src, blendFunc_.dst );
	}
	
	glBindTexture(GL_TEXTURE_2D, texture_.name);
	
	unsigned int offset = (unsigned int)coords;
	unsigned int diff = offsetof( ccV2F_T2F, vertices);
	glVertexPointer(2, GL_FLOAT, sizeof(ccV2F_T2F), (void*) (offset + diff));
	diff = offsetof( ccV2F_T2F, texCoords);
	glTexCoordPointer(2, GL_FLOAT, sizeof(ccV2F_T2F), (void*) (offset + diff));
	
	for(int i=0;i<[triangles count];i++){
		Vector3D *tri = [triangles objectAtIndex:i];
		short indices[] = {tri.x, tri.y, tri.z};
		glDrawElements(GL_TRIANGLE_STRIP, 3, GL_UNSIGNED_SHORT, indices);
	}
		
	if(newBlend) { glBlendFunc(CC_BLEND_SRC, CC_BLEND_DST); }
	
	glColor4ub( 255, 255, 255, 255);
	
	glEnableClientState(GL_COLOR_ARRAY);
}
@end
