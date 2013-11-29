#import "cocos2d.h"

@interface Minimap : CCLayer {
@public
	NSMutableArray *wallVertices1;
	NSMutableArray *wallVertices2;
	NSMutableDictionary *projectiles;
	NSMutableArray *staticObjects;
	CGPoint actor;	//A pointer to the main actor we will follow
	float scale;	//How small the minimap is

}

@property (nonatomic, retain) NSMutableArray *wallVertices1;
@property (nonatomic, retain) NSMutableArray *wallVertices2;
@property (nonatomic, retain) NSMutableDictionary *projectiles;
@property (nonatomic, retain) NSMutableArray *staticObjects;
@property (readwrite, assign) CGPoint actor;
@property (readwrite, assign) float scale;

-(void) draw;
-(void) addWallWithVertex1:(CGPoint)v1 withVertex2:(CGPoint)v2;
-(void) addStaticObject:(CGPoint)p;
-(void) setProjectile:(CGPoint)point withKey:(NSString*)key;
-(void) setActor:(CGPoint)point;

@end

