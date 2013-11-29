#import "Recipe.h"

@interface Ch8_Cocos2d_X : Recipe
{}

-(CCLayer*) runRecipe;

@end



//Implementation
@implementation Ch8_Cocos2d_X

-(CCLayer*) runRecipe {
	//Superclass initialization and message
	[super runRecipe];
	[self showMessage:@"Please refer to the project 'Ch8_Cocos2d-X'."];

	return self;
}

@end
