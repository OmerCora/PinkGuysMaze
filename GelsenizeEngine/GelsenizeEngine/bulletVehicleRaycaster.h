#pragma once

#include <btBulletDynamicsCommon.h>

class bulletVehicleRaycaster : public btVehicleRaycaster
{
	btDynamicsWorld*	m_dynamicsWorld;
public:
	bulletVehicleRaycaster(btDynamicsWorld* world)
		:m_dynamicsWorld(world)
	{
	}

	virtual void* castRay(const btVector3& from, const btVector3& to, btVehicleRaycasterResult& result);

	int collisionFilterMask;
	int collisionFilterGroup;
	bool useFilter;
};