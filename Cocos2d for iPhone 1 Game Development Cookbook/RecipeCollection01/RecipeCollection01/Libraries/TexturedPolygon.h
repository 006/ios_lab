#import "cocos2d.h"

@class Vector3D;

//TexturedPolygon Class
typedef struct _ccV2F_T2F
{
	ccVertex2F		vertices;
	ccTex2F			texCoords;
} ccV2F_T2F;

@interface TexturedPolygon : CCSprite {
@public
	NSMutableArray* vertices;
	NSMutableArray* triangles;
	ccV2F_T2F* coords;
}
@property (nonatomic, retain) NSMutableArray* vertices;
@property (nonatomic, retain) NSMutableArray* triangles;

+(id) createWithFile:(NSString*)file withVertices:(NSArray*)verts;
+(id) createWithFile:(NSString*)file withVertices:(NSArray*)verts withTriangles:(NSArray*)tris;
-(Vector3D*) getTriangleIndicesFromPoint1:(CGPoint)p1 point2:(CGPoint)p2 point3:(CGPoint)p3;
-(void) addAnimFrameWithFile:(NSString*)file toArray:(NSMutableArray*)arr;
-(void) setCoordInfo;

@end

