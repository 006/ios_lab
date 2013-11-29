#import "cocos2d.h"
#import "ShadowLabel.h"

@interface GameMenuWindow : CCNode {
	@public
		bool isOpen;
		CGSize size;
		NSString *title;
		CCSprite *content;
		CCSprite *bg;
		CCSprite *titleBar;
		
		NSUInteger touchHash;
		bool isTouched;
		CGPoint touchedPoint;
}

@property(readwrite, assign) bool isOpen;
@property(readwrite, assign) CGSize size;
@property(readwrite, assign) NSString *title;
@property(nonatomic, retain) CCSprite *content;
@property(nonatomic, retain) CCSprite *bg;
@property(nonatomic, retain) CCSprite *titleBar;
@property(readwrite, assign) NSUInteger touchHash;
@property(readwrite, assign) bool isTouched;

+(id) windowWithTitle:(NSString*)t size:(CGSize)s;
-(void) minimize:(id)sender;
-(CGRect) titleBarRect;
-(CGRect) rect;
-(void) bringToFront;
-(void) ccTouchesBegan:(NSSet *)touches withEvent:(UIEvent *)event;
-(void) ccTouchesMoved:(NSSet *)touches withEvent:(UIEvent *)event;
-(void) ccTouchesEnded:(NSSet *)touches withEvent:(UIEvent *)event;

@end