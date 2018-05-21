#pragma once
#include <functional>

#include "externals.h"
#include "iPhysicsWorld.h"
#include "cRigidBody.h"
#include "cContact.h"
#include <vector>

namespace nPhysics
{
	class cPhysicsWorld : public iPhysicsWorld
	{
	public:
		cPhysicsWorld();
		~cPhysicsWorld();

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
		virtual void GetWheelTransforms(std::vector<glm::vec3>&positions, std::vector<glm::quat>&orientations, std::vector<glm::mat4>&transformations);
		virtual void WarpCharacter(glm::vec3 position);

		virtual void MousePositionCallback(float xPosition, float yPosition);
		virtual void KeyboardCallback(int key, int action);

		virtual bool AddBody(iCollisionBody* body);
		virtual bool RemoveBody(iCollisionBody* body);

		virtual void TimeStep(float dt);
	private:
		std::vector<cContact> contacts;
		float totalTime;

		//not used 
		std::vector<cRigidBody*> mRigidBodies;//not use right now
	};
}