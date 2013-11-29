#import "SSAStarNode.h"

@class SSAStarNode;

@interface SSNeighborNode : NSObject {
    float cost;	//The cost to this node
	SSAStarNode *node;	//The node
	int moveType;	//How we should move to this node
	CGPoint launchVector;	//If we need to JUMP, the launch vector
}

@property (readwrite, assign) float cost;
@property (readwrite, assign) SSAStarNode *node;
@property (readwrite, assign) int moveType;
@property (readwrite, assign) CGPoint launchVector;

@end


@implementation SSNeighborNode

@synthesize cost, node, moveType, launchVector;

@end