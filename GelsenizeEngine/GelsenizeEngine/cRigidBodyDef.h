#pragma once

#include "externals.h"

#include "iGameObject.h"

namespace nPhysics
{
	class cRigidBodyDef
	{
	public:
		cRigidBodyDef();
		// initial position
		glm::vec3 Position;
		// initial velocity
		glm::vec3 Velocity;
		// initial rotation
		glm::vec3 Angle;
		glm::quat Orientation;

		// mass
		float Mass;
		// doesn't move (only position matters if this is true)
		bool Static;
		float restitution;
		float friction;
		float linearDamping;
		float angularDamping;

		int ownerID;
		iGameObject* ownerObject;
	};
}