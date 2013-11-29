#import "GameHelper.h"
#import "GameActor.h"
#import "SSGameWaypoint.h"
#import "Vector3D.h"

#define WP_TIMEOUT 50.0f
#define WP_RUN_DIST_THRESHOLD_X 4.0f
#define WP_RUN_DIST_THRESHOLD_Y 64.0f
#define WP_JUMP_DIST_THRESHOLD_X 256.0f
#define WP_JUMP_DIST_THRESHOLD_Y 64.0f
#define TIMES_BLOCKED_FAIL 2

@interface SSGameActor : GameActor
{
	CGPoint maxSpeed;	//How fast the actor can run and/or jump
	float timeoutCounter;
}

@property (readwrite, assign) CGPoint maxSpeed;

-(void) processWaypoints;
-(void) addWaypoint:(SSGameWaypoint*)waypoint;
-(void) runToWaypoint:(SSGameWaypoint*)wp speedMod:(float)speedMod constrain:(bool)constrain;
-(void) jumpToWaypoint:(SSGameWaypoint*)wp;
+(Vector3D*) canJumpFrom:(CGPoint)src to:(CGPoint)dst radius:(float)radius world:(b2World*)world maxSpeed:(CGPoint)maxSpeed;
+(CGPoint) getLaunchVector:(CGPoint)vect jumpHeightMod:(float)jumpHeightMod gravity:(float)gravity;
-(void) flyToWaypoint:(SSGameWaypoint*)wp;

@end

@implementation SSGameActor

@synthesize maxSpeed;

-(id) init {
    if( (self=[super init]) ) {
		maxSpeed = CGPointMake(10.0f,20.0f);
		timeoutCounter = 0.0f;
    }
    return self;
}

-(void) processWaypoints {
	if(waypoints.count == 0){ return; }
	// Here we need to process waypoints not by basic distance but by how quickly they can be EITHER jumped to or run to
	// in 2D space. Estimating run time will require a little bit of calc. We already have the jump time.
	bool removeFirstWaypoint = NO;
	bool blocked = NO;
	
	CGPoint worldPosition = CGPointMake(self.body->GetPosition().x * PTM_RATIO, self.body->GetPosition().y * PTM_RATIO);
	
	SSGameWaypoint *wp = [waypoints objectAtIndex:0];
	CGPoint vectorToNextPoint = ccp([GameHelper absoluteValue:worldPosition.x - wp.position.x], 
		[GameHelper absoluteValue:worldPosition.y - wp.position.y]);
	float distanceToNextPoint = [GameHelper distanceP1:worldPosition toP2:wp.position];
	
	if(distanceToNextPoint >= wp.lastDistance){
		timeoutCounter+=1.0f;
		if(timeoutCounter > WP_TIMEOUT){
			blocked = YES;	//Looks like you are blocked. Dropping all waypoints
		}
	}else{
		wp.lastDistance = distanceToNextPoint;
		[wp processPreCallback];
	}
	
	if(wp.moveType == MOVE_TYPE_RUN && vectorToNextPoint.x <= WP_RUN_DIST_THRESHOLD_X && 
	  vectorToNextPoint.y <= WP_RUN_DIST_THRESHOLD_Y){
		removeFirstWaypoint = YES;
		[self stopRunning];
		[wp processPostCallback];		
	}else if(wp.moveType == MOVE_TYPE_JUMP && vectorToNextPoint.x <= WP_JUMP_DIST_THRESHOLD_X && 
	  vectorToNextPoint.y <= WP_JUMP_DIST_THRESHOLD_Y){
		removeFirstWaypoint = YES;
		[self stopRunning];
		[wp processPostCallback];			
	}else{
		if(wp.moveType == MOVE_TYPE_RUN){
			float speedMod = wp.speedMod;
			if(distanceToNextPoint < [self runSpeed]/PTM_RATIO){
				speedMod = (distanceToNextPoint)/([self runSpeed]/PTM_RATIO);
			}
			[self runToWaypoint:wp speedMod:speedMod constrain:NO];
		}else if(wp.moveType == MOVE_TYPE_JUMP && wp.active){
			//We can't jump while in the air
			if([GameHelper absoluteValue:self.body->GetLinearVelocity().y] < 0.5f){
				[self jumpToWaypoint:wp];
				wp.active = NO;
			}
		}else if(wp.moveType == MOVE_TYPE_JUMP && !wp.active){
			//Do nothing
		}else if(wp.moveType == MOVE_TYPE_FLY){
			[self flyToWaypoint:wp];
		}
	}
	if(blocked){
		timeoutCounter = 0.0f;
		timesBlocked = 0;
		[self stopRunning];
		[waypoints removeAllObjects];
	
	}else if(removeFirstWaypoint){	
		//Stop the actor if he's running
		if(waypoints.count == 1){
			b2Vec2 v = self.body->GetLinearVelocity();
			self.body->SetLinearVelocity(b2Vec2(0,v.y/10));
		}
	
		timeoutCounter = 0.0f;
		[waypoints removeObjectAtIndex:0];
		timesBlocked = 0;
	}
}

-(void) addWaypoint:(SSGameWaypoint*)waypoint {
	[waypoints addObject:waypoint];
}

-(void) runToWaypoint:(SSGameWaypoint*)wp speedMod:(float)speedMod constrain:(bool)constrain {
	//Run either left or right
	
	//We can't run while in the air...
	if([GameHelper absoluteValue:self.body->GetLinearVelocity().y] != 0){
		return;
	}
	
	CGPoint src = ccp(self.body->GetPosition().x*PTM_RATIO, self.body->GetPosition().y*PTM_RATIO);
	CGPoint dst = ccp(wp.position.x, wp.position.y);
	
	b2Vec2 v = self.body->GetLinearVelocity();
	if([GameHelper absoluteValue:dst.x - src.x] < WP_RUN_DIST_THRESHOLD_X){
		self.body->SetLinearVelocity(b2Vec2(0,v.y));
	}else if(dst.x < src.x){
		self.body->SetLinearVelocity(b2Vec2(-1 * speedMod * runSpeed,v.y));
	}else{
		self.body->SetLinearVelocity(b2Vec2(speedMod * runSpeed,v.y));
	}
}

-(void) jumpToWaypoint:(SSGameWaypoint*)wp {
	//Jump to a location
	self.body->SetLinearVelocity(b2Vec2(wp.launchVector.x, wp.launchVector.y));
}

+(Vector3D*) canJumpFrom:(CGPoint)src to:(CGPoint)dst radius:(float)radius world:(b2World*)world maxSpeed:(CGPoint)maxSpeed {
	float x = dst.x - src.x;
	float y = dst.y - src.y;
	if(y == 0){ y = 0.00001f; } //Prevent divide by zero

	bool foundJumpSolution = NO;
	bool triedAngles = NO;
	CGPoint launchVector;
	float jumpHeightMod = 0.5f;
		
	while(!triedAngles){
		//Gravity
		float gravity = world->GetGravity().y;
		if(gravity == 0){ gravity = 0.00001f; } //Prevent divide by zero
		
		launchVector = [SSGameActor getLaunchVector:CGPointMake(x,y) jumpHeightMod:jumpHeightMod gravity:gravity];	
		
		bool hitObject = NO;
		bool movingTooFast = NO;
	
		/* Make sure jump doesn't hit an object */
		Vector3D *at = [GameHelper quadraticA:gravity*0.5f B:launchVector.y C:y*-1];
		float airTime;
		if(at.x > at.y){
			airTime = at.x;
		}else{
			airTime = at.y;
		}
		
		//Do a ray test sequence (from 0.1 to 0.9 of airTime)
		for(float t=airTime/10; t<airTime-airTime/10; t+= airTime/10){	
			if(hitObject){ break; }
		
			float t1 = t + airTime/10;
	
			float x1 = launchVector.x * t + src.x;
			float y1 = launchVector.y * t + (0.5f) * gravity * pow(t,2) + src.y;
			
			float x2 = launchVector.x * t1 + src.x;
			float y2 = launchVector.y * t1 + (0.5f) * gravity * pow(t1,2) + src.y;
										
			//Point Test	
			for(b2Body *b = world->GetBodyList(); b; b = b->GetNext()){
				if (b->GetUserData() != NULL) {
					GameObject *obj = (GameObject*)b->GetUserData();
					if(obj.tag == GO_TAG_WALL && obj->polygonShape){
						for(float x_ = -radius; x_ <= radius; x_+= radius*2){
							for(float y_ = -radius; y_ <= radius; y_+= radius*2){	
								if(obj->polygonShape->TestPoint(b->GetTransform(), b2Vec2(x1+x_,y1+y_))){
									hitObject = YES;
									break; break;
								}
							}
						}
						if(hitObject){ break; }
					}
				}
			}
			
			//RayCast Test
			for(float x_ = -radius; x_ <= radius; x_+= radius*2){
				for(float y_ = -radius; y_ <= radius; y_+= radius*2){								
					if([GameHelper distanceP1:CGPointMake(x1+x_,y1+y_) toP2:CGPointMake(x2+x_,y2+y_)] > 0){
						RayCastMultipleCallback callback;
						world->RayCast(&callback, b2Vec2(x1+x_,y1+y_), b2Vec2(x2+x_,y2+y_));
					
						for(int i=0; i<callback.m_count; i++){
							GameObject *obj = (GameObject*)callback.m_fixtures[i]->GetBody()->GetUserData();
							if(obj.tag == GO_TAG_WALL){
								hitObject = YES;
								break;
							}
						}
					}
					if(hitObject){ break; break; }
				}
			}
		}
		
		//Make sure the launchVector is not too fast for this actor
		if(!hitObject){
			if([GameHelper absoluteValue:launchVector.x] > maxSpeed.x || [GameHelper absoluteValue:launchVector.y] > maxSpeed.y){
				movingTooFast = YES;
			}
		}

		if(hitObject || movingTooFast){
			//This jump failed, try another
			if(jumpHeightMod <= 0.5f && jumpHeightMod >= 0.2f){	//First, try between 0.5f and 0.1f
				jumpHeightMod -= 0.1f;
			}else if(jumpHeightMod > 0.5f && jumpHeightMod < 1.0f){	//Then try between 0.6f and 1.0f
				jumpHeightMod += 0.1f;
			}else if(jumpHeightMod < 0.2f){
				jumpHeightMod = 0.6f;
			}else if(jumpHeightMod >= 1.0f){
				//FAIL
				triedAngles = YES;
			}
		}else{
			//SUCCESS
			foundJumpSolution = YES;
			triedAngles = YES;
		}
	}

	if(foundJumpSolution){
		return [Vector3D x:launchVector.x y:launchVector.y z:0];
	}else{
		return nil;
	}
}

+(CGPoint) getLaunchVector:(CGPoint)vect jumpHeightMod:(float)jumpHeightMod gravity:(float)gravity {
	//Gravity
	if(gravity == 0){ gravity = 0.00001f; } //Prevent divide by zero
	
	//The angle between the points
	float directionAngle = [GameHelper vectorToRadians:ccp(vect.x, vect.y)];

	//Jump height is a percentage of X distance, usually 0.5f
	float apexX;
	if(vect.y > 0){
		apexX = vect.x - (vect.x*0.5f*pow([GameHelper absoluteValue:sinf(directionAngle)],0.5f/jumpHeightMod));
	}else{
		apexX = vect.x*0.5f*pow([GameHelper absoluteValue:sinf(directionAngle)],0.5f/jumpHeightMod);
	}
	
	float apexY;
	if(vect.y > 0){
		apexY = vect.y + [GameHelper absoluteValue:vect.x*jumpHeightMod]*[GameHelper absoluteValue:sinf(directionAngle)];
	}else{
		apexY = [GameHelper absoluteValue:vect.x*jumpHeightMod]*[GameHelper absoluteValue:sinf(directionAngle)];
	}

	//Get launch vector
	float vectY = sqrtf(2*(-1)*gravity*apexY);
	float vectX = (apexX*(-1)*gravity) / vectY;
	
	return CGPointMake(vectX, vectY);
}

-(void) flyToWaypoint:(SSGameWaypoint*)wp {
	//Apply anti-gravity force
	self.body->ApplyForce(-self.body->GetMass() * self.body->GetWorld()->GetGravity(), self.body->GetWorldCenter());

	//Apply directional flying force
	CGPoint worldPosition = CGPointMake(self.body->GetPosition().x * PTM_RATIO, self.body->GetPosition().y * PTM_RATIO);
	CGPoint vector = ccp(wp.position.x - worldPosition.x, wp.position.y - worldPosition.y);
	
	float radians = [GameHelper vectorToRadians:vector];
	CGPoint normalVector = [GameHelper radiansToVector:radians];
		
	float flySpeed = 10.0f;
		
	self.body->ApplyForce(b2Vec2(normalVector.x * flySpeed, normalVector.y * flySpeed), self.body->GetPosition());
}

-(void) stopRunning {
	
}

@end