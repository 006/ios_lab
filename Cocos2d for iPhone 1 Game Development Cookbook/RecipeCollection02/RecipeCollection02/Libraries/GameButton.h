#import "cocos2d.h"
#import "TouchableSprite.h"

@interface GameButton : TouchableSprite {
	@public
		NSString* upSpriteFrame;
		NSString* downSpriteFrame;
		NSString* name;
}

@property (readwrite, assign) NSString* upSpriteFrame;
@property (readwrite, assign) NSString* downSpriteFrame;
@property (readwrite, assign) NSString* name;

-(id)init;
-(void)dealloc;
-(void)processTouch:(CGPoint)point;
-(void)processRelease;

@end


@implementation GameButton

@synthesize upSpriteFrame, downSpriteFrame, name;

-(id)init {
    self = [super init];
    if (self != nil) {

    }
    return self;
}

-(void)dealloc {
    [super dealloc];
}

- (void)processTouch:(CGPoint)point {
	CCSpriteFrameCache *cache = [CCSpriteFrameCache sharedSpriteFrameCache];
	[self setDisplayFrame:[cache spriteFrameByName:downSpriteFrame]];
	pressed = true;
	[self setColor:ccc3(255,200,200)];
}

- (void)processRelease {
	CCSpriteFrameCache *cache = [CCSpriteFrameCache sharedSpriteFrameCache];
	[self setDisplayFrame:[cache spriteFrameByName:upSpriteFrame]];
	pressed = false;
	[self setColor:ccc3(255,255,255)];
}

@end