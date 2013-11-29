#import "Recipe.h"
#import "mcLua.hpp"
#import "ShadowLabel.h"

enum {
	TAG_BL_BG = 0,
	TAG_BL_DOOR = 1,
	TAG_BL_GUY_1 = 2,
	TAG_BL_GUY_2 = 3,
	TAG_BL_LOUIE = 4,
	TAG_BL_OFFICER = 5,
	TAG_BL_YOUR_GUN = 6,
	TAG_BL_GUN_POINTED_AT_YOU = 7
};

//Interface
@interface Ch7_LuaDecisionTree : Recipe
{
	class mcLuaManager * lua_;
	mcLuaScript * sc;
	int response;
	CCLabelTTF *textLabel;
	NSString *text;
	NSString *bufferString;
	float timeElapsed;
	bool dialogForward;
	bool canMoveForward;
	bool wrapNext;
	
	int optionsOnScreen;
	CCNode *optionsNode;
	
	bool gunsDown;
}

-(CCLayer*) runRecipe;
-(void) addSpriteFrame:(NSString*)name z:(int)z tag:(int)tag visible:(bool)v;
-(void) addSpriteFile:(NSString*)name z:(int)z tag:(int)tag visible:(bool)v;
-(void) step:(ccTime)delta;
-(void) desc:(NSString*)str;
-(void) anim:(NSString*)str;
-(void) dialog:(NSString*)str;
-(int) logic:(NSString*)str;
-(void) dialogOption:(NSString*)str;
-(void) actionOption:(NSString*)str;
-(void) presentOptions;
-(void) selectOption:(id)sender;

@property (readwrite, assign) int response;
@property (readwrite, assign) mcLuaScript * sc;

@end

//Callback pointer
Ch7_LuaDecisionTree *ldtRecipe = nil;

//Static C functions
static int ldtDesc(lua_State * l) {
	[ldtRecipe desc:[NSString stringWithUTF8String:lua_tostring(l,1)]];
	ldtRecipe.sc->YieldPause();
	return (lua_yield(l, 0));
}
static int ldtAnim(lua_State * l) {
	[ldtRecipe anim:[NSString stringWithUTF8String:lua_tostring(l,1)]];
	return 0;
}
static int ldtDialog(lua_State * l) {
	[ldtRecipe dialog:[NSString stringWithUTF8String:lua_tostring(l,1)]];
	ldtRecipe.sc->YieldPause();
	return (lua_yield(l, 0));
}
static int ldtLogic(lua_State * l) {
	int num = [ldtRecipe logic:[NSString stringWithUTF8String:lua_tostring(l,1)]];
	lua_pushnumber(l,num);
	return 1;
}
static int ldtDialogOption(lua_State * l) {
	[ldtRecipe dialogOption:[NSString stringWithUTF8String:lua_tostring(l,1)]];
	return 0;
}
static int ldtActionOption(lua_State * l) {
	[ldtRecipe actionOption:[NSString stringWithUTF8String:lua_tostring(l,1)]];
	return 0;
}
static int ldtGetResponse(lua_State * l) {
	lua_pushnumber(l,ldtRecipe.response);	//Push a number onto the stack
	return 1;	//We are returning 1 result
}
static int ldtPresentOptions(lua_State * l) {
	[ldtRecipe presentOptions];
	ldtRecipe.sc->YieldPause();
	return (lua_yield(l, 0));
}

//Implementation
@implementation Ch7_LuaDecisionTree

@synthesize response, sc;

-(CCLayer*) runRecipe {
	//Superclass initialization
	[super runRecipe];

	//Initial variables
	response = 0;
	text = @"";
	bufferString = nil;
	timeElapsed = 0;
	dialogForward = NO;
	wrapNext = NO;
	optionsOnScreen = 0;
	canMoveForward = NO;
	gunsDown = NO;
	
	//Add sprite plist file
	[[CCSpriteFrameCache sharedSpriteFrameCache] addSpriteFramesWithFile:@"big_louie.plist"];
	
	//Add sprite frames
	[self addSpriteFrame:@"bl_bg.png" z:0 tag:TAG_BL_BG visible:YES];
	[self addSpriteFrame:@"bl_door.png" z:1 tag:TAG_BL_DOOR visible:YES];
	[self addSpriteFrame:@"bl_officer.png" z:2 tag:TAG_BL_OFFICER visible:NO];
	[self addSpriteFrame:@"bl_louie.png" z:3 tag:TAG_BL_LOUIE visible:YES];
	[self addSpriteFrame:@"bl_guy_1.png" z:3 tag:TAG_BL_GUY_1 visible:NO];
	[self addSpriteFrame:@"bl_guy_2.png" z:2 tag:TAG_BL_GUY_2 visible:NO];
	[self addSpriteFrame:@"bl_your_gun.png" z:4 tag:TAG_BL_YOUR_GUN visible:NO];
	[self addSpriteFile:@"bl_gun_pointed_at_you.png" z:4 tag:TAG_BL_GUN_POINTED_AT_YOU visible:NO];
	
		
	//Options Node
	optionsNode = [[CCNode alloc] init];
	optionsNode.position = ccp(0,0);
	optionsNode.visible = NO;
	[self addChild:optionsNode z:6];
			
	//Text label
	textLabel = [CCLabelBMFont labelWithString:@"" fntFile:@"eurostile_30.fnt"];
	textLabel.position = ccp(10,115);
	textLabel.scale = 0.65f;
	textLabel.color = ccc3(255,255,255);
	[textLabel setAnchorPoint:ccp(0,1)];
	[self addChild:textLabel z:6];

	//Dialog background image
	CCSprite *dialog_bg = [CCSprite spriteWithFile:@"dialog_bg.png"];
	dialog_bg.position = ccp(240,61.5f);
	[self addChild:dialog_bg z:5];

	//Set callback pointer
	ldtRecipe = self;

	//Lua initialization
	lua_ = new mcLuaManager;
	
	//Lua function wrapper library
	static const luaL_reg scriptLib[] = 
	{
		{"desc", ldtDesc },	
		{"anim", ldtAnim },
		{"dialog", ldtDialog },
		{"logic", ldtLogic },
		{"dialogOption", ldtDialogOption },
		{"actionOption", ldtActionOption },	
		{"getResponse", ldtGetResponse },
		{"presentOptions", ldtPresentOptions },
		{NULL, NULL}
	};
	lua_->LuaOpenLibrary("scene",scriptLib);

	//Open Lua script
	sc = lua_->CreateScript();
	NSString *filePath = [[NSBundle mainBundle] pathForResource:@"decision_tree.lua" ofType:@""]; 
	sc->LoadFile([filePath UTF8String]);

	//Set initial update method counter
	lua_->Update(0);
	
	//Schedule step method
	[self schedule: @selector(step:)];

	return self;
}

/* Add sprite frame helper method */
-(void) addSpriteFrame:(NSString*)name z:(int)z tag:(int)tag visible:(bool)v {
	CCSprite *sprite = [CCSprite spriteWithSpriteFrameName:name];
	sprite.position = ccp(240,160);
	sprite.visible = v;
	[self addChild:sprite z:z tag:tag];
}

/* Add sprite file helper method */
-(void) addSpriteFile:(NSString*)name z:(int)z tag:(int)tag visible:(bool)v {
	CCSprite *sprite = [CCSprite spriteWithFile:name];
	sprite.position = ccp(240,160);
	sprite.visible = v;
	[self addChild:sprite z:z tag:tag];
}

-(void) cleanRecipe {
	free(lua_);
	lua_ = nil;
	
	[text release];
	text = nil;
	
	[bufferString release];
	bufferString = nil;
	
	[super cleanRecipe];
}

-(void) step:(ccTime)delta {
	//Update Lua script runner
	lua_->Update(delta);
	
	//Increment elapsed time
	timeElapsed += delta;
		
	//If enough time has passed and there is still data in the buffer, we show another character
	if(bufferString && timeElapsed > 0.002f && [bufferString length] > 0){
		//Reset elapsed time
		timeElapsed = 0.0f;
	
		//Add next character
		NSString *nextChar = [[bufferString substringToIndex:1] retain];	
		text = [[NSString stringWithFormat:@"%@%@", text, nextChar] retain];
	
		//Text wrapping
		if([text length] > 0 && [text length]%40 == 0){
			wrapNext = YES;
		}
		
		//Wrap at next word break
		if(wrapNext && [[text substringFromIndex:text.length-1] isEqualToString:@" "]){
			text = [[NSString stringWithFormat:@"%@\n", text] retain];
			wrapNext = NO;
		}
	
		//Chop off last letter in buffer
		if([bufferString length] > 1){
			bufferString = [[bufferString substringFromIndex:1] retain];
		}else{
			bufferString = @"";
		}
		
		//Finally, set text label
		[textLabel setString:text];
	}else{
		//Move dialog forward as necessary
		if(!bufferString || [bufferString length] == 0){
			if(dialogForward){
				dialogForward = NO;
				[bufferString release];
				bufferString = nil;
			}else{
				canMoveForward = YES;
			}
		}
	}
}

/* Show text callback */
-(void) showText:(NSString*)str {
	text = @"";
	bufferString = [str retain];
}

/* Description callback */
-(void) desc:(NSString*)str {
	[self showText:[NSString stringWithFormat:@"[%@]",str]];
}

/* Animation callback */
-(void) anim:(NSString*)str {
	if([str isEqualToString:@"Open door"]){
		[self getChildByTag:TAG_BL_DOOR].visible = NO;
	}else if([str isEqualToString:@"Enter officer"]){
		[self getChildByTag:TAG_BL_OFFICER].visible = YES;
	}else if([str isEqualToString:@"Pull guns"]){
		[self getChildByTag:TAG_BL_YOUR_GUN].visible = YES;
		[self getChildByTag:TAG_BL_GUY_1].visible = YES;
		[self getChildByTag:TAG_BL_GUY_2].visible = YES;
	}else if([str isEqualToString:@"Louie looks away"]){
		CCSprite *sprite = (CCSprite*)[self getChildByTag:TAG_BL_LOUIE];
		[sprite setDisplayFrame:[[CCSpriteFrameCache sharedSpriteFrameCache] spriteFrameByName:@"bl_louie_look_away.png"]];
	}else if([str isEqualToString:@"Louie looks at you"]){
		CCSprite *sprite = (CCSprite*)[self getChildByTag:TAG_BL_LOUIE];
		[sprite setDisplayFrame:[[CCSpriteFrameCache sharedSpriteFrameCache] spriteFrameByName:@"bl_louie.png"]];
	}else if([str isEqualToString:@"Louie scowls"]){
		CCSprite *sprite = (CCSprite*)[self getChildByTag:TAG_BL_LOUIE];
		[sprite setDisplayFrame:[[CCSpriteFrameCache sharedSpriteFrameCache] spriteFrameByName:@"bl_louie_angry.png"]];
	}else if([str isEqualToString:@"Officer shocked"]){
		CCSprite *sprite = (CCSprite*)[self getChildByTag:TAG_BL_OFFICER];
		[sprite setDisplayFrame:[[CCSpriteFrameCache sharedSpriteFrameCache] spriteFrameByName:@"bl_officer_shocked.png"]];
	}else if([str isEqualToString:@"Put guns down"]){
		CCSprite *sprite1 = (CCSprite*)[self getChildByTag:TAG_BL_GUY_1];
		[sprite1 setDisplayFrame:[[CCSpriteFrameCache sharedSpriteFrameCache] spriteFrameByName:@"bl_guy_1_no_gun.png"]];
		CCSprite *sprite2 = (CCSprite*)[self getChildByTag:TAG_BL_GUY_2];
		[sprite2 setDisplayFrame:[[CCSpriteFrameCache sharedSpriteFrameCache] spriteFrameByName:@"bl_guy_2_no_gun.png"]];
	}else if([str isEqualToString:@"Pull gun on Louie"]){
		CCSprite *sprite = (CCSprite*)[self getChildByTag:TAG_BL_YOUR_GUN];
		sprite.position = ccp(140,160);
	}else if([str isEqualToString:@"Pull gun on men"]){
		CCSprite *sprite = (CCSprite*)[self getChildByTag:TAG_BL_YOUR_GUN];
		sprite.flipX = YES;
		sprite.position = ccp(340,160);
	}else if([str isEqualToString:@"Gun pointed at you"]){
		CCSprite *sprite = (CCSprite*)[self getChildByTag:TAG_BL_GUN_POINTED_AT_YOU];
		sprite.visible = YES;
	}
}

/* Dialog callback */
-(void) dialog:(NSString*)str {
	[self showText:str];
}

/* Logic callback */
-(int) logic:(NSString*)str {
	int num = 0;
		
	if([str isEqualToString:@"Put guns down"]){
		gunsDown = YES;
	}else if([str isEqualToString:@"Are guns down?"]){
		if(gunsDown){
			num = 1;
		}else{
			num = 0;
		}
	}else if([str isEqualToString:@"You win"]){
		[self showMessage:@"You WIN!!"];
	}
	
	return num;
}

/* Dialog option callback */
-(void) dialogOption:(NSString*)str {
	[CCMenuItemFont setFontName:@"Arial"];
	[CCMenuItemFont setFontSize:16];
	
	optionsOnScreen += 1;
	
	//Add dialog option to screen
	NSString *dialogStr = [NSString stringWithFormat:@"\"%@\"", str];
	CCMenuItemFont *optionItem = [CCMenuItemFont itemFromString:dialogStr target:self selector:@selector(selectOption:)];
	optionItem.tag = optionsOnScreen;
	optionItem.position = ccp(10,135 - optionsOnScreen*20);
	[optionItem setAnchorPoint:ccp(0,1)];
	
	CCMenu *menu = [CCMenu menuWithItems:optionItem, nil];
	menu.position = ccp(0,0);
	[optionsNode addChild:menu];
}

/* Action option callback */
-(void) actionOption:(NSString*)str {
	[CCMenuItemFont setFontName:@"Arial"];
	[CCMenuItemFont setFontSize:16];
	
	optionsOnScreen += 1;
	
	//Add action option to screen
	NSString *optionStr = [NSString stringWithFormat:@"[%@]", str];
	CCMenuItemFont *optionItem = [CCMenuItemFont itemFromString:optionStr target:self selector:@selector(selectOption:)];
	optionItem.tag = optionsOnScreen;
	optionItem.position = ccp(10,135 - optionsOnScreen*20);
	[optionItem setAnchorPoint:ccp(0,1)];
	
	CCMenu *menu = [CCMenu menuWithItems:optionItem, nil];
	menu.position = ccp(0,0);
	[optionsNode addChild:menu];
}

/* Present options callback */
-(void) presentOptions {
	text = @"";
	[textLabel setString:text];
	optionsNode.visible = YES;
}

/* Select option callback */
-(void) selectOption:(id)sender {
	CCMenuItemFont *item = (CCMenuItemFont*)sender;
	response = item.tag;
	
	//Remove all children
	for(CCNode *n in [optionsNode children]){
		[optionsNode removeChild:n cleanup:YES];
	}
	[self removeChild:optionsNode cleanup:YES];
	
	//Re-add optionsNode
	optionsNode = [[CCNode alloc] init];
	optionsNode.position = ccp(0,0);
	optionsNode.visible = NO;
	[self addChild:optionsNode z:6];
	
	optionsOnScreen = 0;
	
	//Resume the script
	sc->YieldResume();
}
-(void) ccTouchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {
	UITouch *touch = [touches anyObject];
	CGPoint point = [touch locationInView: [touch view]];
	point = [[CCDirector sharedDirector] convertToGL: point];		

	//Move the dialog forward
	if(point.y < 123.0f && canMoveForward){
		bufferString = @"";
		dialogForward = YES;
		wrapNext = NO;
		sc->YieldResume();
		canMoveForward = NO;
	}
}


@end