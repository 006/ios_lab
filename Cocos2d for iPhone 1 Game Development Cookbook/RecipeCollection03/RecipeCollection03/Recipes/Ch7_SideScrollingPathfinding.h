#import "Recipe.h"
#import "GameArea2D.h"
#import "GameObject.h"
#import "GameSensor.h"
#import "GameMisc.h"
#import "GameHelper.h"
#import "Vector3D.h"
#import "DrawLayer.h"
#import "SSAStarNode.h"
#import "SSGameActor.h"
#import "SSGameWaypoint.h"
#import "SSAStarPathNode.h"
#import "RayCast.h"

@class GameArea2D;
@class GameObject;
@class GameSensor;
@class GameMisc;
@class SSGameActor;
@class SSGameWaypoint;
@class SSAStarNode;
@class SSAStarPathNode;
@class DrawLayer;

@interface Ch7_SideScrollingPathfinding : GameArea2D
{
	SSGameActor *actor;
	DrawLayer *drawLayer;	
	NSMutableArray *nodes;
}

@property (nonatomic, retain) SSGameActor *actor;

-(CCLayer*) runRecipe;
-(void) linkRunNeighbor:(SSAStarNode*)n1 with:(SSAStarNode*)n2;
-(void) step:(ccTime)dt;
-(void) addActor;
-(void) drawLayer;
-(void) addDrawLayer;
-(void) handleCollisionWithObjA:(GameObject*)objA withObjB:(GameObject*)objB;
-(void) handleCollisionWithSensor:(GameSensor*)sensor withMisc:(GameMisc*)misc;
-(void) handleCollisionWithMisc:(GameMisc*)a withMisc:(GameMisc*)b;
-(void) tapWithPoint:(CGPoint)point;
-(SSAStarNode*) nodeClosestToPoint:(CGPoint)p;
-(NSMutableArray*) findPathFrom:(SSAStarNode*)startNode to:(SSAStarNode*)endNode;

@end

//Implementation
@implementation Ch7_SideScrollingPathfinding

@synthesize actor;

-(CCLayer*) runRecipe {
	//Set game area size
	gameAreaSize = ccp(1000/PTM_RATIO,600/PTM_RATIO);	//Box2d units

	//Superclass initialization and message
	[super runRecipe];
	[message setString:@"Tap the screen move the actor to that point.\nTap again to queue another waypoint."];
	
	//Set 2D side scrolling gravity
	world->SetGravity(b2Vec2(0,-20.0f));
	
	//Add three platforms
	[self addBoxAtPoint:ccp(625,200) size:ccp(200,50)];
	[self addBoxAtPoint:ccp(75,300) size:ccp(230,50)];
	[self addBoxAtPoint:ccp(125,100) size:ccp(220,50)];

	//Create Actor
	[self addActor];
	
	//Add draw layer
	[self addDrawLayer];
	
	//Create level boundaries
	[self addLevelBoundaries];
	
	//Initial variables
	cameraZoom = 0.45f;
	
	/* Create SSAStarNodes */
	nodes = [[NSMutableArray alloc] init];
	
	//distance between nodes that the actor can run between
	float nodeRunDistInterval = 100.0f;
	
	//How far to search for nodes the actor can jump to
	float maxJumpSearchDist = 500.0f;
	
	//Add some nodes to the bottom of the level
	for(float x=20.0f; x<=gameAreaSize.x*PTM_RATIO-20.0f; x+=nodeRunDistInterval){
		SSAStarNode *node = [[SSAStarNode alloc] init];
		node.position = ccp(x,20.0f);
		node.body = levelBoundary.body;
		node.actor = actor;
		[nodes addObject:node];
	}
	
	//Link those nodes together as 'run neighbors'
	for(int i=0; i<nodes.count-1; i++){
		SSAStarNode *n1 = (SSAStarNode*)[nodes objectAtIndex:i];
		SSAStarNode *n2 = (SSAStarNode*)[nodes objectAtIndex:i+1];
		
		[self linkRunNeighbor:n1 with:n2];		
	}
	
	/* Add nodes to all level platforms */	
	for(b2Body *b = world->GetBodyList(); b; b = b->GetNext()){
		if (b->GetUserData() != NULL) {
			GameObject *obj = (GameObject*)b->GetUserData();
			
			if(obj.tag == GO_TAG_WALL && obj->polygonShape){
				//Nodes on this body only
				NSMutableArray *nodesThisBody = [[[NSMutableArray alloc] init] autorelease];
				
				//Process each polygon vertex
				for(int i=0; i<obj->polygonShape->m_vertexCount; i++){
					b2Vec2 vertex = obj->polygonShape->m_vertices[i];
					
					//All nodes are 1 unit above their corresponding platform
					b2Vec2 nodePosition = b2Vec2(vertex.x + b->GetPosition().x,vertex.y + b->GetPosition().y+1.0f);
					
					//Move nodes inward to lessen chance of missing a jump
					if(obj->polygonShape->m_centroid.x < vertex.x){
						nodePosition = b2Vec2(nodePosition.x-0.5f, nodePosition.y);
					}else{
						nodePosition = b2Vec2(nodePosition.x+0.5f, nodePosition.y);
					}
			
					//If this node position is not inside the polygon we create an SSAStarNode
					if(!obj->polygonShape->TestPoint(b->GetTransform(), nodePosition)){			
						SSAStarNode *node = [[SSAStarNode alloc] init];
						node.position = ccp(nodePosition.x*PTM_RATIO,(nodePosition.y)*PTM_RATIO);
						node.body = b;
						node.actor = actor;
						[nodes addObject:node];
						[nodesThisBody addObject:node];
					}
				}
				
				//Add in-between nodes (for running)
				bool done = NO;
				while(!done){
					if(nodesThisBody.count == 0){ break; }
				
					done = YES;
					for(int i=0; i<nodesThisBody.count-1; i++){
						SSAStarNode *n1 = (SSAStarNode*)[nodesThisBody objectAtIndex:i];
						SSAStarNode *n2 = (SSAStarNode*)[nodesThisBody objectAtIndex:i+1];
						
						if([GameHelper absoluteValue:n1.position.y-n2.position.y] > 0.1f){
							//These are not side by side
							continue;
						}
						
						if( [GameHelper distanceP1:n1.position toP2:n2.position] > nodeRunDistInterval ){
							CGPoint midPoint = [GameHelper midPointP1:n1.position p2:n2.position];
							b2Vec2 mp = b2Vec2(midPoint.x/PTM_RATIO, midPoint.y/PTM_RATIO);
							
							//If node is not in the polygon, add it
							if(!obj->polygonShape->TestPoint(b->GetTransform(), mp)){
								SSAStarNode *node = [[SSAStarNode alloc] init];
								node.position = [GameHelper midPointP1:n1.position p2:n2.position];
								node.body = b;
								node.actor = actor;
								[nodesThisBody insertObject:node atIndex:i+1];
								[nodes insertObject:node atIndex:[nodes indexOfObject:n2]];
								done = NO;
								break;
							}
						}
					}					
				}
				
				//Link all of the neighboring nodes on this body
				for(int i=0; i<nodesThisBody.count-1; i++){
					if(nodesThisBody.count == 0){ break; }
				
					SSAStarNode *n1 = (SSAStarNode*)[nodesThisBody objectAtIndex:i];
					SSAStarNode *n2 = (SSAStarNode*)[nodesThisBody objectAtIndex:i+1];
				
					if([GameHelper absoluteValue:n1.position.y-n2.position.y] > 0.1f){
						//These are not side by side
						continue;
					}
					
					//Two-way link
					[self linkRunNeighbor:n1 with:n2];
				}
			}
		}
	}
	
	//Neighbor all other nodes (for jumping)
	for(int i=0; i<nodes.count; i++){
		for(int j=0; j<nodes.count; j++){
			if(i==j){ continue; }
		
			SSAStarNode *n1 = (SSAStarNode*)[nodes objectAtIndex:i];
			SSAStarNode *n2 = (SSAStarNode*)[nodes objectAtIndex:j];
		
			if(n1.body == n2.body){ continue; }
		
			if( [GameHelper distanceP1:n1.position toP2:n2.position] <= maxJumpSearchDist ){
				CGPoint src = ccp(n1.position.x/PTM_RATIO, n1.position.y/PTM_RATIO);
				CGPoint dst = ccp(n2.position.x/PTM_RATIO, n2.position.y/PTM_RATIO);
				
				//Calculate our jump "launch" vector
				Vector3D *launchVector3D = [SSGameActor canJumpFrom:src to:dst radius:actor.circleShape->m_radius*1.5f world:world maxSpeed:actor.maxSpeed];
				
				if(launchVector3D){
					//Only neighbor up if a jump can be made
					//1-way link
					if(![n1 containsNeighborForNode:n2]){
						SSNeighborNode *neighbor = [[SSNeighborNode alloc] init];
						neighbor.launchVector = ccp(launchVector3D.x, launchVector3D.y);
						neighbor.node = n2;
						neighbor.moveType = MOVE_TYPE_JUMP;
						neighbor.cost = [n1 costToNeighbor:neighbor];
						[n1.neighbors addObject:neighbor];
					}
				}
			}
		}
	}
	
	//Schedule step method
	[self schedule:@selector(step:)];

	return self;
}

-(void) linkRunNeighbor:(SSAStarNode*)n1 with:(SSAStarNode*)n2 {
	//2-way link
	if(![n1 containsNeighborForNode:n2]){
		SSNeighborNode *neighbor = [[SSNeighborNode alloc] init];
		neighbor.moveType = MOVE_TYPE_RUN;
		neighbor.node = n2;
		neighbor.cost = [n1 costToNeighbor:neighbor];
		[n1.neighbors addObject:neighbor];
	}
	if(![n2 containsNeighborForNode:n1]){
		SSNeighborNode *neighbor = [[SSNeighborNode alloc] init];
		neighbor.moveType = MOVE_TYPE_RUN;
		neighbor.node = n1;
		neighbor.cost = [n2 costToNeighbor:neighbor];
		[n2.neighbors addObject:neighbor];
	}
}

-(void) step: (ccTime) dt {	
	[super step:dt];
	
	//Process actor waypoints
	[actor processWaypoints];
	
	//Rotate actor toward waypoints
	if(actor.waypoints.count > 0){
		CGPoint movementVector = ccp(actor.body->GetLinearVelocity().x, actor.body->GetLinearVelocity().y);
		actor.body->SetTransform(actor.body->GetPosition(), -1 * [GameHelper vectorToRadians:movementVector] + PI_CONSTANT/2);
	}
	
	//The actor never has angular velocity (he never rolls)
	actor.body->SetAngularVelocity(0.0f);
}

/* Add actor */
-(void) addActor {
	actor = [[SSGameActor alloc] init];
	actor.gameArea = self;
	
	actor.bodyDef->type = b2_dynamicBody;
	actor.bodyDef->position.Set(240/PTM_RATIO,160/PTM_RATIO);
	actor.bodyDef->userData = actor;
				
	actor.body = world->CreateBody(actor.bodyDef);
	
	actor.circleShape = new b2CircleShape();
	actor.circleShape->m_radius = 32.0f/PTM_RATIO/3;
	actor.fixtureDef->shape = actor.circleShape;
	actor.fixtureDef->friction = 1.0f;
	actor.fixtureDef->restitution = 0.0f;
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
		for(int i=0; i<nodes.count; i++){
			SSAStarNode *node = (SSAStarNode*)[nodes objectAtIndex:i];
			
			glColor4ub(255,255,255,32);
			ccDrawPoint(node.position);
			glColor4ub(255,255,255,255);
			
			//Draw neighbor lines (there is going to be a lot of them)
			for(int i=0; i<node.neighbors.count; i++){
				SSNeighborNode *neighbor = [node.neighbors objectAtIndex:i];
				
				glColor4ub(16,16,16,8);
				ccDrawLine(node.position, neighbor.node.position);
				glColor4ub(255,255,255,255);
			}
		}
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
	CGPoint p = [self convertTouchCoord:point];

	SSAStarNode *startNode;
	SSAStarNode *endNode;
	
	//Find the two closest nodes to the actor position (start) and where you tapped (end)
	CGPoint actorPosition = ccp(actor.body->GetPosition().x*PTM_RATIO, actor.body->GetPosition().y*PTM_RATIO);
	if(actor.waypoints.count > 0){
		SSGameWaypoint *wp = [actor.waypoints objectAtIndex:actor.waypoints.count-1];
		actorPosition = wp.position;
	}
	startNode = [self nodeClosestToPoint:actorPosition];
	endNode = [self nodeClosestToPoint:p];
		
	//Add the start node as the initial waypoint
	[actor addWaypoint:[SSGameWaypoint createWithPosition:startNode.position moveType:MOVE_TYPE_RUN speedMod:1.0f]];

	//Run the pathfinding algorithm
	NSMutableArray *foundPath = [SSAStarPathNode findPathFrom:startNode to:endNode];

	if(!foundPath){
		[self showMessage:@"No Path Found"];
	}else if(foundPath.count == 1){
		[self showMessage:@"No Path Found"];
		SSAStarPathNode *node = (SSAStarPathNode*)[foundPath objectAtIndex:0];
		[actor addWaypoint:node.wp];
	}else{
		[self showMessage:@"Found Path"];
		//Add all waypoints on path to actor
		for(int i=foundPath.count-1; i>=0; i--){
			SSAStarPathNode *node = (SSAStarPathNode*)[foundPath objectAtIndex:i];
			[actor addWaypoint:node.wp];
			//Add a run waypoint after a jump
			if(node.wp.moveType == MOVE_TYPE_JUMP){
				SSGameWaypoint *wp = [SSGameWaypoint createWithPosition:node.wp.position moveType:MOVE_TYPE_RUN speedMod:0.5f];
				[actor addWaypoint:wp];
			}
		}
	}
	
	[foundPath release]; foundPath = nil;
}

-(SSAStarNode*) nodeClosestToPoint:(CGPoint)p {
	SSAStarNode *foundNode = nil;
	for(int i=0; i<nodes.count; i++){
		SSAStarNode *node = (SSAStarNode*)[nodes objectAtIndex:i];	
		if(i==0 ||	[GameHelper distanceP1:node.position toP2:p] < 
					[GameHelper distanceP1:foundNode.position toP2:p]){
			foundNode = node;
		}	
	}
	return foundNode;
}

-(NSMutableArray*) findPathFrom:(SSAStarNode*)startNode to:(SSAStarNode*)endNode {
	return [SSAStarPathNode findPathFrom:startNode to:endNode];
}


@end
