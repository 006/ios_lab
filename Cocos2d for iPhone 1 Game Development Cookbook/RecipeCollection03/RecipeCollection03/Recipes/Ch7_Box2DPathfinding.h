#import "Recipe.h"
#import "GameArea2D.h"
#import "GameObject.h"
#import "GameSensor.h"
#import "GameMisc.h"
#import "GameHelper.h"
#import "Vector3D.h"
#import "GameActor.h"
#import "GameWaypoint.h"
#import "DrawLayer.h"
#import "AStarNode.h"
#import "AStarPathNode.h"
#import "RayCast.h"

@class GameArea2D;
@class GameObject;
@class GameSensor;
@class GameMisc;
@class GameActor;
@class GameWaypoint;
@class DrawLayer;

//Interface
@interface Ch7_Box2DPathfinding : GameArea2D
{
	GameActor *actor;
	float actorRadius;
	DrawLayer *drawLayer;
	
	NSMutableArray *grid;
	float nodeSpace;	//The space between each node, increase this to increase A* efficiency at the cost of accuracy
	int gridSizeX;
	int gridSizeY;
}

@property (nonatomic, retain) GameActor *actor;

-(CCLayer*) runRecipe;
-(void) addNeighbor:(AStarNode*)node toGridNodeX:(int)x Y:(int)y;
-(void) step:(ccTime)dt;
-(void) addActor;
-(void) drawLayer;
-(void) addDrawLayer;
-(void) handleCollisionWithObjA:(GameObject*)objA withObjB:(GameObject*)objB;
-(void) handleCollisionWithSensor:(GameSensor*)sensor withMisc:(GameMisc*)misc;
-(void) handleCollisionWithMisc:(GameMisc*)a withMisc:(GameMisc*)b;
-(void) tapWithPoint:(CGPoint)point;

@end

//Implementation
@implementation Ch7_Box2DPathfinding

@synthesize actor;

-(CCLayer*) runRecipe {
	//Set game area size
	gameAreaSize = ccp(1000/PTM_RATIO,600/PTM_RATIO);	//Box2d units

	//Superclass initialization and message
	[super runRecipe];
	[message setString:@"Tap the screen move the actor to that point.\nTap again to queue another waypoint."];

	//Add some geometry
	[self addRandomPolygons:5];
	[self addRandomBoxes:5];

	//Initial variables
	nodeSpace = 32.0f;
	cameraZoom = 0.45f;
	actorRadius = nodeSpace/PTM_RATIO/3;
	
	//Add draw layer
	[self addDrawLayer];
	
	//Create level boundaries
	[self addLevelBoundaries];
		
	//Create 2D array (grid)
	gridSizeX = (int)(gameAreaSize.x*PTM_RATIO/nodeSpace);
	gridSizeY = (int)(gameAreaSize.y*PTM_RATIO/nodeSpace);
	grid = [[NSMutableArray alloc] initWithCapacity:(gridSizeX)];
	for(int x=0; x<gridSizeX; x++){
		[grid addObject:[[NSMutableArray alloc] initWithCapacity:(gridSizeY)]];
	}	

	//Create AStar nodes
	for(int x=0; x<gridSizeX; x++){
		for(int y=0; y<gridSizeY; y++){
			//Add a node
			AStarNode *node = [[AStarNode alloc] init];
			node.position = ccp(x*nodeSpace + nodeSpace/2, y*nodeSpace + nodeSpace/2);
			[[grid objectAtIndex:x] addObject:node];
		}
	}
		
	//Add neighbors
	for(int x=0; x<gridSizeX; x++){
		for(int y=0; y<gridSizeY; y++){
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
	
	//Remove neighbors from positive TestPoint and RayCast tests	
	for(int x=0; x<gridSizeX; x++){
		for(int y=0; y<gridSizeY; y++){
			//Add a node
			AStarNode *node = [[grid objectAtIndex:x] objectAtIndex:y];
			
			//If a node itself is colliding with an object we cut off all connections
			for (b2Body* b = world->GetBodyList(); b; b = b->GetNext()){
				if (b->GetUserData() != NULL) {
					GameObject *obj = (GameObject*)b->GetUserData();
					if(obj->polygonShape){
						b2Vec2 nodePosition = b2Vec2(node.position.x/PTM_RATIO, node.position.y/PTM_RATIO);
						
						//Test this node point against this polygon
						if(obj->polygonShape->TestPoint(b->GetTransform(), nodePosition)){
							for(int i=0; i<node.neighbors.count; i++){
								//Remove connections
								AStarNode *neighbor = [node.neighbors objectAtIndex:i];
								[node.neighbors removeObject:neighbor];
								[neighbor.neighbors removeObject:node];
							}
						}
					}
				}	
			}	
			
			//Test all node to neighbor connections using a RayCast test
			for(int i=0; i<node.neighbors.count; i++){
				AStarNode *neighbor = [node.neighbors objectAtIndex:i];
				
				//Do a RayCast from the node to the neighbor.
				//If there is something in the way, remove the link
				b2Vec2 nodeP = b2Vec2(node.position.x/PTM_RATIO, node.position.y/PTM_RATIO);
				b2Vec2 neighborP = b2Vec2(neighbor.position.x/PTM_RATIO, neighbor.position.y/PTM_RATIO);
				
				//Do 4 tests (based on actor size)
				for(float x = -actorRadius; x <= actorRadius; x+= actorRadius*2){
					for(float y = -actorRadius; y <= actorRadius; y+= actorRadius*2){
						RayCastAnyCallback callback;
						world->RayCast(&callback, b2Vec2(nodeP.x+x,nodeP.y+y), b2Vec2(neighborP.x+x,neighborP.y+y));
		
						if(callback.m_hit){
							//Remove connections
							[node.neighbors removeObject:neighbor];
							[neighbor.neighbors removeObject:node];
							break; break;
						}
					}
				}
			}
		}
	}
	
	//Create Actor
	[self addActor];

	//Schedule step method
	[self schedule:@selector(step:)];

	return self;
}

/* Add neighbor helper method */
-(void) addNeighbor:(AStarNode*)node toGridNodeX:(int)x Y:(int)y {
	if(x >= 0 && y >= 0 && x < gridSizeX && y < gridSizeY){
		AStarNode *neighbor = [[grid objectAtIndex:x] objectAtIndex:y];
		if(![AStarNode isNode:neighbor inList:node.neighbors]){
			[node.neighbors addObject:neighbor];
		}
	}
}

-(void) step: (ccTime) dt {	
	[super step:dt];
	
	//Process actor waypoints
	[actor processWaypoints];
	
	//Turn actor to face the proper direction
	if(actor.waypoints.count > 0){
		CGPoint movementVector = ccp(actor.body->GetLinearVelocity().x, actor.body->GetLinearVelocity().y);
		actor.body->SetTransform(actor.body->GetPosition(), -1 * [GameHelper vectorToRadians:movementVector] + PI_CONSTANT/2);
	}
}

/* Add actor */
-(void) addActor {
	actor = [[GameActor alloc] init];
	actor.gameArea = self;
	
	actor.bodyDef->type = b2_dynamicBody;
	actor.bodyDef->position.Set(240/PTM_RATIO,160/PTM_RATIO);
	actor.bodyDef->userData = actor;
				
	actor.body = world->CreateBody(actor.bodyDef);
	
	actor.circleShape = new b2CircleShape();
	actor.circleShape->m_radius = actorRadius;
	actor.fixtureDef->shape = actor.circleShape;
	actor.fixtureDef->isSensor = NO;
	
	actor.body->CreateFixture(actor.fixtureDef);
}

/* Draw layer */
-(void) drawLayer {
	//Draw waypoints
	glColor4ub(255,255,0,32);
	CGPoint actorPosition = ccp(actor.body->GetPosition().x*PTM_RATIO, actor.body->GetPosition().y*PTM_RATIO);
	if(actor.waypoints.count == 1){
		GameWaypoint *gw = (GameWaypoint*)[actor.waypoints objectAtIndex:0];
		ccDrawLine(actorPosition, gw.position);
	}else if(actor.waypoints.count > 1){
		for(int i=0; i<actor.waypoints.count-1; i++){			
			GameWaypoint *gw = (GameWaypoint*)[actor.waypoints objectAtIndex:i];
			GameWaypoint *gwNext = (GameWaypoint*)[actor.waypoints objectAtIndex:i+1];
			
			if(i == 0){
				//From actor to first waypoint
				ccDrawLine(actorPosition, gw.position);
				ccDrawLine(gw.position, gwNext.position);
			}else{
				//From this waypoint to next one
				ccDrawLine(gw.position, gwNext.position);
			}	
		}
	}
	glColor4ub(255,255,255,255);
	
	//Only draw the AStarNodes and connections in the simulator (too slow for the device)
	NSString *model = [[UIDevice currentDevice] model];

	if([model isEqualToString:@"iPhone Simulator"] || [model isEqualToString:@"iPad Simulator"]){
		//Draw AStarNodes
		glColor4ub(16,16,16,8);
		for(int x=0; x<gridSizeX; x++){
			for(int y=0; y<gridSizeY; y++){
				//Draw node
				AStarNode *node = [[grid objectAtIndex:x] objectAtIndex:y];
				ccDrawPoint(node.position);
				
				//Draw neighbor lines (there is going to be a lot of them)
				for(int i=0; i<node.neighbors.count; i++){
					AStarNode *neighbor = [node.neighbors objectAtIndex:i];
					ccDrawLine(node.position, neighbor.position);
				}
			}
		}	
		glColor4ub(255,255,255,255);
	}
}

/* Add draw layer */
-(void) addDrawLayer {
	drawLayer = [[DrawLayer alloc] init];
	drawLayer.position = ccp(0,0);
	drawLayer.recipe = self;
	[gameNode addChild:drawLayer z:10];
}

/* Find a path and add it (as a set of waypoints) when we tap the screen */
-(void) tapWithPoint:(CGPoint)point {
	//Convert touch coordinate to physical coordinate
	CGPoint endPoint = [self convertTouchCoord:point];
	if(endPoint.x < 0 || endPoint.y < 0 || endPoint.x >= gameAreaSize.x*PTM_RATIO || endPoint.y >= gameAreaSize.y*PTM_RATIO){
		return;
	}
	
	//Actor position
	CGPoint actorPosition = ccp(actor.body->GetPosition().x*PTM_RATIO, actor.body->GetPosition().y*PTM_RATIO);
	
	//We use the last waypoint position if applicable
	if(actor.waypoints.count > 0){
		actorPosition = [[actor.waypoints objectAtIndex:actor.waypoints.count-1] position];
	}
	
	//Starting node
	AStarNode *startNode = [[grid objectAtIndex:(int)(actorPosition.x/nodeSpace)] objectAtIndex:(int)(actorPosition.y/nodeSpace)];

	//Make sure the start node is actually properly connected
	if(startNode.neighbors.count == 0){
		bool found = NO; float n = 1;
		while(!found){
			//Search the nodes around this point for a properly connected starting node
			for(float x = -n; x<= n; x+= n){
				for(float y = -n; y<= n; y+= n){
					if(x == 0 && y == 0){ continue; }
					float xIndex = ((int)(actorPosition.x/nodeSpace))+x;
					float yIndex = ((int)(actorPosition.y/nodeSpace))+y;
					if(xIndex >= 0 && yIndex >= 0 && xIndex < gridSizeX && yIndex < gridSizeY){
						AStarNode *node = [[grid objectAtIndex:xIndex] objectAtIndex:yIndex];
						if(node.neighbors.count > 0){
							startNode = node;
							found = YES;
							break; break;
						}
					}					
				}
			}
			n += 1;
		}
	}
	
	//End node
	AStarNode *endNode = [[grid objectAtIndex:(int)(endPoint.x/nodeSpace)] objectAtIndex:(int)(endPoint.y/nodeSpace)];	

	//Run the pathfinding algorithm
	NSMutableArray *foundPath = [AStarPathNode findPathFrom:startNode to:endNode];
	
	if(!foundPath){
		[self showMessage:@"No Path Found"];
	}else{
		[self showMessage:@"Found Path"];
		//Add found path as a waypoint set to the actor
		for(int i=foundPath.count-1; i>=0; i--){
			CGPoint pathPoint = [[foundPath objectAtIndex:i] CGPointValue];
			[actor addWaypoint:[GameWaypoint createWithPosition:pathPoint withSpeedMod:1.0f]];
		}
	}
	
	[foundPath release]; foundPath = nil;
}

@end
