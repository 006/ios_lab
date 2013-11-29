//NOTE: We've added b2RopeJoint to our Box2D distribution here.
//When I finally upgrade all the recipes to the latest version of Cocos2d, Box2D should have b2RopeJoint.
//If not I'll have to add it back.

#import "SideScrollerRecipe.h"
#import "VRope.h"

@interface Ch4_Rope : SideScrollerRecipe
{
	bool onRope;
	b2Body* anchorBody;
	CCSpriteBatchNode* ropeSpriteSheet;
	NSMutableArray* vRopes;
	float ropeUseTimer;
}

-(CCLayer*) runRecipe;
-(void) step:(ccTime)delta;
-(void) addRopeAnchor;
-(void) useRope;
-(void) releaseRope;

@end

//Implementation
@implementation Ch4_Rope

-(CCLayer*) runRecipe {
	[super runRecipe];
	[message setString:@"Press B to fire a rope."];
	
	//Initialization
	onRope = NO;
	ropeUseTimer = 0;
	
	//Move gunman to left
	gunman.body->SetTransform(b2Vec2(2,10), gunman.body->GetAngle());
	
	//Create buttons
	[self createButtonWithPosition:ccp(340,75) withUpFrame:@"b_button_up.png" withDownFrame:@"b_button_down.png" withName:@"B"];
	[self createButtonWithPosition:ccp(420,75) withUpFrame:@"a_button_up.png" withDownFrame:@"a_button_down.png" withName:@"A"];
	
	//Create ground
	[self addBoxWithPosition:ccp(20,40) file:@"bricks.png" density:5.0f];
	[self addBoxWithPosition:ccp(60,40) file:@"bricks.png" density:5.0f];
	[self addBoxWithPosition:ccp(20,80) file:@"bricks.png" density:5.0f];
	[self addBoxWithPosition:ccp(60,80) file:@"bricks.png" density:5.0f];
	
	[self addBoxWithPosition:ccp(440,40) file:@"bricks.png" density:5.0f];
	[self addBoxWithPosition:ccp(480,40) file:@"bricks.png" density:5.0f];
	[self addBoxWithPosition:ccp(440,80) file:@"bricks.png" density:5.0f];
	[self addBoxWithPosition:ccp(480,80) file:@"bricks.png" density:5.0f];
	
	//Add invisible rope anchor
	[self addRopeAnchor];
	
	return self;
}

-(void) step:(ccTime)delta {
	[super step:delta];
	
	//Process button input
	for(id b in buttons){
		GameButton *button = (GameButton*)b;
		if(button.pressed && [button.name isEqualToString:@"B"]){
			if(!onRope){
				[self useRope];
			}else{
				[self releaseRope];
			}
		}
		if(button.pressed && [button.name isEqualToString:@"A"]){
			if(onRope){
				[self releaseRope];
			}else{
				[self processJump];
			}
		}else if(!button.pressed && [button.name isEqualToString:@"A"]){
			jumpCounter = -10.0f;
		}
	}
	
	//Update all ropes
	for(id v in vRopes){
		VRope *rope = (VRope*)v;
		[rope update:delta];
		[rope updateSprites];
	}
	
	//Decrement our use timer
	ropeUseTimer -= delta;
}

-(void) addRopeAnchor {
	//Add rope anchor body
	b2BodyDef anchorBodyDef;
	anchorBodyDef.position.Set(240/PTM_RATIO,350/PTM_RATIO); //center body on screen
	anchorBody = world->CreateBody(&anchorBodyDef);
	
	//Add rope spritesheet to layer
	ropeSpriteSheet = [CCSpriteBatchNode batchNodeWithFile:@"rope.png" ];
	[self addChild:ropeSpriteSheet];
	
	//Init array that will hold references to all our ropes
	vRopes = [[NSMutableArray alloc] init];
}

-(void) useRope {
	if(ropeUseTimer > 0){
		return;
	}else{
		ropeUseTimer = 0.2f;
	}
	
	//The rope joint goes from the anchor to the gunman
	b2RopeJointDef jd;
	jd.bodyA = anchorBody;
	jd.bodyB = gunman.body;
	jd.localAnchorA = b2Vec2(0,0);
	jd.localAnchorB = b2Vec2(0,0);
	jd.maxLength= (gunman.body->GetPosition() - anchorBody->GetPosition()).Length();

	//Create VRope with two b2bodies and pointer to spritesheet
	VRope *newRope = [[VRope alloc] init:anchorBody body2:gunman.body spriteSheet:ropeSpriteSheet];
	
	//Create joint
	newRope.joint = world->CreateJoint(&jd);
	[vRopes addObject:newRope];	
	
	//Keep track of 'onRope' state
	onRope = !onRope;
}

-(void) releaseRope {
	if(ropeUseTimer > 0){
		return;
	}else{
		ropeUseTimer = 0.2f;
	}
	
	//Jump off the rope
	[self processJump];
	
	//Destroy the rope
	for(id v in vRopes){
		VRope *rope = (VRope*)v;
		world->DestroyJoint(rope.joint);
		[rope removeSprites];
		[rope release];
	}
	[vRopes removeAllObjects];
	
	//Keep track of 'onRope' state
	onRope = !onRope;
}

@end