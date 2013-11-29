#import "cocos2d.h"
#import "TouchableSprite.h"

@interface DPad : TouchableSprite {
	@public
		CGPoint pressedVector;
		int direction;
}

@property (readwrite, assign) CGPoint pressedVector;
@property (readwrite, assign) int direction;

-(id) init;
-(void) dealloc;
-(void) processTouchFromDirection:(int)dir;
-(void) processTouch:(CGPoint)point;
-(void) processRelease;

@end


@implementation DPad

@synthesize pressedVector, direction;

-(id) init {
    self = [super init];
    if (self != nil) {
		pressedVector = ccp(0,0);
		direction = NO_DIRECTION;
		
		CCSpriteFrameCache *cache = [CCSpriteFrameCache sharedSpriteFrameCache];
		[cache addSpriteFramesWithFile:@"dpad_buttons.plist"];
		
		//Set the sprite display frame
		[self setDisplayFrame:[cache spriteFrameByName:@"d_pad_normal.png"]];
    }
    return self;
}

-(void) dealloc {
    [super dealloc];
}

-(void) processTouchFromDirection:(int)dir {
	//NSLog(@"TOUCH DIR");
	
	if(dir == NO_DIRECTION){
		[self processRelease];
	}else if(dir == LEFT){
		[self processTouch:ccp(-50,0)];
	}else if(dir == UP_LEFT){
		[self processTouch:ccp(-50,50)];
	}else if(dir == UP_LEFT){
		[self processTouch:ccp(-50,50)];
	}else if(dir == UP){
		[self processTouch:ccp(0,50)];
	}else if(dir == UP_RIGHT){
		[self processTouch:ccp(50,50)];
	}else if(dir == RIGHT){
		[self processTouch:ccp(50,0)];
	}else if(dir == DOWN_RIGHT){
		[self processTouch:ccp(50,-50)];
	}else if(dir == DOWN){
		[self processTouch:ccp(0,-50)];
	}else if(dir == DOWN_LEFT){
		[self processTouch:ccp(-50,-50)];
	}
}

-(void) processTouch:(CGPoint)point {
	//NSLog(@"Processing touch");

	CCSpriteFrameCache *cache = [CCSpriteFrameCache sharedSpriteFrameCache];
	
	//Set a color visual cue if pressed
	[self setColor:ccc3(255,200,200)];
	pressed = true;
		
	CGPoint center = CGPointMake( self.rect.origin.x+self.rect.size.width/2, self.rect.origin.y+self.rect.size.height/2 );
	
	//Process center dead zone
	if([GameHelper distanceP1:point toP2:center] < self.rect.size.width/10){
		//NSLog(@"NO TOUCH (dead zone)");
	
		[self setDisplayFrame:[cache spriteFrameByName:@"d_pad_normal.png"]];
		self.rotation = 0;
		pressedVector = ccp(0,0);
		direction = NO_DIRECTION;
		return;
	}
	
	//Process direction
	float radians = [GameHelper vectorToRadians:CGPointMake(point.x-center.x, point.y-center.y)];
	float degrees = [GameHelper radiansToDegrees:radians] + 90;

	float sin45 = 0.7071067812f;
	
	if(degrees >= 337.5 || degrees < 22.5){
		[self setDisplayFrame:[cache spriteFrameByName:@"d_pad_horizontal.png"]];
		self.rotation = 180; pressedVector = ccp(-1,0); direction = LEFT;
	}else if(degrees >= 22.5 && degrees < 67.5){
		[self setDisplayFrame:[cache spriteFrameByName:@"d_pad_diagonal.png"]];
		self.rotation = -90; pressedVector = ccp(-sin45,sin45); direction = UP_LEFT;
	}else if(degrees >= 67.5 && degrees < 112.5){
		[self setDisplayFrame:[cache spriteFrameByName:@"d_pad_horizontal.png"]];
		self.rotation = -90; pressedVector = ccp(0,1); direction = UP;
	}else if(degrees >= 112.5 && degrees < 157.5){
		[self setDisplayFrame:[cache spriteFrameByName:@"d_pad_diagonal.png"]];
		self.rotation = 0; pressedVector = ccp(sin45,sin45); direction = UP_RIGHT;
	}else if(degrees >= 157.5 && degrees < 202.5){
		[self setDisplayFrame:[cache spriteFrameByName:@"d_pad_horizontal.png"]];
		self.rotation = 0; pressedVector = ccp(1,0); direction = RIGHT;
	}else if(degrees >= 202.5 && degrees < 247.5){
		[self setDisplayFrame:[cache spriteFrameByName:@"d_pad_diagonal.png"]];
		self.rotation = 90; pressedVector = ccp(sin45,-sin45); direction = DOWN_RIGHT;
	}else if(degrees >= 247.5 && degrees < 292.5){
		[self setDisplayFrame:[cache spriteFrameByName:@"d_pad_horizontal.png"]];
		self.rotation = 90; pressedVector = ccp(0,-1); direction = DOWN;
	}else{
		[self setDisplayFrame:[cache spriteFrameByName:@"d_pad_diagonal.png"]];
		self.rotation = 180; pressedVector = ccp(-sin45,-sin45); direction = DOWN_LEFT;
	}
}

-(void) processRelease {
	[self setColor:ccc3(255,255,255)];
	
	CCSpriteFrameCache *cache = [CCSpriteFrameCache sharedSpriteFrameCache];
	[self setDisplayFrame:[cache spriteFrameByName:@"d_pad_normal.png"]];
	self.rotation = 0;
	pressed = false;
	pressedVector = ccp(0,0);
	direction = NO_DIRECTION;
}

@end