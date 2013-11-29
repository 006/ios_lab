#import "Recipe.h"

//Recipe Implementation

@implementation Recipe

-(CCLayer*) runRecipe {
	//Enable touches
	self.isTouchEnabled = YES;

	CGSize screenSize = [CCDirector sharedDirector].winSize;

	//Message
	message = [CCLabelBMFont labelWithString:@"" fntFile:@"eurostile_30.fnt"];
	message.position = ccp(10,screenSize.height-35);
	message.anchorPoint = ccp(0,1);
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
	NSLog(@"%@",m);
	[message setString:m];
	[self runAction:[CCSequence actions:[CCDelayTime actionWithDuration:500.0f], 
		[CCCallFunc actionWithTarget:self selector:@selector(resetMessage)], nil]];
}
-(void) appendMessage:(NSString*)m {
	[self stopAllActions];
	NSLog(@"%@",m);
	[message setString:[NSString stringWithFormat:@"%@\n%@",[message string],m]];
}
-(void) appendMessageWithWrap:(NSString*)m {
	[self stopAllActions];
	
	//Check for bad characters
	for(NSUInteger i=0; i<[m length]; i++) {
		unichar c = [m characterAtIndex:i];
		if(c >= kCCBMFontMaxChars) { 
			m = [m stringByReplacingCharactersInRange:NSMakeRange(i, 1) withString:@"'"];
		}
	}
	
	NSMutableString *str = [m mutableCopy];

	for(int i=0; i<[str length]; i+=40){
		while(i < [str length] && [str characterAtIndex:i] != 32){
			i++;
		}
		if(i >= [str length]){
			break;
		}
		[str insertString:@"\n" atIndex:i];
	}
	
	[message setString:[NSString stringWithFormat:@"%@\n%@",[message string],[str copy]]];
}
-(void) drawLayer {
	//ABSTRACT
}

@end
