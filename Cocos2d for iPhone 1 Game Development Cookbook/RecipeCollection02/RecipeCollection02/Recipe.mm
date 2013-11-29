#import "Recipe.h"

//Recipe Implementation

@implementation Recipe

-(CCLayer*) runRecipe {
	//Enable touches
	self.isTouchEnabled = YES;

	//Message
	message = [CCLabelBMFont labelWithString:@"" fntFile:@"eurostile_30.fnt"];
	message.position = ccp(160,270);
	message.scale = 0.5f;
	[message setColor:ccc3(255,255,255)];
	[self addChild:message z:10];

	//Remove message after 5 seconds
	[self runAction:[CCSequence actions:[CCDelayTime actionWithDuration:5.0f], 
		[CCCallFunc actionWithTarget:self selector:@selector(resetMessage)], nil]];

	return nil;
}
-(void) cleanRecipe {
	[self removeAllChildrenWithCleanup:YES];
}
/* Reset message callback */
-(void) resetMessage {
	[message setString:@""];
}
-(void) showMessage:(NSString*)m {
	[self stopAllActions];
	[message setString:m];
	[self runAction:[CCSequence actions:[CCDelayTime actionWithDuration:5.0f], 
		[CCCallFunc actionWithTarget:self selector:@selector(resetMessage)], nil]];
}

@end
