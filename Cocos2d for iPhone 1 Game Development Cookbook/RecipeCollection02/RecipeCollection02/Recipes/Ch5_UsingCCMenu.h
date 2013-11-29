#import "Recipe.h"
#import "Box2D.h"

//Options Menu
@interface OptionsMenu : CCLayer {
	CCLabelBMFont *message;
}

+(id) scene;
-(void) back:(id)sender;
-(void) soundToggle: (id) sender;
-(void) difficultyToggle: (id) sender;

@end

@implementation OptionsMenu

+(id) scene {
	//Create a scene
	CCScene *s = [CCScene node];	
	OptionsMenu *node = [OptionsMenu node];
	[s addChild:node z:0 tag:0];
	return s;
}

-(id) init {
	if( (self=[super init] )) {	
		//Random background color
		CCSprite *bg = [CCSprite spriteWithFile:@"blank.png"];
		bg.position = ccp(240,160);
		[bg setTextureRect:CGRectMake(0,0,480,320)];
		[bg setColor:ccc3(arc4random()%150,arc4random()%150,arc4random()%150)];
		[self addChild:bg];
		
		//Init our message label
		message = [CCLabelBMFont labelWithString:@"" fntFile:@"eurostile_30.fnt"];
		message.position = ccp(160,270);
		message.scale = 0.5f;
		[message setColor:ccc3(255,255,255)];
		[self addChild:message z:10];
	
		//Disabled title label for Sound option
		CCMenuItemFont *title1 = [CCMenuItemFont itemFromString:@"Sound"];
		[title1 setIsEnabled:NO];
		title1.color = ccc3(0,0,0);
		
		//Toggleable item for Sound option
		CCMenuItemToggle *item1 = [CCMenuItemToggle itemWithTarget:self selector:@selector(soundToggle:) items:
			[CCMenuItemFont itemFromString: @"On"], [CCMenuItemFont itemFromString: @"Off"], nil];

		//Disabled title label for Difficulty option
		CCMenuItemFont *title2 = [CCMenuItemFont itemFromString:@"Difficulty"];
		[title2 setIsEnabled:NO];
		title2.color = ccc3(0,0,0);
		
		//Toggleable item for Difficulty option
		CCMenuItemToggle *item2 = [CCMenuItemToggle itemWithTarget:self selector:@selector(difficultyToggle:) items:
			[CCMenuItemFont itemFromString: @"Easy"], [CCMenuItemFont itemFromString: @"Medium"],
			[CCMenuItemFont itemFromString: @"Hard"], [CCMenuItemFont itemFromString: @"Insane"], nil];
	
		//Back button
		CCMenuItemFont *back = [CCMenuItemFont itemFromString:@"Back" target:self selector:@selector(back:)];
	
		//Finally, create our menu
		CCMenu *menu = [CCMenu menuWithItems:
					  title1, title2,
					  item1, item2,
					  back, nil]; // 5 items.
		
		//Align items in columns
		[menu alignItemsInColumns:
		 [NSNumber numberWithUnsignedInt:2],
		 [NSNumber numberWithUnsignedInt:2],
		 [NSNumber numberWithUnsignedInt:1],
		 nil
		];
		
		[self addChild:menu];
	}
	return self;
}
-(void) back:(id)sender {
	[[CCDirector sharedDirector] popScene];
}
//Use the 'selectedIndex' variable to identify the touched item
-(void) soundToggle: (id) sender {
	CCMenuItem *item = (CCMenuItem*)sender;
	[message setString:[NSString stringWithFormat:@"Selected Sound Index:%d", [item selectedIndex]]];
}
-(void) difficultyToggle: (id) sender {
	CCMenuItem *item = (CCMenuItem*)sender;
	[message setString:[NSString stringWithFormat:@"Selected Difficulty Index:%d", [item selectedIndex]]];
}

@end


@interface Ch5_UsingCCMenu : Recipe
{
	CCMenuItemFont *optionsItem;
	CCMenu *menu;
}

-(CCLayer*) runRecipe;
-(void) buttonTouched:(id)sender;
-(void) options:(id)sender;
-(void) enableOptions:(id)sender;
-(void) reAlign:(id)sender;

@end

@implementation Ch5_UsingCCMenu

-(CCLayer*) runRecipe {
	[super runRecipe];
    
	//Set font size/name
	[CCMenuItemFont setFontSize:30];
	[CCMenuItemFont setFontName:@"Marker Felt"];
	
	//Image Button
	CCMenuItemSprite *imageButton = [CCMenuItemSprite itemFromNormalSprite:[CCSprite spriteWithFile:@"button_unselected.png"] 
		selectedSprite:[CCSprite spriteWithFile:@"button_selected.png"] disabledSprite:[CCSprite spriteWithFile:@"button_disabled.png"]
		target:self selector:@selector(buttonTouched:)];  
	
	//Enable Options Label
	CCLabelBMFont *enableOptionsLabel = [CCLabelBMFont labelWithString:@"Enable Options" fntFile:@"eurostile_30.fnt"];
	CCMenuItemLabel *enableOptions = [CCMenuItemLabel itemWithLabel:enableOptionsLabel target:self selector:@selector(enableOptions:)];

	//Options Label
	optionsItem = [CCMenuItemFont itemFromString:@"Options" target:self selector:@selector(options:)];
	optionsItem.isEnabled = NO;
	
	//Re-Align Label
	CCMenuItemFont *reAlign = [CCMenuItemFont itemFromString:@"Re-Align" target:self selector:@selector(reAlign:)];
	
	//Add menu items
	menu = [CCMenu menuWithItems: imageButton, enableOptions, optionsItem, reAlign, nil];
	[menu alignItemsVertically];
	[self addChild:menu];	

	return self;
}

-(void) buttonTouched:(id)sender {
    [message setString:@"Button touched!"];
}

-(void) options:(id)sender {
	[[CCDirector sharedDirector] pushScene:[OptionsMenu scene]];
}

-(void) enableOptions:(id)sender {
	optionsItem.isEnabled = ~optionsItem.isEnabled;
}

//Randomly re-align our menu
-(void) reAlign:(id)sender {
	int n = arc4random()%6;
	if(n == 0){
		[menu alignItemsVertically];
	}else if(n == 1){
		[menu alignItemsHorizontally];
	}else if(n == 2){
		[menu alignItemsHorizontallyWithPadding:arc4random()%30];
	}else if(n == 3){
		[menu alignItemsVerticallyWithPadding:arc4random()%30];
	}else if(n == 4){
		[menu alignItemsInColumns: [NSNumber numberWithUnsignedInt:2], [NSNumber numberWithUnsignedInt:2], nil];
	}else if(n == 5){
		[menu alignItemsInRows: [NSNumber numberWithUnsignedInt:2], [NSNumber numberWithUnsignedInt:2], nil];
	}
}

@end
