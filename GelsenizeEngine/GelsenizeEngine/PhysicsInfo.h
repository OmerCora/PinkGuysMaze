#ifndef _PHYSICSINFO_HG_
#define _PHYSICSINFO_HG_

#include "GelStructures.h"

#include "iRigidBody.h"

using namespace nPhysics;

class PhysicsInfo
{
public:
	PhysicsInfo();
	~PhysicsInfo();

	bool enabled;
	bool ethereal;
	bool useSphericalBody;

	GelVelocity velocity;
	GelAcceleration acceleration;
	GelRotationalVelocity rotationalVelocity; //glm::vec3
	GelVelocity velocityDirected;

	//iRigidBody* rigidBody;
	iCollisionBody *collisionBody;
private:
	


	
};

#endif // !_PHYSICSINFO_HG_
