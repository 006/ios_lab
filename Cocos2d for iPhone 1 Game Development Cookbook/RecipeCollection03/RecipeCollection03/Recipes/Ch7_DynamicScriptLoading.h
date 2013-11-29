#import "Recipe.h"
#import "mcLua.hpp"
#import "Reachability.h"

//Interface
@interface Ch7_DynamicScriptLoading : Recipe
{
	class mcLuaManager * lua_;
}

-(CCLayer*) runRecipe;
-(void) step:(ccTime)delta;
-(void) loadScript;

@end

//Callback pointer
Ch7_DynamicScriptLoading *dslRecipe = nil;

//Static append message C function
static int dslAppendMessage(lua_State * l)
{
	//Pass lua string into append message method
	[dslRecipe appendMessage:[NSString stringWithUTF8String:lua_tostring(l,1)]];
	return 0;
}

//Implementation
@implementation Ch7_DynamicScriptLoading

-(CCLayer*) runRecipe {
	//Superclass initialization
	[super runRecipe];

	//Set callback pointer
	dslRecipe = self;

	//Lua initialization
	lua_ = new mcLuaManager;
	
	//Lua function wrapper library
	static const luaL_reg scriptLib[] = 
	{
		{"appendMessage", dslAppendMessage },		
		{NULL, NULL}
	};
	lua_->LuaOpenLibrary("scene",scriptLib);
	
	//Load Lua script
	[self loadScript];

	//Set initial update method counter
	lua_->Update(0);
	
	//Schedule step method
	[self schedule: @selector(step:)];

	//Reload script button
	CCMenuItemFont *reloadItem = [CCMenuItemFont itemFromString:@"Reload Script" target:self selector:@selector(loadScript)];
	CCMenu *menu = [CCMenu menuWithItems:reloadItem, nil];
	[self addChild:menu];

	return self;
}

-(void) step:(ccTime)delta {
	//Update Lua script runner
	lua_->Update(delta);
}

-(void) loadScript{
	//Reset message
	[self resetMessage];
	
	//Make sure cocos2dcookbook.com is reachable
	Reachability* reachability = [Reachability reachabilityWithHostName:@"cocos2dcookbook.com"];
	NetworkStatus remoteHostStatus = [reachability currentReachabilityStatus];

	if(remoteHostStatus == NotReachable) { 
		[self showMessage:@"Script not reachable."];
	}else{
		[self appendMessage:@"Loading script from http://cocos2dcookbook.com/public/\nch7_remote_script.lua"];
		
		//Load script via NSURL
		mcLuaScript *sc = lua_->CreateScript(); 
		NSString *remoteScriptString = [NSString stringWithContentsOfURL:[NSURL URLWithString:@"http://cocos2dcookbook.com/public/ch7_remote_script.lua"] 
			encoding:NSUTF8StringEncoding error:nil];
		sc->LoadString([remoteScriptString UTF8String]);	
	}
}

-(void) cleanRecipe {
	free(lua_);
	lua_ = nil;
}

@end