#import "Recipe.h"
#import "Ch7_LineOfSight.h"

@interface Ch7_AIFlocking : Ch7_LineOfSight
{
}

-(CCLayer*) runRecipe;
-(void) setGameAreaSize;
-(void) processBoids;
-(b2Vec2) boidRule1:(GameActor*)b;
-(b2Vec2) boidRule2:(GameActor*)b;
-(b2Vec2) boidRule3:(GameActor*)b;

@end


//Implementation
@implementation Ch7_AIFlocking

-(CCLayer*) runRecipe {
	//Set game area size
	[self setGameAreaSize];

	//Superclass initialization and message
	[super runRecipe];
	[message setString:@"Use the DPad to avoid the AI actors."];

	//Add some geometry
	[self addRandomPolygons:15];
	
	//Initial variables
	cameraZoom = 0.225f;

	return self;
}

-(void) step:(ccTime)delta {
	[super step:delta];
	
	//Process the 'boids' flocking algorithm
	[self processBoids];
}

//Set game area size
-(void) setGameAreaSize {
	gameAreaSize = ccp(2000/PTM_RATIO,1200/PTM_RATIO);	//Box2d units
}

/* Add actor */
-(void) addActor {
	actor = [[GameActor alloc] init];
	actor.gameArea = self;
	
	actor.bodyDef->type = b2_dynamicBody;
	actor.bodyDef->position.Set(gameAreaSize.x/2,gameAreaSize.y/2);
	actor.bodyDef->userData = actor;
				
	actor.body = world->CreateBody(actor.bodyDef);
	
	actor.circleShape = new b2CircleShape();
	actor.circleShape->m_radius = 0.5f;
	actor.fixtureDef->shape = actor.circleShape;
	actor.fixtureDef->filter.categoryBits = CB_ACTOR;
	actor.fixtureDef->filter.maskBits = CB_EVERYTHING & ~CB_ENEMY & ~CB_ACTOR;
	
	actor.body->CreateFixture(actor.fixtureDef);
	
	actor.body->SetLinearDamping(2.0f);
}

/* Add 30 'boid' enemies */
-(void) addEnemies {
	enemies = [[NSMutableArray alloc] init];

	for(int i=0; i<30; i++){
		GameActor *enemy = [[GameActor alloc] init];
		enemy.gameArea = self;
		enemy.tag = 0;	//We use this to store whether or not we can see the actor
		
		enemy.bodyDef->type = b2_dynamicBody;
		enemy.bodyDef->position.Set((arc4random()%((int)(gameAreaSize.x-2)))+1.0f,(arc4random()%((int)(gameAreaSize.y-2)))+1.0f);
		enemy.bodyDef->userData = enemy;
					
		enemy.body = world->CreateBody(enemy.bodyDef);
		
		enemy.circleShape = new b2CircleShape();
		enemy.circleShape->m_radius = 0.5f;
		enemy.fixtureDef->shape = enemy.circleShape;
		enemy.fixtureDef->filter.categoryBits = CB_ENEMY;
		enemy.fixtureDef->filter.maskBits = CB_EVERYTHING & ~CB_ACTOR;
		
		enemy.body->CreateFixture(enemy.fixtureDef);
		
		[enemies addObject:enemy];
	}
}

/* Make the flock of 'boids' follow the actor */
-(void) followActorWithEnemies {
	//If enemies can see the user they follow
	for(int i=0; i<enemies.count; i++){
		//Align enemies
		GameActor *enemy = [enemies objectAtIndex:i];
		CGPoint directionVector = CGPointMake(actor.body->GetPosition().x - enemy.body->GetPosition().x, actor.body->GetPosition().y - enemy.body->GetPosition().y);
		float radians = [GameHelper vectorToRadians:directionVector];
		enemy.body->SetTransform(enemy.body->GetPosition(), -1 * radians + PI_CONSTANT/2);
		
		b2Vec2 normal = actor.body->GetPosition() - enemy.body->GetPosition();
		CGPoint vector = ccp(normal.x, normal.y);
		CGPoint normalVector = [GameHelper radiansToVector:[GameHelper vectorToRadians:vector]];
		
		//If so, follow the actor
		b2Vec2 v = enemy.body->GetLinearVelocity();
		enemy.body->SetLinearVelocity(b2Vec2(v.x + normalVector.x*0.2f, v.y + normalVector.y*0.2f));
	}
}

/* Process boids algorithm */
-(void) processBoids {
	for(int i=0; i<enemies.count; i++){
		GameActor *b = [enemies objectAtIndex:i];
		
		b2Vec2 v1 = b2Vec2(0,0);
		b2Vec2 v2 = b2Vec2(0,0);
		b2Vec2 v3 = b2Vec2(0,0);
		
		v1 = [self boidRule1:b];
		v2 = [self boidRule2:b];
		v3 = [self boidRule3:b];
	
		b2Vec2 v = b.body->GetLinearVelocity();
		b2Vec2 newV = v+v1+v2+v3;
		
		/* Limit velocity */
		float vLimit = 7.5f;
		b2Vec2 absV = b2Vec2([GameHelper absoluteValue:newV.x], [GameHelper absoluteValue:newV.y]);
		if(absV.x > vLimit || absV.y > vLimit){
			float ratio;
			if(absV.x > absV.y){
				ratio = vLimit / absV.x;			
			}else{
				ratio = vLimit / absV.y;
			}
			newV = b2Vec2( newV.x*ratio, newV.y*ratio );
		}		
		b.body->SetLinearVelocity(newV);
	}
}

/* Clump the Boids together */
-(b2Vec2) boidRule1:(GameActor*)bJ {
	b2Vec2 pcJ = b2Vec2(0,0);
	float N = enemies.count;
	
	for(int i=0; i<enemies.count; i++){
		GameActor *b = [enemies objectAtIndex:i];
		if(b != bJ){
			pcJ += b.body->GetPosition();
		}
	}
	
	pcJ = b2Vec2(pcJ.x/(N-1), pcJ.y/(N-1));
	
	return b2Vec2( (pcJ.x - bJ.body->GetPosition().x)/100.0f, (pcJ.y - bJ.body->GetPosition().y)/100.0f );
}

/* Keep the Boids apart from each other */
-(b2Vec2) boidRule2:(GameActor*)bJ  {
	//Set optimal distance boids should keep between themselves
	float padding = 1.5f;
	b2Vec2 c = b2Vec2(0,0);
	
	for(int i=0; i<enemies.count; i++){
		GameActor *b = [enemies objectAtIndex:i];
		if(b != bJ){
			CGPoint bPos = ccp(b.body->GetPosition().x, b.body->GetPosition().y);
			CGPoint bJPos = ccp(bJ.body->GetPosition().x, bJ.body->GetPosition().y);
			if([GameHelper distanceP1:bPos toP2:bJPos] < padding){ //This was 100 in the example
				c = c - (b.body->GetPosition() - bJ.body->GetPosition());
			}
		}
	}
	
	return c;
}

/* Match up all Boid velocities */
-(b2Vec2) boidRule3:(GameActor*)bJ  {
	b2Vec2 pvJ = b2Vec2(0,0);
	
	for(int i=0; i<enemies.count; i++){
		GameActor *b = [enemies objectAtIndex:i];
		if(b != bJ){
			pvJ += b.body->GetLinearVelocity();
		}
	}
	
	pvJ = b2Vec2(pvJ.x/enemies.count-1, pvJ.y/enemies.count-1);
	
	b2Vec2 v = bJ.body->GetLinearVelocity();
	return b2Vec2((pvJ.x - v.x)/30.0f, (pvJ.y - v.y)/30.0f);
}

@end
