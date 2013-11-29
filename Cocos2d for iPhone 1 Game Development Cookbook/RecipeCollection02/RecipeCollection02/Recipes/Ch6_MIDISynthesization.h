#import "Recipe.h"
#import "MIDISampleGenerator.h"

static const int kWhiteKeyNumbers[] = { 0, 2, 4, 5, 7, 9, 11 };
static const int kWhiteKeyCount = sizeof(kWhiteKeyNumbers) / sizeof(int);
static const int kBlackKey1Numbers[] = { 1, 3 };
static const int kBlackKey1Count = sizeof(kBlackKey1Numbers) / sizeof(int);
static const int kBlackKey2Numbers[] = { 6, 8, 10 };
static const int kBlackKey2Count = sizeof(kBlackKey2Numbers) / sizeof(int);

@interface Ch6_MIDISynthesization : Recipe
{
	MIDISampleGenerator *sampleGenerator;
  
	CCNode* keyboard;
	float keySpriteScale;
	NSMutableArray *whiteKeys;
	NSMutableArray *blackKeys;
	NSMutableDictionary *keyTouches;
}

-(CCLayer*) runRecipe;
-(void) initKeyboard;
-(void) randomize:(id)sender;
-(void) slideLeft:(id)sender;
-(void) slideRight:(id)sender;
-(bool) keyPressed:(CCSprite*)key withHash:(NSString*)hashKey;
-(bool) keyReleased:(int)note remove:(bool)remove;
-(bool) touchedSprite:(CCSprite*)sprite withPoint:(CGPoint)touch;

@end

@implementation Ch6_MIDISynthesization

-(CCLayer*) runRecipe {
	[super runRecipe];

	//Show message
	[self showMessage:@"Play a tune on the keyboard"];

	//Init sample generator
	sampleGenerator = [[MIDISampleGenerator alloc] init];

	//Menu items
	[CCMenuItemFont setFontSize:25];
	[CCMenuItemFont setFontName:@"Marker Felt"];
	CCMenuItemFont *randomizeItem = [CCMenuItemFont itemFromString:@"Randomize" target:self selector:@selector(randomize:)];
	CCMenuItemFont *slideLeft = [CCMenuItemFont itemFromString:@"Left" target:self selector:@selector(slideLeft:)];
	CCMenuItemFont *slideRight = [CCMenuItemFont itemFromString:@"Right" target:self selector:@selector(slideRight:)];
	
	CCMenu *menu = [CCMenu menuWithItems:slideLeft, randomizeItem, slideRight, nil];
	[menu alignItemsHorizontallyWithPadding:100.0f];
	menu.position = ccp(240,230);
	[self addChild:menu];
	
	//Init keyboard
	[self initKeyboard];
	
	return self;
}

-(void) initKeyboard {
	//Data structure init
	whiteKeys = [[NSMutableArray alloc] init];
	blackKeys = [[NSMutableArray alloc] init];
	keyTouches = [[NSMutableDictionary alloc] init];
	
	//Main keyboard node
	keyboard = [[CCNode alloc] init];
	keyboard.position = ccp(0,120);
	
	//Helper values
	keySpriteScale = 0.40f;
	float keySize = 112*keySpriteScale;
	float whiteKeyHeight = 336*keySpriteScale;
	int noteStart = 16;
	int nextOctave = 12;
	CGPoint blackKey1Offset = ccp(keySize * 0.6666f, 68.666f * keySpriteScale);
	CGPoint blackKey2Offset = ccp(keySize * 0.6666f + keySize * 3, 68.666f * keySpriteScale);
	
	//Add white keys
	for(int x=0; x<2; x++){
		for(int i=0; i<kWhiteKeyCount; i++){
			int toneNumber = kWhiteKeyNumbers[i] + noteStart + x*nextOctave;
			CCSprite *whiteKey = [CCSprite spriteWithFile:@"piano_white_key.png"];
			whiteKey.scale = keySpriteScale;
			whiteKey.position = ccp((i + kWhiteKeyCount*x)  * keySize, 0);
			whiteKey.tag = toneNumber;
		
			[keyboard addChild:whiteKey z:1];
			[whiteKeys addObject:whiteKey];
		}
	}
	
	//Add first black key set
	for(int x=0; x<2; x++){
		for(int i=0; i<kBlackKey1Count; i++){
			int toneNumber = kBlackKey1Numbers[i] + noteStart + x*nextOctave;
			CCSprite *blackKey = [CCSprite spriteWithFile:@"piano_black_key.png"];
			blackKey.scale = keySpriteScale;
			blackKey.position = ccp((i + kWhiteKeyCount*x) * keySize + blackKey1Offset.x, blackKey1Offset.y);
			blackKey.tag = toneNumber;
		
			[keyboard addChild:blackKey z:2];
			[blackKeys addObject:blackKey];
		}
	}
	
	//Add second black key set
	for(int x=0; x<2; x++){
		for(int i=0; i<kBlackKey2Count; i++){
			int toneNumber = kBlackKey2Numbers[i] + noteStart + x*nextOctave;
			CCSprite *blackKey = [CCSprite spriteWithFile:@"piano_black_key.png"];
			blackKey.scale = keySpriteScale;
			blackKey.position = ccp((i + kWhiteKeyCount*x) * keySize + blackKey2Offset.x, blackKey2Offset.y);
			blackKey.tag = toneNumber;
		
			[keyboard addChild:blackKey z:2];
			[blackKeys addObject:blackKey];
		}
	}
	
	//Black background
	CCSprite *bg = [CCSprite spriteWithFile:@"blank.png"];
	bg.color = ccc3(0,0,0);
	[bg setTextureRect:CGRectMake(0, 0, kWhiteKeyCount*2 * keySize, whiteKeyHeight+20)];
	bg.position = ccp(727.5*keySpriteScale,0);
	[keyboard addChild:bg z:0];
	
	//Finally, add keyboard
	[self addChild:keyboard];
}

-(void) randomize:(id)sender {
	//Randomize values including Modulation, Oscillation, Filter, etc
	[sampleGenerator randomize];
}

-(void) slideLeft:(id)sender {
	//Slide keyboard to the left
	float keySize = 112*keySpriteScale;
	[keyboard runAction:[CCEaseInOut actionWithAction:[CCMoveBy actionWithDuration:0.25f position:ccp(keySize,0)] rate:2]];
}

-(void) slideRight:(id)sender {
	//Slide keyboard to the right
	float keySize = 112*keySpriteScale;
	[keyboard runAction:[CCEaseInOut actionWithAction:[CCMoveBy actionWithDuration:0.25f position:ccp(-keySize,0)] rate:2]];
}

-(bool) keyPressed:(CCSprite*)key withHash:(NSString*)hashKey {
	//Set darker key color
	[key setColor:ccc3(255,100,100)];
	
	//Play note
	[sampleGenerator noteOn:key.tag];
	
	//Keep track of touch
	[keyTouches setObject:[NSNumber numberWithInt:key.tag] forKey:hashKey];
	return YES;
}

-(bool) keyReleased:(int)note remove:(bool)remove {
	//Set lighter key color
	bool keyReleased = NO;
	for(id b in blackKeys){
		if(keyReleased){ break; }
		CCSprite *blackKey = (CCSprite*)b;
		if(blackKey.tag == note){
			blackKey.color = ccc3(255,255,255);
			keyReleased = YES;
		}
	}
	for(id w in whiteKeys){
		if(keyReleased){ break; }
		CCSprite *whiteKey = (CCSprite*)w;
		if(whiteKey.tag == note){
			whiteKey.color = ccc3(255,255,255);
			keyReleased = YES;
		}
	}

	if(keyReleased){
		//Stop playing note
		[sampleGenerator noteOff:note];
		
		//Remove tracking
		if(remove){ [keyTouches removeObjectForKey:[NSNumber numberWithInt:note]]; }
	}
	
	return keyReleased;
}

//Tells us whether or not we touched a sprite
-(bool) touchedSprite:(CCSprite*)sprite withPoint:(CGPoint)touch {
	float w = [sprite contentSize].width * [sprite scale];
	float h = [sprite contentSize].height * [sprite scale];
	CGPoint point = CGPointMake([sprite position].x - (w/2) + sprite.parent.position.x, 
		[sprite position].y - (h/2) + sprite.parent.position.y);		
	CGRect rect = CGRectMake(point.x, point.y, w, h);
			
	return pointIsInRect(touch, rect);
}

-(void) ccTouchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {
	//Process multiple touches
	for(int i=0; i<[[touches allObjects] count]; i++){
		UITouch *touch = [[touches allObjects] objectAtIndex:i];
		CGPoint point = [touch locationInView: [touch view]];
		point = [[CCDirector sharedDirector] convertToGL: point];		

		//Touch hash tracking
		NSString *hashKey = [NSString stringWithFormat:@"%d",[touch hash]];
		bool keyPressed = NO;
		
		//Process black keys first
		for(id b in blackKeys){
			CCSprite *blackKey = (CCSprite*)b;
			if([self touchedSprite:blackKey withPoint:point]){
				keyPressed = [self keyPressed:blackKey withHash:hashKey];
			}
		}
		for(id w in whiteKeys){
			if(keyPressed){ break; }
			CCSprite *whiteKey = (CCSprite*)w;
			if([self touchedSprite:whiteKey withPoint:point]){
				keyPressed = [self keyPressed:whiteKey withHash:hashKey];
			}
		}
	}
}

-(void) ccTouchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {
	//Process multiple touches
	for(int i=0; i<[[touches allObjects] count]; i++){
		UITouch *touch = [[touches allObjects] objectAtIndex:i];
		CGPoint point = [touch locationInView: [touch view]];
		point = [[CCDirector sharedDirector] convertToGL: point];		

		//Touch hash tracking
		NSString *hashKey = [NSString stringWithFormat:@"%d",[touch hash]];
		bool keyPressed = NO;
		
		//Process black keys first
		for(id b in blackKeys){
			CCSprite *blackKey = (CCSprite*)b;
			if([self touchedSprite:blackKey withPoint:point]){
				NSNumber *storedKeyNumber = nil;
				storedKeyNumber = [keyTouches objectForKey:hashKey];
				if(storedKeyNumber){
					//This hash is already touching something
					if([storedKeyNumber intValue] != blackKey.tag){
						//Moved onto a key from another key
						[self keyReleased:[storedKeyNumber intValue] remove:NO];
						[self keyPressed:blackKey withHash:hashKey];
					}
				}else{
					//Moved onto a key from nothing
					[self keyPressed:blackKey withHash:hashKey];
				}
				keyPressed = YES;
			}
		}
		for(id w in whiteKeys){
			if(keyPressed){ break; }
			CCSprite *whiteKey = (CCSprite*)w;
			if([self touchedSprite:whiteKey withPoint:point]){
				NSNumber *storedKeyNumber = nil;
				storedKeyNumber = [keyTouches objectForKey:hashKey];
				if(storedKeyNumber){
					//This hash is already touching something
					if([storedKeyNumber intValue] != whiteKey.tag){
						//Moved onto a key from another key
						[self keyReleased:[storedKeyNumber intValue] remove:NO];
						[self keyPressed:whiteKey withHash:hashKey];
					}
				}else{
					//Moved onto a key from nothing
					[self keyPressed:whiteKey withHash:hashKey];
				}
				keyPressed = YES;
			}
		}
		
		//If we moved off of a key we stop playing the last note we pressed
		if(!keyPressed && [keyTouches objectForKey:hashKey]){
			int note = [[keyTouches objectForKey:hashKey] intValue];
			[self keyReleased:note remove:YES];
		}
	}
}
-(void) ccTouchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {
	//Stop sounds and remove sprites
	for(int i=0; i<[[touches allObjects] count]; i++){
		UITouch *touch = [[touches allObjects] objectAtIndex:i];
		CGPoint point = [touch locationInView: [touch view]];
		point = [[CCDirector sharedDirector] convertToGL: point];		

		//Touch hash tracking
		NSString *hashKey = [NSString stringWithFormat:@"%d",[touch hash]];
		int note = [[keyTouches objectForKey:hashKey] intValue];
		
		//Release note
		[self keyReleased:note remove:YES];
	}
}

-(void) cleanRecipe {
	//Remove sample generator
	[sampleGenerator release];
	[keyTouches release];

	[super cleanRecipe];
}

@end