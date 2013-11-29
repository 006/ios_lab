#import "cocos2d.h"
#import "Recipe.h"

//Interface

@interface Ch1_PixelFormat : Recipe
{
}

-(CCLayer*) runRecipe;

@end



//Implementation

@implementation Ch1_PixelFormat

-(CCLayer*) runRecipe {
	// Set the pixel format before loading the image
	// RGBA 8888 image (32-bit)
	[CCTexture2D setDefaultAlphaPixelFormat:kTexture2DPixelFormat_RGBA8888];
	CCSprite *sprite1 = [CCSprite spriteWithFile:@"gradient1.png"];
	[sprite1 setPosition:ccp(120,80)];
	[self addChild:sprite1];	
	
	CCLabelTTF *type1 = [CCLabelTTF labelWithString:@"RGBA 8888 image (32-bit)" fontName:@"Marker Felt" fontSize:16];
	type1.position = ccp(120,80);
	[self addChild:type1];
	
	
	
	// Set the pixel format before loading the image
	// RGBA 4444 image (16-bit)
	[CCTexture2D setDefaultAlphaPixelFormat:kTexture2DPixelFormat_RGBA4444];
	CCSprite *sprite2 = [CCSprite spriteWithFile:@"gradient2.png"];
	[sprite2 setPosition:ccp(120,240)];
	[self addChild:sprite2];	
	
	CCLabelTTF *type2 = [CCLabelTTF labelWithString:@"RGBA 4444 image (16-bit)" fontName:@"Marker Felt" fontSize:16];
	type2.position = ccp(120,240);
	[self addChild:type2];
	
	
	
	// Set the pixel format before loading the image
	// RGB5A1 image (16-bit)
	[CCTexture2D setDefaultAlphaPixelFormat:kTexture2DPixelFormat_RGB5A1];
	CCSprite *sprite3 = [CCSprite spriteWithFile:@"gradient3.png"];
	[sprite3 setPosition:ccp(360,80)];
	[self addChild:sprite3];
	
	CCLabelTTF *type3 = [CCLabelTTF labelWithString:@"RGB5A1 image (16-bit)" fontName:@"Marker Felt" fontSize:16];
	type3.position = ccp(360,80);
	[self addChild:type3];
	
	
	
	// Set the pixel format before loading the image
	// RGB565 image (16-bit)
	[CCTexture2D setDefaultAlphaPixelFormat:kTexture2DPixelFormat_RGB565];
	CCSprite *sprite4 = [CCSprite spriteWithFile:@"gradient4.png"];
	[sprite4 setPosition:ccp(360,240)];
	[self addChild:sprite4];	
	
	CCLabelTTF *type4 = [CCLabelTTF labelWithString:@"RGB565 image (16-bit)" fontName:@"Marker Felt" fontSize:16];
	type4.position = ccp(360,240);
	[self addChild:type4];
	
	
	
	// restore the default pixel format
	[CCTexture2D setDefaultAlphaPixelFormat:kTexture2DPixelFormat_Default];
	
	return self;
}

@end
