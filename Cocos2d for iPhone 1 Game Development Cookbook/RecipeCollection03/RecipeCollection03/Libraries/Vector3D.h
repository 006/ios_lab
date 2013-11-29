//Vector3D Class
@interface Vector3D : NSObject {
@public
	float x;
	float y;
	float z;
}
@property (readwrite, assign) float x;
@property (readwrite, assign) float y;
@property (readwrite, assign) float z;

+(id) x:(float)iX y:(float)iY z:(float)iZ;
-(id) initWithX:(float)iX y:(float)iY z:(float)iZ;
+(bool) isVector:(Vector3D*)v1 equalTo:(Vector3D*)v2;

@end