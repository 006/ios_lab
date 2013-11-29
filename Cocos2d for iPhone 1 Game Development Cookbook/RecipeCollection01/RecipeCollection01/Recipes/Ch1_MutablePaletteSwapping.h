#import "cocos2d.h"
#import "Recipe.h"
#import "CCTexture2DMutable.h"

//Interface
@interface Ch1_MutablePaletteSwapping : Recipe
{
}

-(CCLayer*) runRecipe;
-(void) swapColor:(ccColor4B)color1 withColor:(ccColor4B)color2 inTexture:(CCTexture2DMutable*)texture withCopy:(CCTexture2DMutable*)copy;
-(bool) isColor:(ccColor4B)color1 equalTo:(ccColor4B)color2 withTolerance:(int)tolerance;

@end

//Implementation
@implementation Ch1_MutablePaletteSwapping

-(CCLayer*) runRecipe {
	//Create a nice looking background
	CCSprite *bg = [CCSprite spriteWithFile:@"baseball_bg_01.png"];
	[bg setPosition:ccp(240,160)];
	bg.opacity = 100;
	[self addChild:bg z:0 tag:0];

	/*** Animate 4 different fielders with different color combinations ***/

	//Set color arrays
	ccColor4B colors1[] = { 
		ccc4(255,217,161,255),
		ccc4(225,225,225,255),
		ccc4(0,0,150,255),
	};
		
	ccColor4B colors2[] = { 
		ccc4(140,100,46,255),
		ccc4(150,150,150,255),
		ccc4(255,0,0,255),
	};
		
	ccColor4B colors3[] = { 
		ccc4(255,217,161,255),
		ccc4(115,170,115,255),
		ccc4(115,170,115,255),
	};
					
	ccColor4B colors4[] = { 
		ccc4(140,100,46,255),
		ccc4(50,50,50,255),
		ccc4(255,255,0,255),
	};

	//Create texture copy to use as an immutable guide.
	CCTexture2DMutable* textureCopy = [[[CCTexture2DMutable alloc] initWithImage:[UIImage imageNamed:@"fielder_run_sentinel_colors.png"]] autorelease];

	//Create our sprites using mutable textures.
	CCSprite *sprite1 = [CCSprite spriteWithTexture:[[[CCTexture2DMutable alloc] initWithImage:[UIImage imageNamed:@"fielder_run_sentinel_colors.png"]] autorelease]];
	CCSprite *sprite2 = [CCSprite spriteWithTexture:[[[CCTexture2DMutable alloc] initWithImage:[UIImage imageNamed:@"fielder_run_sentinel_colors.png"]] autorelease]];
	CCSprite *sprite3 = [CCSprite spriteWithTexture:[[[CCTexture2DMutable alloc] initWithImage:[UIImage imageNamed:@"fielder_run_sentinel_colors.png"]] autorelease]];
	CCSprite *sprite4 = [CCSprite spriteWithTexture:[[[CCTexture2DMutable alloc] initWithImage:[UIImage imageNamed:@"fielder_run_sentinel_colors.png"]] autorelease]];

	//Set sprite positions
	[sprite1 setPosition:ccp(125,75)];
	[sprite2 setPosition:ccp(125,225)];
	[sprite3 setPosition:ccp(325,75)];
	[sprite4 setPosition:ccp(325,225)];

	//Swap colors in each sprite mutable texture and apply the changes.
	[self swapColor:ccc4(0,0,255,255) withColor:colors1[0] inTexture:sprite1.texture withCopy:textureCopy];
	[self swapColor:ccc4(0,255,0,255) withColor:colors1[1] inTexture:sprite1.texture withCopy:textureCopy];
	[self swapColor:ccc4(255,0,0,255) withColor:colors1[2] inTexture:sprite1.texture withCopy:textureCopy];
	[sprite1.texture apply];

	[self swapColor:ccc4(0,0,255,255) withColor:colors2[0]  inTexture:sprite2.texture withCopy:textureCopy];
	[self swapColor:ccc4(0,255,0,255) withColor:colors2[1]  inTexture:sprite2.texture withCopy:textureCopy];
	[self swapColor:ccc4(255,0,0,255) withColor:colors2[2]  inTexture:sprite2.texture withCopy:textureCopy];
	[sprite2.texture apply];
	
	[self swapColor:ccc4(0,0,255,255) withColor:colors3[0]  inTexture:sprite3.texture withCopy:textureCopy];
	[self swapColor:ccc4(0,255,0,255) withColor:colors3[1]  inTexture:sprite3.texture withCopy:textureCopy];
	[self swapColor:ccc4(255,0,0,255) withColor:colors3[2]  inTexture:sprite3.texture withCopy:textureCopy];
	[sprite3.texture apply];
	
	[self swapColor:ccc4(0,0,255,255) withColor:colors4[0]  inTexture:sprite4.texture withCopy:textureCopy];
	[self swapColor:ccc4(0,255,0,255) withColor:colors4[1]  inTexture:sprite4.texture withCopy:textureCopy];
	[self swapColor:ccc4(255,0,0,255) withColor:colors4[2]  inTexture:sprite4.texture withCopy:textureCopy];
	[sprite4.texture apply];

	//Finally, add the sprites to the scene.
	[self addChild:sprite1 z:0 tag:0];
	[self addChild:sprite2 z:0 tag:1];
	[self addChild:sprite3 z:0 tag:2];
	[self addChild:sprite4 z:0 tag:3];
	
	return self;
}

-(void) swapColor:(ccColor4B)color1 withColor:(ccColor4B)color2 inTexture:(CCTexture2DMutable*)texture withCopy:(CCTexture2DMutable*)copy {
	//Look through the texture, find all pixels of the specified color and change them.
	//We use a tolerance of 200 here.
	for(int x=0; x<texture.pixelsWide; x++){
		for(int y=0; y<texture.pixelsHigh; y++){
			if( [self isColor:[copy pixelAt:ccp(x,y)] equalTo:color1 withTolerance:200] ){
				[texture setPixelAt:ccp(x,y) rgba:color2];
			}
		}
	}		
}

-(bool) isColor:(ccColor4B)color1 equalTo:(ccColor4B)color2 withTolerance:(int)tolerance {
	//If the colors are equal within a tolerance we change them.
	bool equal = YES;
	if( abs(color1.r - color2.r) + abs(color1.g - color2.g) + 
		abs(color1.b - color2.b) + abs(color1.a - color2.a) > tolerance ){
			equal = NO;
	}
	return equal;
}

@end
