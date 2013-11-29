#import "Recipe.h"
#import "GameArea2D.h"
#import "GameObject.h"
#import "GameMisc.h"
#import "TexturedPolygon.h"

@class GameArea2D;
@class GameObject;
@class GameMisc;

@interface Ch4_DifferentShapes : GameArea2D
{
}

-(CCLayer*) runRecipe;
-(void) addNewObjectWithCoords:(CGPoint)p;
-(void) ccTouchesEnded:(NSSet *)touches withEvent:(UIEvent *)event;

@end


//Implementation
@implementation Ch4_DifferentShapes

-(CCLayer*) runRecipe {
	[super runRecipe];
	[message setString:@"Tap to create shapes."];
			
	//Create level boundaries
	[self addLevelBoundaries];

	return self;
}

/*
	Here add an object randomly chosen from the following:
		- a rectangle
		- a square
		- a circle
		- a polygon
*/
-(void) addNewObjectWithCoords:(CGPoint)p
{
	//Initialize the object
	GameMisc *obj = [[GameMisc alloc] init];
	obj.gameArea = self;
	
	obj.bodyDef->type = b2_dynamicBody;
	obj.bodyDef->position.Set(p.x/PTM_RATIO, p.y/PTM_RATIO);
	obj.bodyDef->userData = obj;
	obj.body = world->CreateBody(obj.bodyDef);
	
	obj.fixtureDef->density = 1.0f;
	obj.fixtureDef->friction = 0.3f;
	obj.fixtureDef->restitution = 0.2f;
	
	//Pick a random shape, size and texture
	int num = arc4random()%5;
	
	if(num == 0){
		//Square
		float textureSize = 128;
		float shapeSize = 16;
		
		obj.sprite = [CCSprite spriteWithFile:@"box2.png"];
		obj.sprite.position = ccp(p.x,p.y);
		obj.sprite.scale = shapeSize / textureSize * 2;
	
		[gameNode addChild:obj.sprite];
		
		obj.polygonShape = new b2PolygonShape();
		obj.polygonShape->SetAsBox(shapeSize/PTM_RATIO, shapeSize/PTM_RATIO);	
		obj.fixtureDef->shape = obj.polygonShape;
		
		obj.body->CreateFixture(obj.fixtureDef);
	}else if(num == 1){
		//Circle
		float textureSize = 117;
		float shapeSize = 24;
		
		obj.sprite = [CCSprite spriteWithFile:@"ball2.png"];
		obj.sprite.position = ccp(p.x,p.y);
		obj.sprite.scale = shapeSize / textureSize * 2;
		
		[gameNode addChild:obj.sprite];
		
		obj.circleShape = new b2CircleShape();
		obj.circleShape->m_radius = shapeSize/PTM_RATIO;
		obj.fixtureDef->shape = obj.circleShape;
		
		obj.fixtureDef->restitution = 0.9f;
		
		obj.body->CreateFixture(obj.fixtureDef);
	}else if(num == 2){
		//Rectangle
		CGPoint textureSize = ccp(54,215);
		CGPoint shapeSize = ccp(12,48);
	
		obj.sprite = [CCSprite spriteWithFile:@"column.png"];
		obj.sprite.position = ccp(p.x,p.y);
		obj.sprite.scaleX = shapeSize.x / textureSize.x * 2;
		obj.sprite.scaleY = shapeSize.y / textureSize.y * 2;
	
		[gameNode addChild:obj.sprite];
	
		obj.polygonShape = new b2PolygonShape();
		obj.polygonShape->SetAsBox(shapeSize.x/PTM_RATIO, shapeSize.y/PTM_RATIO);
		obj.fixtureDef->shape = obj.polygonShape;
		
		obj.body->CreateFixture(obj.fixtureDef);
	}else if(num == 3){		
		//Convex Polygon
		float polygonSize = 2.0f;
				
		CGPoint vertexArr[] = { ccp(0,.5f), ccp(.25f,0), ccp(.75f,0), ccp(.9f,.3f), ccp(1,.8f), 
			ccp(.8f,1), ccp(.4f,.95f), ccp(.1f,.8f) };
		int32 numVerts = 8;
		b2Vec2 vertices[8];
		
		NSMutableArray *vertexArray = [[NSMutableArray alloc] init];
		
		for(int i=0; i<numVerts; i++){
			vertices[i].Set(vertexArr[i].x*polygonSize, vertexArr[i].y*polygonSize);
			[vertexArray addObject:[NSValue valueWithCGPoint:ccp(vertexArr[i].x*PTM_RATIO*polygonSize, 
				vertexArr[i].y*PTM_RATIO*polygonSize)]];
		}
				
		ccTexParams params = {GL_NEAREST,GL_NEAREST_MIPMAP_NEAREST,GL_REPEAT,GL_REPEAT};
		obj.sprite = [TexturedPolygon createWithFile:@"box2.png" withVertices:vertexArray];
		[obj.sprite.texture setTexParameters:&params];
		[obj.sprite setPosition:ccp(p.x,p.y)];
		
		[gameNode addChild:obj.sprite z:1];		
			
		obj.polygonShape = new b2PolygonShape();
		obj.polygonShape->Set(vertices, numVerts);
		obj.fixtureDef->shape = obj.polygonShape;
		
		obj.body->CreateFixture(obj.fixtureDef);
	}else if(num == 4){
		//Concave Multi-Fixture Polygon
		obj.sprite = [CCSprite spriteWithFile:@"blank.png"];
		obj.sprite.position = ccp(p.x,p.y);
		[gameNode addChild:obj.sprite z:1];	
			
		ccColor3B color = ccc3(arc4random()%255, arc4random()%255, arc4random()%255);
			
		for(int i=0; i<2; i++){
			CGPoint shapeSize;
		
			if(i == 0){
				shapeSize = ccp(2.0f, 0.4f);
			}else{
				shapeSize = ccp(0.4f, 2.0f);
			}
		
			CGPoint vertexArr[] = { ccp(0,0), ccp(shapeSize.x,0), ccp(shapeSize.x,shapeSize.y), ccp(0,shapeSize.y) };
					
			int32 numVerts = 4;
			b2Vec2 vertices[4];
			
			NSMutableArray *vertexArray = [[NSMutableArray alloc] init];
			
			for(int i=0; i<numVerts; i++){
				vertices[i].Set(vertexArr[i].x, vertexArr[i].y);
				[vertexArray addObject:[NSValue valueWithCGPoint:ccp(vertexArr[i].x*PTM_RATIO, 
					vertexArr[i].y*PTM_RATIO)]];
			}
					
			ccTexParams params = {GL_NEAREST,GL_NEAREST_MIPMAP_NEAREST,GL_REPEAT,GL_REPEAT};
			CCSprite *sprite = [TexturedPolygon createWithFile:@"box2.png" withVertices:vertexArray];
			[sprite.texture setTexParameters:&params];
			[sprite setPosition:ccp(0,0)];
			[sprite setColor:color];
			[obj.sprite addChild:sprite];
				
			obj.polygonShape = new b2PolygonShape();
			obj.polygonShape->Set(vertices, numVerts);
			obj.fixtureDef->shape = obj.polygonShape;
			
			obj.body->CreateFixture(obj.fixtureDef);
		}
	}
	
	//Set a random color
	[obj.sprite setColor:ccc3(arc4random()%255, arc4random()%255, arc4random()%255)];
}

- (void)ccTouchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {
	for( UITouch *touch in touches ) {
		CGPoint location = [touch locationInView: [touch view]];
		location = [[CCDirector sharedDirector] convertToGL: location];
		[self addNewObjectWithCoords: location];
	}
}

@end
