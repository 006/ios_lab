#import "cocos2d.h"
#import "Recipe.h"

//Interface

@interface Ch3_SavingSimpleData : Recipe
{
	NSString *difficulty;
	CCMenuItemFont* easyMIF;
	CCMenuItemFont* mediumMIF;
	CCMenuItemFont* hardMIF;
	CCMenuItemFont* insaneMIF;
	CCMenu *mainMenu;
}
-(CCLayer*) runRecipe;
-(void) loadDifficulty;
-(void) saveDifficulty;
-(void) chooseEasy;
-(void) chooseMedium;
-(void) chooseHard;
-(void) chooseInsane;
-(void) resetMenuColors;
-(void) setDifficultyFromValue;

@end



//Implementation

@implementation Ch3_SavingSimpleData

-(CCLayer*) runRecipe {
	//Set font size
	[CCMenuItemFont setFontSize:30];

	//Add main label
	CCLabelBMFont *chooseDifficultyLabel = [CCLabelBMFont labelWithString:@"CHOOSE DIFFICULTY:" fntFile:@"eurostile_30.fnt"];
	chooseDifficultyLabel.position = ccp(240,250);
	chooseDifficultyLabel.scale = 0.5f;
	[self addChild:chooseDifficultyLabel z:1];
	
	//Add difficulty choices
	easyMIF = [CCMenuItemFont itemFromString:@"Easy" target:self selector:@selector(chooseEasy)];
	mediumMIF = [CCMenuItemFont itemFromString:@"Medium" target:self selector:@selector(chooseMedium)];
	hardMIF = [CCMenuItemFont itemFromString:@"Hard" target:self selector:@selector(chooseHard)];
	insaneMIF = [CCMenuItemFont itemFromString:@"Insane" target:self selector:@selector(chooseInsane)];
		
	mainMenu = [CCMenu menuWithItems:easyMIF, mediumMIF, hardMIF, insaneMIF, nil];
	[mainMenu alignItemsVertically];
	mainMenu.position = ccp(240,140);
    [self addChild:mainMenu z:1];	
	
	//Load any previously chosen difficulty
	[self loadDifficulty];
	
	return self;
}

-(void) loadDifficulty {
	//If a difficulty is set we use that, otherwise we choose Medium
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	if([defaults stringForKey:@"simple_data_difficulty"]){
		difficulty = [defaults stringForKey:@"simple_data_difficulty"];
		[self setDifficultyFromValue];
	}else{
		[self chooseMedium];
	}
}
-(void) saveDifficulty {
	//Save our difficulty
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	[defaults setObject:difficulty forKey:@"simple_data_difficulty"];
	[defaults synchronize];
}
-(void) resetMenuColors {
	//Menu color management
	[easyMIF setColor:ccc3(255,255,255)];
	[mediumMIF setColor:ccc3(255,255,255)];
	[hardMIF setColor:ccc3(255,255,255)];
	[insaneMIF setColor:ccc3(255,255,255)];
}
-(void) setDifficultyFromValue {
	//More menu color management
	[self resetMenuColors];
	
	if([difficulty isEqualToString:@"Easy"]){
		[easyMIF setColor:ccc3(255,0,0)];
	}else if([difficulty isEqualToString:@"Medium"]){
		[mediumMIF setColor:ccc3(255,0,0)];
	}else if([difficulty isEqualToString:@"Hard"]){
		[hardMIF setColor:ccc3(255,0,0)];
	}else if([difficulty isEqualToString:@"Insane"]){
		[insaneMIF setColor:ccc3(255,0,0)];
	}
	
	[self saveDifficulty];
}

/* Shortcut callback methods */
-(void) chooseEasy {
	difficulty = @"Easy";
	[self setDifficultyFromValue];
}
-(void) chooseMedium {
	difficulty = @"Medium";
	[self setDifficultyFromValue];
}
-(void) chooseHard {
	difficulty = @"Hard";
	[self setDifficultyFromValue];
}
-(void) chooseInsane {
	difficulty = @"Insane";
	[self setDifficultyFromValue];
}

@end
