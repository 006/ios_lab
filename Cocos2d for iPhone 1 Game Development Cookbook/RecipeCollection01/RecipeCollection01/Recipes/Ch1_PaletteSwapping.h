#import "cocos2d.h"
#import "Recipe.h"

//Interface

@interface Ch1_PaletteSwapping : Recipe
{
}

-(CCLayer*) runRecipe;
-(void) animateFielderWithColors:(ccColor3B[])colors withPosition:(CGPoint)pos;

@end



//Implementation

@implementation Ch1_PaletteSwapping

-(CCLayer*) runRecipe {
	//Create a nice looking background
	CCSprite *bg = [CCSprite spriteWithFile:@"baseball_bg_02.png"];
	[bg setPosition:ccp(240,160)];
	bg.opacity = 100;
	[self addChild:bg z:0 tag:0];

	/*** Animate 4 different fielders with different color combinations ***/

	//Set color arrays
	ccColor3B colors1[] = { 
		ccc3(255,217,161),	//Light skin tone
		ccc3(225,225,225),	//Light gray
		ccc3(0,0,150),		//Dark blue
		ccc3(255,255,255),	//White
	};
	ccColor3B colors2[] = { 
		ccc3(140,100,46),	//Dark skin tone
		ccc3(150,150,150),	//Gray
		ccc3(255,0,0),		//Red
		ccc3(255,255,255),	//White
	};
	ccColor3B colors3[] = { 
		ccc3(255,217,161),	//Light skin tone
		ccc3(115,170,115),	//Dull green
		ccc3(115,170,115),	//Dull green
		ccc3(255,255,255),	//White
	};
	ccColor3B colors4[] = { 
		ccc3(140,100,46),	//Dark skin tone
		ccc3(50,50,50),		//Dark gray
		ccc3(255,255,0),	//Yellow
		ccc3(255,255,255),	//White
	};
	
	//Animate fielders with colors
	[self animateFielderWithColors:colors1 withPosition:ccp(150,70)];
	[self animateFielderWithColors:colors2 withPosition:ccp(150,200)];
	[self animateFielderWithColors:colors3 withPosition:ccp(300,200)];
	[self animateFielderWithColors:colors4 withPosition:ccp(300,70)];

	return self;
}

-(void) animateFielderWithColors:(ccColor3B[])colors withPosition:(CGPoint)pos {
	//The names of our layers
	NSString *layers[] = {
		@"skin",
		@"uniform",
		@"trim",
		@"black_lines",
	};

	//Number of layers
	int numLayers = 4;
	
	for(int i=0; i<numLayers; i+=1){
		NSString *layerName = layers[i];
		ccColor3B color = colors[i];
		
		//We need each plist, the first frame name and finally a name for the animation
		NSString *plistName = [NSString stringWithFormat:@"fielder_run_%@.plist", layerName];
		NSString *firstFrameName = [NSString stringWithFormat:@"fielder_run_%@_01.png", layerName];
		NSString *animationName = [NSString stringWithFormat:@"fielder_run_%@", layerName];

		//Add plist frames to the SpriteFrameCache
		[[CCSpriteFrameCache sharedSpriteFrameCache] addSpriteFramesWithFile:plistName];

		//Get the first sprite frame
		CCSpriteFrame *firstFrame = [[CCSpriteFrameCache sharedSpriteFrameCache] spriteFrameByName:firstFrameName];
		
		//Create our sprite
		CCSprite *sprite = [CCSprite spriteWithSpriteFrame:firstFrame];
	
		//Set color and position
		sprite.position = pos;
		sprite.color = color;
	
		//Create the animation and add frames
		CCAnimation *animation = [[CCAnimation alloc] initWithName:animationName delay:0.15f];
		for(int i=1; i<=8; i+=1){
			CCSpriteFrame *frame = [[CCSpriteFrameCache sharedSpriteFrameCache] spriteFrameByName:[NSString stringWithFormat:@"fielder_run_%@_0%i.png",layerName,i]];
			[animation addFrame:frame];
		}
	
		//Run the repeating animation
		[sprite runAction:[CCRepeatForever actionWithAction: [CCAnimate actionWithAnimation:animation]]];
	
		//Finally, add the sprite
		[self addChild:sprite];
	}
}

@end