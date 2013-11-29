#import "Minimap.h"

@implementation Minimap

@synthesize wallVertices1, wallVertices2, projectiles, scale, staticObjects, actor;

-(id)init {
    self = [super init];
    if (self != nil) {	
		wallVertices1 = [[NSMutableArray alloc] init];
		wallVertices2 = [[NSMutableArray alloc] init];
		projectiles = [[NSMutableDictionary alloc] init];
		staticObjects = [[NSMutableArray alloc] init];
		scale = 2.5f;
    }
    return self;
}

-(void)dealloc {
	[wallVertices1 release];
	[wallVertices2 release];
	[projectiles release];
	[staticObjects release];

    [super dealloc];
}

-(void) addWallWithVertex1:(CGPoint)v1 withVertex2:(CGPoint)v2 {
	[wallVertices1 addObject:[NSValue valueWithCGPoint:v1]];	
	[wallVertices2 addObject:[NSValue valueWithCGPoint:v2]];	
}
-(void) setProjectile:(CGPoint)point withKey:(NSString*)key {
	[projectiles setObject:[NSValue valueWithCGPoint:point] forKey:key];
}
-(void) addStaticObject:(CGPoint)p {
	[staticObjects addObject:[NSValue valueWithCGPoint:p]];
}
-(void) draw {	
	// line: color, width, anti-aliased
	glEnable(GL_LINE_SMOOTH);
			
	//Draw walls
	glLineWidth( 2.0f );
	for(int i=0; i<[wallVertices1 count]; i++){	
		NSValue *v1 = [wallVertices1 objectAtIndex:i]; NSValue *v2 = [wallVertices2 objectAtIndex:i];
		CGPoint p1 = [v1 CGPointValue]; CGPoint p2 = [v2 CGPointValue];
		
		glColor4ub(96,96,96,0);
		ccDrawLine( CGPointMake(p1.x*scale, p1.y*scale), CGPointMake(p2.x*scale, p2.y*scale) );
	}

	//Draw projectiles
	glPointSize(4);
	for(id key in projectiles){
		NSValue *v = [projectiles objectForKey:key];
		CGPoint p = [v CGPointValue];
		
		glColor4ub(255,128,0,128);
		ccDrawPoint( CGPointMake(p.x*scale, p.y*scale) );
	}
	
	//Draw static objects
	glPointSize(4);
	for(int i=0; i<[staticObjects count]; i++){	
		NSValue *v = [staticObjects objectAtIndex:i];
		CGPoint p = [v CGPointValue];
		
		glColor4ub(96,96,96,0);
		ccDrawPoint( CGPointMake(p.x*scale, p.y*scale) );
	}

	//Draw Actor
	glPointSize(6);
	glColor4ub(0,64,255,255);
	ccDrawPoint(CGPointMake(actor.x*scale, actor.y*scale));	

	// restore original values
	glLineWidth(1);
	glDisable(GL_LINE_SMOOTH);
	glColor4ub(255,255,255,255);
	glPointSize(1);
}

@end
