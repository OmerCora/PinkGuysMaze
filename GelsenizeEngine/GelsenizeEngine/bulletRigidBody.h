#pragma once

#include "externals.h"
#include "SimpleShapes.h"
#include "cRigidBodyDef.h"
#include "iRigidBody.h"

namespace nPhysics
{
	class bulletRigidBody : public iRigidBody
	{
	public:
		bulletRigidBody(const cRigidBodyDef& def, iShape* shape);
		virtual ~bulletRigidBody();
		
		virtual void ClearForces();

		virtual void Teleport(glm::vec3 position);

		virtual void GetTransform(glm::mat4& transformation);
		virtual void SetProperties(glm::vec3& position, glm::vec3& velocity, glm::vec3& acceleration, glm::quat& orientation, glm::vec3 angularVelocity);

		virtual void GetMass(float& mass);
		virtual void GetInverseMass(float& inverseMass);

		virtual void GetPreviousPosition(glm::vec3& position);
		virtual void SetPreviousPosition(glm::vec3& position);

		virtual void GetPosition(glm::vec3& position);
		virtual void GetVelocity(glm::vec3& velocity);
		virtual void GetAcceleration(glm::vec3& acceleration);
		virtual void GetOrientation(glm::quat& orientation);
		virtual void GetAngularVelocity(glm::vec3& angularVelocity);
		virtual bool IsStatic(void);
		virtual eShapeType GetShapeType(void);
		virtual iShape* GetShape(void);
		virtual btRigidBody* GetBulletBody(void);
		//virtual void GetBody(btRigidBody* rigidBodyOut);
		virtual int GetOwnerID(void);
		virtual iGameObject* GetOwnerObject(void);

		virtual void GetSpecialScale(glm::vec3& specialScale);

		//new methods
		virtual void ApplyAngularImpulse(const glm::vec3& impulse, const glm::vec3& worldPoint);
		virtual void ApplyImpulseAtPoint(const glm::vec3& impulse, const glm::vec3& worldPoint);
		virtual void GetVelocityAtPoint(const glm::vec3& worldPoint, glm::vec3& velocityOut);
	protected:
		iShape* mShape;

		btRigidBody* rigidBody;
		iGameObject* ownerObject;
		int ownerID;

		/*float mMass;

		glm::vec3 mPreviousPosition;
		glm::vec3 mPosition;
		glm::vec3 mVelocity;
		glm::vec3 mAcceleration;

		glm::vec3 mAngle;
		glm::vec3 mAngularVelocity;
		glm::quat mOrientation;*/
		//glm::vec3 mTorque;

		//bool mStatic;
	};
}