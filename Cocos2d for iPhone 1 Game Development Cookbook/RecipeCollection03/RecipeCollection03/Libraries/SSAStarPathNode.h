#import "SSGameWaypoint.h"
#import "SSNeighborNode.h"

@class SSGameWaypoint;
@class SSNeighborNode;

@interface SSAStarPathNode : NSObject {
	SSGameWaypoint *wp;
	SSAStarNode *node;	//The actual node this "path" node points to
	SSAStarPathNode *previous;	//The previous node on our path
	float cost;	//The cumulative cost of reaching this node
}

@property (readwrite, assign) SSGameWaypoint *wp;
@property (readwrite, assign) SSAStarNode *node;
@property (readwrite, assign) SSAStarPathNode *previous;
@property (readwrite, assign) float cost;

+(id) createWithSSAStarNode:(SSAStarNode*)node;
+(NSMutableArray*) findPathFrom:(SSAStarNode*)fromNode to:(SSAStarNode*)toNode;
+(SSAStarPathNode*)lowestCostNodeInArray:(NSMutableArray*)a;
+(bool) isPathNode:(SSAStarPathNode*)a inList:(NSArray*)list;

@end



@implementation SSAStarPathNode

@synthesize wp, node, previous, cost;

-(id) init {
    if( (self=[super init]) ) {
		cost = 0.0f;
    }
    return self;
}

+(id) createWithSSAStarNode:(SSAStarNode*)node {
	if(!node){	//Can't have a path node without a real node
		return nil;
	}
	SSAStarPathNode *pathNode = [[SSAStarPathNode alloc] init];
	pathNode.node = node;
	return pathNode;
}

+(NSMutableArray*) findPathFrom:(SSAStarNode*)fromNode to:(SSAStarNode*)toNode {
	NSMutableArray *foundPath = [[NSMutableArray alloc] init];
	[foundPath retain];

	if(fromNode.position.x == toNode.position.x && fromNode.position.y == toNode.position.y){
		return nil;
	 } 
	
	NSMutableArray *openList = [[[NSMutableArray alloc] init] autorelease];
	NSMutableArray *closedList = [[[NSMutableArray alloc] init] autorelease];
	
	SSAStarPathNode *currentNode = nil;
	SSAStarPathNode *aNode = nil;
	
	SSAStarPathNode *startNode = [SSAStarPathNode createWithSSAStarNode:fromNode];
	startNode.wp = [SSGameWaypoint createWithPosition:startNode.node.position moveType:MOVE_TYPE_RUN speedMod:1.0f];
	
	SSAStarPathNode *endNode = [SSAStarPathNode createWithSSAStarNode:toNode];
	endNode.wp = [SSGameWaypoint createWithPosition:endNode.node.position moveType:MOVE_TYPE_RUN speedMod:1.0f];
	
	[openList addObject:startNode];
		
	while(openList.count > 0){
		currentNode = [SSAStarPathNode lowestCostNodeInArray:openList];
			
		if( currentNode.node.position.x == endNode.node.position.x &&
			currentNode.node.position.y == endNode.node.position.y){
			
			//Path Found!
			aNode = currentNode;
			while(aNode.previous != nil){
				//Mark path
				[foundPath addObject:aNode];
				aNode = aNode.previous;
			}
			[foundPath addObject:aNode];
			return foundPath;
		}else{
			//Still searching
			[closedList addObject:currentNode];
			[openList removeObject:currentNode];
			
			for(int i=0; i<currentNode.node.neighbors.count; i++){
				SSNeighborNode *neighbor = (SSNeighborNode*)[currentNode.node.neighbors objectAtIndex:i];
				SSAStarPathNode *aNode = [SSAStarPathNode createWithSSAStarNode:neighbor.node];
				
				//If currentNode and aNode are on the same body, we run. Otherwise, we jump.
				aNode.cost = currentNode.cost + [currentNode.node costToNeighbor:neighbor] + [aNode.node costToNode:endNode.node];
				aNode.previous = currentNode;
				aNode.wp = [SSGameWaypoint createWithPosition:aNode.node.position moveType:neighbor.moveType speedMod:1.0f];
				aNode.wp.launchVector = neighbor.launchVector;
				
				if(aNode.node.active && ![SSAStarPathNode isPathNode:aNode inList:openList] && ![SSAStarPathNode isPathNode:aNode inList:closedList]){
					[openList addObject:aNode];
				}
			}
		}
	}
	
	//No Path Found
	[foundPath addObject:startNode];
	
	return foundPath;
}

//Find the lowest cost node in an array
+(SSAStarPathNode*)lowestCostNodeInArray:(NSMutableArray*)a {
	SSAStarPathNode *lowest = nil;
	for(int i=0; i<a.count; i++){
		SSAStarPathNode *node = [a objectAtIndex:i];
		if(!lowest || node.cost < lowest.cost){
			lowest = node;
		}
	}
	return lowest;
}

//Is a path node in a given list?
+(bool) isPathNode:(SSAStarPathNode*)a inList:(NSArray*)list {
	for(int i=0; i<list.count; i++){
		SSAStarPathNode *b = [list objectAtIndex:i];
		if(a.node.position.x == b.node.position.x && a.node.position.y == b.node.position.y){
			return YES;
		}
	}
	return NO;
}

@end