#import "Recipe.h"
#import "LoopingMenu.h"

@interface Ch5_HorizScrollMenu : Recipe
{
}

-(CCLayer*) runRecipe;
-(void) bookClicked:(id)sender;

@end

@implementation Ch5_HorizScrollMenu

-(CCLayer*) runRecipe {
	[super runRecipe];
    
	message.position = ccp(70,270);
	
	/* Create default sprites and 'selected' sprites */
	CCSprite *book1 = [CCSprite spriteWithFile:@"book1.jpg"];
	CCSprite *book2 = [CCSprite spriteWithFile:@"book2.jpg"];
	CCSprite *book3 = [CCSprite spriteWithFile:@"book3.jpg"];
	CCSprite *book4 = [CCSprite spriteWithFile:@"book4.jpg"];
	CCSprite *book5 = [CCSprite spriteWithFile:@"book5.jpg"];
	
	CCSprite *book1_selected = [CCSprite spriteWithFile:@"book1.jpg"]; book1_selected.color = ccc3(128,128,180); [book1_selected setBlendFunc: (ccBlendFunc) { GL_ONE, GL_ONE }];
	CCSprite *book2_selected = [CCSprite spriteWithFile:@"book2.jpg"]; book2_selected.color = ccc3(128,128,180); [book2_selected setBlendFunc: (ccBlendFunc) { GL_ONE, GL_ONE }];
	CCSprite *book3_selected = [CCSprite spriteWithFile:@"book3.jpg"]; book3_selected.color = ccc3(128,128,180); [book3_selected setBlendFunc: (ccBlendFunc) { GL_ONE, GL_ONE }];
	CCSprite *book4_selected = [CCSprite spriteWithFile:@"book4.jpg"]; book4_selected.color = ccc3(128,128,180); [book4_selected setBlendFunc: (ccBlendFunc) { GL_ONE, GL_ONE }];
	CCSprite *book5_selected = [CCSprite spriteWithFile:@"book5.jpg"]; book5_selected.color = ccc3(128,128,180); [book5_selected setBlendFunc: (ccBlendFunc) { GL_ONE, GL_ONE }];
		
	/* Create CCMenuItemSprites */
	CCMenuItemSprite* item1 = [CCMenuItemSprite itemFromNormalSprite:book1 selectedSprite:book1_selected target:self selector:@selector(bookClicked:)];
	item1.tag = 1;
	CCMenuItemSprite* item2 = [CCMenuItemSprite itemFromNormalSprite:book2 selectedSprite:book2_selected target:self selector:@selector(bookClicked:)];
	item2.tag = 2;
	CCMenuItemSprite* item3 = [CCMenuItemSprite itemFromNormalSprite:book3 selectedSprite:book3_selected target:self selector:@selector(bookClicked:)];
	item3.tag = 3;
	CCMenuItemSprite* item4 = [CCMenuItemSprite itemFromNormalSprite:book4 selectedSprite:book4_selected target:self selector:@selector(bookClicked:)];
	item4.tag = 4;
	CCMenuItemSprite* item5 = [CCMenuItemSprite itemFromNormalSprite:book5 selectedSprite:book5_selected target:self selector:@selector(bookClicked:)];
	item5.tag = 5;
	
	//Initialize and add LoopingMenu
	LoopingMenu *menu = [LoopingMenu menuWithItems:item1, item2, item3, item4, item5, nil];
	menu.position = ccp(240, 150);
	[menu alignItemsHorizontallyWithPadding:0];
	[self addChild:menu];
	
	return self;
}

//Book clicked callback
-(void) bookClicked:(id)sender {
	CCMenuItemSprite *sprite = (CCMenuItemSprite*)sender;
	[self showMessage:[NSString stringWithFormat:@"Book clicked: %d", sprite.tag]];
}

@end
