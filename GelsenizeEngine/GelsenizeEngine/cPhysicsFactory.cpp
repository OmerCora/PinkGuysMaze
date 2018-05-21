#include "cPhysicsFactory.h"

#include "SimpleShapes.h"

namespace nPhysics
{
	iPhysicsWorld * cPhysicsFactory::CreatePhysicsWorld()
	{	
		return new nPhysics::cPhysicsWorld();
	}

	iRigidBody * cPhysicsFactory::CreateRigidBody(const cRigidBodyDef & rbDef, iShape * shape)
	{
		return new nPhysics::cRigidBody(rbDef, shape);
	}

	iShape * cPhysicsFactory::CreateShape(const nShapes::sSphere & sphere)
	{
		return new nPhysics::cSphereShape(sphere);
	}

	iShape * cPhysicsFactory::CreateShape(const nShapes::sPlane & plane)
	{
		return new nPhysics::cPlaneShape(plane);
	}

	iShape * cPhysicsFactory::CreateShape(const nShapes::sBox & box)
	{
		return nullptr;
	}

	iShape * cPhysicsFactory::CreateShape(const nShapes::sCapsule & capsule)
	{
		return nullptr;
	}

	iShape * cPhysicsFactory::CreateShape(const nShapes::sCylinder & capsule)
	{
		return nullptr;
	}

	cPhysicsFactory::cPhysicsFactory()
	{
	}

	cPhysicsFactory::~cPhysicsFactory()
	{
	}
}


