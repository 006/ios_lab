#import "GameActor.h"
#import "GameWaypoint.h"
#import "GameHelper.h"
#import "GameArea2D.h"
#import "Box2D.h"

#define WAYPOINT_DIST_THRESHOLD 16.0f
#define TIMES_BLOCKED_FAIL 2

@implementation GameActor

@synthesize waypoints, runSpeed, direction, lastVelocity, animCount, color;

-(id) init {
    if( (self=[super init]) ) {
		waypoints = [[NSMutableArray alloc] init];
		runSpeed = 10.0f;
		timesBlocked = 0;
		lastAngularVelocity = 0.0f;
		animCount = 0.0f;
		lastVelocity = ccp(0,0);
		direction = DOWN_LEFT;
		color = @"lightblue";
    }
    return self;
}

-(void) addWaypoint:(GameWaypoint*)waypoint {
	[waypoints addObject:waypoint];
}

-(void) processWaypoints {
	bool removeFirstWaypoint = NO;
	
	//The actor's position onscreen
	CGPoint worldPosition = CGPointMake(self.body->GetPosition().x * PTM_RATIO, self.body->GetPosition().y * PTM_RATIO);
	
	//Process waypoints
	for(GameWaypoint *wp in waypoints){
		float distanceToNextPoint = [GameHelper distanceP1:worldPosition toP2:CGPointMake(wp.position.x, wp.position.y)];
		
		//If we didn't make progress to the next point, increment timesBlocked
		if(distanceToNextPoint >= wp.lastDistance){
			timesBlocked++;
			
			//Drop this waypoint if we failed to move a number of times
			if(timesBlocked > TIMES_BLOCKED_FAIL){
				distanceToNextPoint = 0.0f;
			}
		}else{
			//If we are just starting toward this point we run our pre-callback
			wp.lastDistance = distanceToNextPoint;
			[wp processPreCallback];
		}

		//If we are close enough to the waypoint we move onto the next one
		if(distanceToNextPoint <= WAYPOINT_DIST_THRESHOLD){
			removeFirstWaypoint = YES;
			[self stopRunning];
			
			//Run post callback
			[wp processPostCallback];
		}else{
			//Keep running toward the waypoint
			float speedMod = wp.speedMod;
			
			//Slow down close to the waypoint
			if(distanceToNextPoint < [self runSpeed]/PTM_RATIO){
				speedMod = (distanceToNextPoint)/([self runSpeed]/PTM_RATIO);
			}
			[self runWithVector:ccp(wp.position.x - worldPosition.x, wp.position.y - worldPosition.y) withSpeedMod:speedMod withConstrain:NO ];
			break;
		}
	}
	if(removeFirstWaypoint){
		[waypoints removeObjectAtIndex:0];
		timesBlocked = 0;
	}
}

-(void) runWithVector:(CGPoint)v withSpeedMod:(float)speedMod withConstrain:(bool)constrain {	
	//Change animation depending on angle
	float radians = [GameHelper vectorToRadians:v];
	float degrees = [GameHelper radiansToDegrees:radians];
	CGPoint constrainedVector;	//Vector constained to only the 8 angles
	CGPoint unconstrainedVector = [GameHelper radiansToVector:radians];	//Unconstrained vector
		
	degrees += 90.0f;
		
	if(degrees >= 337.5f || degrees < 22.5f){
		direction = LEFT;
	}else if(degrees >= 22.5f && degrees < 67.5f){
		direction = UP_LEFT;
	}else if(degrees >= 67.5f && degrees < 112.5f){
		direction = UP;
	}else if(degrees >= 112.5f && degrees < 157.5f){
		direction = UP_RIGHT;
	}else if(degrees >= 157.5f && degrees < 202.5f){
		direction = RIGHT;
	}else if(degrees >= 202.5f && degrees < 247.5f){
		direction = DOWN_RIGHT;
	}else if(degrees >= 247.5f && degrees < 292.5f){
		direction = DOWN;
	}else{
		direction = DOWN_LEFT;
	}
	
	constrainedVector = [GameActor getNormalVectorFromDirection:direction];
	
	if(constrain){
		self.body->SetLinearVelocity(b2Vec2(constrainedVector.x*runSpeed*speedMod, constrainedVector.y*runSpeed*speedMod));
	}else{
		self.body->SetLinearVelocity(b2Vec2(unconstrainedVector.x*runSpeed*speedMod, unconstrainedVector.y*runSpeed*speedMod));
	}
	
	if(lastAngularVelocity != 0.0f && lastAngularVelocity == self.body->GetAngularVelocity()){
		self.body->SetAngularVelocity(0.0f);
	}
	lastAngularVelocity = self.body->GetAngularVelocity();
	lastVelocity = ccp(self.body->GetLinearVelocity().x, self.body->GetLinearVelocity().y);
}

-(void) stopRunning {
	self.body->SetLinearVelocity(b2Vec2(0.0f,0.0f));
}

+(CGPoint) getNormalVectorFromDirection:(int)dir{
	CGPoint v;
	if(dir == LEFT){
		v = ccp(-1,0);
	}else if(dir == UP_LEFT){
		v = ccp(-0.7071067812,0.7071067812);
	}else if(dir == UP){
		v = ccp(0,1);
	}else if(dir == UP_RIGHT){
		v = ccp(0.7071067812,0.7071067812);
	}else if(dir == RIGHT){
		v = ccp(1,0);
	}else if(dir == DOWN_RIGHT){
		v = ccp(0.7071067812,-0.7071067812);
	}else if(dir == DOWN){
		v = ccp(0,-1);
	}else if(dir == DOWN_LEFT){
		v = ccp(-0.7071067812,-0.7071067812);
	}
	return v;
}

-(void) animateActor {
	CCSpriteFrameCache *cache = [CCSpriteFrameCache sharedSpriteFrameCache];

	//Animation delay is inverse speed
	float delay = 0.5f/sqrt(pow(self.lastVelocity.x,2)+pow(self.lastVelocity.y,2));
	if(delay > 0.5f){ delay = 0.5f; }
	CCAnimation *animation = [[CCAnimation alloc] initWithName:@"gunman_anim" delay:delay];	

	NSString *dir;
	bool flipX = NO;
	bool moving = YES;
	
	if( sqrt(pow(self.lastVelocity.x,2)+pow(self.lastVelocity.y,2)) < 0.2f){ moving = NO; }
		
	if(self.direction == LEFT){ dir = @"right"; flipX = YES; }
	else if(self.direction == UP_LEFT){ dir = @"up_right"; flipX = YES; }
	else if(self.direction == UP){ dir = @"up"; }
	else if(self.direction == UP_RIGHT){ dir = @"up_right"; }
	else if(self.direction == RIGHT){ dir = @"right"; }
	else if(self.direction == DOWN_RIGHT){ dir = @"down_right"; }
	else if(self.direction == DOWN){ dir = @"down"; }
	else if(self.direction == DOWN_LEFT){ dir = @"down_right"; flipX = YES; }

	((CCSprite*)[self.sprite getChildByTag:0]).flipX = flipX;

	if(moving){
		[animation addFrame:[cache spriteFrameByName:[NSString stringWithFormat:@"gunman_%@_run_%@_01.png",color,dir]]];
		[animation addFrame:[cache spriteFrameByName:[NSString stringWithFormat:@"gunman_%@_stand_%@.png",color,dir]]];
		[animation addFrame:[cache spriteFrameByName:[NSString stringWithFormat:@"gunman_%@_run_%@_02.png",color,dir]]];
		[animation addFrame:[cache spriteFrameByName:[NSString stringWithFormat:@"gunman_%@_stand_%@.png",color,dir]]];
	}else{
		[animation addFrame:[cache spriteFrameByName:[NSString stringWithFormat:@"gunman_%@_stand_%@.png",color,dir]]];
	}

	//animateActor calls itself indefinitely
	[[self.sprite getChildByTag:0] runAction:[CCSequence actions: 
		[CCAnimate actionWithAnimation:animation],
		[CCCallFunc actionWithTarget:self selector:@selector(animateActor)], nil ]];
}

-(void) timeShift:(float)seconds vect:(CGPoint)vect magnitude:(float)magnitude {
	b2Vec2 force = b2Vec2(vect.x*magnitude, vect.y*magnitude);

	body->SetLinearVelocity(body->GetLinearVelocity() + seconds * (1.0f/body->GetMass() * force));
		
	float clamp = clampf(1.0f - seconds * body->GetLinearDamping(), 0.0f, 1.0f);
	b2Vec2 v = body->GetLinearVelocity();
	body->SetLinearVelocity(b2Vec2(v.x*clamp, v.y*clamp));
}

@end