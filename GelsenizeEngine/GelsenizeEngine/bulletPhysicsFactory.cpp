#include "bulletPhysicsFactory.h"

#include "SimpleShapes.h"

nPhysics::bulletPhysicsFactory::bulletPhysicsFactory()
{
}

nPhysics::bulletPhysicsFactory::~bulletPhysicsFactory()
{
}

nPhysics::iPhysicsWorld * nPhysics::bulletPhysicsFactory::CreatePhysicsWorld()
{
	return new bulletPhysicsWorld();
}

nPhysics::iRigidBody * nPhysics::bulletPhysicsFactory::CreateRigidBody(const cRigidBodyDef & rbDef, iShape * shape)
{
	return new bulletRigidBody(rbDef, shape);
}

nPhysics::iShape * nPhysics::bulletPhysicsFactory::CreateShape(const nShapes::sSphere & sphere)
{
	return new nPhysics::bulletSphereShape(sphere);
}

nPhysics::iShape * nPhysics::bulletPhysicsFactory::CreateShape(const nShapes::sPlane & plane)
{
	return new nPhysics::bulletPlaneShape(plane);
}

nPhysics::iShape * nPhysics::bulletPhysicsFactory::CreateShape(const nShapes::sBox & box)
{
	return new nPhysics::bulletBoxShape(box);
}

nPhysics::iShape * nPhysics::bulletPhysicsFactory::CreateShape(const nShapes::sCapsule & capsule)
{
	return new nPhysics::bulletCapsuleShape(capsule.width, capsule.height);
}

nPhysics::iShape * nPhysics::bulletPhysicsFactory::CreateShape(const nShapes::sCylinder & cylinder)
{
	return new nPhysics::bulletCylinderShape(cylinder.radius, cylinder.height);
}
