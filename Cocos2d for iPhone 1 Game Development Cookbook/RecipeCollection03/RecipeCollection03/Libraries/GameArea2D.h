#import "cocos2d.h"
#import "Box2D.h"
#import "GLES-Render.h"
#import "Recipe.h"
#import "DebugDrawNode.h"
#import "GameObject.h"
#import "ObjectCallback.h"
#import "QueuedAction.h"
class basicContactFilter;
class basicContactListener;

#define PTM_RATIO 32

enum {
	GO_TAG_WALL = 0,
	GO_TAG_ACTOR = 1
};

@interface GameArea2D : Recipe
{
	b2World* world;
	b2DebugDraw *m_debugDraw;
	bool debugDraw;
	CCNode *gameNode;
	DebugDrawNode *debugDrawNode;
	GameObject *levelBoundary;
	
	NSMutableArray *bodiesToDestroy;
	NSMutableArray *postDestructionCallbacks;
	NSMutableArray *bodiesToCreate;
	NSMutableArray *queuedActions;
	
	CGPoint gameAreaSize;
	float cameraZoom;	//Starts at 1.0f. Smaller means more zoomed out.
	float cameraZoomTo;
	float cameraZoomSpeed; //Must be greater than 0 but less than 1
	float lastMultiTouchZoomDistance;	//How far apart your fingers were last time
	CGPoint draggedToPoint;	//How far we dragged from initial touch
	CGPoint lastTouchedPoint;	//Where we last touched
	int cameraState;	//What is the camera currently doing?
	
	NSMutableDictionary *allTouches;
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
-(void) addRandomPolygons:(int)num;
-(void) addRandomBoxes:(int)num;
-(void) addPolygonAtPoint:(CGPoint)p;
-(void) addBoxAtPoint:(CGPoint)p size:(CGPoint)s;

-(void) markBodyForDestruction:(GameObject*)obj;
-(void) destroyBodies;
-(void) markBodyForCreation:(GameObject*)obj;
-(void) createBodies;
-(void) runQueuedActions;

//Drawing
-(void) drawLayer;

//Camera
-(bool) checkCameraBoundsWithFailPosition:(CGPoint*)failPosition;
-(bool) checkCameraBounds;
-(void) setCameraPosition:(CGPoint)position;
-(CGPoint) convertCamera:(CGPoint)p;
-(void) setCameraZoom:(float)zoom;
-(void) zoomTo:(float)zoom withSpeed:(float)speed;
-(void) processZoomStep;
-(CGPoint) convertTouchCoord:(CGPoint)touchPoint;
-(CGPoint) convertGameCoord:(CGPoint)gamePoint;
-(void) centerCameraOnGameCoord:(CGPoint)gamePoint;

//Touches
-(void) ccTouchesBegan:(NSSet *)touches withEvent:(UIEvent *)event;
-(void) ccTouchesMoved:(NSSet *)touches withEvent:(UIEvent *)event;
-(void) ccTouchesEnded:(NSSet *)touches withEvent:(UIEvent *)event;
-(bool) hudBegan:(NSSet*)touches withEvent:(UIEvent*)event;
-(bool) hudMoved:(NSSet*)touches withEvent:(UIEvent*)event;
-(bool) hudEnded:(NSSet*)touches withEvent:(UIEvent*)event;
-(void) tapWithPoint:(CGPoint)point;

//Misc
-(float) distanceP1:(CGPoint)firstTouch toP2:(CGPoint)secondTouch;


@end