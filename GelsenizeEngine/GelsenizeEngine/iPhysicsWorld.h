#pragma once

#include "iRigidBody.h"
#include <functional>

#include <vector>

namespace nPhysics
{
	class iPhysicsWorld
	{
	public:
		virtual ~iPhysicsWorld() {}

		virtual bool RegisterCollisionListener(std::function< void(iGameObject*, iGameObject*) >) = 0;

		virtual bool AddPoint2PointConstraint(iCollisionBody * body1, glm::vec3 pivot) = 0;
		virtual bool AddPoint2PointConstraint(iCollisionBody * body1, iCollisionBody * body2, glm::vec3 pivot, glm::vec3 pivot2) = 0;
		virtual bool AddHingeConstraint(iCollisionBody * body1, glm::vec3 from) = 0;
		virtual bool AddSliderConstraint(iCollisionBody * body1, glm::vec3 from) = 0;
		virtual bool AddConeTwistConstraint(iCollisionBody * body1, glm::vec3 from) = 0;

		virtual bool SaveShape(iShape shapeToSave) = 0;
		virtual bool SaveShape(std::vector<iShape> &allShapes) = 0;

		virtual bool CreateCharacter(iGameObject* characterOwnerObject) = 0;
		virtual void GetCharacterBodyProperties(glm::vec3 & specialScale, eShapeType & shapeType) = 0;

		virtual void WarpVehicle(glm::vec3 position) = 0;
		virtual void GetWheelTransforms(std::vector<glm::vec3>&positions, std::vector<glm::quat>&orientations, std::vector<glm::mat4>&transformations) = 0;
		virtual void WarpCharacter(glm::vec3 position) = 0;

		virtual void MousePositionCallback(float xPosition, float yPosition) = 0;
		virtual void KeyboardCallback(int key, int action) = 0;

		//handled by objectmanager
		virtual bool AddBody(iCollisionBody* body) = 0;
		virtual bool RemoveBody(iCollisionBody* body) = 0;

		virtual void TimeStep(float dt) = 0;
	};
}