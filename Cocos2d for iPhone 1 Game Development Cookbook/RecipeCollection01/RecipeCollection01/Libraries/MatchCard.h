#import "cocos2d.h"

@interface MatchCard : CCNode
{
	@public
		CCSprite *cardUp;
		CCSprite *cardDown;
		bool cardIsUp;
		bool animating;
		bool hasBeenFlipped;
		NSString* cardType;
}

@property (readwrite, assign) bool cardIsUp;
@property (nonatomic, retain) CCSprite *cardUp;
@property (nonatomic, retain) CCSprite *cardDown;
@property (nonatomic, retain) NSString *cardType;

+(id)createWithSpriteFrameName:(NSString*)frameName;
-(id)initWithSpriteFrameName:(NSString*)frameName;
-(void)setAllPositions:(CGPoint)position;
-(CGRect)rect;
-(void)flipCard;
-(void)flipCardNoAnim;

@end

@implementation MatchCard

@synthesize cardIsUp, cardDown, cardUp, cardType;

+(id)createWithSpriteFrameName:(NSString*)frameName {
	return [[self alloc] initWithSpriteFrameName:frameName];
}

-(id)initWithSpriteFrameName:(NSString*)frameName {
    self = [super init];
    if (self != nil) {
		CCSpriteFrameCache *cache = [CCSpriteFrameCache sharedSpriteFrameCache];
		//[cache addSpriteFramesWithFile:@"match_card.plist"];
					
		cardIsUp = YES;
		animating = NO;
		hasBeenFlipped = NO;
		
		cardDown = [CCSprite spriteWithSpriteFrame:[cache spriteFrameByName:@"match_card_back.png"]];
		cardDown.opacity = 0;
		cardDown.scale = 0.5f;
		[cardDown retain];
	
		cardUp = [CCSprite spriteWithSpriteFrame:[cache spriteFrameByName:frameName]];
		cardUp.opacity = 255;
		cardUp.scale = 0.5f;
		[cardUp retain];
		
		cardType = frameName;
    }
    return self;
}

-(void)dealloc {
    [super dealloc];
	[cardDown release];
	[cardUp release];
}

-(void) setAllPositions:(CGPoint)position {
	cardDown.position = position;
	cardUp.position = position;
	self.position = position;
}

-(void) flipCardNoAnim {
	if(animating){
		return;
	}
	
	if(!cardIsUp){
		cardUp.opacity = 255;
		cardDown.opacity = 0;
		cardIsUp = YES;
	}else{
		cardDown.opacity = 255;
		cardUp.opacity = 0;
		cardIsUp = NO;
	}
}

-(void) flipCard {
	if(animating){
		return;
	}
	
	hasBeenFlipped = YES;
	
	cardUp.position = self.position;
	cardDown.position = self.position;
	cardUp.opacity = 0;
	cardDown.opacity = 0;
	
	CCActionInterval* flipCardIn = 
		[CCSequence actions:
			[CCSpawn actions:
				[CCSequence actions:[CCScaleTo actionWithDuration:0.25f scale:0.75f], [CCScaleTo actionWithDuration:0.25f scale:0.5f],nil],
				[CCFlipX3D actionWithDuration:0.5f],
				[CCSequence actions:[CCDelayTime actionWithDuration:0.325f], [CCFadeIn actionWithDuration:0], nil],
				[CCMoveTo actionWithDuration:0.5f position:ccp(480-self.position.x,self.position.y)],
				nil
			],
			[CCCallFunc actionWithTarget:self selector:@selector(doneAnimating)],
			nil
		];

	CCActionInterval* flipCardOut =
		[CCSequence actions:
			[CCFadeIn actionWithDuration:0],
			[CCSpawn actions:
				[CCSequence actions:[CCScaleTo actionWithDuration:0.25f scale:0.75f], [CCScaleTo actionWithDuration:0.25f scale:0.5f],nil],
				[CCFlipX3D actionWithDuration:0.5f],
				[CCSequence actions:[CCDelayTime actionWithDuration:0.325f], [CCFadeOut actionWithDuration:0], nil],
				[CCMoveTo actionWithDuration:0.5f position:ccp(480-self.position.x,self.position.y)],
				nil
			],
			[CCCallFunc actionWithTarget:self selector:@selector(doneAnimating)],
			nil
		];

	[cardUp.parent reorderChild:cardUp z:1];
	[cardDown.parent reorderChild:cardDown z:1];

	if(!cardIsUp){
		[cardDown runAction: flipCardOut];
		[cardUp runAction: flipCardIn];
		animating = YES;
		cardIsUp = YES;
	}else{
		[cardDown runAction: flipCardIn];
		[cardUp runAction: flipCardOut];
		animating = YES;
		cardIsUp = NO;
	}
}

- (void)processTouch:(CGPoint)point {
	[self flipCard];
}

-(void) doneAnimating {
	animating = NO;
	[cardUp.parent reorderChild:cardUp z:0];
	[cardDown.parent reorderChild:cardDown z:0];
}

- (CGRect) rect {
	//We set our scale mod to make sprite easier to press.
	//This also lets us press 2 sprites with 1 touch if they are sufficiently close.
	float scaleMod = 1.0f;
	float w = [cardUp contentSize].width * [cardUp scale] * scaleMod;
	float h = [cardUp contentSize].height * [cardUp scale] * scaleMod;
	float pointX = self.position.x - (w/2);
	float pointY = self.position.y - (h/2);
	CGPoint point = CGPointMake(pointX,pointY);

	return CGRectMake(point.x, point.y, w, h); 
}

@end