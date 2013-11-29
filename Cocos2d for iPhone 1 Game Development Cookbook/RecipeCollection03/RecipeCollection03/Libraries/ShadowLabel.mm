#import "ShadowLabel.h"

@implementation ShadowLabel

@synthesize shadow, shadowColor, activeColor;

+(id) labelFromString:(NSString*)value target:(id)r selector:(SEL)s {
	CCLabelTTF *label = [CCLabelTTF labelWithString:value fontName:[CCMenuItemFont fontName] fontSize:[CCMenuItemFont fontSize]];
	ShadowLabel *node = [ShadowLabel itemWithLabel:label target:r selector:s];
	[ShadowLabel init:node withValue:value];
	return node;
}

+(id) labelFromString:(NSString*)value {
	CCLabelTTF *label = [CCLabelTTF labelWithString:value fontName:[CCMenuItemFont fontName] fontSize:[CCMenuItemFont fontSize]];
	ShadowLabel *node = [ShadowLabel itemWithLabel:label];
	[ShadowLabel init:node withValue:value];
	return node;
}

+(void) init:(ShadowLabel*)node withValue:(NSString*)value {
	node.shadowColor = ccc3(0,0,0);
	node.shadow = [CCMenuItemFont itemFromString:value];
	[node.shadow retain];	
	node.shadow.color = node.shadowColor;
	[node.shadow setDisabledColor:node.shadowColor];
	[node.shadow setIsEnabled:NO];
}

-(void) setPosition: (CGPoint)newPosition {
	[shadow setPosition:ccp(newPosition.x-2,newPosition.y-2)];
	[super setPosition:ccp(newPosition.x, newPosition.y)];
}

-(void) selected {
	[shadow setIsEnabled:YES];
	[shadow selected];
	[shadow setIsEnabled:NO];
	
	colorBackup = self.color;
	self.color = activeColor;
	
	[super selected];
}

-(void) unselected {
	[shadow setIsEnabled:YES];
	[shadow unselected];
	[shadow setIsEnabled:NO];
	
	self.color = colorBackup;
	
	[super unselected];
}

-(void) activate {
	[shadow setIsEnabled:YES];
	[shadow activate];
	[shadow setIsEnabled:NO];	
	[super activate];
}

-(void) setIsEnabled:(BOOL)enabled {
	isEnabled_ = enabled;
	if(shadow){ shadow.isEnabled = enabled; }
}

-(void) setString:(NSString *)label {
	[super setString:label];
	if(shadow){ [shadow setString:label]; }
}

-(void) setAnchorPoint:(CGPoint)anchorPoint {
	[super setAnchorPoint:anchorPoint];
	if(shadow){ [shadow setAnchorPoint:anchorPoint]; }
}

@end