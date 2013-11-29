#import "Vector3D.h"

@implementation Vector3D
@synthesize x,y,z;

+(id) x:(float)iX y:(float)iY z:(float)iZ {
	return [[[self alloc] initWithX:iX y:iY z:iZ] autorelease];
}

-(id) initWithX:(float)iX y:(float)iY z:(float)iZ {
	if((self = [super init])){
		self.x = iX;
		self.y = iY;
		self.z = iZ;
	}
	return self;
}
+(bool) isVector:(Vector3D*)v1 equalTo:(Vector3D*)v2 {
	if(v1.x == v2.x and v1.y == v2.y and v1.z == v2.z){
		return YES;
	}
	return NO;
}

@end