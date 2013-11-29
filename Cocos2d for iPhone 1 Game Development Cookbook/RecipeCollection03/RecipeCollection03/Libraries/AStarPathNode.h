#import "GameHelper.h"
#import "AStarNode.h"

@interface AStarPathNode : NSObject
{
	AStarNode *node;	//The actual node this "path" node points to
	AStarPathNode *previous;	//The previous node on our path
	float cost;	//The cumulative cost of reaching this node
}

@property (readwrite, assign) AStarNode *node;
@property (readwrite, assign) AStarPathNode *previous;
@property (readwrite, assign) float cost;

+(id) createWithAStarNode:(AStarNode*)node;
+(NSMutableArray*) findPathFrom:(AStarNode*)fromNode to:(AStarNode*)toNode;
+(AStarPathNode*)lowestCostNodeInArray:(NSMutableArray*)a;
+(bool) isPathNode:(AStarPathNode*)a inList:(NSArray*)list;

@end

@implementation AStarPathNode

@synthesize node, previous, cost;

-(id) init {
    if( (self=[super init]) ) {
		cost = 0.0f;
    }
    return self;
}

+(id) createWithAStarNode:(AStarNode*)node {
	if(!node){	//Can't have a path node without a real node
		return nil;
	}
	AStarPathNode *pathNode = [[AStarPathNode alloc] init];
	pathNode.node = node;
	return pathNode;
}

/* Our implementation of the A* search algorithm */
+(NSMutableArray*) findPathFrom:(AStarNode*)fromNode to:(AStarNode*)toNode {
	NSMutableArray *foundPath = [[NSMutableArray alloc] init];

	if(fromNode.position.x == toNode.position.x && fromNode.position.y == toNode.position.y){
		return nil;
	 } 
	
	NSMutableArray *openList = [[[NSMutableArray alloc] init] autorelease];
	NSMutableArray *closedList = [[[NSMutableArray alloc] init] autorelease];
	
	AStarPathNode *currentNode = nil;
	AStarPathNode *aNode = nil;
	
	AStarPathNode *startNode = [AStarPathNode createWithAStarNode:fromNode];
	AStarPathNode *endNode = [AStarPathNode createWithAStarNode:toNode];
	[openList addObject:startNode];
		
	while(openList.count > 0){
		currentNode = [AStarPathNode lowestCostNodeInArray:openList];
			
		if( currentNode.node.position.x == endNode.node.position.x &&
			currentNode.node.position.y == endNode.node.position.y){
			
			//Path Found!
			aNode = currentNode;
			while(aNode.previous != nil){
				//Mark path
				[foundPath addObject:[NSValue valueWithCGPoint: CGPointMake(aNode.node.position.x, aNode.node.position.y)]];
				aNode = aNode.previous;
			}
			[foundPath addObject:[NSValue valueWithCGPoint: CGPointMake(aNode.node.position.x, aNode.node.position.y)]];
			return foundPath;
		}else{
			//Still searching
			[closedList addObject:currentNode];
			[openList removeObject:currentNode];
			
			for(int i=0; i<currentNode.node.neighbors.count; i++){
				AStarPathNode *aNode = [AStarPathNode createWithAStarNode:[currentNode.node.neighbors objectAtIndex:i]];
				aNode.cost = currentNode.cost + [currentNode.node costToNode:aNode.node] + [aNode.node costToNode:endNode.node];
				aNode.previous = currentNode;
				
				if(aNode.node.active && ![AStarPathNode isPathNode:aNode inList:openList] && ![AStarPathNode isPathNode:aNode inList:closedList]){
					[openList addObject:aNode];
				}
			}
		}
	}
	
	//No Path Found
	return nil;
}

/* Helper method: Find the lowest cost node in an array */
+(AStarPathNode*)lowestCostNodeInArray:(NSMutableArray*)a {
	AStarPathNode *lowest = nil;
	for(int i=0; i<a.count; i++){
		AStarPathNode *node = [a objectAtIndex:i];
		if(!lowest || node.cost < lowest.cost){
			lowest = node;
		}
	}
	return lowest;
}

/* Helper method: Is a path node in a given list? */
+(bool) isPathNode:(AStarPathNode*)a inList:(NSArray*)list {
	for(int i=0; i<list.count; i++){
		AStarPathNode *b = [list objectAtIndex:i];
		if(a.node.position.x == b.node.position.x && a.node.position.y == b.node.position.y){
			return YES;
		}
	}
	return NO;
}

@end