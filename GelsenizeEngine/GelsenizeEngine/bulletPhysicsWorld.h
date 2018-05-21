#pragma once

//#include <btBulletDynamicsCommon.h>

#include <functional>

#include "externals.h"
#include "iPhysicsWorld.h"
#include "bulletRigidBody.h" //bullet
#include "cContact.h" //not really
#include <vector>

#include "GelKinematicCharacterController.h"

namespace nPhysics
{
	class bulletPhysicsWorld : public iPhysicsWorld
	{
	public:
		bulletPhysicsWorld();
		~bulletPhysicsWorld();

		virtual bool RegisterCollisionListener(std::function< void(iGameObject*, iGameObject*) > listener);

		virtual bool AddPoint2PointConstraint(iCollisionBody * body1, glm::vec3 pivot);
		virtual bool AddPoint2PointConstraint(iCollisionBody * body1, iCollisionBody * body2, glm::vec3 pivot, glm::vec3 pivot2);
		virtual bool AddHingeConstraint(iCollisionBody * body1, glm::vec3 from);
		virtual bool AddSliderConstraint(iCollisionBody * body1, glm::vec3 from);
		virtual bool AddConeTwistConstraint(iCollisionBody * body1, glm::vec3 from);

		virtual bool SaveShape(iShape shapeToSave);
		virtual bool SaveShape(std::vector<iShape> &allShapes);

		virtual bool CreateCharacter(iGameObject* characterOwnerObject);
		virtual void GetCharacterBodyProperties(glm::vec3 & specialScale, eShapeType & shapeType);

		virtual void WarpVehicle(glm::vec3 position);
		virtual void WarpCharacter(glm::vec3 position);
		virtual void GetWheelTransforms(std::vector<glm::vec3>&positions, std::vector<glm::quat>&orientations, std::vector<glm::mat4>&transformations);

		virtual void MousePositionCallback(float xPosition, float yPosition);
		virtual void KeyboardCallback(int key, int action);

		virtual bool AddBody(iCollisionBody* body);
		virtual bool RemoveBody(iCollisionBody* body);

		virtual void TimeStep(float dt);
	private:
		void CheckGhost(btPairCachingGhostObject* ghostObject);

		btRigidBody* CreateGroundRigidBodyFromShape(btCollisionShape* groundShape);
		btRigidBody* CreateChassisRigidBodyFromShape(btCollisionShape* chassisShape);

		void AddWheels(
			btVector3* halfExtents,
			btRaycastVehicle* vehicle,
			btRaycastVehicle::btVehicleTuning tuning);

		void CreateVehicle();
		btRaycastVehicle *vehicle;
		bool drivingVehicle;

		void toggleDrivingMode();

		//mouse tracking
		float mousePercentX;
		float mousePercentY;
		bool leftSide;
		bool topSide;

		//Character Controller
		btCollisionShape* ghostShape;
		btPairCachingGhostObject* ghostObject;
		btGhostPairCallback* ghostPairCallback;
		btRigidBody* characterRigidBody;
		btCollisionShape* characterShape;
		btPairCachingGhostObject* actorGhost;
		eShapeType characterShapeType;
		glm::vec3 characterShapeSpecialScale;

		std::vector<cContact> contacts;
		float totalTime;

		bool  jumpControlLock;
		bool  jumpControlLock_internal;
		float jumpControlLockAcc;
		bool  jumpOngoing;
		bool  jumpOngoing_internal;
		float jumpAcc;
		float jumpAcc_internal;
		float jumpLock;
		float jumpLock_internal;

		float fallOngoing;
		float fallAcc;
		float fallAcc_internal;

		float skillOngoing;
		float skillAcc;
		float skillAcc_internal;

		//std::vector<cRigidBody*> mRigidBodies;//not use right now
		//std::vector<bulletRigidBody*> mRigidBodies;
		std::vector<iCollisionBody*> mCollisionBodies;
		std::vector<iShape> shapes;
		btBroadphaseInterface* broadphase;

		btDefaultCollisionConfiguration* collisionConfiguration;
		btCollisionDispatcher* dispatcher;

		btSequentialImpulseConstraintSolver* solver;

		btDiscreteDynamicsWorld* dynamicsWorld;
		OiJE::GelKinematicCharacterController* characterController;

		std::function< void (iGameObject*, iGameObject*) > collisionListener;

		void ProcessCharacterInputs(float deltaTime);
	};
}