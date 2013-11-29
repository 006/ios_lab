#import "GestureLine.h"

@interface GestureShapeLayer : CCLayer {
	NSMutableArray *points;
	NSMutableArray *lines;
	CGRect circleRect;
	bool drawCircle;
	bool drawLines;
}

-(void) draw;
-(void) setCircleRectFromPoints;

@property (readwrite, assign) NSMutableArray *points;
@property (readwrite, assign) NSMutableArray *lines;
@property (readwrite, assign) CGRect circleRect;
@property (readwrite, assign) bool drawCircle;
@property (readwrite, assign) bool drawLines;

@end

@implementation GestureShapeLayer

@synthesize points, lines, circleRect, drawCircle, drawLines;

-(id)init {
    self = [super init];
    if (self != nil) {
		drawCircle = NO;
		drawLines = NO;
	}
    return self;
}

-(void) draw {
	//Set line width.
	glLineWidth(4.0f);
	
	//Set point size
	glPointSize(16);
	
	//Enable line smoothing
	glEnable(GL_LINE_SMOOTH);
	
	//Set color to blue
	glColor4ub(0, 0, 255, 255);

	//We either draw a circle or a series of lines
	if(drawCircle){
		//Draw a circle using circleRect information
		ccDrawCircle(circleRect.origin, circleRect.size.width, 0.0f, 100, NO);
	}else if(drawLines){
		//Draw all lines
		for(int i=0; i<lines.count; i++){
			GestureLine *line = (GestureLine*)[lines objectAtIndex:i];
			ccDrawLine(line.point1, line.point2);
		}
	}
		
	//Restore original values
	glLineWidth(1);
	glDisable(GL_LINE_SMOOTH);
	glColor4ub(255,255,255,255);
	glPointSize(1);	
}

-(void) setCircleRectFromPoints {
	//We approximate the circle dimensions based on the point array
	CGPoint p1 = [[points objectAtIndex:((int)points.count/2)] CGPointValue];
	CGPoint p2 = [[points objectAtIndex:0] CGPointValue];
	CGPoint midPoint = ccp((p1.x+p2.x)/2,(p1.y+p2.y)/2);

	float diameter = distanceBetweenPoints(p1,p2);
	float radius = diameter/2;
	
	circleRect = CGRectMake(midPoint.x, midPoint.y, radius, radius);
}

-(void) dealloc {
	[points release];
	[lines release];
	[super dealloc];
}

@end