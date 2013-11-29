#import "Recipe.h"

@interface Ch5_UsingCCLabel : Recipe
{
}

-(CCLayer*) runRecipe;

@end

@implementation Ch5_UsingCCLabel

-(CCLayer*) runRecipe {
	[super runRecipe];

	//CCLabelAtlas for fixed-width bitmap fonts
	CCLabelAtlas *labelAtlas = [CCLabelAtlas labelWithString:@"Atlas Label Test" charMapFile:@"tuffy_bold_italic-charmap.png" itemWidth:48 itemHeight:65 startCharMap:' '];
	[self addChild:labelAtlas z:0];
	labelAtlas.anchorPoint = ccp(0.5f,0.5f);
	labelAtlas.scale = 0.5f;
	labelAtlas.position = ccp(240,220);	
	[labelAtlas setColor:ccc3(0,255,0)];
	[labelAtlas runAction:[CCRepeatForever actionWithAction: [CCSequence actions: [CCScaleTo actionWithDuration:1.0f scale:0.5f], [CCScaleTo actionWithDuration:1.0f scale:0.25f], nil]]];

	//CCLabelBMFont for variable-width bitmap fonts using FNT files
	CCLabelBMFont *labelBMFont = [CCLabelBMFont labelWithString:@"Bitmap Label Test" fntFile:@"eurostile_30.fnt"];
	[self addChild:labelBMFont z:0];
	labelBMFont.position = ccp(240,160);
	for(id c in labelBMFont.children){
		CCSprite *child = (CCSprite*)c;
		[child setColor:ccc3(arc4random()%255,arc4random()%255,arc4random()%255)];
		[child runAction:[CCRepeatForever actionWithAction:
			[CCSequence actions: [CCScaleTo actionWithDuration:arc4random()%2+1 scale:1.75f], [CCScaleTo actionWithDuration:arc4random()%2+1 scale:0.75f], nil]
		]];
	}
			
	//CCLabelTTF for true-type fonts
	CCLabelTTF *labelTTF = [CCLabelTTF labelWithString:@"True-Type Label Test" fontName:@"arial_narrow.otf" fontSize:32];
	[self addChild:labelTTF z:0];
	labelTTF.position = ccp(240,100);
	[labelTTF runAction:[CCRepeatForever actionWithAction: [CCSequence actions: [CCScaleTo actionWithDuration:2.0f scale:1.5f], [CCScaleTo actionWithDuration:2.0f scale:0.5f], nil]]];
	[labelTTF setColor:ccc3(0,0,255)];

	return self;
}

@end
