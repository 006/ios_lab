#import "cocos2d.h"
#import "Recipe.h"
#import "SimpleTimedGameRecipe.h"

enum {	//Game state
	MID_GAME,
	GAME_OVER
};

enum {
	Z_TIMED_GAME_HUD = 20
};

//Interface
@interface SimpleTimedGameRecipe : Recipe
{
	int currentScore;
	NSString *currentPlayerName;
	int hiScore;
	NSMutableArray *hiScores;
	CCLabelBMFont *hiScoreNumberLabel;
	CCLabelBMFont *currentScoreNumberLabel;
	CCLabelBMFont *playerNameStringLabel;
	CCMenu *mainMenu;
	CCMenu *namesToChoose;
	CCMenu *hiScoresMenu;
	CCSprite *darkSprite;
	float gameTimer;
	CCLabelBMFont *gameTimeNumberLabel;
	int gameState;
}

-(CCLayer*) runRecipe;
-(void) loadHiScores;
-(void) deleteHiScores;
-(void) addHiScore;
-(void) viewHiScores;
-(void) addHiScoresToMenu;
-(void) setHiScore;
-(void) setCurrentScore;
-(void) setPlayerName;
-(void) startNewGame;
-(void) gameOver;
-(void) showNames;
-(void) chooseName1;
-(void) chooseName2;
-(void) chooseName3;
-(void) chooseName:(NSString*)name;

@end



//Implementation

@implementation SimpleTimedGameRecipe

-(CCLayer*) runRecipe {
	CGSize s = [[CCDirector sharedDirector] winSize];
	CCSpriteFrameCache *cache = [CCSpriteFrameCache sharedSpriteFrameCache];
	self.isTouchEnabled = YES;
	[CCMenuItemFont setFontSize:20];
	
	gameState = GAME_OVER;
	[self loadHiScores];
	currentPlayerName = @"Player1";
	gameTimer = 0;
	
	darkSprite = [CCSprite spriteWithFile:@"blank.png"];
	[darkSprite setTextureRect:CGRectMake(0,0,480,320)];
	darkSprite.position = ccp(240,160);
	[darkSprite setColor:ccc3(0,0,0)];
	darkSprite.opacity = 100;
	darkSprite.visible = YES;
	[self addChild:darkSprite z:Z_TIMED_GAME_HUD];
	
	message = [CCLabelBMFont labelWithString:@"" fntFile:@"eurostile_30.fnt"];
	message.position = ccp(160,270);
	message.scale = 0.75f;
	[message setColor:ccc3(255,0,0)];
	[self addChild:message z:Z_TIMED_GAME_HUD];
	
	CCLabelBMFont *hiScoreLabel = [CCLabelBMFont labelWithString:@"HI SCORE:" fntFile:@"eurostile_30.fnt"];
	hiScoreLabel.position = ccp(330,300);
	hiScoreLabel.scale = 0.5f;
	[self addChild:hiScoreLabel z:Z_TIMED_GAME_HUD];
	
	hiScoreNumberLabel = [CCLabelBMFont labelWithString:@"" fntFile:@"eurostile_30.fnt"];
	hiScoreNumberLabel.position = ccp(330,280);
	hiScoreNumberLabel.scale = 0.5f;
	[self addChild:hiScoreNumberLabel z:Z_TIMED_GAME_HUD];	
	
	CCLabelBMFont *currentScoreLabel = [CCLabelBMFont labelWithString:@"YOUR SCORE:" fntFile:@"eurostile_30.fnt"];
	currentScoreLabel.position = ccp(430,300);
	currentScoreLabel.scale = 0.5f;
	[self addChild:currentScoreLabel z:Z_TIMED_GAME_HUD];
	
	currentScoreNumberLabel = [CCLabelBMFont labelWithString:@"" fntFile:@"eurostile_30.fnt"];
	currentScoreNumberLabel.position = ccp(430,280);
	currentScoreNumberLabel.scale = 0.5f;
	[self addChild:currentScoreNumberLabel z:Z_TIMED_GAME_HUD];	
	
	CCLabelBMFont *gameTimeLabel = [CCLabelBMFont labelWithString:@"TIME LEFT:" fntFile:@"eurostile_30.fnt"];
	gameTimeLabel.position = ccp(430,250);
	gameTimeLabel.scale = 0.5f;
	[self addChild:gameTimeLabel z:Z_TIMED_GAME_HUD];
	
	gameTimeNumberLabel = [CCLabelBMFont labelWithString:@"" fntFile:@"eurostile_30.fnt"];
	gameTimeNumberLabel.position = ccp(430,230);
	gameTimeNumberLabel.scale = 0.5f;
	[self addChild:gameTimeNumberLabel z:Z_TIMED_GAME_HUD];	
	
	CCLabelBMFont *playerNameLabel = [CCLabelBMFont labelWithString:@"YOUR NAME:" fntFile:@"eurostile_30.fnt"];
	playerNameLabel.position = ccp(430,200);
	playerNameLabel.scale = 0.5f;
	[self addChild:playerNameLabel z:Z_TIMED_GAME_HUD];
	
	playerNameStringLabel = [CCLabelBMFont labelWithString:@"" fntFile:@"eurostile_30.fnt"];
	playerNameStringLabel.position = ccp(430,170);
	playerNameStringLabel.scale = 0.5f;
	[self addChild:playerNameStringLabel z:Z_TIMED_GAME_HUD];


	CCMenuItemFont* newGameMIF = [CCMenuItemFont itemFromString:@"NEW GAME" target:self selector:@selector(startNewGame)];
	CCMenuItemFont* chooseNameMIF = [CCMenuItemFont itemFromString:@"CHOOSE NAME" target:self selector:@selector(showNames)];
	CCMenuItemFont* viewHiScoresMIF = [CCMenuItemFont itemFromString:@"VIEW HI SCORES" target:self selector:@selector(viewHiScores)];
	CCMenuItemFont* deleteHiScoresMIF = [CCMenuItemFont itemFromString:@"DELETE HI SCORES" target:self selector:@selector(deleteHiScores)];
	
	mainMenu = [CCMenu menuWithItems:newGameMIF, chooseNameMIF, viewHiScoresMIF, deleteHiScoresMIF, nil];
	[mainMenu alignItemsVertically];
	mainMenu.position = ccp(240,140);
    [self addChild:mainMenu z:Z_TIMED_GAME_HUD];
	
	CCMenuItemFont* name1 = [CCMenuItemFont itemFromString:@"Player 1" target:self selector:@selector(chooseName1)];
	CCMenuItemFont* name2 = [CCMenuItemFont itemFromString:@"Player 2" target:self selector:@selector(chooseName2)];
	CCMenuItemFont* name3 = [CCMenuItemFont itemFromString:@"Player 3" target:self selector:@selector(chooseName3)];
	namesToChoose = [CCMenu menuWithItems:name1, name2, name3, nil];
	[namesToChoose alignItemsVertically];
    namesToChoose.position = ccp(240,180);
	namesToChoose.visible = NO;
    [self addChild:namesToChoose z:Z_TIMED_GAME_HUD];
				
	[self setCurrentScore];
	[self setHiScore];
	[self setPlayerName];
		
	[self schedule:@selector(step:)];

	return nil;
}

-(void) step:(ccTime)delta {
	if(gameState == MID_GAME){
		gameTimer -= delta;
		[gameTimeNumberLabel setString:[NSString stringWithFormat:@"%d",((int)gameTimer)] ];
	}
	
	if(gameTimer < 0 && gameState == MID_GAME){
		[self gameOver];
	}
}

-(void) setCurrentScore {
	[currentScoreNumberLabel setString: [NSString stringWithFormat:@"%d",currentScore] ];
	
	if(currentScore > hiScore){
		hiScore = currentScore;
		[self setHiScore];
	}
}

-(void) setHiScore {
	[hiScoreNumberLabel setString: [NSString stringWithFormat:@"%d",hiScore] ];
}

-(void) setPlayerName {
	[playerNameStringLabel setString: currentPlayerName];
}

-(void) loadHiScores {
	/* ABSTRACT */
}

-(void) addHiScore {
	/* ABSTRACT */
}

-(void) deleteHiScores {
	/* ABSTRACT */
}

-(void) viewHiScores {
	[self loadHiScores];

	[self removeChild:hiScoresMenu cleanup:YES];
	
	hiScoresMenu = [CCMenu menuWithItems:nil];
	
	[self addHiScoresToMenu];

	[hiScoresMenu addChild:[CCMenuItemFont itemFromString:@"<back>" target:self selector:@selector(hideHiScores)]];
	[hiScoresMenu alignItemsVertically];
	hiScoresMenu.position = ccp(240,180);
    [self addChild:hiScoresMenu z:Z_TIMED_GAME_HUD];

	hiScoresMenu.visible = YES;
	mainMenu.visible = NO;
}

-(void) addHiScoresToMenu {
	/* ABSTRACT */
}

-(void) hideHiScores {
	hiScoresMenu.visible = NO;
	mainMenu.visible = YES;
}

-(void) startNewGame {
	gameTimer = 30;
	
	gameState = MID_GAME;
		
	darkSprite.visible = NO;
	currentScore = 0;
	mainMenu.visible = NO;
	[self setCurrentScore];
	[self setHiScore];
	[self setPlayerName];
}

-(void) gameOver {
	gameState = GAME_OVER;

	darkSprite.visible = YES;
	mainMenu.visible = YES;
	namesToChoose.visible = NO;
	
	[message setString:@"GAME OVER!"];
		
	[self addHiScore];
}

-(void) showNames {
	mainMenu.visible = NO;
	namesToChoose.visible = YES;
}

-(void) chooseName1 { [self chooseName:@"Player1"]; }

-(void) chooseName2 { [self chooseName:@"Player2"]; }

-(void) chooseName3 { [self chooseName:@"Player3"]; }

-(void) chooseName:(NSString*)name {
	currentPlayerName = name;
	mainMenu.visible = YES;
	namesToChoose.visible = NO;
	[self setPlayerName];
}

@end