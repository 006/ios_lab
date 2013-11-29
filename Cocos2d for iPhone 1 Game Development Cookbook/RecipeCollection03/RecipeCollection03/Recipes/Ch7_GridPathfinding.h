#import "Recipe.h"
#import "AStarNode.h"
#import "AStarPathNode.h"
#import "DrawLayer.h"

@interface Ch7_GridPathfinding : Recipe
{
	NSMutableArray *grid;
	CGPoint gridSize;
	float nodeSpace;	//The space between each node
	CCNode *gridNode;	//Where we draw everything
	NSMutableDictionary *sprites;	//A way to keep track of our sprites
	CGPoint touchedNode;	//Where we touched
	bool touchedNodeIsNew;
	NSMutableArray *foundPath;
	DrawLayer *drawLayer;
	CGPoint startCoord;
	CGPoint endCoord;
	bool addMode;
}

-(CCLayer*) runRecipe;
-(void) step:(ccTime)delta;
-(void) addDrawLayer;
-(void) drawLayer;
-(void) switchMode:(id)sender;
-(void) findPath:(id)sender;
-(void) addNeighbor:(AStarNode*)node toGridNodeX:(int)x Y:(int)y;
-(void) addGridArt;
-(void) ccTouchesBegan:(NSSet *)touches withEvent:(UIEvent *)event;
-(void) ccTouchesMoved:(NSSet *)touches withEvent:(UIEvent *)event;
-(void) flipNodeWithTouchedNode:(CGPoint)point;

@end

//Implementation
@implementation Ch7_GridPathfinding

-(CCLayer*) runRecipe {
	//Superclass initialization and message
	[super runRecipe];
	[self showMessage:@"Tap and hold to draw walls.\nPress 'Find Path' to run the simulation."];

	//Initial variables
	gridSize = ccp(25,15);
	nodeSpace = 16.0f;
	touchedNode = ccp(0,0);
	startCoord = ccp(2,2);
	endCoord = ccp(gridSize.x-3, gridSize.y-3);
	touchedNodeIsNew = NO;
	foundPath = [[NSMutableArray alloc] init];
	addMode = YES;

	//Seperate grid node
	gridNode = [[CCNode alloc] init];
	gridNode.position = ccp(35,15);
	[self addChild:gridNode z:3];

	//Create 2D array (grid)
	grid = [[NSMutableArray alloc] initWithCapacity:((int)gridSize.x)];
	for(int x=0; x<gridSize.x; x++){
		[grid addObject:[[NSMutableArray alloc] initWithCapacity:((int)gridSize.y)]];
	}

	//Create AStar nodes and place them in the grid
	for(int x=0; x<gridSize.x; x++){
		for(int y=0; y<gridSize.y; y++){
			//Add a node
			AStarNode *node = [[AStarNode alloc] init];
			node.position = ccp(x*nodeSpace + nodeSpace/2, y*nodeSpace + nodeSpace/2);
			[[grid objectAtIndex:x] addObject:node];
		}
	}
	
	//Add neighbor nodes
	for(int x=0; x<gridSize.x; x++){
		for(int y=0; y<gridSize.y; y++){
			//Add a node
			AStarNode *node = [[grid objectAtIndex:x] objectAtIndex:y];
				
			//Add self as neighbor to neighboring nodes
			[self addNeighbor:node toGridNodeX:x-1 Y:y-1]; //Top-Left
			[self addNeighbor:node toGridNodeX:x-1 Y:y]; //Left
			[self addNeighbor:node toGridNodeX:x-1 Y:y+1]; //Bottom-Left
			[self addNeighbor:node toGridNodeX:x Y:y-1]; //Top
			
			[self addNeighbor:node toGridNodeX:x Y:y+1]; //Bottom
			[self addNeighbor:node toGridNodeX:x+1 Y:y-1]; //Top-Right
			[self addNeighbor:node toGridNodeX:x+1 Y:y]; //Right
			[self addNeighbor:node toGridNodeX:x+1 Y:y+1]; //Bottom-Right
		}
	}

	//Add visual represenation of nodes
	[self addGridArt];
	
	/* Menu items */
	CCMenuItemFont *findPathItem = [CCMenuItemFont itemFromString:@"Find Path" target:self selector:@selector(findPath:)];
	findPathItem.scale = 0.65f;
	
	CCMenuItemToggle *switchModeItem = [CCMenuItemToggle itemWithTarget:self selector:@selector(switchMode:) items:
			[CCMenuItemFont itemFromString: @"Switch Mode: Remove Wall"], [CCMenuItemFont itemFromString: @"Switch Mode: Add Wall"], nil];
	switchModeItem.scale = 0.65f;
	
	CCMenu *menu = [CCMenu menuWithItems:findPathItem, switchModeItem, nil];
	[menu alignItemsVertically];
	menu.position = ccp(350,290);
	[self addChild:menu z:5];
		
	//Add draw layer
	[self addDrawLayer];
	
	//Schedule step method
	[self schedule:@selector(step:)];

	return self;
}

-(void) step:(ccTime)delta {
	//Flip a touched node
	if(touchedNodeIsNew){
		[self flipNodeWithTouchedNode:touchedNode];
		touchedNodeIsNew = NO;
	}
}

/* Switch between adding nodes and adding walls */
-(void) switchMode:(id)sender {
	addMode = !addMode;
}

/* Add draw layer */
-(void) addDrawLayer {
	drawLayer = [[DrawLayer alloc] init];
	drawLayer.position = ccp(0,0);
	drawLayer.recipe = self;
	[gridNode addChild:drawLayer z:10];
}

-(void) drawLayer {
	//If we have less than two nodes in our path we return
	if(foundPath.count < 2){
		return;
	}

	glColor4ub(0,0,255,255);
	
	//Draw all edges of the path
	for(int i=0; i<foundPath.count-1; i++){
		CGPoint p1 = [[foundPath objectAtIndex:i] CGPointValue];
		CGPoint p2 = [[foundPath objectAtIndex:i+1] CGPointValue];
		ccDrawLine(p1,p2);
	}

	glColor4ub(255,255,255,255);
}

/* Find a path from the startNode to the endNode */
-(void) findPath:(id)sender {
	AStarNode *startNode = [[grid objectAtIndex:(int)startCoord.x] objectAtIndex:(int)startCoord.y];
	AStarNode *endNode = [[grid objectAtIndex:(int)endCoord.x] objectAtIndex:endCoord.y];	

	if(foundPath){
		[foundPath removeAllObjects];
		[foundPath release];
	}
	foundPath = nil;
	
	//Run the pathfinding algorithm
	foundPath = [AStarPathNode findPathFrom:startNode to:endNode];
	
	if(!foundPath){
		[self showMessage:@"No Path Found"];
	}else{
		[self showMessage:@"Found Path"];
	}
}

/* Helper method for adding neighbor nodes */
-(void) addNeighbor:(AStarNode*)node toGridNodeX:(int)x Y:(int)y {
	if(x >= 0 && y >= 0 && x < gridSize.x && y < gridSize.y){
		AStarNode *neighbor = [[grid objectAtIndex:x] objectAtIndex:y];
		[node.neighbors addObject:neighbor];
	}
}

/* Add sprites which correspond to grid nodes */
-(void) addGridArt {
	sprites = [[NSMutableDictionary alloc] init];

	for(int x=0; x<gridSize.x; x++){
		for(int y=0; y<gridSize.y; y++){
			AStarNode *node = [[grid objectAtIndex:x] objectAtIndex:y];

			CCSprite *sprite = [CCSprite spriteWithFile:@"gridNode.png"];
			sprite.position = node.position;
			if(node.active){
				sprite.color = ccc3(200,200,200);
			}else{
				sprite.color = ccc3(100,100,100);
			}
			[gridNode addChild:sprite];
			[sprites setObject:sprite forKey:[NSString stringWithFormat:@"(%i,%i)",x,y]];
		}
	}
	
	//Add start point at (0,0)
	CCSprite *startSprite = [CCSprite spriteWithFile:@"start_button.png"];
	startSprite.position = ccp(startCoord.x*nodeSpace + nodeSpace/2, startCoord.y*nodeSpace + nodeSpace/2);
	[gridNode addChild:startSprite];
	
	//Add end point at (gridSize.x-1,gridSize.y-1)
	CCSprite *endSprite = [CCSprite spriteWithFile:@"end_button.png"];
	endSprite.position = ccp(endCoord.x*nodeSpace + nodeSpace/2,endCoord.y*nodeSpace + nodeSpace/2);
	[gridNode addChild:endSprite];
}

/* Process touching and moving to add/remove walls */
-(void) ccTouchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {
	UITouch *touch = [touches anyObject];
	CGPoint point = [touch locationInView: [touch view]];
	point = [[CCDirector sharedDirector] convertToGL: point];
	
	int x = ((point.x - gridNode.position.x) / nodeSpace);
	int y = ((point.y - gridNode.position.y) / nodeSpace);
	CGPoint tp = CGPointMake(x,y);
	
	touchedNodeIsNew = YES;
	touchedNode = tp;
}

-(void) ccTouchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {
	UITouch *touch = [touches anyObject];
	CGPoint point = [touch locationInView: [touch view]];
	point = [[CCDirector sharedDirector] convertToGL: point];	
	
	int x = ((point.x - gridNode.position.x) / nodeSpace);
	int y = ((point.y - gridNode.position.y) / nodeSpace);
	CGPoint tp = CGPointMake(x,y);
	
	if(tp.x != touchedNode.x || tp.y != touchedNode.y){
		touchedNodeIsNew = YES;
	}
	touchedNode = tp;
}

/* Flip a node when touched */
-(void) flipNodeWithTouchedNode:(CGPoint)point {
	int x = point.x;
	int y = point.y;
	
	if(x == 0 && y == 0){
		return;
	}
	if(x == gridSize.x-1 && y == gridSize.y-1){
		return;
	}
	
	if(x < 0 || y < 0 || x > gridSize.x-1 || y > gridSize.y-1){
		return;
	}
	
	AStarNode *node = [[grid objectAtIndex:x] objectAtIndex:y];
	CCSprite *sprite = [sprites objectForKey:[NSString stringWithFormat:@"(%i,%i)",x,y]];
	
	if(node.active && addMode){
		//Remove node as neighbor and vice versa
		node.active = NO;
		sprite.color = ccc3(100,100,100);
	}else if(!addMode){
		node.active = YES;
		//Change sprite color
		sprite.color = ccc3(200,200,200);
	}
}

-(void) cleanRecipe {
	[sprites release];
	
	[foundPath release]; foundPath = nil;
	
	[super cleanRecipe];
}
@end
