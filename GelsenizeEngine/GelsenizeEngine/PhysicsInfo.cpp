#include "PhysicsInfo.h"

#include "cPhysicsFactory.h"

PhysicsInfo::PhysicsInfo()
{
	this->velocity.x = this->velocity.y = this->velocity.z = 0.0f;
	this->acceleration.x = this->acceleration.y = this->acceleration.z = 0.0f;
	this->rotationalVelocity.x = this->rotationalVelocity.y = this->rotationalVelocity.z = 0.0f;
	this->velocityDirected.x = this->velocityDirected.y = this->velocityDirected.z = 0.0f;

	this->enabled = false;
	this->ethereal = false;
	this->useSphericalBody = false;

	iPhysicsFactory* myPhysicsFactory = new nPhysics::cPhysicsFactory();
	//rigid body definition
	cRigidBodyDef bodyDef;
	bodyDef.Static = true;
	bodyDef.Mass = 1.0f;

	//generic rigid body class
	/*iRigidBody* rigidBody = myPhysicsFactory->CreateRigidBody(
		bodyDef,
		myPhysicsFactory->CreateShape(nPhysics::nShapes::sSphere(1.0f)));
	this->rigidBody = rigidBody;*/
	this->collisionBody = 0;
}

PhysicsInfo::~PhysicsInfo()
{
}