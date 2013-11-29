#import "Recipe.h"
#import "SlidingMenuGrid.h"

@interface Ch5_VertSlidingMenuGrid : Recipe
{
}

-(CCLayer*) runRecipe;
-(void) buttonClicked:(id)sender;
-(ccColor3B) randomColor;

@end

@implementation Ch5_VertSlidingMenuGrid

-(CCLayer*) runRecipe {
	[super runRecipe];
    
	message.position = ccp(200,270);
	[self showMessage:@"Tap a button or slide the menu grid up or down."];
	
	//Init item array
	NSMutableArray* allItems = [[NSMutableArray alloc] init];
	
	/* Create 45 CCMenuItemSprite objects with tags, callback methods and randomized colors */
	for (int i = 1; i <= 45; ++i) {
		CCSprite* normalSprite = [CCSprite spriteWithFile:@"sliding_menu_button_0.png"];
		CCSprite* selectedSprite = [CCSprite spriteWithFile:@"sliding_menu_button_1.png"];
		ccColor3B color = [self randomColor];
		normalSprite.color = color;
		selectedSprite.color = color;
		
		CCMenuItemSprite* item = [CCMenuItemSprite itemFromNormalSprite:normalSprite selectedSprite:selectedSprite target:self selector:@selector(buttonClicked:)];
		item.tag = i;
		
		//Add each item to array
		[allItems addObject:item];
	}
	
	//Init SlidingMenuGrid object with array and some other information
	SlidingMenuGrid* menuGrid = [SlidingMenuGrid menuWithArray:allItems cols:5 rows:3 position:ccp(70.f,220.f) padding:ccp(90.f,80.f) verticalPages:true];
	[self addChild:menuGrid z:1];	
	
	return self;
}

//Button clicked callback
-(void) buttonClicked:(id)sender {
	CCMenuItemSprite *sprite = (CCMenuItemSprite*)sender;
	[self showMessage:[NSString stringWithFormat:@"Button clicked: %d", sprite.tag]];
}

//Random base color method
-(ccColor3B) randomColor {
	int num = arc4random()%13;
	if(num == 0){
		return ccc3(255,255,255);
	}else if(num == 1){
		return ccc3(255,0,0);
	}else if(num == 2){
		return ccc3(0,255,0);
	}else if(num == 3){
		return ccc3(0,255,0);
	}else if(num == 4){
		return ccc3(255,255,0);
	}else if(num == 5){
		return ccc3(0,255,255);
	}else if(num == 6){
		return ccc3(255,0,255);
	}else if(num == 7){
		return ccc3(255,128,0);
	}else if(num == 8){
		return ccc3(255,0,128);
	}else if(num == 9){
		return ccc3(128,255,0);
	}else if(num == 10){
		return ccc3(0,255,128);
	}else if(num == 11){
		return ccc3(128,0,255);
	}else{
		return ccc3(0,128,255);
	}
}

@end
