#import <UIKit/UIKit.h>
#import <stdlib.h>

#import "cocos2d.h"

@interface ShadowLabel : CCMenuItemLabel {
	@public
		CCMenuItemLabel *shadow;
		ccColor3B shadowColor;
		ccColor3B activeColor;
}

@property(nonatomic, retain) CCMenuItemLabel *shadow;
@property(readwrite, assign) ccColor3B shadowColor;
@property(readwrite, assign) ccColor3B activeColor;

+(id) labelFromString:(NSString*)value target:(id)r selector:(SEL)s;
+(id) labelFromString:(NSString*)value;
+(void) init:(ShadowLabel*)node withValue:(NSString*)value;
-(void) setPosition:(CGPoint)newPosition;

@end