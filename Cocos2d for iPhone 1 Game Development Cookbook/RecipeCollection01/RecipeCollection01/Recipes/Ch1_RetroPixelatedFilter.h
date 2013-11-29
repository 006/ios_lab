#import "Ch1_DrawingSprites.h"

enum {
	NO_SCAN_LINES = 0,
	VERT_SCAN_LINES = 1,
	HORIZ_SCAN_LINES = 2,
	FULL_SCAN_LINES = 3
};

//Interface
@interface Ch1_RetroPixelatedFilter : Ch1_DrawingSprites
{
	CCSprite *scanLinesVert;
	CCSprite *scanLinesHoriz;
	int scanLineMode;
}

-(void) switchMode;

@end

@implementation Ch1_RetroPixelatedFilter

-(CCLayer*) runRecipe {
	[super runRecipe];

	scanLineMode = NO_SCAN_LINES;

	//Vertical scan lines
	scanLinesVert = [CCSprite spriteWithFile:@"scanlines_vert.png"];
	scanLinesVert.position = ccp(240,160);
	scanLinesVert.opacity = 120;
	[self addChild:scanLinesVert z:3];
	
	//Horizontal scan lines
	scanLinesHoriz = [CCSprite spriteWithFile:@"scanlines_horiz.png"];
	scanLinesHoriz.position = ccp(240,160);
	scanLinesHoriz.opacity = 120;
	[self addChild:scanLinesHoriz z:3];
	
	[CCMenuItemFont setFontSize:30];
	
	//Add button to switch between modes
	CCMenuItemFont *switchMode = [CCMenuItemFont itemFromString:@"Switch Scanline Mode" target:self selector:@selector(switchMode)];
	CCMenu *switchModeMenu = [CCMenu menuWithItems:switchMode, nil];
    switchModeMenu.position = ccp( 240 , 20 );
    [self addChild:switchModeMenu z:6];

	[self switchMode];

	return self;
}

-(void) switchMode {
	scanLineMode++;
	if(scanLineMode > 3){
		scanLineMode = 0;
	}
	
	if(scanLineMode == NO_SCAN_LINES){
		scanLinesVert.visible = NO;
		scanLinesHoriz.visible = NO;
	}else if(scanLineMode == VERT_SCAN_LINES){
		scanLinesVert.visible = YES;
		scanLinesHoriz.visible = NO;
	}else if(scanLineMode == HORIZ_SCAN_LINES){
		scanLinesVert.visible = NO;
		scanLinesHoriz.visible = YES;
	}else{
		scanLinesVert.visible = YES;
		scanLinesHoriz.visible = YES;		
	}
}

@end