#pragma once
#include <glm\fwd.hpp>
#include <glm\glm.hpp>
#include "iCollisionBody.h"
#include "iShape.h"

#include <btBulletDynamicsCommon.h>

class iGameObject;

namespace nPhysics
{
	class iRigidBody : public iCollisionBody
	{
	public:
		virtual ~iRigidBody() {}

		virtual void ClearForces() = 0;

		virtual void Teleport(glm::vec3 position) = 0;

		virtual void GetTransform(glm::mat4& transformOut) = 0;
		virtual void SetProperties(glm::vec3& position, glm::vec3& velocity, glm::vec3& acceleration, glm::quat& orientation, glm::vec3 angularVelocity) = 0;

		virtual void GetMass(float& mass) = 0;

		virtual void GetPreviousPosition(glm::vec3& position) = 0;
		virtual void SetPreviousPosition(glm::vec3& position) = 0;

		virtual void GetPosition(glm::vec3& position) = 0;
		virtual void GetVelocity(glm::vec3& velocity) = 0;
		virtual void GetAcceleration(glm::vec3& acceleration) = 0;
		virtual void GetOrientation(glm::quat& orientation) = 0;
		virtual void GetAngularVelocity(glm::vec3& angularVelocity) = 0;
		virtual bool IsStatic(void) = 0;
		virtual eShapeType GetShapeType(void) = 0;

		virtual iShape* GetShape(void) = 0;
		virtual btRigidBody* GetBulletBody(void) { return 0; }
		virtual int GetOwnerID(void) = 0;
		virtual iGameObject* GetOwnerObject(void) = 0;

		/*void cRigidBody::mGetVelocityAtPoint(glm::vec3 worldPoint, glm::vec3& velOut)

		{

			velOut = this->mLinVel + glm::cross(this->mAngVel, worldPoint - this->mPosition);

		}



		void cRigidBody::mApplyAngularImpulse(glm::vec3 impulse, glm::vec3& worldPoint)

		{

			this->mAngVel = cross(worldPoint - this->mPosition, impulse * this->mMassInv);

		}*/

		//new methods
		virtual void GetInverseMass(float& inverseMass) = 0;
		virtual void ApplyAngularImpulse(const glm::vec3& impulse, const glm::vec3& worldPoint) = 0;
		virtual void ApplyImpulseAtPoint(const glm::vec3& impulse, const glm::vec3& worldPoint) = 0;
		virtual void GetVelocityAtPoint(const glm::vec3& worldPoint, glm::vec3& velocityOut) = 0;
	protected:
		iRigidBody() : iCollisionBody(RIGID_BODY_TYPE) {}
		iRigidBody(const iRigidBody& other) : iCollisionBody(other) {}
		iRigidBody& operator=(const iRigidBody& other) { return *this; }
	};

	class iSoftBody : public iCollisionBody
	{
	public:
		// virtual glm::mat4 GetTransform() = 0;

		virtual glm::vec3 GetNodePosition(int idxNode) = 0;
		virtual int NumberOfNodes() = 0;

		virtual void ApplyForce(const glm::vec3& impulse) = 0;
	protected:
		iSoftBody() : iCollisionBody(SOFT_BODY_TYPE) {}
		iSoftBody(const iSoftBody& other) : iCollisionBody(other) {}
		iSoftBody& operator=(const iSoftBody& other) { return *this; }
	private:

		// ctors are private

	};
}