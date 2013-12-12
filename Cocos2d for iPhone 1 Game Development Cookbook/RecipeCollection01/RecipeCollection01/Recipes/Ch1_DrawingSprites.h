#import "cocos2d.h"
#import "Recipe.h"

enum
{
	TAG_ALICE_SPRITE = 0,
	TAG_CAT_SPRITE = 1,
	TAG_TREE_SPRITE_1 = 2,
	TAG_TREE_SPRITE_2 = 3,
	TAG_TREE_SPRITE_3 = 4,
	TAG_CLOUD_BATCH = 5,
	TAG_GRASS_BATCH_1 = 6,
	TAG_GRASS_BATCH_2 = 7
};

//Interface
@interface Ch1_DrawingSprites : Recipe
{
}

-(CCLayer*) runRecipe;
-(void) drawColoredSpriteAt:(CGPoint)position withRect:(CGRect)rect withColor:(ccColor3B)color withZ:(float)z;
@end



//Implementation
@implementation Ch1_DrawingSprites

-(CCLayer*) runRecipe
{
	/*** Draw a sprite using CCSprite ***/
	CCSprite *tree1 = [CCSprite spriteWithFile:@"tree.png"];
	[tree1 setPosition:ccp(20,20)];
	tree1.anchorPoint = ccp(0.5f,0);
	[tree1 setScale:1.5f];
	[self addChild:tree1 z:2 tag:TAG_TREE_SPRITE_1];
	
	/*** Draw a sprite CGImageRef ***/
	UIImage *uiImage = [UIImage imageNamed: @"cheshire_cat.png"];
	CGImageRef imageRef = [uiImage CGImage];
	CCSprite *cat = [CCSprite spriteWithCGImage:imageRef key:@"cheshire_cat.png"];
	[cat setPosition:ccp(250,180)];
	[cat setScale:0.4f];
	[self addChild:cat z:3 tag:TAG_CAT_SPRITE];

	/*** Draw a sprite using CCTexture2D ***/
	CCTexture2D *texture = [[CCTextureCache sharedTextureCache] addImage:@"tree.png"];
	CCSprite *tree2 = [CCSprite spriteWithTexture:texture];
	[tree2 setPosition:ccp(300,20)];
	tree2.anchorPoint = ccp(0.5f,0);
	[tree2 setScale:2.0f];
	[self addChild:tree2 z:2 tag:TAG_TREE_SPRITE_2];

	/*** Draw a sprite using CCSpriteFrameCache and CCTexture2D ***/
	CCSpriteFrame *frame = [CCSpriteFrame frameWithTexture:texture rect:tree2.textureRect];
	[[CCSpriteFrameCache sharedSpriteFrameCache] addSpriteFrame:frame name:@"tree.png"];
	CCSprite *tree3 = [CCSprite spriteWithSpriteFrame:[[CCSpriteFrameCache sharedSpriteFrameCache] spriteFrameByName:@"tree.png"]];
	[tree3 setPosition:ccp(400,20)];
	tree3.anchorPoint = ccp(0.5f,0);
	[tree3 setScale:1.25f];
	[self addChild:tree3 z:2 tag:TAG_TREE_SPRITE_3];
	
	/*** Load a set of spriteframes from a PLIST file and draw one by name ***/
	
	//Get the sprite frame cache singleton
	CCSpriteFrameCache *cache = [CCSpriteFrameCache sharedSpriteFrameCache];

	//Load our scene sprites from a spritesheet
	[cache addSpriteFramesWithFile:@"alice_scene_sheet.plist"];

	//Specify the sprite frame and load it into a CCSprite
	CCSprite *alice = [CCSprite spriteWithSpriteFrameName:@"alice.png"];
	
	//Generate Mip Maps for the sprite
	[alice.texture generateMipmap];
	ccTexParams texParams = { GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE };	
	[alice.texture setTexParameters:&texParams];	
	
	//Set other information. 
	[alice setPosition:ccp(120,20)];
	[alice setScale:0.4f];
	alice.anchorPoint = ccp(0.5f,0);
	[self addChild:alice z:2 tag:TAG_ALICE_SPRITE];
	
	//Make Alice grow and shrink. @_@
	[alice runAction: [CCRepeatForever actionWithAction:
		[CCSequence actions:[CCScaleTo actionWithDuration:4.0f scale:0.7f], [CCScaleTo actionWithDuration:4.0f scale:0.1f], nil] ] ];	

	/*** Draw sprites using CCBatchSpriteNode ***/
	
	//Clouds
	CCSpriteBatchNode *cloudBatch = [CCSpriteBatchNode batchNodeWithFile:@"cloud_01.png" capacity:10];
	[self addChild:cloudBatch z:1 tag:TAG_CLOUD_BATCH];
	for(int x=0; x<10; x++)
    {
		CCSprite *s = [CCSprite spriteWithBatchNode:cloudBatch rect:CGRectMake(0,0,64,64)];
		[s setOpacity:100];
		[cloudBatch addChild:s];
		[s setPosition:ccp(arc4random()%500-50, arc4random()%150+200)];
	}
	
	//Middleground Grass
	CCSpriteBatchNode *grassBatch1 = [CCSpriteBatchNode batchNodeWithFile:@"grass_01.png" capacity:30];
	[self addChild:grassBatch1 z:1 tag:TAG_GRASS_BATCH_1];
	for(int x=0; x<30; x++)
    {
		CCSprite *s = [CCSprite spriteWithBatchNode:grassBatch1 rect:CGRectMake(0,0,64,64)];
		[s setOpacity:255];
		[grassBatch1 addChild:s];
		[s setPosition:ccp(arc4random()%500-50, arc4random()%20+70)];
	}
	
	//Foreground Grass
	CCSpriteBatchNode *grassBatch2 = [CCSpriteBatchNode batchNodeWithFile:@"grass_01.png" capacity:30];
	[self addChild:grassBatch2 z:3 tag:TAG_GRASS_BATCH_2];
	for(int x=0; x<30; x++)
    {
		CCSprite *s = [CCSprite spriteWithBatchNode:grassBatch2 rect:CGRectMake(0,0,64,64)];
		[s setOpacity:255];
		[grassBatch2 addChild:s];
		[s setPosition:ccp(arc4random()%500-50, arc4random()%40-10)];
	}

	/*** Draw colored rectangles using a 1px x 1px white texture ***/

	//Draw the sky using blank.png
	[self drawColoredSpriteAt:ccp(240,190) withRect:CGRectMake(0,0,480,260) withColor:ccc3(150,200,200) withZ:0];
	
	//Draw the ground using blank.png
	[self drawColoredSpriteAt:ccp(240,30) withRect:CGRectMake(0,0,480,60) withColor:ccc3(80,50,25) withZ:0];

	return self;
}

-(void) drawColoredSpriteAt:(CGPoint)position withRect:(CGRect)rect withColor:(ccColor3B)color withZ:(float)z {
	CCSprite *sprite = [CCSprite spriteWithFile:@"blank.png"];
	[sprite setPosition:position];
	[sprite setTextureRect:rect];
	[sprite setColor:color];
	[self addChild:sprite];
	
	//Set Z Order
	[self reorderChild:sprite z:z];
}


@end
