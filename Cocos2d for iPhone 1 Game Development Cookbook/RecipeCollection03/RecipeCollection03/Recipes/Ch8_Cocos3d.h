#import "Recipe.h"

@interface Ch8_Cocos3d : Recipe
{}

-(CCLayer*) runRecipe;

@end



//Implementation
@implementation Ch8_Cocos3d

-(CCLayer*) runRecipe {
	//Superclass initialization and message
	[super runRecipe];
	[self showMessage:@"Please refer to the project 'Ch8_Cocos3d'."];

	return self;
}

@end
