#import "cocos2d.h"
#import "TouchableSprite.h"

enum {
	DPAD_NO_DIRECTION,
	DPAD_UP,
	DPAD_UP_RIGHT,
	DPAD_RIGHT,
	DPAD_DOWN_RIGHT,
	DPAD_DOWN,
	DPAD_DOWN_LEFT,
	DPAD_LEFT,
	DPAD_UP_LEFT
};

@interface DPad : TouchableSprite {
	@public
		CGPoint pressedVector;
		int direction;
}

@property (readwrite, assign) CGPoint pressedVector;
@property (readwrite, assign) int direction;

-(id)init;
-(void)dealloc;
-(void)processTouch:(CGPoint)point;
-(void)processRelease;

@end


@implementation DPad

@synthesize pressedVector, direction;

-(id)init {
    self = [super init];
    if (self != nil) {
		pressedVector = ccp(0,0);
		direction = DPAD_NO_DIRECTION;
		
		CCSpriteFrameCache *cache = [CCSpriteFrameCache sharedSpriteFrameCache];
		[cache addSpriteFramesWithFile:@"dpad_buttons.plist"];
		
		//Set the sprite display frame
		[self setDisplayFrame:[cache spriteFrameByName:@"d_pad_normal.png"]];
    }
    return self;
}

-(void)dealloc {
    [super dealloc];
}

- (void)processTouch:(CGPoint)point {
	CCSpriteFrameCache *cache = [CCSpriteFrameCache sharedSpriteFrameCache];
	
	//Set a color visual cue if pressed
	[self setColor:ccc3(255,200,200)];
	pressed = true;
		
	CGPoint center = CGPointMake( self.rect.origin.x+self.rect.size.width/2, self.rect.origin.y+self.rect.size.height/2 );
	
	//Process center dead zone
	if(distanceBetweenPoints(point, center) < self.rect.size.width/10){
		[self setDisplayFrame:[cache spriteFrameByName:@"d_pad_normal.png"]];
		self.rotation = 0;
		pressedVector = ccp(0,0);
		direction = DPAD_NO_DIRECTION;
		return;
	}
	
	//Process direction
	float radians = vectorToRadians( CGPointMake(point.x-center.x, point.y-center.y) );
	float degrees = radiansToDegrees(radians) + 90;

	float sin45 = 0.7071067812f;
	
	if(degrees >= 337.5 || degrees < 22.5){
		[self setDisplayFrame:[cache spriteFrameByName:@"d_pad_horizontal.png"]];
		self.rotation = 180; pressedVector = ccp(-1,0); direction = DPAD_LEFT;
	}else if(degrees >= 22.5 && degrees < 67.5){
		[self setDisplayFrame:[cache spriteFrameByName:@"d_pad_diagonal.png"]];
		self.rotation = -90; pressedVector = ccp(-sin45,sin45); direction = DPAD_UP_LEFT;
	}else if(degrees >= 67.5 && degrees < 112.5){
		[self setDisplayFrame:[cache spriteFrameByName:@"d_pad_horizontal.png"]];
		self.rotation = -90; pressedVector = ccp(0,1); direction = DPAD_UP;
	}else if(degrees >= 112.5 && degrees < 157.5){
		[self setDisplayFrame:[cache spriteFrameByName:@"d_pad_diagonal.png"]];
		self.rotation = 0; pressedVector = ccp(sin45,sin45); direction = DPAD_UP_RIGHT;
	}else if(degrees >= 157.5 && degrees < 202.5){
		[self setDisplayFrame:[cache spriteFrameByName:@"d_pad_horizontal.png"]];
		self.rotation = 0; pressedVector = ccp(1,0); direction = DPAD_RIGHT;
	}else if(degrees >= 202.5 && degrees < 247.5){
		[self setDisplayFrame:[cache spriteFrameByName:@"d_pad_diagonal.png"]];
		self.rotation = 90; pressedVector = ccp(sin45,-sin45); direction = DPAD_DOWN_RIGHT;
	}else if(degrees >= 247.5 && degrees < 292.5){
		[self setDisplayFrame:[cache spriteFrameByName:@"d_pad_horizontal.png"]];
		self.rotation = 90; pressedVector = ccp(0,-1); direction = DPAD_DOWN;
	}else{
		[self setDisplayFrame:[cache spriteFrameByName:@"d_pad_diagonal.png"]];
		self.rotation = 180; pressedVector = ccp(-sin45,-sin45); direction = DPAD_DOWN_LEFT;
	}
}

- (void)processRelease {
	[self setColor:ccc3(255,255,255)];
	
	CCSpriteFrameCache *cache = [CCSpriteFrameCache sharedSpriteFrameCache];
	[self setDisplayFrame:[cache spriteFrameByName:@"d_pad_normal.png"]];
	self.rotation = 0;
	pressed = false;
	pressedVector = ccp(0,0);
	direction = DPAD_NO_DIRECTION;
}

@end