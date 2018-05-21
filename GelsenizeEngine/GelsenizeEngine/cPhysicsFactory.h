#pragma once

#include "cRigidBody.h"
#include "cPhysicsWorld.h"

#include "iPhysicsWorld.h"
#include "iPhysicsFactory.h"

namespace nPhysics
{
	class cPhysicsFactory: public iPhysicsFactory
	{
	public:
		cPhysicsFactory();
		~cPhysicsFactory();

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