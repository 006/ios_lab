#import "Recipe.h"
#import "Ch8_Tiled.h"

@interface Ch7_TileMapPathfinding : Ch8_Tiled
{
	NSMutableArray *grid;
	float actorRadius;
}

-(CCLayer*) runRecipe;
-(void) addNeighbor:(AStarNode*)node toGridNodeX:(int)x Y:(int)y;
-(void) tapWithPoint:(CGPoint)point;
-(bool) hudBegan:(NSSet*)touches withEvent:(UIEvent*)event;
-(bool) hudMoved:(NSSet*)touches withEvent:(UIEvent*)event;
-(bool) hudEnded:(NSSet*)touches withEvent:(UIEvent*)event;
-(void) drawLayer;
-(void) addDrawLayer;
-(void) cameraFollowActor;

@end



//Implementation
@implementation Ch7_TileMapPathfinding

-(CCLayer*) runRecipe {
	//Superclass initialization and message
	[super runRecipe];
	[message setString:@"Tap the screen move the actor to that point.\nTap again to queue another waypoint."];

	//Shorter variable names
	float mw = tileMap.mapSize.width;
	float mh = tileMap.mapSize.height;
	float tw = tileMap.tileSize.width;
	float th = tileMap.tileSize.height;	

	actorRadius = actor.circleShape->m_radius;

	//Create 2D array (grid)
	grid = [[NSMutableArray alloc] initWithCapacity:((int)mw)];
	for(int x=0; x<mw; x++){
		[grid addObject:[[NSMutableArray alloc] initWithCapacity:((int)mh)]];
	}

	//Create active and inactive nodes determined by the "Collidable" TMX layer
	CCTMXLayer *collidableLayer = [tileMap layerNamed:@"Collidable"];
	for(int x=0; x<mw; x++){
		for(int y=0; y<mh; y++){
			//Add a node
			AStarNode *node = [[AStarNode alloc] init];
			node.position = ccp(x*tw + tw/2, y*th + th/2);
			if([collidableLayer tileAt:ccp(x,y)]){ node.active = NO; }
			[[grid objectAtIndex:x] addObject:node];
		}
	}
	
	//Add neighbors
	for(int x=0; x<mw; x++){
		for(int y=0; y<mh; y++){
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

	//Remove neighbors from positive RayCast tests
	for(int x=0; x<mw; x++){
		for(int y=0; y<mh; y++){
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

	//Initial variables
	dPad.visible = NO;
	cameraZoom = 0.75f;
	actor.runSpeed = 5.0f;

	//Add draw layer
	[self addDrawLayer];
	
	return self;
}

-(void) step: (ccTime) dt {	
	//DPad disabled
	dPad.pressed = NO;

	[super step:dt];
	
	//Process actor waypoints
	[actor processWaypoints];
	
	//Orient the actor properly
	if(actor.waypoints.count > 0){
		CGPoint movementVector = ccp(actor.body->GetLinearVelocity().x, actor.body->GetLinearVelocity().y);
		actor.body->SetTransform(actor.body->GetPosition(), -1 * [GameHelper vectorToRadians:movementVector] + PI_CONSTANT/2);
	}
}

/* Disable camera following */
-(void) cameraFollowActor {
	//Do nothing
}

/* Enable camera zooming */
-(void) setCameraZoom:(float)zoom {
	if(zoom < 0.1f){ zoom = 0.1f; }	//Lower limit
	if(zoom > 4.0f){ zoom = 4.0f; }	//Upper limit
	gameNode.scale = zoom;
	cameraZoom = zoom;
}

/* Add neighbor helper method */
-(void) addNeighbor:(AStarNode*)node toGridNodeX:(int)x Y:(int)y {
	float mw = tileMap.mapSize.width;
	float mh = tileMap.mapSize.height;
	if(x >= 0 && y >= 0 && x < mw && y < mh){
		AStarNode *neighbor = [[grid objectAtIndex:x] objectAtIndex:y];
		[node.neighbors addObject:neighbor];
	}
}

/* Add draw layer */
-(void) addDrawLayer {
	drawLayer = [[DrawLayer alloc] init];
	drawLayer.position = ccp(0,0);
	drawLayer.recipe = self;
	[gameNode addChild:drawLayer z:10000];
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
	
	float tw = tileMap.tileSize.width;
	float th = tileMap.tileSize.height;	
	float mw = tileMap.mapSize.width;
	float mh = tileMap.mapSize.height;
	
	//Starting node
	AStarNode *startNode = [[grid objectAtIndex:(int)(actorPosition.x/tw)] objectAtIndex:(int)(actorPosition.y/th)];

	//Make sure the start node is actually properly connected
	if(startNode.neighbors.count == 0){
		bool found = NO; float n = 1;
		while(!found){
			//Search the nodes around this point for a properly connected starting node
			for(float x = -n; x<= n; x+= n){
				for(float y = -n; y<= n; y+= n){
					if(x == 0 && y == 0){ continue; }
					float xIndex = ((int)(actorPosition.x/tw))+x;
					float yIndex = ((int)(actorPosition.y/th))+y;
					if(xIndex >= 0 && yIndex >= 0 && xIndex < mw && yIndex < mh){
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
	AStarNode *endNode = [[grid objectAtIndex:(int)(endPoint.x/tw)] objectAtIndex:(int)(endPoint.y/th)];	

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

/* DPad disabled */
-(bool) hudBegan:(NSSet*)touches withEvent:(UIEvent*)event {
	return NO;
}

-(bool) hudMoved:(NSSet*)touches withEvent:(UIEvent*)event {
	return NO;
}

-(bool) hudEnded:(NSSet*)touches withEvent:(UIEvent*)event {
	return NO;
}

@end