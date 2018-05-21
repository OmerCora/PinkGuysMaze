#pragma once
#include "iRigidBody.h"
#include "cRigidBodyDef.h"
#include "shapes.h"
#include "iShape.h"
#include "iPhysicsWorld.h"

namespace nPhysics
{
	class iPhysicsFactory
	{
	public:
		virtual ~iPhysicsFactory() {}
		virtual iPhysicsWorld* CreatePhysicsWorld() = 0;
		virtual iRigidBody* CreateRigidBody(const cRigidBodyDef& rbDef, iShape* shape) = 0;
		virtual iShape* CreateShape(const nShapes::sSphere& sphere) = 0;
		virtual iShape* CreateShape(const nShapes::sPlane& plane) = 0;
		virtual iShape* CreateShape(const nShapes::sBox& box) = 0;
		virtual iShape* CreateShape(const nShapes::sCapsule& capsule) = 0;
		virtual iShape* CreateShape(const nShapes::sCylinder& capsule) = 0;
	};
}