#import "Recipe.h"
#import "GameMenuWindow.h"

@interface Ch5_MenuWindows : Recipe
{
	NSMutableArray *windows;
}

-(CCLayer*) runRecipe;

@end

@implementation Ch5_MenuWindows

-(CCLayer*) runRecipe {
	[super runRecipe];
 
	//Initialization
	windows = [[NSMutableArray alloc] init];
	CCNode *windowContainer = [[CCNode alloc] init];
 
	/* Create three menu windows with randomized positions */
	GameMenuWindow *window1 = [GameMenuWindow windowWithTitle:@"Window 1" size:CGSizeMake(arc4random()%200+120,arc4random()%100+50)];
	window1.position = ccp(arc4random()%100+150,arc4random()%140+100);
	[windowContainer addChild:window1 z:1];
	[windows addObject:window1];
 
	GameMenuWindow *window2 = [GameMenuWindow windowWithTitle:@"Window 2" size:CGSizeMake(arc4random()%200+120,arc4random()%100+50)];
	window2.position = ccp(arc4random()%100+150,arc4random()%140+100);
	[windowContainer addChild:window2 z:2];
	[windows addObject:window2];
	
	GameMenuWindow *window3 = [GameMenuWindow windowWithTitle:@"Window 3" size:CGSizeMake(arc4random()%200+120,arc4random()%100+50)];
	window3.position = ccp(arc4random()%100+150,arc4random()%140+100);
	[windowContainer addChild:window3 z:3];
	[windows addObject:window3];
	
	//Sort our window array by zOrder
	//This allows ordered touching
	NSSortDescriptor *sorter = [[NSSortDescriptor alloc] initWithKey:@"self.zOrder" ascending:NO];
	[windows sortUsingDescriptors:[NSArray arrayWithObject:sorter]];
	
	//Add window container node
	[self addChild:windowContainer];
	
	return self;
}

-(void) ccTouchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {	
	UITouch *touch = [touches anyObject];
	CGPoint point = [touch locationInView: [touch view]];
	point = [[CCDirector sharedDirector] convertToGL: point];
	
	//Sort our window array before we process a touch
	NSSortDescriptor *sorter = [[NSSortDescriptor alloc] initWithKey:@"self.zOrder" ascending:NO];
	[windows sortUsingDescriptors:[NSArray arrayWithObject:sorter]];

	//Grab the window by touching the top bar. Otherwise, merely bring the window to the front
	for(GameMenuWindow* w in windows){
		if(pointIsInRect(point, [w titleBarRect])){
			[w ccTouchesBegan:touches withEvent:event];
			return;
		}else if(pointIsInRect(point, [w rect])){
			[w bringToFront];
			return;
		}
	}
}
-(void) ccTouchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {	
	UITouch *touch = [touches anyObject];
	CGPoint point = [touch locationInView: [touch view]];
	point = [[CCDirector sharedDirector] convertToGL: point];
	
	//If we touched a window them we can drag it
	for(GameMenuWindow* w in windows){
		if(w.isTouched){
			[w ccTouchesMoved:touches withEvent:event];
		}
	}
}
-(void) ccTouchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {	
	UITouch *touch = [touches anyObject];
	CGPoint point = [touch locationInView: [touch view]];
	point = [[CCDirector sharedDirector] convertToGL: point];
	
	//End a touch if neccessary
	for(GameMenuWindow* w in windows){
		if(w.isTouched){
			[w ccTouchesEnded:touches withEvent:event];
		}
	}
}

@end
