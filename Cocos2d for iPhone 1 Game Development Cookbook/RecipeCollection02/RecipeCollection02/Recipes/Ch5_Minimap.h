#import "Recipe.h"
#import "Ch4_TopDownIsometric.h"
#import "Minimap.h"

@interface Ch5_Minimap : Ch4_TopDownIsometric
{
	Minimap *minimap;
}

-(CCLayer*) runRecipe;
-(void) step:(ccTime)delta;

@end

@implementation Ch5_Minimap

-(CCLayer*) runRecipe {
	//Initialize the Minimap object
	minimap = [[Minimap alloc] init];
	minimap.position = ccp(300,140);
	[self addChild:minimap z:10];

	//Run our top-down isometric game recipe
	[super runRecipe];
	
	//Add trees as static objects
	for(id t in trees){
		GameObject *tree = (GameObject*)t;
		[minimap addStaticObject:ccp(tree.body->GetPosition().x, tree.body->GetPosition().y)];
	}
	
	return self;
}

-(void) step:(ccTime)delta {
	[super step:delta];
	
	//Set the actor position
	[minimap setActor: ccp(gunman.body->GetPosition().x, gunman.body->GetPosition().y)];
	
	//Set individual projectile positions
	for(id b in balls){
		GameObject *ball = (GameObject*)b;
		[minimap setProjectile:ccp(ball.body->GetPosition().x, ball.body->GetPosition().y) withKey:[NSString stringWithFormat:@"%d", ball.tag]];
	}
}

//We overload this method to automatically add walls to the minimap
-(void) addBrickWallFrom:(CGPoint)p1 to:(CGPoint)p2 height:(float)height {
	//Convert wall vertex positions to the properly scaled Box2D coordinates
	CGPoint vert1 = ccp(p1.x/PTM_RATIO,p1.y/PTM_RATIO/PERSPECTIVE_RATIO);
	CGPoint vert2 = ccp(p2.x/PTM_RATIO,p2.y/PTM_RATIO/PERSPECTIVE_RATIO);
	
	//Add both wall vertices
	[minimap addWallWithVertex1:vert1 withVertex2:vert2];

	[super addBrickWallFrom:p1 to:p2 height:height];
}

@end
