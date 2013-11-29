#import "cocos2d.h"

@interface TouchableSprite : CCSprite
{
	@public
		bool pressed;			//Is this sprite pressed
		NSUInteger touchHash;	//Used to identify individual touches
}

@property (readwrite, assign) bool pressed;
@property (readwrite, assign) NSUInteger touchHash;

- (id)init;
- (bool)checkTouchWithPoint:(CGPoint)point;
- (CGRect)rect;
- (void)processTouch:(CGPoint)point;
- (void)processRelease;
- (void)ccTouchesBegan:(NSSet *)touches withEvent:(UIEvent *)event;
- (void)ccTouchesMoved:(NSSet *)touches withEvent:(UIEvent *)event;
- (void)ccTouchesEnded:(NSSet *)touches withEvent:(UIEvent *)event;

@end

@implementation TouchableSprite

@synthesize pressed, touchHash;

-(id)init {
    self = [super init];
    if (self != nil) {
		pressed = NO;
    }
    return self;
}

-(void)dealloc {
    [super dealloc];
}

- (bool)checkTouchWithPoint:(CGPoint)point {
	if(pointIsInRect(point, [self rect])){
		return YES;
	}else{
		return NO;
	}
}

- (CGRect) rect {
	//We set our scale mod to make sprite easier to press.
	//This also lets us press 2 sprites with 1 touch if they are sufficiently close.
	float scaleMod = 1.5f;
	float w = [self contentSize].width * [self scale] * scaleMod;
	float h = [self contentSize].height * [self scale] * scaleMod;
	CGPoint point = CGPointMake([self position].x - (w/2), [self position].y - (h/2));
	
	return CGRectMake(point.x, point.y, w, h); 
}

- (void)ccTouchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {
	UITouch *touch = [touches anyObject];
    CGPoint point = [touch locationInView: [touch view]];
	point = [[CCDirector sharedDirector] convertToGL: point];

	//We use circle collision for our buttons
	if(pointIsInCircle(point, self.position, self.rect.size.width/2)){		
		touchHash = [touch hash];
		[self processTouch:point];
	}
}

- (void)ccTouchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {
	UITouch *touch = [touches anyObject];
    CGPoint point = [touch locationInView: [touch view]];
	point = [[CCDirector sharedDirector] convertToGL: point];
	
	if(pointIsInCircle(point, self.position, self.rect.size.width/2)){
		if(touchHash == [touch hash]){		//If we moved on this sprite
			[self processTouch:point];
		}else if(!pressed){					//If a new touch moves onto this sprite
			touchHash = [touch hash];
			[self processTouch:point];
		}
	}else if(touchHash == [touch hash]){	//If we moved off of this sprite
		[self processRelease];
	}
}

- (void)ccTouchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {
	UITouch *touch = [touches anyObject];	
	CGPoint point = [touch locationInView: [touch view]];
	point = [[CCDirector sharedDirector] convertToGL: point];
	
	if(touchHash == [touch hash]){	//If the touch which pressed this sprite ended we release
		[self processRelease];
	}
}

- (void)processTouch:(CGPoint)point {
	pressed = YES;
}

- (void)processRelease {
	pressed = NO;
}

@end