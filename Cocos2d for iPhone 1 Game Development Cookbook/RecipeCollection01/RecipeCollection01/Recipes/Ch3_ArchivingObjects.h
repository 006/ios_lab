#import "cocos2d.h"
#import "Recipe.h"
#import "SimpleCharacter.h"

//Interface

@interface Ch3_ArchivingObjects : Recipe
{
	SimpleCharacter * character;
	CCNode *hud;
}

-(CCLayer*) runRecipe;
-(void) loadCharacter;
-(void) saveCharacter;
-(void) loadHUD;
-(void) addTextToHUD:(NSString*)text atPosition:(CGPoint)pos withColor:(ccColor3B)color;
-(void) randomCharacter;
-(void) deleteData;
-(void) addTextToHUD:(NSString*)text atPosition:(CGPoint)pos withColor:(ccColor3B)color;
-(void) removeCharacter;

@end

//Implementation

@implementation Ch3_ArchivingObjects

-(CCLayer*) runRecipe {
	//Set font size
	[CCMenuItemFont setFontSize:20];
	
	//Add our message
	message = [CCLabelBMFont labelWithString:@"" fntFile:@"eurostile_30.fnt"];
	message.position = ccp(160,270);
	message.scale = 0.75f;
	[message setColor:ccc3(255,0,0)];
	[self addChild:message z:1];

	//Set up menu buttons
	CCMenuItemFont* randomCharacterMIF = [CCMenuItemFont itemFromString:@"<RANDOM>" target:self selector:@selector(randomCharacter)];
	CCMenuItemFont* deleteDataMIF = [CCMenuItemFont itemFromString:@"<DELETE DATA>" target:self selector:@selector(deleteData)];

	CCMenu *menu = [CCMenu menuWithItems:randomCharacterMIF, deleteDataMIF, nil];
	[menu alignItemsHorizontally];
	menu.position = ccp(240,50);
    [self addChild:menu z:1];

	//Finally, load our character
	[self loadCharacter];
	
	return self;
}

-(void) removeCharacter {
	[self removeChildByTag:0 cleanup:YES];
	[self removeChildByTag:1 cleanup:YES];
	character = [[SimpleCharacter alloc] init];
}

-(void) randomCharacter {
	[message setString:@""];

	[self removeCharacter];
	
	//Choose a random color and a random class
	int colorNum = arc4random()%6;
	int charClassNum = arc4random()%4;
	
	if(colorNum == 0){
		character.charColor = @"Red";
	}else if(colorNum == 1){
		character.charColor = @"Blue";
	}else if(colorNum == 2){
		character.charColor = @"Green";
	}else if(colorNum == 3){
		character.charColor = @"Yellow";
	}else if(colorNum == 4){
		character.charColor = @"Orange";
	}else if(colorNum == 5){
		character.charColor = @"Purple";
	}

	//Choose random attributes
	character.strength = arc4random()%10+5;
	character.dexterity = arc4random()%10+5;
	character.constitution = arc4random()%10+5;
	character.intelligence = arc4random()%10+5;
	character.wisdom = arc4random()%10+5;
	character.charisma = arc4random()%10+5;
	
	//Make specific attributes higher for specific classes
	if(charClassNum == 0){
		character.charClass = @"Wizard";
		character.intelligence += 10;
	}else if(charClassNum == 1){
		character.charClass = @"Warrior";
		character.strength += 5;
		character.constitution += 5;
	}else if(charClassNum == 2){
		character.charClass = @"Ranger";
		character.dexterity += 5;
		character.constitution += 5;
	}else if(charClassNum == 3){
		character.charClass = @"Shaman";
		character.wisdom += 10;
	}
	
	//Finally, add the character
	[character addCharacterToNode:self atPosition:ccp(300,180)];
	
	//Update the HUD
	[self loadHUD];
	
	//Save the character on the disk
	[self saveCharacter];
}

-(void) loadCharacter {
	//Our archive file name
	NSString *fileName = @"dnd_character.archive";

	//We get our file path
	NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
	NSString *documentsDirectory = [paths objectAtIndex:0];
	NSString *filePath = [documentsDirectory stringByAppendingPathComponent:fileName];

	if(![[NSFileManager defaultManager] fileExistsAtPath:filePath]){	
		//If file doesn't exist in document directory create a new default character and save it
		character = [[SimpleCharacter alloc] init];
		[NSKeyedArchiver archiveRootObject:character toFile:filePath];
	}else{
		//If it does we load it
		character = [[NSKeyedUnarchiver unarchiveObjectWithFile:filePath] retain];
	}
	
	//Add character and reload HUD
	[character addCharacterToNode:self atPosition:ccp(300,180)];	
	[self loadHUD];
}

-(void) saveCharacter {
	//Our archive file name
	NSString *fileName = @"dnd_character.archive";

	//We get our file path
	NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
	NSString *documentsDirectory = [paths objectAtIndex:0];
	NSString *filePath = [documentsDirectory stringByAppendingPathComponent:fileName];
	
	//Save character
	[NSKeyedArchiver archiveRootObject:character toFile:filePath];
}

-(void) loadHUD {
	//Remove old HUD
	if(hud){ [self removeChild:hud cleanup:YES]; }

	//Init new HUD
	hud = [[CCNode alloc] init];
	hud.position = ccp(0,0);
	
	//Add text to new HUD
	[self addTextToHUD:@"STR:" atPosition:ccp(100,220) withColor:ccc3(255,255,255)];
	[self addTextToHUD:[NSString stringWithFormat:@"%i",character.strength] atPosition:ccp(130,220) withColor:ccc3(255,255,255)];

	[self addTextToHUD:@"DEX:" atPosition:ccp(100,200) withColor:ccc3(255,255,255)];
	[self addTextToHUD:[NSString stringWithFormat:@"%i",character.dexterity] atPosition:ccp(130,200) withColor:ccc3(255,255,255)];

	[self addTextToHUD:@"CON:" atPosition:ccp(100,180) withColor:ccc3(255,255,255)];
	[self addTextToHUD:[NSString stringWithFormat:@"%i",character.constitution] atPosition:ccp(130,180) withColor:ccc3(255,255,255)];

	[self addTextToHUD:@"INT:" atPosition:ccp(100,160) withColor:ccc3(255,255,255)];
	[self addTextToHUD:[NSString stringWithFormat:@"%i",character.intelligence] atPosition:ccp(130,160) withColor:ccc3(255,255,255)];

	[self addTextToHUD:@"WIS:" atPosition:ccp(100,140) withColor:ccc3(255,255,255)];
	[self addTextToHUD:[NSString stringWithFormat:@"%i",character.wisdom] atPosition:ccp(130,140) withColor:ccc3(255,255,255)];

	[self addTextToHUD:@"CHA:" atPosition:ccp(100,120) withColor:ccc3(255,255,255)];
	[self addTextToHUD:[NSString stringWithFormat:@"%i",character.charisma] atPosition:ccp(130,120) withColor:ccc3(255,255,255)];

	[self addTextToHUD:[NSString stringWithFormat:@"%@ %@",character.charColor,character.charClass] atPosition:ccp(130,250) withColor:ccc3(255,255,255)];

	[self addChild:hud z:1];
}

-(void) addTextToHUD:(NSString*)text atPosition:(CGPoint)pos withColor:(ccColor3B)color {
	CCLabelBMFont *label = [CCLabelBMFont labelWithString:text fntFile:@"eurostile_30.fnt"];
	label.position = pos;
	label.scale = 0.5f;
	label.color = color;
	[hud addChild:label z:1];
}

-(void) deleteData {
	//Our archive file name
	NSString *fileName = @"dnd_character.archive";

	//We get our file path
	NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
	NSString *documentsDirectory = [paths objectAtIndex:0];
	NSString *filePath = [documentsDirectory stringByAppendingPathComponent:fileName];

	//Delete our file
	[[NSFileManager defaultManager] removeItemAtPath:filePath error:nil];
	
	//Set removal message
	[message setString:@"Data deleted!"];

	//Remove character node and load a new default character
	[self removeCharacter];
	[self loadCharacter];
}

@end
