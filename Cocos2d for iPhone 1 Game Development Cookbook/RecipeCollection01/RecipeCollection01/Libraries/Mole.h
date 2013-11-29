#import "cocos2d.h"
#import "TouchableSprite.h"

enum {
	MOLE_MOVING_UP,
	MOLE_UP,
	MOLE_MOVING_DOWN,
	MOLE_DOWN,
	MOLE_HIT
};

@interface Mole : TouchableSprite {
	@public
		int state;
		CCAnimation *normalAnim;
		CCAnimation *hitAnim;
		CGPoint downPosition;
}

@property (readwrite, assign) int state;

-(id)init;
-(void)dealloc;
-(CGRect)rect;
-(void)processTouch:(CGPoint)point;
-(void)processRelease;
-(void)setDownPosition:(CGPoint)point;
-(void)startPopUp;
-(void)finishPopUp;
-(void)startHideDown;
-(void)finishHideDown;
-(void)gotHit;

@end


@implementation Mole

@synthesize state;

-(id)init {
    self = [super init];
    if (self != nil) {
		state = MOLE_DOWN;
		
		CCSpriteFrameCache *cache = [CCSpriteFrameCache sharedSpriteFrameCache];
		
		normalAnim = [[CCAnimation alloc] initWithName:@"mole_normal" delay:1.0f];
		[normalAnim addFrame:[cache spriteFrameByName:@"mole_normal.png"]];
		
		hitAnim = [[CCAnimation alloc] initWithName:@"mole_hit" delay:1.0f];
		[hitAnim addFrame:[cache spriteFrameByName:@"mole_hit.png"]];
		
		[self runAction:[CCRepeatForever actionWithAction: [CCAnimate actionWithAnimation:normalAnim]]];
	}
    return self;
}

-(void)dealloc {
    [super dealloc];
}

- (CGRect) rect {
	//We set our scale mod to make sprite easier to press.
	//This also lets us press 2 sprites with 1 touch if they are sufficiently close.
	float scaleMod = 1.5f;
	float w = [self contentSize].width * [self scale] * scaleMod;
	float h = [self contentSize].height * [self scale] * scaleMod;
	CGPoint point = CGPointMake([self position].x - (w/2), [self position].y - (h/2));
	
	return CGRectMake(point.x, point.y+50, w, h/2); 
}

- (void)ccTouchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {
	UITouch *touch = [touches anyObject];
    CGPoint point = [touch locationInView: [touch view]];
	point = [[CCDirector sharedDirector] convertToGL: point];
	
	if(pointIsInRect(point, self.rect)){
		touchHash = [touch hash];
		[self processTouch:point];
	}
}

-(void)setDownPosition:(CGPoint)point {
	downPosition = point;
	self.position = point;
}


-(void)processTouch:(CGPoint)point {
	if(state == MOLE_UP){
		[self gotHit];
	}
}

-(void)startPopUp {
	[self setColor:ccc3(255,255,255)];

	state = MOLE_MOVING_UP;
	
	[self stopAllActions];
	[self runAction:[CCRepeatForever actionWithAction: [CCAnimate actionWithAnimation:normalAnim]]];	
	[self runAction:[CCSequence actions:
		[CCMoveTo actionWithDuration:0.25f position:ccp(downPosition.x,downPosition.y+20)],
		[CCCallFunc actionWithTarget:self selector:@selector(finishPopUp)],
		nil
	]];
}

-(void)finishPopUp {
	state = MOLE_UP;
}

-(void)startHideDown {
	state = MOLE_MOVING_DOWN;
	[self runAction:[CCSequence actions:
		[CCMoveTo actionWithDuration:0.25f position:ccp(downPosition.x,downPosition.y)],
		[CCCallFunc actionWithTarget:self selector:@selector(finishHideDown)],
		nil
	]];
}

-(void)finishHideDown {
	state = MOLE_DOWN;
	[self runAction:[CCRepeatForever actionWithAction: [CCAnimate actionWithAnimation:normalAnim]]];
	[self setColor:ccc3(255,255,255)];
}

-(void)gotHit {
	[self.parent processMoleHit];

	[self setColor:ccc3(255,100,100)];

	state = MOLE_HIT;

	[self stopAllActions];
	[self runAction:[CCRepeatForever actionWithAction: [CCAnimate actionWithAnimation:hitAnim]]];

	[self runAction:[CCSequence actions:
		[CCEaseInOut actionWithAction:[CCMoveTo actionWithDuration:2 position:ccp(downPosition.x,downPosition.y)] rate:2],
		[CCCallFunc actionWithTarget:self selector:@selector(finishHideDown)],
		nil
	]];
}

@end