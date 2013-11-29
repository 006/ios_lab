#import "DebugDrawNode.h"

@implementation DebugDrawNode

@synthesize world;

+(id) createWithWorld:(b2World*)worldPtr{
    return [[self alloc] initWithWorld:worldPtr];
}

-(id) initWithWorld:(b2World*)worldPtr{
	if( (self = [self init]) ) {
		self.world = worldPtr;
	}
	return self;
}

-(void) draw
{
	glDisable(GL_TEXTURE_2D);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	
	world->DrawDebugData();
	
	glEnable(GL_TEXTURE_2D);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
}

@end
