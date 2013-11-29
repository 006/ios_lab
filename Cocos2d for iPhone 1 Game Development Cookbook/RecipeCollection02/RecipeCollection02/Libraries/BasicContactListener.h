#ifndef BASIC_CONTACT_LISTENER_H
#define BASIC_CONTACT_LISTENER_H

class basicContactListener : public b2ContactListener
{
	public:
		void BeginContact(b2Contact* contact);
		void EndContact(b2Contact* contact);
		void PreSolve(b2Contact* contact, const b2Manifold* oldManifold);
		void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse);
};

void basicContactListener::BeginContact(b2Contact* contact) 
{	
	b2Body *bodyA = contact->GetFixtureA()->GetBody();
	b2Body *bodyB = contact->GetFixtureB()->GetBody();

	if(bodyA and bodyB){
		GameObject *objA = (GameObject*)bodyA->GetUserData();
		GameObject *objB = (GameObject*)bodyB->GetUserData();
		GameArea2D *gameArea = (GameArea2D*)objA.gameArea;
		[gameArea handleCollisionWithObjA:objA withObjB:objB];
	}	
}

void basicContactListener::EndContact(b2Contact* contact) 
{ 

}
void basicContactListener::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
{

}
void basicContactListener::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
{

}

#endif