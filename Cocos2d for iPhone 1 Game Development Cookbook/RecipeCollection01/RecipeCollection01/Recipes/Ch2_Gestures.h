#import "cocos2d.h"
#import "Recipe.h"
#import "GestureShapeLayer.h"

//Interface

@interface Ch2_Gestures : Recipe
{
	CCNode *breadcrumbs;
	GestureShapeLayer *gestureShapeLayer;

	//Gesture variables
	CGPoint firstPoint;
	CGPoint lastPoint;
	float angle;
	CGPoint vertex;
}

-(CCLayer*) runRecipe;
-(void) resetMessage;
-(void) addBreadcrumbs;
-(void) removeBreadcrumbs;
-(void) addCrumbWithPosition:(CGPoint)p;

-(void) newGestureWithPoint:(CGPoint)point;
-(void) addGesturePoint:(CGPoint)point override:(bool)override;
-(void) finishGestureWithPoint:(CGPoint)point;

@end

//Implementation

@implementation Ch2_Gestures

-(CCLayer*) runRecipe {
	//Init message
	message = [CCLabelBMFont labelWithString:@"Draw a rectangle, triangle, circle or line" fntFile:@"eurostile_30.fnt"];
	message.position = ccp(200,270);
	message.scale = 0.65f;
	[message setColor:ccc3(255,0,0)];
	[self addChild:message z:3];
	
	//Allow touching
	self.isTouchEnabled = YES;
	
	//Set font size
	[CCMenuItemFont setFontSize:20];

	//Add our breadcrumbs node
	[self addBreadcrumbs];
	
	//Init GestureShapeLayer
	gestureShapeLayer = [[GestureShapeLayer alloc] init];
	gestureShapeLayer.position = ccp(0,0);
	[self addChild:gestureShapeLayer z:1];
		
	return self;
}

-(void) resetMessage {
	[message setString:@"Draw a rectangle, triangle, circle or line"];
}

-(void) addBreadcrumbs {
	breadcrumbs = [[CCNode alloc] init];
	breadcrumbs.position = ccp(0,0);
	[self addChild:breadcrumbs z:0];
}

-(void) removeBreadcrumbs {
	[self removeChild:breadcrumbs cleanup:YES];
	breadcrumbs = nil;
}

-(void) addCrumbWithPosition:(CGPoint)p {
	CCSprite *crumb = [CCSprite spriteWithFile:@"blank.png"];
	[crumb setTextureRect:CGRectMake(0,0,2,2)];
	[crumb setColor:ccc3(255,255,0)];
	crumb.position = p;
	[breadcrumbs addChild:crumb];
}

/* Process touches */
-(void) ccTouchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {	
	UITouch *touch = [touches anyObject];
    CGPoint point = [touch locationInView: [touch view]];
	point = [[CCDirector sharedDirector] convertToGL: point];
	
	//Add a breadcrumb
	[self addCrumbWithPosition:point];
	
	//Start a new gesture
	[self newGestureWithPoint:point];
}
-(void) ccTouchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {	
	UITouch *touch = [touches anyObject];
    CGPoint point = [touch locationInView: [touch view]];
	point = [[CCDirector sharedDirector] convertToGL: point];
	
	//Add another breadcrumb
	[self addCrumbWithPosition:point];

	//Add a point to our current gesture
	[self addGesturePoint:point override:NO];
}
-(void) ccTouchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {	
	UITouch *touch = [touches anyObject];
    CGPoint point = [touch locationInView: [touch view]];
	point = [[CCDirector sharedDirector] convertToGL: point];

	//Reset breadcrumbs
	[self removeBreadcrumbs];
	[self addBreadcrumbs];
		
	//Finish our gesture
	[self finishGestureWithPoint:point];
}

-(void) newGestureWithPoint:(CGPoint)point {
	[self resetMessage];

	//Init gesture variables
	gestureShapeLayer.points = [[NSMutableArray alloc] init];
	gestureShapeLayer.lines = [[NSMutableArray alloc] init];
	firstPoint = point;
	lastPoint = point;
	vertex = point;
	[gestureShapeLayer.points addObject:[NSValue valueWithCGPoint:point]];
	
	gestureShapeLayer.drawCircle = NO;
	gestureShapeLayer.drawLines = NO;
}

-(void) addGesturePoint:(CGPoint)point override:(bool)override {
	//Set our angle change tolerance to 40 degrees. If it changes more than this we consider this a 'line'
	float angleDiffTolerance = 40.0f;
	
	//Check the old angle versus the new one
	CGPoint vect = ccp(point.x-lastPoint.x, point.y-lastPoint.y);
	float newAngle = radiansToDegrees( vectorToRadians(vect) );
	
	//Add a line if the angle changed significantly
	if(gestureShapeLayer.points.count > 1){
		float angleDiff = angleDifference(newAngle, angle);
		if(override || (angleDiff > angleDiffTolerance && distanceBetweenPoints(vertex, point) > 15.0f)){
			[gestureShapeLayer.lines addObject:[GestureLine point1:vertex point2:point]];
			vertex = point;
		}
	}
	
	//Update values
	angle = newAngle;
	lastPoint = point;
	[gestureShapeLayer.points addObject:[NSValue valueWithCGPoint:point]];
}

-(void) finishGestureWithPoint:(CGPoint)point {
	[self addGesturePoint:point override:YES];
	
	gestureShapeLayer.drawCircle = NO;
	gestureShapeLayer.drawLines = NO;
	
	//To finish gestures which require the end to be close to the beginning point we supply this distance tolerance
	float lastPointTolerance = 100.0f;
	
	//Rectangles, triangles and circles
	if(distanceBetweenPoints(firstPoint, lastPoint) <= lastPointTolerance){
		if(gestureShapeLayer.lines.count == 4){	//4 lines
			[message setString:@"Rectangle"];
			gestureShapeLayer.drawLines = YES;
		}else if(gestureShapeLayer.lines.count == 3){	//3 lines
			[message setString:@"Triangle"];
			gestureShapeLayer.drawLines = YES;
		}else if(gestureShapeLayer.lines.count <= 1){	//0 or 1 lines
			[message setString:@"Circle"];
			[gestureShapeLayer setCircleRectFromPoints];
			gestureShapeLayer.drawCircle = YES;
		}else{
			[self resetMessage];
			gestureShapeLayer.lines = [[NSMutableArray alloc] init];
		}
	}else{	//Lines and angles
		if(gestureShapeLayer.lines.count == 1){	//1 line
			[message setString:@"Line"];
			gestureShapeLayer.drawLines = YES;
		}else if(gestureShapeLayer.lines.count == 2){	//2 lines
			[message setString:@"Angle"];
			gestureShapeLayer.drawLines = YES;
		}else{
			[self resetMessage];
			gestureShapeLayer.lines = [[NSMutableArray alloc] init];
		}
	}
}

@end
