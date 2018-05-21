#pragma once

//#include <btBulletDynamicsCommon.h>

#include "bulletRigidBody.h" //bullet
#include "bulletPhysicsWorld.h" //bullet

#include "iPhysicsWorld.h"
#include "iPhysicsFactory.h"

namespace nPhysics
{
	class bulletPhysicsFactory : public iPhysicsFactory
	{
	public:
		bulletPhysicsFactory();
		~bulletPhysicsFactory();

		virtual iPhysicsWorld* CreatePhysicsWorld();
		virtual iRigidBody* CreateRigidBody(const cRigidBodyDef& rbDef, iShape* shape);
		virtual iShape* CreateShape(const nShapes::sSphere& sphere);
		virtual iShape* CreateShape(const nShapes::sPlane& plane);
		virtual iShape* CreateShape(const nShapes::sBox& box);
		virtual iShape* CreateShape(const nShapes::sCapsule& capsule);
		virtual iShape* CreateShape(const nShapes::sCylinder& capsule);
	private:

	};
}