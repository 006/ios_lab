#import "GameHelper.h"
#import "Box2D.h"
#import "cocos2d.h"

@implementation GameHelper

//2D cross product of OA and OB vectors, i.e. z-component of their 3D cross product.
//Returns true for clockwise turn or the points are collinear, false another.
+(bool) clockwiseO:(Vector3D*)O A:(Vector3D*)A B:(Vector3D*)B {
	return (A.x - O.x) * (B.y - O.y) - (A.y - O.y) * (B.x - O.x) <= 0;
}

//Given a set of points it returns a polygon surrounding those points (a convex hull)
//From here: http://www.algorithmist.com/index.php/Monotone_Chain_Convex_Hull.cpp
+(NSMutableArray*) convexHull:(NSMutableArray*)P {
	int n = P.count;
	int k = 0;
	NSMutableArray *H = [[NSMutableArray alloc] initWithCapacity:n*2];
	
	//Sort points lexicographically (by X, then secondarily by Y)
	NSSortDescriptor *xDescriptor = [[NSSortDescriptor alloc] initWithKey:@"self.x" ascending:YES];
	NSSortDescriptor *yDescriptor = [[NSSortDescriptor alloc] initWithKey:@"self.y" ascending:YES];
	
	NSArray *descriptors = [NSArray arrayWithObjects:xDescriptor, yDescriptor, nil];
	NSArray *sortedP = [P sortedArrayUsingDescriptors:descriptors];
	
	//Build lower hull
	for (int i = 0; i < n; i ++) {
		while (k >= 2 && [self clockwiseO:[H objectAtIndex:k-2] A:[H objectAtIndex:k-1] B:[sortedP objectAtIndex:i]]){
			k--;
		}
		[H insertObject:[sortedP objectAtIndex:i] atIndex:k++];
	};
 	
	//Build upper hull
	for (int i = n-2, t = k + 1; i >= 0; i --) {		
		while (k >= t && [self clockwiseO:[H objectAtIndex:k-2] A:[H objectAtIndex:k-1] B:[sortedP objectAtIndex:i]]){
			k--;
		}
		[H insertObject:[sortedP objectAtIndex:i] atIndex:k++];
	};	
	
	[H removeObjectsInRange:NSMakeRange(k, H.count-k)];

	//Remove all duplicate objects
	NSMutableArray *noDupeArray = [[NSMutableArray alloc] init];
	for(int i = 0; i<H.count; i++){
		if(![noDupeArray containsObject:[H objectAtIndex:i]]){
			[noDupeArray addObject:[H objectAtIndex:i]];
		}
	}

	return noDupeArray;
}
+(float) distanceP1:(CGPoint)p1 toP2:(CGPoint)p2 {
	return sqrt( pow( (p1.x-p2.x) ,2) + pow( (p1.y-p2.y) ,2) );
}
+(float) degreesToRadians:(float)d {
	return d * (PI_CONSTANT/180);
}
+(float) radiansToDegrees:(float)r {
	return r * (180/PI_CONSTANT);
}
+(float) vectorToRadians:(CGPoint)vector{
	if(vector.y == 0){ vector.y = 0.000001f; }
	float baseRadians = atan(vector.x/vector.y);
	if(vector.y < 0){ baseRadians += PI_CONSTANT; }	//Adjust for -Y
	return baseRadians;
}
+(CGPoint) radiansToVector:(float)radians{
	return CGPointMake(sin(radians), cos(radians));
}
+(Vector3D*) quadraticA:(float)a B:(float)b C:(float)c {
  Vector3D *solution = nil;
  if ([GameHelper absoluteValue:a] < 1e-6) {
    if ([GameHelper absoluteValue:b] < 1e-6) {
      solution = [GameHelper absoluteValue:c] < 1e-6 ? [Vector3D x:0 y:0 z:0] : nil;
    } else {
      solution = [Vector3D x:-c/b y:-c/b z:0];
    }
  } else {
    float disc = b*b - 4*a*c;
    if (disc >= 0) {
      disc = sqrt(disc);
      a = 2*a;
      solution = [Vector3D x:(-b-disc)/a y:(-b+disc)/a z:0];
    }
  }
  return solution;
}
+(float) absoluteValue:(float)a {
	if(a < 0){ a*= -1; }
	return a;
}
+(CGPoint) midPointP1:(CGPoint)p1 p2:(CGPoint)p2 {
	return CGPointMake( (p1.x+p2.x)/2 , (p1.y+p2.y)/2 );
}


+(bool) point:(CGPoint)p isInRect:(CGRect)r {
	bool isInRect = false;
	if( p.x < r.origin.x + r.size.width && 
	   p.x > r.origin.x &&
	   p.y < r.origin.y + r.size.height &&
	   p.y > r.origin.y )
	{
		isInRect = true;
	}
	return isInRect;
}
+(bool) point:(CGPoint)p isInCircle:(CGPoint)origin withRadius:(float)radius {
	bool isInCircle = false;
	if([GameHelper distanceP1:p toP2:origin ] <= radius){
		isInCircle = true;
	}
	return isInCircle;
}

+(b2Vec2) b2ClampA:(const b2Vec2&)a low:(const b2Vec2&)low high:(const b2Vec2&)high {
	return b2Max(low, b2Min(a, high));
}

+(NSString*) sanitizeString:(NSString*)str {
	for(NSUInteger i=0; i<[str length]; i++) {
		unichar c = [str characterAtIndex:i];
		if(c >= kCCBMFontMaxChars) { 
			str = [str stringByReplacingCharactersInRange:NSMakeRange(i, 1) withString:@"'"];
		}
	}
	return str;
}

@end