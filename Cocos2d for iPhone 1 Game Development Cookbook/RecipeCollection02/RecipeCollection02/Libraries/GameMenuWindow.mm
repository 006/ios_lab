#import "GameMenuWindow.h"

static float titleBarHeight = 20.0f;
static float borderSize = 2.0f;

enum {
	TS_NONE,
	TS_TAP,
	TS_HOLD,
	TS_DRAG
};

@implementation GameMenuWindow

@synthesize isOpen, size, title, content, titleBar, touchHash, isTouched, bg;

+(id) windowWithTitle:(NSString*)t size:(CGSize)s {
	GameMenuWindow *win = [[GameMenuWindow alloc] init];
	win.title = t;
	win.size = s;
	
	win.bg = [CCSprite spriteWithFile:@"blank.png"];
	[win.bg setTextureRect:CGRectMake(0,0,win.size.width,win.size.height)];
	[win.bg setColor:ccc3(0,0,0)];
	win.bg.position = ccp(0,0);
	win.bg.opacity = 100;
	[win addChild:win.bg];
	
	CGSize contentBgSize = CGSizeMake(win.size.width-borderSize*2, win.size.height - (borderSize*3 + titleBarHeight));
	
	win.content = [CCSprite spriteWithFile:@"blank.png"];
	[win.content setTextureRect:CGRectMake(0,0,contentBgSize.width,contentBgSize.height)];
	[win.content setColor:ccc3(255,255,255)];
	win.content.position = ccp(0,-titleBarHeight/2-borderSize*0.5f);
	[win addChild:win.content];
	
	win.titleBar = [CCSprite spriteWithFile:@"blank.png"];
	[win.titleBar setTextureRect:CGRectMake(0,0,win.size.width-borderSize*2,titleBarHeight)];
	win.titleBar.color = ccc3(200,200,255);
	win.titleBar.opacity = 255;
	win.titleBar.position = ccp(0,win.size.height/2-borderSize-titleBarHeight/2);
	[win addChild:win.titleBar];
	
	//Title Item
	[CCMenuItemFont setFontSize:20];
	[CCMenuItemFont setFontName:@"Marker Felt"];
	
	CCMenuItemFont *titleItem = [CCMenuItemFont itemFromString:t];
	titleItem.position = ccp(-238,-162);
	titleItem.anchorPoint = ccp(0,0);
	titleItem.disabledColor = ccc3(0,0,0);
	[titleItem setIsEnabled:NO];
	
	CCMenu *menu1 = [CCMenu menuWithItems: titleItem, nil];
	[win.titleBar addChild:menu1];
	
	//Minimize Button
	[CCMenuItemFont setFontSize:40];
	[CCMenuItemFont setFontName:@"Arial"];
	CCMenuItemFont *minMinusFont = [CCMenuItemFont itemFromString:@"-"];
	minMinusFont.color = ccc3(0,0,0);
	
	[CCMenuItemFont setFontSize:30];
	CCMenuItemFont *minPlusFont = [CCMenuItemFont itemFromString:@"+"];
	minPlusFont.color = ccc3(0,0,0);
	
	CCMenuItemToggle *itemToggle = [CCMenuItemToggle itemWithTarget:win selector:@selector(minimize:) items: minMinusFont, minPlusFont, nil];
	CCMenu *itemToggleMenu = [CCMenu menuWithItems: itemToggle, nil];
	itemToggleMenu.position = ccp(win.size.width-20.0f,10.0f);
	[win.titleBar addChild:itemToggleMenu];
	
	win.isOpen = YES;
	
	return win;
}

-(void) minimize:(id)sender {
	isOpen = !isOpen;
	
	if(isOpen){
		content.visible = YES;
		bg.visible = YES;
	}else{
		content.visible = NO;
		bg.visible = NO;
	}
	
	[self bringToFront];
}

- (CGRect) titleBarRect {
	float scaleMod = 1.0f;
	float w = self.size.width * scaleMod;
	float h = titleBarHeight * scaleMod;
	float pointX = titleBar.position.x + self.position.x - self.size.width/2;
	float pointY = titleBar.position.y + self.position.y - titleBarHeight/2;
	CGPoint point = CGPointMake(pointX,pointY);

	return CGRectMake(point.x, point.y, w, h); 
}

- (CGRect) rect {
	if(!isOpen){
		return [self titleBarRect];
	}

	float scaleMod = 1.0f;
	float w = self.size.width * scaleMod;
	float h = self.size.height * scaleMod;
	float pointX = self.position.x - self.size.width/2;
	float pointY = self.position.y - self.size.height/2;
	CGPoint point = CGPointMake(pointX,pointY);

	return CGRectMake(point.x, point.y, w, h); 
}

-(void) bringToFront {
	int count = 1;
	for(id c in self.parent.children){
		GameMenuWindow *child = (GameMenuWindow*)c;
		[self.parent reorderChild:child z:count];
		count += 1;
		child.titleBar.color = ccc3(200,200,255);
	}
	[self.parent reorderChild:self z:count+1];
	titleBar.color = ccc3(100,100,255);
}

-(void) ccTouchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {	
	UITouch *touch = [touches anyObject];
    CGPoint point = [touch locationInView: [touch view]];
	point = [[CCDirector sharedDirector] convertToGL: point];
	
	touchHash = [touch hash];
	isTouched = YES;
	touchedPoint = point;
	[self bringToFront];
}
-(void) ccTouchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {	
	if(!isTouched){ return; }
	
	UITouch *touch = [touches anyObject];
    CGPoint point = [touch locationInView: [touch view]];
	point = [[CCDirector sharedDirector] convertToGL: point];
		
	if(touchHash == [touch hash]){		//If we moved on this sprite
		CGPoint touchedDiff = ccp(point.x-touchedPoint.x, point.y-touchedPoint.y);
		self.position = ccp( self.position.x + touchedDiff.x, self.position.y + touchedDiff.y );
		touchedPoint = point;		
	}
}
-(void) ccTouchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {	
	if(!isTouched){ return; }
	
	UITouch *touch = [touches anyObject];
    CGPoint point = [touch locationInView: [touch view]];
	point = [[CCDirector sharedDirector] convertToGL: point];

	if(touchHash == [touch hash]){
		isTouched = NO;
	}
}

@end