#import "GameHelper.h"
#import "AStarNode.h"
#import "Box2D.h"
#import "SSGameActor.h"
#import "SSNeighborNode.h"

@class SSNeighborNode;

@interface SSAStarNode : AStarNode
{
	b2Body *body;	//The static body this node is associated with
	SSGameActor *actor;	//The actor who will be using these nodes
}

@property (readwrite, assign) b2Body *body;
@property (readwrite, assign) SSGameActor *actor;

-(float) costToNeighbor:(SSNeighborNode*)node;
-(float) costToNode:(SSAStarNode*)node;
-(bool) containsNeighborForNode:(SSAStarNode*)node;

@end

@implementation SSAStarNode

@synthesize body, actor;

-(id) init {
    if( (self=[super init]) ) {
		
    }
    return self;
}

-(float) costToNeighbor:(SSNeighborNode*)nn {
	SSAStarNode *node = nn.node;

	//Here we use jumping/running to determine cost. We could also possibly use a heuristic.
	CGPoint src = ccp(self.position.x/PTM_RATIO, self.position.y/PTM_RATIO);
	CGPoint dst = ccp(node.position.x/PTM_RATIO, node.position.y/PTM_RATIO);
	
	float cost;
	if(node.body == self.body){
		//Compute simple distance
		float runTime = ([GameHelper distanceP1:src toP2:dst]) / actor.runSpeed;
		cost = runTime * node.costMultiplier;
	}else{
		//Compute a jump		
		float y = dst.y - src.y;
		if(y == 0){ y = 0.00001f; } //Prevent divide by zero
				
		CGPoint launchVector = nn.launchVector;
		float gravity = actor.body->GetWorld()->GetGravity().y;
		
		Vector3D *at = [GameHelper quadraticA:gravity*0.5f B:launchVector.y C:y*-1];
		float airTime;
		if(at.x > at.y){
			airTime = at.x;
		}else{
			airTime = at.y;
		}
		cost = airTime * node.costMultiplier;
	}

	return cost;
}

-(float) costToNode:(SSAStarNode*)node {
	//This is a simple heuristic
	CGPoint src = ccp(self.position.x/PTM_RATIO, self.position.y/PTM_RATIO);
	CGPoint dst = ccp(node.position.x/PTM_RATIO, node.position.y/PTM_RATIO);
	
	//Compute simple distance
	float runTime = ([GameHelper distanceP1:src toP2:dst]) / actor.runSpeed;
	float cost = runTime * node.costMultiplier;

	return cost;
}

-(bool) containsNeighborForNode:(SSAStarNode*)node {
	for(int i=0; i<neighbors.count; i++){
		SSNeighborNode *n = [neighbors objectAtIndex:i];
		if(n.node == node){
			return YES;
		}
	}
	return NO;
}

@end