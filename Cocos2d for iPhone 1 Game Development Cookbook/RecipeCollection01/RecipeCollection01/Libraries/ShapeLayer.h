#import "ExtraPrimitives.h"

//Our ShapeLayer Class
@interface ShapeLayer : CCLayer {
}

-(void) draw;
-(void) drawSolidCircle:(CGPoint)center radius:(float)radius segments:(int)segments;


@end

@implementation ShapeLayer

-(void) draw {
	//Set line width.
	glLineWidth(4.0f);
	
	//Set point size
	glPointSize(16);
	
	//Enable line smoothing
	glEnable(GL_LINE_SMOOTH);
		
	//Draw a blue quadratic bezier curve
	glColor4ub(0, 0, 255, 255);
	ccDrawQuadBezier(ccp(100,0), ccp(240,70), ccp(380,0), 10);
		
	//Draw a hollow purple circle
	glColor4ub(255, 0, 255, 255);
	ccDrawCircle(ccp(240,160), 125.0f, 0.0f, 100, NO);
		
	//Draw a solid red lines
	glColor4ub(255, 0, 0, 255);
	ccDrawLine(ccp(170,220), ccp(220,190));
	ccDrawLine(ccp(260,190), ccp(310,220));
	
	//Draw a green point
	glColor4ub(0, 255, 0, 255);
	ccDrawPoint(ccp(200,180));
	ccDrawPoint(ccp(280,180));
	
	//Draw a turquoise solid circle
	glColor4ub(0, 128, 255, 50);
	ccDrawSolidCircle(ccp(200,180), 25.0f, 0.0f, 20, NO);
	ccDrawSolidCircle(ccp(280,180), 25.0f, 0.0f, 20, NO);
	
	//Draw a brown hollow circle
	glColor4ub(64,32, 0, 255);
	ccDrawCircle(ccp(200,180), 25.0f, 0.0f, 100, NO);
	ccDrawCircle(ccp(280,180), 25.0f, 0.0f, 100, NO);
	
	//Draw brown lines
	glColor4ub(64,32, 0, 255);
	ccDrawLine(ccp(225,180), ccp(255,180));
	ccDrawLine(ccp(305,180), ccp(370,160));
	ccDrawLine(ccp(175,180), ccp(110,160));
	
	//Draw an orange polygon
	glColor4ub(255, 128, 0, 255);
	CGPoint vertices[5]={ ccp(230,150),ccp(240,160),ccp(250,150),ccp(245,140),ccp(235,140) };
	ccDrawPoly(vertices, 5, YES);

	//Draw a yellow cubic bezier curve
	glColor4ub(255, 255, 0, 255);
	ccDrawCubicBezier(ccp(170,90), ccp(220,150), ccp(260,50), ccp(320,100), 10);
	
	//Restore original values
	glLineWidth(1);
	glDisable(GL_LINE_SMOOTH);
	glColor4ub(255,255,255,255);
	glPointSize(1);	
}

@end