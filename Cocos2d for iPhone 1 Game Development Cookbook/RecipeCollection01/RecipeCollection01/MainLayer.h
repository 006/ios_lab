#import "cocos2d.h"

@interface MainLayer : CCLayer {
	NSMutableDictionary *recipes;
	int numRecipes;
	int currentRecipe;
}

+(id) scene;
-(void) loadRecipes;
-(void) addButtons;
-(void) prevCallback:(id)sender;
-(void) nextCallback:(id)sender;
-(void) removeRecipe;
-(void) addRecipe;
-(void) addBackground;

@end
