#import "Vector3D.h"

#define PI_CONSTANT 3.14159f

typedef enum {
	NO_DIRECTION = 0,
	LEFT = 1,
	UP_LEFT = 2,
	UP = 3,
	UP_RIGHT = 4,
	RIGHT = 5,
	DOWN_RIGHT = 6,
	DOWN = 7,
	DOWN_LEFT = 8
} DirectionType;

@interface GameHelper : NSObject {
@public

}

+(bool) clockwiseO:(Vector3D*)O A:(Vector3D*)A B:(Vector3D*)B;
+(NSMutableArray*) convexHull:(NSMutableArray*)P;
+(float) distanceP1:(CGPoint)p1 toP2:(CGPoint)p2;
+(float) degreesToRadians:(float)d;
+(float) radiansToDegrees:(float)r;
+(float) vectorToRadians:(CGPoint)vector;
+(CGPoint) radiansToVector:(float)radians;
+(Vector3D*) quadraticA:(float)a B:(float)b C:(float)c;
+(float) absoluteValue:(float)a;
+(CGPoint) midPointP1:(CGPoint)p1 p2:(CGPoint)p2;
+(bool) point:(CGPoint)p isInRect:(CGRect)r;
+(bool) point:(CGPoint)p isInCircle:(CGPoint)origin withRadius:(float)radius;
+(NSString*) sanitizeString:(NSString*)str;

@end