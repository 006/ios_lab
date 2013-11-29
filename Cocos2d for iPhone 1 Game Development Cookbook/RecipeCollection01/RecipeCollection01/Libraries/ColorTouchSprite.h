#import "cocos2d.h"

enum {
	TS_NONE,
	TS_TAP,
	TS_HOLD,
	TS_DRAG
};

@interface ColorTouchSprite : CCSprite
{
	@public
		int holdTime;				//How long have we held down on this?
		int touchedState;			//Current touched state
		bool isTouched;				//Are we touching this currently?
		int lastMoved;				//How long has it been since we moved this?
		CGPoint lastTouchedPoint;	//Where did we last touch?
}

@property (readwrite, assign) int touchedState;

-(id) init;
-(void) step;
-(CGRect) rect;
-(void) ccTouchesBegan:(NSSet *)touches withEvent:(UIEvent *)event;
-(void) ccTouchesMoved:(NSSet *)touches withEvent:(UIEvent *)event;
-(void) ccTouchesEnded:(NSSet *)touches withEvent:(UIEvent *)event;

@end

//Implementation
@implementation ColorTouchSprite

@synthesize touchedState;

-(id) init {
	holdTime = 0;
	lastMoved = 0;
	touchedState = TS_NONE;
	isTouched = NO;
	lastTouchedPoint = ccp(0,0);
	
	[self schedule:@selector(step)];
	
	return [super init];
}

-(void) step {
	//We use holdTime to determine the difference between a tap and a hold
	if(isTouched){
		holdTime += 1;
		lastMoved += 1;
	}else{
		holdTime += 1;
		if(holdTime > 60){
			touchedState = TS_NONE;
		}
	}
	
	//If you are holding and you haven't moved in a while change the state
	if(holdTime > 10 && isTouched && lastMoved > 30){
		touchedState = TS_HOLD;
	}
}

- (CGRect) rect {
	float scaleMod = 1.0f;
	float w = [self contentSize].width * [self scale] * scaleMod;
	float h = [self contentSize].height * [self scale] * scaleMod;
	CGPoint point = CGPointMake([self position].x - (w/2), [self position].y - (h/2));
	
	return CGRectMake(point.x, point.y, w, h); 
}

/* Process touches */
-(void) ccTouchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {	
	UITouch *touch = [touches anyObject];
    CGPoint point = [touch locationInView: [touch view]];
	point = [[CCDirector sharedDirector] convertToGL: point];
	
	isTouched = YES;
	holdTime = 0;
	touchedState = TS_NONE;
	
	lastTouchedPoint = point;
}
-(void) ccTouchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {	
	if(!isTouched){ return; }
	
	UITouch *touch = [touches anyObject];
    CGPoint point = [touch locationInView: [touch view]];
	point = [[CCDirector sharedDirector] convertToGL: point];
		
	//We have a drag threshold of 3 pixels.
	if(touchedState == TS_DRAG || distanceBetweenPoints(lastTouchedPoint, point) > 3){
		touchedState = TS_DRAG;
		self.position = point;
		lastMoved = 0;
	}
	lastTouchedPoint = point;
}
-(void) ccTouchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {	
	if(!isTouched){ return; }
	
	UITouch *touch = [touches anyObject];
    CGPoint point = [touch locationInView: [touch view]];
	point = [[CCDirector sharedDirector] convertToGL: point];

	//A short hold time after a touch ended means a tap.
	if(holdTime < 10){
		touchedState = TS_TAP;
	}
	holdTime = 0;
	isTouched = NO;
	
	lastTouchedPoint = point;
}

@end