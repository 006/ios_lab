#ifndef BASIC_CONTACT_FILTER_H
#define BASIC_CONTACT_FILTER_H

//Filter override located in b2WorldCallbacks.h and b2WorldCallbacks.cpp
class basicContactFilter : public b2ContactFilter
{
	public:
		bool ShouldCollide(b2Fixture* fixtureA, b2Fixture* fixtureB);
};

bool basicContactFilter::ShouldCollide(b2Fixture* fixtureA, b2Fixture* fixtureB)
{
	const b2Filter& filterA = fixtureA->GetFilterData();
	const b2Filter& filterB = fixtureB->GetFilterData();

	if (filterA.groupIndex == filterB.groupIndex && filterA.groupIndex != 0)
	{
		return filterA.groupIndex > 0;
	}

	bool collide = (filterA.maskBits & filterB.categoryBits) != 0 && (filterA.categoryBits & filterB.maskBits) != 0;

	return collide;
}

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