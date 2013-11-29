#import "cocos2d.h"
#import "GameHelper.h"

//Shared enums
enum {
	TAG_EFFECT_NODE = 0
};

//Recipe Interface
@interface Recipe : CCLayer {
	CCLabelBMFont *message;
}

-(CCLayer*) runRecipe;
-(void) cleanRecipe;
-(void) resetMessage;
-(void) showMessage:(NSString*)m;
-(void) appendMessage:(NSString*)m;
-(void) appendMessageWithWrap:(NSString*)m;
-(void) drawLayer;

@end