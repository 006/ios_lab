#import "Recipe.h"

@interface LoadingRecipe : Recipe
{
}

-(CCLayer*) runRecipe;

@end

@implementation LoadingRecipe

-(CCLayer*) runRecipe {
	[super runRecipe];

	[self showMessage:@"Loading..."];

	return self;
}

@end

