#include "cRigidBodyDef.h"

namespace nPhysics
{
	cRigidBodyDef::cRigidBodyDef()
		: Static(false), Mass(1.f)
	{
		restitution = 0.5;
		friction = 0.5;
		linearDamping = 0.0f;
		angularDamping = 0.0f;
	}
}