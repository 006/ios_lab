#import "cocos2d.h"
#import "Box2D.h"
#import "GLES-Render.h"
#import "Recipe.h"
#import "DebugDrawNode.h"
#import "GameObject.h"
#import "GameObjectCallback.h"
#import "QueuedAction.h"
class basicContactListener;

#define PTM_RATIO 32

@interface GameArea2D : Recipe
{
	b2World* world;
	b2DebugDraw *m_debugDraw;
	bool debugDraw;
	CCNode *gameNode;
	
	NSMutableArray *bodiesToDestroy;
	NSMutableArray *postDestructionCallbacks;
	NSMutableArray *bodiesToCreate;
	NSMutableArray *queuedActions;
}

@property (readwrite, assign) bool debugDraw;

-(CCLayer*) runRecipe;
-(void) step: (ccTime) dt;
-(void) draw;
-(void) handleCollisionWithObjA:(GameObject*)objA withObjB:(GameObject*)objB;
-(void) dealloc;
-(void) showDebugDraw;
-(void) showMinimalDebugDraw;
-(void) hideDebugDraw;
-(void) initDebugDraw;
-(void) swapDebugDraw;
-(void) addLevelBoundaries;

-(void) markBodyForDestruction:(GameObject*)obj;
-(void) destroyBodies;
-(void) markBodyForCreation:(GameObject*)obj;
-(void) createBodies;
-(void) runQueuedActions;

@end