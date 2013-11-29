#import "Recipe.h"
#import "mcLua.hpp"

//Interface
@interface Ch7_LuaScripting : Recipe
{
	class mcLuaManager * lua_;
	mcLuaScript * sc;
}

-(CCLayer*) runRecipe;
-(void) step:(ccTime)delta;
-(void) resumeScript:(id)sender;

@end

//Callback pointer
Ch7_LuaScripting *lsRecipe = nil;

//Static append message C function
static int lsAppendMessage(lua_State * l)
{
	//Pass lua string into append message method
	[lsRecipe appendMessage:[NSString stringWithUTF8String:lua_tostring(l,1)]];
	return 0;
}

//Implementation
@implementation Ch7_LuaScripting

-(CCLayer*) runRecipe {
	//Superclass initialization and message
	[super runRecipe];
	[self showMessage:@"Loading Lua script..."];

	//Set callback pointer
	lsRecipe = self;

	//Lua initialization
	lua_ = new mcLuaManager;
	
	//Lua function wrapper library
	static const luaL_reg scriptLib[] = 
	{
		{"appendMessage", lsAppendMessage },		
		{NULL, NULL}
	};
	lua_->LuaOpenLibrary("scene",scriptLib);

	//Open Lua script
	sc = lua_->CreateScript();
	NSString *filePath = [[NSBundle mainBundle] pathForResource:@"show_messages.lua" ofType:@""]; 
	sc->LoadFile([filePath UTF8String]);

	//Set initial update method counter
	lua_->Update(0);
	
	//Schedule step method
	[self schedule: @selector(step:)];

	//Resume button
	CCMenuItemFont *resumeItem = [CCMenuItemFont itemFromString:@"Resume Script" target:self selector:@selector(resumeScript:)];
	CCMenu *menu = [CCMenu menuWithItems:resumeItem, nil];
	[self addChild:menu];

	return self;
}

-(void) step:(ccTime)delta {
	//Update Lua script runner
	lua_->Update(delta);
}

/* Resume script callback */
-(void) resumeScript:(id)sender {
	sc->YieldResume();
}

-(void) cleanRecipe {
	free(lua_);
	lua_ = nil;
}

@end