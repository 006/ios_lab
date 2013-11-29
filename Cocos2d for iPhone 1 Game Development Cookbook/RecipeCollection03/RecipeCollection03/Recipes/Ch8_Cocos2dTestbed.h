#import "Recipe.h"

@interface Ch8_Cocos2dTestbed : Recipe
{

}

-(CCLayer*) runRecipe;

@end

@implementation Ch8_Cocos2dTestbed

-(CCLayer*) runRecipe {
	[super runRecipe];
	[self showMessage:@"Please refer to the XCode Workspace file \ninside the latest release of Cocos2d-iPhone."];	

	return self;
}

@end
