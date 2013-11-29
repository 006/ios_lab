#import "cocos2d.h"
#import "TouchableSprite.h"

enum {
	AS_NO_DIRECTION,
	AS_UP,
	AS_UP_RIGHT,
	AS_RIGHT,
	AS_DOWN_RIGHT,
	AS_DOWN,
	AS_DOWN_LEFT,
	AS_LEFT,
	AS_UP_LEFT
};

@interface AnalogStick : TouchableSprite {
	@public
		CGPoint _pressedVector;	//Internal _pressedVector with no outer dead zone
		CCSprite *nub;
		CCSprite *bar;
		int direction;
}

@property (readonly) CGPoint pressedVector;	//External pressedVector with a dead zone
@property (nonatomic, retain) CCSprite *nub;
@property (nonatomic, retain) CCSprite *bar;
@property (readwrite, assign) int direction;

-(id)init;
-(void)dealloc;
-(void)processTouch:(CGPoint)point;
-(void)processRelease;
-(void)repositionNub;
-(void)repositionBarWithPoint:(CGPoint)point;
-(void)resetDirection;
-(float)magnitude;

@end


@implementation AnalogStick

@synthesize pressedVector, nub, bar, direction;

-(id)init {
    self = [super init];
    if (self != nil) {
		self.scale = 0.5f;
		
		_pressedVector = ccp(0,0);
		
		CCSpriteFrameCache *cache = [CCSpriteFrameCache sharedSpriteFrameCache];
		[cache addSpriteFramesWithFile:@"analog_stick.plist"];
		
		//Set the sprite display frame
		[self setDisplayFrame:[cache spriteFrameByName:@"analog_pad.png"]];
		
		//Init the bar, set position and display frame
		bar = [[CCSprite alloc] init];
		[bar setDisplayFrame:[cache spriteFrameByName:@"analog_bar.png"]];
		[self repositionBarWithPoint:self.position];
		[self addChild:bar];
		
		//Init the nub, set position and display frame
		nub = [[CCSprite alloc] init];
		[self repositionNub];
		[nub setDisplayFrame:[cache spriteFrameByName:@"analog_nub.png"]];
		[self addChild:nub];
	}
    return self;
}

-(void)dealloc {
    [super dealloc];
}

-(void)processTouch:(CGPoint)point {
	self.pressed = YES;
	[self setColor:ccc3(255,200,200)];
	[nub setColor:ccc3(255,200,200)];
	[bar setColor:ccc3(255,200,200)];

	CGPoint center = CGPointMake( self.rect.origin.x+self.rect.size.width/2, self.rect.origin.y+self.rect.size.height/2 );
	_pressedVector = CGPointMake((point.x-center.x)/(self.rect.size.width/2), (point.y-center.y)/(self.rect.size.height/2));

	[self repositionNub];
	[self repositionBarWithPoint:point];
	[self resetDirection];
}

-(void)processRelease {
	[self setColor:ccc3(255,255,255)];
	[nub setColor:ccc3(255,255,255)];
	[bar setColor:ccc3(255,255,255)];
	
	self.pressed = NO;
	_pressedVector = ccp(0,0);
	[self repositionNub];
	[self repositionBarWithPoint:self.position];
}

-(void)repositionNub {
	float width = ([self contentSize].width);
	float height = ([self contentSize].height);

	nub.position = ccp(_pressedVector.x*(width/2)+width/2, 
		_pressedVector.y*(height/2)+height/2);
}

-(void)repositionBarWithPoint:(CGPoint)point {
	float width = ([self contentSize].width);
	float height = ([self contentSize].height);
		
	//Rotation	
	float radians = vectorToRadians( _pressedVector );
	float degrees = radiansToDegrees(radians);	
	bar.rotation = degrees;
	
	CCSpriteFrameCache *cache = [CCSpriteFrameCache sharedSpriteFrameCache];
	[bar setDisplayFrame:[cache spriteFrameByName:@"analog_bar.png"]];
	
	float distFromCenter = distanceBetweenPoints(point, self.position);
	float sizeMod = distFromCenter / [self contentSize].width;
	float oldHeight = bar.textureRect.size.height;
	float newHeight = oldHeight * sizeMod * 5;
	
	//Custom fixes
	if(newHeight < 100){ newHeight = 100.0f; }
	if(distFromCenter < 3){ newHeight = 0.0f; }
	
	bar.textureRect = CGRectMake(bar.textureRect.origin.x,bar.textureRect.origin.y+ (oldHeight-newHeight),
		bar.textureRect.size.width,newHeight );
	
	bar.anchorPoint = ccp(0.5f,0);
	CGPoint directionVector = radiansToVector(radians-PI/2);
	bar.position = ccp(width/2 + directionVector.x*width/4, height/2 + directionVector.y*height/4);
}

-(void) resetDirection {
	if(_pressedVector.x == 0 && _pressedVector.y == 0){
		direction = AS_NO_DIRECTION;
		return;
	}

	float radians = vectorToRadians(_pressedVector);
	float degrees = radiansToDegrees(radians) + 90;
	
	if(degrees >= 337.5 || degrees < 22.5){
		direction = AS_LEFT;
	}else if(degrees >= 22.5 && degrees < 67.5){
		direction = AS_UP_LEFT;
	}else if(degrees >= 67.5 && degrees < 112.5){
		direction = AS_UP;
	}else if(degrees >= 112.5 && degrees < 157.5){
		direction = AS_UP_RIGHT;
	}else if(degrees >= 157.5 && degrees < 202.5){
		direction = AS_RIGHT;
	}else if(degrees >= 202.5 && degrees < 247.5){
		direction = AS_DOWN_RIGHT;
	}else if(degrees >= 247.5 && degrees < 292.5){
		direction = AS_DOWN;
	}else{
		direction = AS_DOWN_LEFT;
	}
}

-(float) magnitude {	
	float m = sqrt( pow(_pressedVector.x,2) + pow(_pressedVector.y,2) );

	//25% end deadzone to make it easier to hold highest magnitude
	m += 0.25f;
	if(m > 1.0f){ m = 1.0f; }
	
	return m;
}

-(CGPoint) pressedVector {
	float m = sqrt( pow(_pressedVector.x,2) + pow(_pressedVector.y,2) );
	m += 0.25f;
	
	CGPoint pv = ccp(_pressedVector.x*1.25f, _pressedVector.y*1.25f);
	
	//25% end deadzone to make it easier to hold highest magnitude
	if(m > 1){
		float radians = vectorToRadians(_pressedVector);
		pv = radiansToVector(radians + PI/2);
	}
	
	return pv;
}

@end