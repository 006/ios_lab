@interface GestureLine : NSObject {
@public
	CGPoint point1;
	CGPoint point2;
}
@property (readwrite, assign) CGPoint point1;
@property (readwrite, assign) CGPoint point2;

+(id) point1:(CGPoint)p1 point2:(CGPoint)p2;
-(id) point1:(CGPoint)p1 point2:(CGPoint)p2;

@end

@implementation GestureLine
@synthesize point1, point2;


+(id) point1:(CGPoint)p1 point2:(CGPoint)p2 {
	return [[self alloc] point1:p1 point2:p2];
}

-(id) point1:(CGPoint)p1 point2:(CGPoint)p2 {
	if(self = [super init]){
		self.point1 = p1;
		self.point2 = p2;
	}
	return self;
}

@end