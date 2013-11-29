#import "Recipe.h"

@interface Ch5_UIKitAlerts : Recipe <UIAlertViewDelegate>
{
}

-(CCLayer*) runRecipe;
-(void)showPieAlert;
-(void)alertView:(UIAlertView*)actionSheet clickedButtonAtIndex:(NSInteger)buttonIndex;

@end

@implementation Ch5_UIKitAlerts

-(CCLayer*) runRecipe {
	[super runRecipe];

	message.position = ccp(160,200);
	message.scale = 0.75f;
	
	[self showPieAlert];

	return self;
}

//Shows a UIAlertView
-(void)showPieAlert {
	UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"Do You Like Pie?" message:@"" delegate:self cancelButtonTitle:@"Cancel" otherButtonTitles:@"Yes",@"No",nil];

	[alert show];
	[alert release];
}

//AlertView callback
-(void)alertView:(UIAlertView *)actionSheet clickedButtonAtIndex:(NSInteger)buttonIndex {
	if(buttonIndex == 0) {
		[self showMessage:@"You remain tight lipped on\nthe 'pie' question."];
	}else if(buttonIndex == 1){
		[self showMessage:@"Ah yes, another lover of pie."];
	}else if(buttonIndex == 2){
		[self showMessage:@"You don't like pie?\nWhat's wrong with you?"];
	}
}

@end
