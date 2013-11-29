#import "cocos2d.h"
#import "Recipe.h"

//Interface
@interface Ch2_PinchZooming : Recipe
{
	CCSprite *bg;
	float lastMultiTouchZoomDistance;
	float cameraZoom;
	bool arrowsIn;
}

-(CCLayer*) runRecipe;
-(bool) hudPressedWithPoint:(CGPoint)point;
-(void) setCameraZoom:(float)zoom;
-(void) setArrows;
-(void) ccTouchesBegan:(NSSet *)touches withEvent:(UIEvent *)event;
-(void) ccTouchesMoved:(NSSet *)touches withEvent:(UIEvent *)event;
-(void) ccTouchesEnded:(NSSet *)touches withEvent:(UIEvent *)event;

@end

//Implementation
@implementation Ch2_PinchZooming

-(CCLayer*) runRecipe {
	//Enable touching
	self.isTouchEnabled = YES;
	
	//Set initial variables
	arrowsIn = NO;
	cameraZoom = 1.0f;
	lastMultiTouchZoomDistance = 0.0f;
		
	//Init background
	bg = [CCSprite spriteWithFile:@"dracula_castle.jpg"];
	bg.position = ccp(240,160);
	[self addChild:bg];
	
	//Set initial zoom
	[self setCameraZoom:1];
	
	return self;
}
/* Check for HUD input */
-(bool) hudPressedWithPoint:(CGPoint)point {
	//There is no HUD.
	return NO;
}
-(void) setCameraZoom:(float)zoom {
	cameraZoom = zoom;
	bg.scale = cameraZoom;
	[self setArrows];
}
-(void) setArrows {
	if(arrowsIn && cameraZoom > 2){
		arrowsIn = NO;
		[self removeChildByTag:0 cleanup:YES];
		[self removeChildByTag:1 cleanup:YES];
		
		CCSprite *arrowUp = [CCSprite spriteWithFile:@"arrow_up.png"];
		arrowUp.position = ccp(240,260);
		arrowUp.scale = 0.25f;
		arrowUp.rotation = 180;
		[arrowUp runAction: [CCRepeatForever actionWithAction:
			[CCSequence actions:
				[CCSpawn actions:
					[CCMoveBy actionWithDuration:1.0f position:ccp(0,-100)], 
					[CCFadeOut actionWithDuration:1.0f],
					nil
				],
				[CCSpawn actions:
					[CCMoveBy actionWithDuration:0.0f position:ccp(0,100)], 
					[CCFadeIn actionWithDuration:0.0f],
					nil
				],
				nil
			]
		]];
		[self addChild:arrowUp z:1 tag:0];
		
		CCSprite *arrowDown = [CCSprite spriteWithFile:@"arrow_up.png"];
		arrowDown.position = ccp(240,60);
		arrowDown.scale = 0.25f;
		[arrowDown runAction: [CCRepeatForever actionWithAction:
			[CCSequence actions:
				[CCSpawn actions:
					[CCMoveBy actionWithDuration:1.0f position:ccp(0,100)], 
					[CCFadeOut actionWithDuration:1.0f],
					nil
				],
				[CCSpawn actions:
					[CCMoveBy actionWithDuration:0.0f position:ccp(0,-100)], 
					[CCFadeIn actionWithDuration:0.0f],
					nil
				],
				nil
			]
		]];
		[self addChild:arrowDown z:1 tag:1];		
	}else if(!arrowsIn && cameraZoom <= 2){
		arrowsIn = YES;
		[self removeChildByTag:0 cleanup:YES];
		[self removeChildByTag:1 cleanup:YES];
		
		CCSprite *arrowUp = [CCSprite spriteWithFile:@"arrow_up.png"];
		arrowUp.position = ccp(240,210);
		arrowUp.scale = 0.25f;
		[arrowUp runAction: [CCRepeatForever actionWithAction:
			[CCSequence actions:
				[CCSpawn actions:
					[CCMoveBy actionWithDuration:1.0f position:ccp(0,100)], 
					[CCFadeOut actionWithDuration:1.0f],
					nil
				],
				[CCSpawn actions:
					[CCMoveBy actionWithDuration:0.0f position:ccp(0,-100)], 
					[CCFadeIn actionWithDuration:0.0f],
					nil
				],
				nil
			]
		]];
		[self addChild:arrowUp z:1 tag:0];
		
		CCSprite *arrowDown = [CCSprite spriteWithFile:@"arrow_up.png"];
		arrowDown.position = ccp(240,110);
		arrowDown.scale = 0.25f;
		arrowDown.rotation = 180;
		[arrowDown runAction: [CCRepeatForever actionWithAction:
			[CCSequence actions:
				[CCSpawn actions:
					[CCMoveBy actionWithDuration:1.0f position:ccp(0,-100)], 
					[CCFadeOut actionWithDuration:1.0f],
					nil
				],
				[CCSpawn actions:
					[CCMoveBy actionWithDuration:0.0f position:ccp(0,100)], 
					[CCFadeIn actionWithDuration:0.0f],
					nil
				],
				nil
			]
		]];
		[self addChild:arrowDown z:1 tag:1];
	}
}
/* Check touches */
-(void) ccTouchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {	
	UITouch *touch = [touches anyObject];
    CGPoint point = [touch locationInView: [touch view]];
	point = [[CCDirector sharedDirector] convertToGL: point];
	
	//If HUD has not been touched we reset lastMultiTouchZoomDistance
	if(![self hudPressedWithPoint:point]){
		lastMultiTouchZoomDistance = 0.0f;
	}
}
-(void) ccTouchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {	
	CGSize s = [[CCDirector sharedDirector] winSize];
	
	//Check for only 2 touches
	if(touches.count == 2){
		NSArray *twoTouch = [touches allObjects];
		
		//Get both touches
		UITouch *tOne = [twoTouch objectAtIndex:0];
		UITouch *tTwo = [twoTouch objectAtIndex:1];
		CGPoint firstTouch = [tOne locationInView:[tOne view]];
		CGPoint secondTouch = [tTwo locationInView:[tTwo view]];
			
		//If HUD hasn't been touched we use this distance and last distance to calculate zooming
		if(![self hudPressedWithPoint:firstTouch] && ![self hudPressedWithPoint:secondTouch]){
			CGFloat currentDistance = distanceBetweenPoints(firstTouch, secondTouch);
		
			if(lastMultiTouchZoomDistance == 0){
				lastMultiTouchZoomDistance = currentDistance;
			}else{
				float difference = currentDistance - lastMultiTouchZoomDistance;

				float newZoom = (cameraZoom + (difference*cameraZoom/s.height));
				if(newZoom < 1.0f){ newZoom = 1.0f; }
				if(newZoom > 4.0f){ newZoom = 4.0f; }
				[self setCameraZoom:newZoom];
				lastMultiTouchZoomDistance = currentDistance;

			}
			
		}
	}
}

-(void) ccTouchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {	
	UITouch *touch = [touches anyObject];
    CGPoint point = [touch locationInView: [touch view]];
	point = [[CCDirector sharedDirector] convertToGL: point];
		
	//If HUD has not been touched we reset lastMultiTouchZoomDistance
	if(![self hudPressedWithPoint:point]){
		lastMultiTouchZoomDistance = 0.0f;
	}
}

@end