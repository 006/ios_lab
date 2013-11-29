#import "cocos2d.h"
#import "Recipe.h"
#import "Mole.h"
#import "SimpleTimedGameRecipe.h"

//Interface
@interface Ch3_SavingDataPlist : SimpleTimedGameRecipe
{	
	NSMutableArray *moles;
	int tagCount;
	int moleCount;
	CCSprite *mallet;
	CGPoint malletPosition;
}

-(CCLayer*) runRecipe;
-(void) step;
-(void) initBackground;
-(void) createMoleAtPosition:(CGPoint)point withZ:(float)z;
-(void) processMoleHit;

-(void) addHiScoresToMenu;
-(void) loadHiScores;
-(void) addHiScore;
-(void) deleteHiScores;
-(void) startNewGame;
-(void) gameOver;
-(void) step:(ccTime)delta;
-(void) ccTouchesBegan:(NSSet *)touches withEvent:(UIEvent *)event;

@end



//Implementation

@implementation Ch3_SavingDataPlist

-(CCLayer*) runRecipe {
	[super runRecipe];
	
	CCSpriteFrameCache *cache = [CCSpriteFrameCache sharedSpriteFrameCache];
	[cache addSpriteFramesWithFile:@"mole.plist"];
	
	tagCount = 0;

	[message setString:@"Welcome to Whack-A-Mole"];
	
	malletPosition = ccp(400,400);
	mallet = [CCSprite spriteWithSpriteFrame:[cache spriteFrameByName:@"mole_mallet.png"]];
	mallet.position = malletPosition;
	mallet.rotation = 90;
	mallet.scale = 0.5f;
	[self addChild:mallet z:10 tag:tagCount];
	tagCount++;
	
	[self initBackground];
	
	//Init moles
	moles = [[NSMutableArray alloc] init]; 
			
	//Add moles
	[self createMoleAtPosition:ccp(50,205) withZ:0];
	[self createMoleAtPosition:ccp(100,205) withZ:0];
	[self createMoleAtPosition:ccp(150,205) withZ:0];
	[self createMoleAtPosition:ccp(200,205) withZ:0];
	[self createMoleAtPosition:ccp(250,205) withZ:0];

	[self createMoleAtPosition:ccp(75,155) withZ:1];
	[self createMoleAtPosition:ccp(125,155) withZ:1];
	[self createMoleAtPosition:ccp(175,155) withZ:1];
	[self createMoleAtPosition:ccp(225,155) withZ:1];
	
	[self createMoleAtPosition:ccp(100,105) withZ:2];
	[self createMoleAtPosition:ccp(150,105) withZ:2];
	[self createMoleAtPosition:ccp(200,105) withZ:2];
	
	[self createMoleAtPosition:ccp(125,55) withZ:3];
	[self createMoleAtPosition:ccp(175,55) withZ:3];
	
	[self createMoleAtPosition:ccp(150,5) withZ:4];
		
	moleCount = moles.count;
				
	return self;
}

-(void) createMoleAtPosition:(CGPoint)point withZ:(float)z {
	CCSpriteFrameCache *cache = [CCSpriteFrameCache sharedSpriteFrameCache];
	
	CCSprite *back = [CCSprite spriteWithSpriteFrame:[cache spriteFrameByName:@"mole_back.png"]];
	back.position = ccp(point.x, point.y);
	[self addChild:back z:z tag:tagCount];
	tagCount++;
	
	Mole *mole = [Mole spriteWithSpriteFrame:[cache spriteFrameByName:@"mole_normal.png"]];
	[mole setDownPosition:ccp(point.x,point.y-30)];
	[self addChild:mole z:z tag:tagCount];
	[moles addObject:mole];
	tagCount++;
	
	CCSprite *front = [CCSprite spriteWithSpriteFrame:[cache spriteFrameByName:@"mole_front.png"]];
	front.position = ccp(point.x, point.y);
	[self addChild:front z:z tag:tagCount];
	tagCount++;

}

-(void) processMoleHit {
	currentScore++;
	[self setCurrentScore];
}

-(void) initBackground {
	ccTexParams params = {GL_NEAREST,GL_NEAREST_MIPMAP_NEAREST,GL_REPEAT,GL_REPEAT};
	
	CGPoint vertexArr[] = { ccp(0,0), ccp(480,0), ccp(480,320), ccp(0,320) };
	int numVerts = 4;
	NSMutableArray *vertices = [[[NSMutableArray alloc] init] autorelease];
	for(int i=0; i<numVerts; i++){
		[vertices addObject:[NSValue valueWithCGPoint:ccp(vertexArr[i].x*1, vertexArr[i].y*1)]];
	}
	
	TexturedPolygon *texturedPoly = [TexturedPolygon createWithFile:@"mole_bg.png" withVertices:vertices];
	[texturedPoly.texture setTexParameters:&params];
	
	texturedPoly.position = ccp(0,0);

	[self addChild:texturedPoly z:0 tag:tagCount];
}

-(void) addHiScoresToMenu {
	for(id score in hiScores){
		NSString *scoreStr = [NSString stringWithFormat:@"%@: %@", [score objectForKey:@"name"], [score objectForKey:@"score"]];
		[hiScoresMenu addChild:[CCMenuItemFont itemFromString:scoreStr]];
	}
}

-(void) loadHiScores {
	//Our template and file names
	NSString *templateName = @"whackamole_template.plist";
	NSString *fileName = @"whackamole.plist";

	//Our dictionary
	NSMutableDictionary *fileDict;

	//We get our file path
	NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
	NSString *documentsDirectory = [paths objectAtIndex:0];
	NSString *filePath = [documentsDirectory stringByAppendingPathComponent:fileName];

	if(![[NSFileManager defaultManager] fileExistsAtPath:filePath]){	
		//If file doesn't exist in document directory create a new one from the template
		fileDict = [NSMutableDictionary dictionaryWithContentsOfFile:getActualPath(templateName)];
	}else{
		//If it does we load it in the dict
		fileDict = [NSMutableDictionary dictionaryWithContentsOfFile:filePath];
	}
	
	//Load hi scores into our dictionary
	hiScores = [fileDict objectForKey:@"hiscores"];
	
	//Set the 'hiScore' variable (the highest score)
	for(id score in hiScores){
		int scoreNum = [[score objectForKey:@"score"] intValue];
		if(hiScore < scoreNum){
			hiScore = scoreNum;
		}
	}
	
	//Write dict to file
	[fileDict writeToFile:filePath atomically:YES];
}

-(void) addHiScore {
	//Our template and file names
	NSString *templateName = @"whackamole_template.plist";
	NSString *fileName = @"whackamole.plist";

	//Our dictionary
	NSMutableDictionary *fileDict;

	//We get our file path
	NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
	NSString *documentsDirectory = [paths objectAtIndex:0];
	NSString *filePath = [documentsDirectory stringByAppendingPathComponent:fileName];

	if(![[NSFileManager defaultManager] fileExistsAtPath:filePath]){	
		//If file doesn't exist in document directory create a new one from the template
		fileDict = [NSMutableDictionary dictionaryWithContentsOfFile:getActualPath(templateName)];
	}else{
		//If it does we load it in the dict
		fileDict = [NSMutableDictionary dictionaryWithContentsOfFile:filePath];
	}

	//Load hi scores into our dictionary
	hiScores = [fileDict objectForKey:@"hiscores"];

	//Add hi score
	bool scoreRecorded = NO;
	
	//Add score if player's name already exists
	for(id score in hiScores){
		NSMutableDictionary *scoreDict = (NSMutableDictionary*)score;
		if([[scoreDict objectForKey:@"name"] isEqualToString:currentPlayerName]){
			if([[scoreDict objectForKey:@"score"] intValue] < currentScore){
				[scoreDict setValue:[NSNumber numberWithInt:currentScore] forKey:@"score"];
			}
			scoreRecorded = YES;
		}
	}
	
	//Add new score if player's name doesn't exist
	if(!scoreRecorded){
		NSMutableDictionary *newScore = [[[NSMutableDictionary alloc] init] autorelease];
		[newScore setObject:currentPlayerName forKey:@"name"];
		[newScore setObject:[NSNumber numberWithInt:currentScore] forKey:@"score"];
		[hiScores addObject:newScore];
	}

	//Write dict to file
	[fileDict writeToFile:filePath atomically:YES];
}

-(void) deleteHiScores {
	//Our file name
	NSString *fileName = @"whackamole.plist";

	//We get our file path
	NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
	NSString *documentsDirectory = [paths objectAtIndex:0];
	NSString *filePath = [documentsDirectory stringByAppendingPathComponent:fileName];

	//Delete our file
	[[NSFileManager defaultManager] removeItemAtPath:filePath error:nil];
	
	[message setString:@"Hi scores deleted!"];
	
	hiScore = 0;
	[self loadHiScores];
}

-(void) startNewGame {
	[super startNewGame];
	
	[message setString:@"WHACK A MOLE!"];
}

-(void) gameOver {
	[super gameOver];

	for(id m in moles){
		[m startHideDown];
	}
}

-(void) step:(ccTime)delta {	
	[super step:delta];

	if(gameState == MID_GAME){
		int randMod = (int)((gameTimer-5)*25);
		if(randMod < moleCount){ randMod = moleCount; }
	
		int num = arc4random()%randMod;
		if(num < moleCount){
			Mole *m = [moles objectAtIndex:num];
			if(m.state == MOLE_DOWN){
				[m startPopUp];
			}else if(m.state == MOLE_UP){
				[m startHideDown];
			}
		}
	}
}

-(void) ccTouchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {	
	if(gameState == GAME_OVER){
		return;
	}
	
	UITouch *touch = [touches anyObject];
    CGPoint point = [touch locationInView: [touch view]];
	point = [[CCDirector sharedDirector] convertToGL: point];
	
	[mallet stopAllActions];
	[mallet runAction: [CCSequence actions:
		[CCSpawn actions: 
			[CCEaseSineInOut actionWithAction:[CCMoveTo actionWithDuration:0.05f position:point]], 
			[CCEaseSineInOut actionWithAction:[CCRotateTo actionWithDuration:0.05f angle:0]], 
			nil],
		[CCSpawn actions: 
			[CCEaseSineInOut actionWithAction:[CCMoveTo actionWithDuration:0.25f position:malletPosition]], 
			[CCEaseSineInOut actionWithAction:[CCRotateTo actionWithDuration:0.25f angle:90]], 
			nil],
		nil
	]];
	
	for(Mole *m in moles){	
		[m ccTouchesBegan:touches withEvent:event];
	}
}

-(void) cleanRecipe {
	[moles release];
	[super cleanRecipe];
}

@end
