#import "GameHelper.h"

@interface AStarNode : NSObject
{
	CGPoint position;	//The node's position on our map
	NSMutableArray *neighbors;	//An array of neighbor AStarNode objects
	bool active;	//Is this node active?
	float costMultiplier;	//Use this to multiply the normal cost to reach this node.
}

@property (readwrite, assign) CGPoint position;
@property (readwrite, assign) NSMutableArray *neighbors;
@property (readwrite, assign) bool active;
@property (readwrite, assign) float costMultiplier;

-(float) costToNode:(AStarNode*)node;
+(bool) isNode:(AStarNode*)a inList:(NSArray*)list;

@end

@implementation AStarNode

@synthesize position, neighbors, active, costMultiplier;

-(id) init {
    if( (self=[super init]) ) {
		active = YES;
		neighbors = [[NSMutableArray alloc] init];
		costMultiplier = 1.0f;
    }
    return self;
}

/* Cost to node heuristic */
-(float) costToNode:(AStarNode*)node {
	CGPoint src = ccp(self.position.x, self.position.y);
	CGPoint dst = ccp(node.position.x, node.position.y);
	float cost = [GameHelper distanceP1:src toP2:dst] * node.costMultiplier;
	return cost;
}

/* Helper method: Is a node in a given list? */
+(bool) isNode:(AStarNode*)a inList:(NSArray*)list {
	for(int i=0; i<list.count; i++){
		AStarNode *b = [list objectAtIndex:i];
		if(a.position.x == b.position.x && a.position.y == b.position.y){
			return YES;
		}
	}
	return NO;
}

@end