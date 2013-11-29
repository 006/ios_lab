#ifndef ISOMETRIC_CONTACT_LISTENER_H
#define ISOMETRIC_CONTACT_LISTENER_H

class isometricContactListener : public b2ContactListener
{
	public:
		void BeginContact(b2Contact* contact);
		void EndContact(b2Contact* contact);
		void PreSolve(b2Contact* contact, const b2Manifold* oldManifold);
		void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse);
};

void isometricContactListener::BeginContact(b2Contact* contact) 
{	
	b2Body *bodyA = contact->GetFixtureA()->GetBody();
	b2Body *bodyB = contact->GetFixtureB()->GetBody();

	if(bodyA and bodyB){
		float lowerZSize;
		if(bodyA->GetZPosition() < bodyB->GetZPosition()){ lowerZSize = bodyA->GetZSize(); }
		else{ lowerZSize = bodyB->GetZSize(); }
		
		//Check for Z Miss and disable collision if neccessary
		if( absoluteValue(bodyA->GetZPosition() - bodyB->GetZPosition()) > lowerZSize ) { //If distance is greater than the height of the bottom one
			contact->SetEnabled(false);
			if(bodyA->GetHandleZMiss() || bodyB->GetHandleZMiss()){
				GameObject *gameObjectA = (GameObject*)bodyA->GetUserData();
				GameObject *gameObjectB = (GameObject*)bodyB->GetUserData();
				[gameObjectA->gameArea handleZMissWithObjA:gameObjectA withObjB:gameObjectB];
				bodyA->SetHandleZMiss(false);
				bodyB->SetHandleZMiss(false);
			}
		//If no Z Miss handle collision
		}else {
			GameObject *gameObjectA = (GameObject*)bodyA->GetUserData();
			GameObject *gameObjectB = (GameObject*)bodyB->GetUserData();
			[gameObjectA->gameArea handleCollisionWithObjA:gameObjectA withObjB:gameObjectB];
		}
	}		
}

void isometricContactListener::EndContact(b2Contact* contact) 
{ 
	//Check for Z Miss and disable collision if neccessary
	b2Body *bodyA = contact->GetFixtureA()->GetBody();
	b2Body *bodyB = contact->GetFixtureB()->GetBody();
	
	if(bodyA and bodyB){
		float lowerZSize;
		if(bodyA->GetZPosition() < bodyB->GetZPosition()){ lowerZSize = bodyA->GetZSize(); }
		else{ lowerZSize = bodyB->GetZSize(); }
		if( absoluteValue(bodyA->GetZPosition() - bodyB->GetZPosition()) > lowerZSize ) { //If distance is greater than the height of the bottom one
			contact->SetEnabled(false);
			if(bodyA->GetHandleZMiss() || bodyB->GetHandleZMiss()){
				GameObject *gameObjectA = (GameObject*)bodyA->GetUserData();
				GameObject *gameObjectB = (GameObject*)bodyB->GetUserData();
				[gameObjectA->gameArea handleZMissWithObjA:gameObjectA withObjB:gameObjectB];
				bodyA->SetHandleZMiss(false);
				bodyB->SetHandleZMiss(false);
			}
		}
	}
}
void isometricContactListener::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
{
	//Check for Z Miss and disable collision if neccessary
	b2Body *bodyA = contact->GetFixtureA()->GetBody();
	b2Body *bodyB = contact->GetFixtureB()->GetBody();

	if(bodyA and bodyB){
		float lowerZSize;
		if(bodyA->GetZPosition() < bodyB->GetZPosition()){ lowerZSize = bodyA->GetZSize(); }
		else{ lowerZSize = bodyB->GetZSize(); }
		if( absoluteValue(bodyA->GetZPosition() - bodyB->GetZPosition()) > lowerZSize ) { //If distance is greater than the height of the bottom one
			contact->SetEnabled(false);
			if(bodyA->GetHandleZMiss() || bodyB->GetHandleZMiss()){
				GameObject *gameObjectA = (GameObject*)bodyA->GetUserData();
				GameObject *gameObjectB = (GameObject*)bodyB->GetUserData();
				[gameObjectA->gameArea handleZMissWithObjA:gameObjectA withObjB:gameObjectB];
				bodyA->SetHandleZMiss(false);
				bodyB->SetHandleZMiss(false);
			}
		}
	}
}

void isometricContactListener::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
{

}

#endif