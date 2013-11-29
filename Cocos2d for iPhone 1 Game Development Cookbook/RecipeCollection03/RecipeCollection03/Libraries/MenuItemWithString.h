#import "Cocos2d.h"

@interface MenuItemWithString : CCMenuItemFont {
    NSString *str;
}

@property (readwrite, assign) NSString *str;

@end