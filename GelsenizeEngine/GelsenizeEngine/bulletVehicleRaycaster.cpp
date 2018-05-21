#include "bulletVehicleRaycaster.h"

void * bulletVehicleRaycaster::castRay(const btVector3 & from, const btVector3 & to, btVehicleRaycasterResult & result)
{
	btCollisionWorld::ClosestRayResultCallback rayCallback(from, to);

	if (useFilter)
	{
		rayCallback.m_collisionFilterMask = collisionFilterMask;
		rayCallback.m_collisionFilterGroup = collisionFilterGroup;
	}

	m_dynamicsWorld->rayTest(from, to, rayCallback);

	if (rayCallback.hasHit())
	{

		btRigidBody* body = (btRigidBody*)(rayCallback.m_collisionObject);
		if (body && body->hasContactResponse())
		{
			result.m_hitPointInWorld = rayCallback.m_hitPointWorld;
			result.m_hitNormalInWorld = rayCallback.m_hitNormalWorld;
			result.m_hitNormalInWorld.normalize();
			result.m_distFraction = rayCallback.m_closestHitFraction;
			return (void*)body;
		}
	}
	return 0;
}
