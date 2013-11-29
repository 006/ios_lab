#import "cocos2d.h"
#import "Recipe.h"
#import "ActualPath.h"
#import "SimpleAnimObject.h"
#import "SimpleTimedGameRecipe.h"
#import <Foundation/Foundation.h>
#import "FMDatabase.h"

enum {	//Disc animation types
	DISC_INTACT,
	DISC_BROKEN
};

//Interface
@interface Ch3_SavingDataSQLite : SimpleTimedGameRecipe
{
	SimpleAnimObject *gun;
	SimpleAnimObject *bullet;
	NSMutableArray *discs;
	int fireBulletCountdown;
	int discRotationNum;
	bool reuseDiscs;
	
	FMDatabase *db;
}

-(CCLayer*) runRecipe;

-(void) fireDiscAtPosition:(CGPoint)position withVelocity:(CGPoint)velocity;
-(void) hitDisc:(SimpleAnimObject*)disc;
-(void) processDiscHit;

-(NSArray *) createDictionariesArrayFromFMResultSet:(FMResultSet *)rs fields:(NSString *)fields;
-(void) writeNewScore:(int)score forName:(NSString*)name;
-(void) addHiScoresToMenu;
-(void) loadHiScores;
-(void) addHiScore;
-(void) deleteHiScores;
-(void) startNewGame;
-(void) gameOver;
-(void) initBackground;
-(void) step:(ccTime)delta;
-(void) ccTouchesBegan:(NSSet *)touches withEvent:(UIEvent *)event;
-(void) ccTouchesMoved:(NSSet *)touches withEvent:(UIEvent *)event;
-(void) ccTouchesEnded:(NSSet *)touches withEvent:(UIEvent *)event;

@end



//Implementation

@implementation Ch3_SavingDataSQLite

-(CCLayer*) runRecipe {
	[super runRecipe];

	[message setString:@"Welcome to Skeet Shooter!"];

	[self initBackground];

	CCSpriteFrameCache *cache = [CCSpriteFrameCache sharedSpriteFrameCache];
	
	[cache addSpriteFramesWithFile:@"skeet.plist"];
	
	discs = [[NSMutableArray alloc] init];
	
	fireBulletCountdown = 0;
	discRotationNum = 0;
	reuseDiscs = NO;
	
	gun = [CCSprite spriteWithSpriteFrame:[cache spriteFrameByName:@"skeet_gun.png"]];
	gun.position = ccp(240,0);
	gun.anchorPoint = ccp(0.5f,0.35f);
	[self addChild:gun z:3];
	
	bullet = [SimpleAnimObject spriteWithSpriteFrame:[cache spriteFrameByName:@"skeet_bullet.png"]];
	bullet.position = gun.position;
	[self addChild:bullet z:2];
		
	return self;
}

-(void) processDiscHit {
	currentScore++;
	[self setCurrentScore];
}

-(void) initBackground {	
	//Draw the sky using blank.png
	CCSprite *sky = [CCSprite spriteWithFile:@"blank.png"];
	[sky setPosition:ccp(240,190)];
	[sky setTextureRect:CGRectMake(0,0,480,260)];
	[sky setColor:ccc3(50,50,100)];
	[self addChild:sky z:0];
			
	//Draw some mountains in the background
	CCSprite *mountains = [CCSprite spriteWithFile:@"mountains.png"];
	[mountains setPosition:ccp(250,200)];
	[mountains setScale:0.6f];
	[self addChild:mountains z:0 tag:TAG_MOUNTAIN_BG];
	
	//Add a gradient below the mountains
	CCLayerGradient *groundGradient = [CCLayerGradient layerWithColor:ccc4(61,33,62,255) fadingTo:ccc4(65,89,54,255) alongVector:ccp(0,-1)];
	[groundGradient setContentSize:CGSizeMake(480,200)];
	[groundGradient setPosition:ccp(0,-50)];
	[self addChild:groundGradient z:0 tag:TAG_GROUND_GRADIENT];
}

-(void) cleanRecipe {
	[discs release];
	
	[super cleanRecipe];
}

-(void) dealloc {
	//Relase our database
	[db close];
	[db release];
	
	[hiScores release];
	
	[super dealloc];
}

-(NSArray *) createDictionariesArrayFromFMResultSet:(FMResultSet *)rs fields:(NSString *)fields {
	//Parse field string into an array
	NSArray * listFields = [fields componentsSeparatedByString:@","];

	//Create an array of dictionaries from each field
	NSMutableArray * items = [NSMutableArray arrayWithCapacity:1];
	while ([rs next]) {
		NSMutableDictionary * item = [NSMutableDictionary dictionaryWithCapacity:1];
		for (int i = 0; i < [listFields count]; i++) {
			NSString * key = [listFields objectAtIndex:i];
			NSString * value = [rs stringForColumn: key];
			if (value == NULL) value = @"";
			[item setObject:value forKey:key];
		}
		[items addObject:item];
	}
	[rs close];

	return items;
}

-(void) writeNewScore:(int)score forName:(NSString*)name {	
	//Find the hi score with this name
	NSString *selectQuery = [NSString stringWithFormat:@"SELECT * FROM hiscores WHERE name = '%@'", name];
	FMResultSet *rs = [db executeQuery:selectQuery];

	//What is the score? Is there a score at all?
	int storedScore = -1;
	while([rs next]){ 
		storedScore = [[rs stringForColumn:@"score"] intValue];
	}
	[rs close];

	if(storedScore == -1){
		//Name doesn't exist, add it
		NSString *insertQuery = [NSString stringWithFormat:@"INSERT INTO hiscores (name, score) VALUES ('%@','%i')", name, score];		
		rs = [db executeQuery:insertQuery];
		while([rs next]){};
		[rs close];
	}else if(score > storedScore){
		//Write new score for existing name
		NSString *updateQuery = [NSString stringWithFormat:@"UPDATE hiscores SET score='%i' WHERE name='%@'", score, name];
		rs = [db executeQuery:updateQuery];
		while([rs next]){};
		[rs close];
	}
}

-(void) addHiScoresToMenu {
	for(id score in hiScores){
		NSString *scoreStr = [NSString stringWithFormat:@"%@: %@", [score objectForKey:@"name"], [score objectForKey:@"score"]];
		[hiScoresMenu addChild:[CCMenuItemFont itemFromString:scoreStr]];
	}
}

-(void) loadHiScores {
	//Our file and template names
	NSString *fileName = @"skeetshooter.sqlite";
	NSString *templateName = @"skeetshooter_template.sqlite";
	
	//We get our file path
	NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
	NSString *documentsDirectory = [paths objectAtIndex:0];
	NSString *filePath = [documentsDirectory stringByAppendingPathComponent:fileName];

	//If file doesn't exist in document directory create a new one from the template
	if(![[NSFileManager defaultManager] fileExistsAtPath:filePath]){	
		[[NSFileManager defaultManager] copyItemAtPath:getActualPath(templateName) 
			toPath:[NSString stringWithFormat:@"%@/%@", documentsDirectory, fileName] error:nil];
	}

	//Initialize the database
	if(!db){
		db = [FMDatabase databaseWithPath:filePath];
		[db setLogsErrors:YES];
		[db setTraceExecution:YES];
		[db retain];
	
		if(![db open]){
			NSLog(@"Could not open db.");
		}else{
			NSLog(@"DB opened successfully.");
		}
	}
	
	//Select all hi scores
	FMResultSet *rs = [db executeQuery:@"select * from hiscores"];
	
	//Load them into an array of dictionaries
	hiScores = [[NSMutableArray alloc] init];
	hiScores = [self createDictionariesArrayFromFMResultSet:rs fields:@"name,score"]; 

	//Set hi score
	for(id score in hiScores){
		int scoreNum = [[score objectForKey:@"score"] intValue];
		if(hiScore < scoreNum){
			hiScore = scoreNum;
		}
	}
}

-(void) addHiScore {
	//Add hi score to db
	[self writeNewScore:currentScore forName:currentPlayerName];
	
	//Reset dictionary
	FMResultSet *rs = [db executeQuery:@"SELECT * FROM hiscores"];
	hiScores = [self createDictionariesArrayFromFMResultSet:rs fields:@"name,score"]; 
}

-(void) deleteHiScores {
	//Our file name
	NSString *fileName = @"skeetshooter.sqlite";

	//We get our file path
	NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
	NSString *documentsDirectory = [paths objectAtIndex:0];
	NSString *filePath = [documentsDirectory stringByAppendingPathComponent:fileName];

	//Delete our file
	[[NSFileManager defaultManager] removeItemAtPath:filePath error:nil];
	
	[message setString:@"Hi scores deleted!"];
	
	hiScore = 0;
	
	//Close and release our db pointer
	[db close];
	[db release];
	db = nil;
	
	//Load new blank hi scores
	[self loadHiScores];
}

-(void) startNewGame {
	[super startNewGame];
	
	[message setString:@"SKEET SHOOTER!"];
}

-(void) gameOver {
	[super gameOver];
}

-(void) step:(ccTime)delta {
	[super step:delta];

	float gravity = 0.1f;

	for(SimpleAnimObject* disc in discs){
		//Apply gravity
		disc.velocity = ccp(disc.velocity.x, disc.velocity.y - gravity);
		
		//Update movement
		[disc update:delta];
					
		if(gameState == MID_GAME){	
			if(pointIsInCircle(bullet.position, disc.position, disc.rect.size.width*1.75f)){
				[self hitDisc:disc];
			}
		}
	}
	[bullet update:delta];
	
	fireBulletCountdown--;
	if(fireBulletCountdown < 0){
		[gun setColor:ccc3(255,200,200)];
	}else{
		[gun setColor:ccc3(255,255,255)];
	}

	if(gameState == MID_GAME){		
		int num = arc4random()%50;
		if(num < 2){
			float x;
			if(num == 0){ x = 480; }
			else{ x = 0; }
			float y = arc4random()%60;
			
			float xVel = arc4random()%4+3;
			if(num == 0){ xVel *= -1; }
			float yVel = 7.5f;
				
			[self fireDiscAtPosition:ccp(x,y) withVelocity:ccp(xVel,yVel)];
		}
	}

}

-(void) hitDisc:(SimpleAnimObject*)disc {
	if(disc.animationType == DISC_BROKEN){
		return;	//We already hit this disc
	}
	
	[self processDiscHit];

	disc.animationType = DISC_BROKEN;

	CCSpriteFrameCache *cache = [CCSpriteFrameCache sharedSpriteFrameCache];
		
	CCAnimation *anim = [[CCAnimation alloc] initWithName:@"disc_break" delay:0.05f];
	for(int i=1; i<=6; i++){
		[anim addFrame:[cache spriteFrameByName:[NSString stringWithFormat:@"skeet_disc_white_break_0%i.png",i]]];
	};
	
	[disc runAction: [CCSequence actions: [CCAnimate actionWithAnimation:anim], [CCFadeOut actionWithDuration:0.0f], nil]];
	
	disc.velocity = ccp(bullet.velocity.x/10, bullet.velocity.y/10);
}
-(void) fireDiscAtPosition:(CGPoint)position withVelocity:(CGPoint)velocity {
	CCSpriteFrameCache *cache = [CCSpriteFrameCache sharedSpriteFrameCache];
	
	if(!reuseDiscs && discs.count >= 10){ reuseDiscs = YES; }
	
	SimpleAnimObject *disc;
	if(!reuseDiscs){
		disc = [SimpleAnimObject spriteWithSpriteFrame:[cache spriteFrameByName:@"skeet_disc_white_00.png"]];
	}else{
		disc = [discs objectAtIndex:discRotationNum];
		CCAnimation *anim = [[CCAnimation alloc] initWithName:@"disc" delay:100.0f];
		[anim addFrame:[cache spriteFrameByName:@"skeet_disc_white_00.png"]];
		disc.opacity = 255;
		[disc runAction: [CCRepeatForever actionWithAction: [CCAnimate actionWithAnimation:anim]]];
		discRotationNum++;
		if(discRotationNum > 9){
			discRotationNum = 0;
		}
	}
		
	disc.animationType = DISC_INTACT;
	disc.position = position;
	disc.velocity = velocity;
	disc.scale = 0.25f;
	
	if(!reuseDiscs){
		[discs addObject:disc];
		[self addChild:disc z:3];
	}
}

-(void) ccTouchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {		
	UITouch *touch = [touches anyObject];
    CGPoint point = [touch locationInView: [touch view]];
	point = [[CCDirector sharedDirector] convertToGL: point];
	
	if(gameState != MID_GAME){ return; }
	
	float radians = vectorToRadians(ccp(point.x - gun.position.x, point.y - gun.position.y));
	float degrees = radiansToDegrees(radians);
	
	gun.rotation = degrees;
}

-(void) ccTouchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {		
	UITouch *touch = [touches anyObject];
    CGPoint point = [touch locationInView: [touch view]];
	point = [[CCDirector sharedDirector] convertToGL: point];
	
	if(gameState != MID_GAME){ return; }
	
	float radians = vectorToRadians(ccp(point.x - gun.position.x, point.y - gun.position.y));
	float degrees = radiansToDegrees(radians);
	
	gun.rotation = degrees;
}

-(void) ccTouchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {		
	UITouch *touch = [touches anyObject];
    CGPoint point = [touch locationInView: [touch view]];
	point = [[CCDirector sharedDirector] convertToGL: point];
	
	if(gameState != MID_GAME){ return; }
	
	if(fireBulletCountdown > 0){
		return;	//Can't fire bullet yet
	}else{
		fireBulletCountdown = 20;
	}
	
	float radians = vectorToRadians(ccp(point.x - gun.position.x, point.y - gun.position.y));
	CGPoint normalVector = radiansToVector(radians+PI/2);
	float degrees = radiansToDegrees(radians);
	
	bullet.position = gun.position;
	bullet.velocity = ccp(normalVector.x*20, normalVector.y*20);
	bullet.rotation = degrees;
	
	
}

@end