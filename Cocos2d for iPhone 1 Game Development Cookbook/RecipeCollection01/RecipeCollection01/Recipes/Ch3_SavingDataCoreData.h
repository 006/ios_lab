#import "cocos2d.h"
#import "Recipe.h"
#import "MatchCard.h"
#import "SimpleTimedGameRecipe.h"
#import <UIKit/UIKit.h>   
#import "Hiscore.h"

//Interface

@interface Ch3_SavingDataCoreData : SimpleTimedGameRecipe <NSFetchedResultsControllerDelegate>
{
	int wrongGuessesLeft;
	NSMutableDictionary *cardDict;
	int cardsFlipped;
	MatchCard *lastFlipped1;
	MatchCard *lastFlipped2;
	int numberOfStrikes;
	int round;
	
    NSManagedObjectModel *managedObjectModel;
    NSManagedObjectContext *managedObjectContext;	    
    NSPersistentStoreCoordinator *persistentStoreCoordinator;
}

@property (nonatomic, retain, readonly) NSManagedObjectModel *managedObjectModel;
@property (nonatomic, retain, readonly) NSManagedObjectContext *managedObjectContext;
@property (nonatomic, retain, readonly) NSPersistentStoreCoordinator *persistentStoreCoordinator;

-(CCLayer*) runRecipe;
-(void) initCardDict;
-(void) createCardPairWithIndex1:(CGPoint)index1 index2:(CGPoint)index2;
-(void) deleteCardsAndStrikes;
-(void) createCards;
-(void) startStudyTime;
-(void) flipNewCards;
-(void) flipCard:(MatchCard*)card;
-(void) compareCards;
-(void) addStrike;
-(void) ccTouchesBegan:(NSSet *)touches withEvent:(UIEvent *)event;
-(void) ccTouchesMoved:(NSSet *)touches withEvent:(UIEvent *)event;
-(void) ccTouchesEnded:(NSSet *)touches withEvent:(UIEvent *)event;
-(void) loadHiScores;
-(void) addHiScoresToMenu;
-(void) addHiScore;
-(void) deleteHiScores;
-(void) initManagedObjectContext;
-(void) getHiScoreFromName:(NSString*)name;

@end



//Implementation

@implementation Ch3_SavingDataCoreData

-(CCLayer*) runRecipe {
	[super runRecipe];

	[message setString:@"Welcome to Memory!"];
	
	[self initCardDict];
	
	CCLabelBMFont *strikesLabel = [CCLabelBMFont labelWithString:@"STRIKES:" fntFile:@"eurostile_30.fnt"];
	strikesLabel.position = ccp(430,120);
	strikesLabel.scale = 0.5f;
	[self addChild:strikesLabel z:Z_TIMED_GAME_HUD];
	
	message.position = ccp(160,275);
	
	[self createCards];
	
	return self;
}

-(void) createCardPairWithIndex1:(CGPoint)index1 index2:(CGPoint)index2 {
	CCSpriteFrameCache *cache = [CCSpriteFrameCache sharedSpriteFrameCache];
	[cache addSpriteFramesWithFile:@"match_card.plist"];
	
	int cardTypeNum = arc4random()%4;
	NSString *cardFrameName;
	if(cardTypeNum == 0){
		cardFrameName = @"match_card_circle.png";
	}else if(cardTypeNum == 1){
		cardFrameName = @"match_card_square.png";
	}else if(cardTypeNum == 2){
		cardFrameName = @"match_card_pentagon.png";
	}else if(cardTypeNum == 3){
		cardFrameName = @"match_card_triangle.png";
	}
	
	MatchCard *card1 = [MatchCard createWithSpriteFrameName:cardFrameName];
	[card1 setAllPositions:ccp(index1.x*50+50,index1.y*80+70)];
	[self addChild:card1.cardUp];
	[self addChild:card1.cardDown];
	[[cardDict objectForKey:[NSNumber numberWithInt:(int)index1.x]] setObject:card1 forKey:[NSNumber numberWithInt:(int)index1.y]];
	
	MatchCard *card2 = [MatchCard createWithSpriteFrameName:cardFrameName];
	[card2 setAllPositions:ccp(index2.x*50+50,index2.y*80+70)];
	[self addChild:card2.cardUp];
	[self addChild:card2.cardDown];
	[[cardDict objectForKey:[NSNumber numberWithInt:(int)index2.x]] setObject:card2 forKey:[NSNumber numberWithInt:(int)index2.y]];
}

-(void) deleteCardsAndStrikes {
	for(int x=0; x<=5; x++){
		for(int y=0; y<=2; y++){
			MatchCard *card = [[cardDict objectForKey:[NSNumber numberWithInt:x]] objectForKey:[NSNumber numberWithInt:y]];
			[[cardDict objectForKey:[NSNumber numberWithInt:x]] removeObjectForKey:[NSNumber numberWithInt:y]];
			[self removeChild:card.cardUp cleanup:YES];
			[self removeChild:card.cardDown cleanup:YES];
			[card release];
		}
	}	
	
	[self removeChildByTag:1 cleanup:YES];
	[self removeChildByTag:2 cleanup:YES];
	[self removeChildByTag:3 cleanup:YES];
}

-(void) startStudyTime {
	[self runAction:[CCSequence actions:[CCDelayTime actionWithDuration:5.0f], 
		[CCCallFunc actionWithTarget:self selector:@selector(flipNewCards)],nil]];
}

-(void) createCards {
	NSMutableArray *numArr = [[[NSMutableArray alloc] init] autorelease];
	for(int x=0; x<=5; x++){
		for(int y=0; y<=2; y++){
			[numArr addObject:[NSValue valueWithCGPoint:ccp(x,y)]];
		}
	}
	
	NSMutableArray *randNumArr = [[[NSMutableArray alloc] init] autorelease];
	for(int z=0; z<18; z++){
		int numX = -1;
		int numY = -1;
		
		while (![numArr containsObject:[NSValue valueWithCGPoint:ccp(numX,numY)]]) {
			numX = arc4random()%6;
			numY = arc4random()%3;
		}
		[numArr removeObject:[NSValue valueWithCGPoint:ccp(numX,numY)]];
		[randNumArr addObject:[NSValue valueWithCGPoint:ccp(numX,numY)]];
	}
	
	for(int i=0; i<18; i+=2){
		CGPoint index1 = [[randNumArr objectAtIndex:i] CGPointValue];
		CGPoint index2 = [[randNumArr objectAtIndex:i+1] CGPointValue];
		[self createCardPairWithIndex1:index1 index2:index2];
	}
}

-(void) flipNewCards {
	for(int x=0; x<=5; x++){
		for(int y=0; y<=2; y++){
			MatchCard *card = [[cardDict objectForKey:[NSNumber numberWithInt:x]] objectForKey:[NSNumber numberWithInt:y]];
			[card flipCardNoAnim];
		}
	}
	[message setString:@"Go!"];
}

-(void) initCardDict {
	cardDict = [[NSMutableDictionary alloc] init];
	for(int x=0; x<=5; x++){
		[cardDict setObject:[[NSMutableDictionary alloc] init] forKey:[NSNumber numberWithInt:x]];
	}
	[cardDict retain];
}

-(void) step:(ccTime)delta {
	[super step:delta];
	if(gameState == MID_GAME){
		
	}
}

-(void) ccTouchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {	
	UITouch *touch = [touches anyObject];
    CGPoint point = [touch locationInView: [touch view]];
	point = [[CCDirector sharedDirector] convertToGL: point];

}

-(void) ccTouchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {	
	UITouch *touch = [touches anyObject];
    CGPoint point = [touch locationInView: [touch view]];
	point = [[CCDirector sharedDirector] convertToGL: point];
	

}

-(void) ccTouchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {	
	UITouch *touch = [touches anyObject];
    CGPoint point = [touch locationInView: [touch view]];
	point = [[CCDirector sharedDirector] convertToGL: point];
	
	if(gameState == MID_GAME){
		for(int x=0; x<=5; x++){
			for(int y=0; y<=2; y++){
				MatchCard *card = [[cardDict objectForKey:[NSNumber numberWithInt:x]] objectForKey:[NSNumber numberWithInt:y]];
				if(pointIsInRect(point, card.rect) && !card.cardIsUp){
					[self flipCard:card];
				}
			}
		}
	}
}

-(void) flipCard:(MatchCard*)card {
	cardsFlipped++;
	
	if(cardsFlipped%2==1){
		lastFlipped1 = card;
	}else{
		lastFlipped2 = card;
	}
	
	if(cardsFlipped != 0 && cardsFlipped%2==0){
		[self compareCards];
	}else{
		[card flipCardNoAnim];
		[message setString:@""];
	}
	
	if(cardsFlipped >= 18 && numberOfStrikes < 3){
		round++;

		[message setString:[NSString stringWithFormat:@"Great job! Round %i!",round]];

		gameTimer += 30;
		cardsFlipped = 0;
		numberOfStrikes = 0;
		[self deleteCardsAndStrikes];
		[self createCards];
		[self startStudyTime];
	}
}

-(void) compareCards {
	if([lastFlipped1.cardType isEqualToString:lastFlipped2.cardType]){
		currentScore++;
		[message setString:@"Correct!"];
		[self setCurrentScore];
		[lastFlipped2 flipCard];
	}else{
		[message setString:@"Wrong!"];
		[self addStrike];
		[lastFlipped2 flipCardNoAnim];
	}
}

-(void) addStrike {
	numberOfStrikes++;
	
	if(numberOfStrikes >= 3){
		[self gameOver];
	}
	
	CCSprite *sprite = [CCSprite spriteWithFile:@"red_x.png"];
	sprite.scale = 0.15f;
	sprite.position = ccp(numberOfStrikes*16+395,100);
	[self addChild:sprite z:Z_TIMED_GAME_HUD tag:numberOfStrikes];
}


-(void) startNewGame {
	[super startNewGame];
	gameTimer = 60;
	cardsFlipped = 0;
	numberOfStrikes = 0;
	round = 1;
	
	[self deleteCardsAndStrikes];
	[self createCards];
	[self startStudyTime];
	
	[message setString:@"Study the cards..."];
}

-(void) gameOver {
	[super gameOver];
}

/*
 Returns the managed object context for the application.
 If the context doesn't already exist, it is created and bound to the persistent store coordinator for the application.
 */
- (NSManagedObjectContext *) managedObjectContext {
	//Return the managedObjectContext if it already exists
    if (managedObjectContext != nil) {
        return managedObjectContext;
    }
	
	//Init the managedObjectContext
    NSPersistentStoreCoordinator *coordinator = [self persistentStoreCoordinator];
    if (coordinator != nil) {
        managedObjectContext = [[NSManagedObjectContext alloc] init];
        [managedObjectContext setPersistentStoreCoordinator: coordinator];
    }
    return managedObjectContext;
}

/*
 Returns the managed object model for the application.
 If the model doesn't already exist, it is created by merging all of the models found in the application bundle.
 */
- (NSManagedObjectModel *)managedObjectModel {
	//Return the managedObjectModel if it already exists
    if (managedObjectModel != nil) {
        return managedObjectModel;
    }
	
	//Init the managedObjectModel
    managedObjectModel = [[NSManagedObjectModel mergedModelFromBundles:nil] retain];    
    return managedObjectModel;
}

/*
 Returns the persistent store coordinator for the application.
 If the coordinator doesn't already exist, it is created and the application's store added to it.
 */
- (NSPersistentStoreCoordinator *)persistentStoreCoordinator {
    //Return the persistentStoreCoordinator if it already exists
	if (persistentStoreCoordinator != nil) {
        return persistentStoreCoordinator;
    }
	
	//Our file name
	NSString *fileName = @"memory.sqlite";

	//We get our file path
	NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
	NSString *documentsDirectory = [paths objectAtIndex:0];
	NSString *filePath = [documentsDirectory stringByAppendingPathComponent:fileName];
	NSURL *filePathURL = [NSURL fileURLWithPath:filePath];	
	
	//Init the persistentStoreCoordinator
    persistentStoreCoordinator = [[NSPersistentStoreCoordinator alloc] initWithManagedObjectModel:[self managedObjectModel]];
    [persistentStoreCoordinator addPersistentStoreWithType:NSSQLiteStoreType configuration:nil URL:filePathURL options:nil error:nil];
	
    return persistentStoreCoordinator;
}
-(void) addHiScoresToMenu {
	for(id score in hiScores){
		Hiscore *hiscore = (Hiscore*)score;
		NSString *scoreStr = [NSString stringWithFormat:@"%@: %i", hiscore.name, [hiscore.score intValue]];
		[hiScoresMenu addChild:[CCMenuItemFont itemFromString:scoreStr]];
	}
}
-(void) loadHiScores {
	//Initialization
	managedObjectContext = self.managedObjectContext;

	//Attempt to create SQLite database
	NSEntityDescription *entity;
	@try{
		//Define our table/entity to use  
		entity = [NSEntityDescription entityForName:@"Hiscore" inManagedObjectContext:managedObjectContext];   
	   
	}@catch (NSException *exception){
		NSLog(@"Caught %@: %@", [exception name], [exception reason]);
	
		//Copy SQLite template because creation failed
		NSString *fileName = @"memory.sqlite";
		NSString *templateName = @"memory_template.sqlite";
	
		//File paths
		NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
		NSString *documentsDirectory = [paths objectAtIndex:0];
		NSString *filePath = [documentsDirectory stringByAppendingPathComponent:fileName];

		if(![[NSFileManager defaultManager] fileExistsAtPath:filePath]){	
			//If file doesn't exist in document directory create a new one from the template
			[[NSFileManager defaultManager] copyItemAtPath:getActualPath(templateName) 
				toPath:[NSString stringWithFormat:@"%@/%@", documentsDirectory, fileName] error:nil];
		}
		
		//Finally define our table/entity to use
		entity = [NSEntityDescription entityForName:@"Hiscore" inManagedObjectContext:managedObjectContext];
	}
	
	//Setup the fetch request  
	NSFetchRequest *request = [[NSFetchRequest alloc] init];  
	[request setEntity:entity];   
  
	//Define how we will sort the records with a descriptor 
	NSSortDescriptor *sortDescriptor = [[NSSortDescriptor alloc] initWithKey:@"score" ascending:NO];  
	NSArray *sortDescriptors = [NSArray arrayWithObject:sortDescriptor];  
	[request setSortDescriptors:sortDescriptors];  
	[sortDescriptor release]; 

	//Init hiScores
	hiScores = [[managedObjectContext executeFetchRequest:request error:nil] mutableCopy];
	
	//Add an intial score if necessary	
	if(hiScores.count < 1){
		NSLog(@"Putting in initial hi score");
		currentScore = 0;
		currentPlayerName = @"Player1";
		[self addHiScore];
		hiScores = [[managedObjectContext executeFetchRequest:request error:nil] mutableCopy];
	}
	
	//Set the hi score
	Hiscore *highest = [hiScores objectAtIndex:0];
	hiScore = [highest.score intValue];
}

-(void) addHiScore {
	bool hasScore = NO;
	
	//Add score if player's name already exists
	for(id score in hiScores){
		Hiscore *hiscore = (Hiscore*)score;
		if([hiscore.name isEqualToString:currentPlayerName]){
			hasScore = YES;
			if(currentScore > [hiscore.score intValue]){
				hiscore.score = [NSNumber numberWithInt:currentScore];
			}
		}
	}

	//Add new score if player's name doesn't exist
	if(!hasScore){
		Hiscore *hiscoreObj = (Hiscore *)[NSEntityDescription insertNewObjectForEntityForName:@"Hiscore" inManagedObjectContext:managedObjectContext];  
		[hiscoreObj setName:currentPlayerName];
		[hiscoreObj setScore:[NSNumber numberWithInt:currentScore]];
		[hiScores addObject:hiscoreObj];
	}
	
	//Save managedObjectContext
    [managedObjectContext save:nil];
}

-(void) deleteHiScores {
	//Delete all Hi Score objects
	NSFetchRequest * allHiScores = [[NSFetchRequest alloc] init];
	[allHiScores setEntity:[NSEntityDescription entityForName:@"Hiscore" inManagedObjectContext:managedObjectContext]];
	[allHiScores setIncludesPropertyValues:NO]; //only fetch the managedObjectID

	NSArray * hs = [managedObjectContext executeFetchRequest:allHiScores error:nil];
	[allHiScores release];
	for (NSManagedObject *h in hs) {
	  [managedObjectContext deleteObject:h];
	}
	
	//Our file name
	NSString *fileName = @"memory.sqlite";

	//We get our file path
	NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
	NSString *documentsDirectory = [paths objectAtIndex:0];
	NSString *filePath = [documentsDirectory stringByAppendingPathComponent:fileName];

	//Delete our file
	[[NSFileManager defaultManager] removeItemAtPath:filePath error:nil];
	
	[message setString:@"Hi scores deleted!"];
	
	hiScore = 0;
	[hiScores removeAllObjects];
	[hiScores release];
	hiScores = nil;
	
	//Finally, load clean hi scores
	[self loadHiScores];
}

-(void) cleanRecipe {
	[cardDict release];
	[super cleanRecipe];
}

@end
