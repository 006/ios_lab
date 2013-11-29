@interface SimpleCharacter : NSObject <NSCoding>
{
	NSString *charColor;
	NSString *charClass;
	int strength;
	int dexterity;
	int constitution;
	int intelligence;
	int wisdom;
	int charisma;
}

@property (readwrite, assign) NSString *charColor;
@property (readwrite, assign) NSString *charClass;
@property (readwrite, assign) int strength;
@property (readwrite, assign) int dexterity;
@property (readwrite, assign) int constitution;
@property (readwrite, assign) int intelligence;
@property (readwrite, assign) int wisdom;
@property (readwrite, assign) int charisma;

- (id) init;
- (void) dealloc;
- (void) encodeWithCoder: (NSCoder *)coder;
- (id) initWithCoder: (NSCoder *)coder;
-(void) addCharacterToNode:(CCNode *)node atPosition:(CGPoint)position;

@end

@implementation SimpleCharacter

@synthesize charColor, charClass, strength, dexterity, constitution, intelligence, wisdom, charisma;

/* This merely adds this character with the proper color to a CCNode */
-(void) addCharacterToNode:(CCNode *)node atPosition:(CGPoint)position {
	ccColor3B color;
	
	if([charColor isEqualToString:@"Red"]){
		color = ccc3(255,0,0);
	}else if([charColor isEqualToString:@"Blue"]){
		color = ccc3(0,0,255);
	}else if([charColor isEqualToString:@"Green"]){
		color = ccc3(0,255,0);
	}else if([charColor isEqualToString:@"Yellow"]){
		color = ccc3(255,255,0);
	}else if([charColor isEqualToString:@"Orange"]){
		color = ccc3(255,150,0);
	}else if([charColor isEqualToString:@"Purple"]){
		color = ccc3(175,0,255);
	}

	CCSpriteFrameCache *cache = [CCSpriteFrameCache sharedSpriteFrameCache];
	[cache addSpriteFramesWithFile:@"dnd_characters.plist"];
	
	CCSprite *drawing = [CCSprite spriteWithSpriteFrame:[cache spriteFrameByName:[NSString stringWithFormat:@"dnd_%@_drawing.png",charClass]]];
	CCSprite *colors = [CCSprite spriteWithSpriteFrame:[cache spriteFrameByName:[NSString stringWithFormat:@"dnd_%@_colors.png",charClass]]];

	drawing.position = position;
	colors.position = position;
	
	drawing.scale = 1.5f;
	colors.scale = 1.5f;
	
	colors.color = color;
	
	[node addChild:colors z:0 tag:0];
	[node addChild:drawing z:1 tag:1];
}

/* This method determines how data is encoded into an NSCoder object */
- (void) encodeWithCoder: (NSCoder *)coder {
	[coder encodeObject:charColor];
	[coder encodeObject:charClass];
	[coder encodeObject:[NSNumber numberWithInt:strength]];
	[coder encodeObject:[NSNumber numberWithInt:dexterity]];
	[coder encodeObject:[NSNumber numberWithInt:constitution]];
	[coder encodeObject:[NSNumber numberWithInt:intelligence]];
	[coder encodeObject:[NSNumber numberWithInt:wisdom]];
	[coder encodeObject:[NSNumber numberWithInt:charisma]];
}

/* This method determines how data is read out from an NSCode object */
-(id) initWithCoder: (NSCoder *) coder {
	[super init];
	charColor = [[coder decodeObject] retain];
	charClass = [[coder decodeObject] retain];
	strength = [[coder decodeObject] intValue];
	dexterity = [[coder decodeObject] intValue];
	constitution = [[coder decodeObject] intValue];
	intelligence = [[coder decodeObject] intValue];
	wisdom = [[coder decodeObject] intValue];
	charisma = [[coder decodeObject] intValue];
	return self;
}

/* Initialization */
-(id) init {
    self = [super init];
    if (self) {
		charColor = @"Red";
		charClass = @"Wizard";
		strength = 10;
		dexterity = 10;
		constitution = 10;
		intelligence = 10;
		wisdom = 10;
		charisma = 10;
    }
    return self;
}

/* All objects must be released here */
- (void) dealloc {
	[charColor release];
	[charClass release];
	[super dealloc];
}
@end