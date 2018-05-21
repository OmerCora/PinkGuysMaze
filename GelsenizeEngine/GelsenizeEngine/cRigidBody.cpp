#include "cRigidBody.h"

nPhysics::cRigidBody::cRigidBody(const cRigidBodyDef & def, iShape * shape)
{
	this->mShape = shape;
	this->mPosition = def.Position;
	this->mVelocity = def.Velocity;
	this->mAngle = def.Angle;
	this->mStatic = def.Static;
	this->mMass = def.Mass;
	this->mInverseMass = 1.0f / def.Mass;

	this->ownerID = def.ownerID;
}

nPhysics::cRigidBody::~cRigidBody()
{
	//delete this->mShape;
}

void nPhysics::cRigidBody::ClearForces()
{
}

void nPhysics::cRigidBody::Teleport(glm::vec3 position)
{
	this->mPosition = position;
}

void nPhysics::cRigidBody::GetTransform(glm::mat4& transformation)
{
	glm::mat4x4 m(0.0f);
	m = glm::translate(m, glm::vec3(mPosition.x, mPosition.y, mPosition.z));

	m              = glm::rotate(m, mPosition.x, glm::vec3(1.0f, 0.0f, 0.0f));
	m			   = glm::rotate(m, mPosition.y, glm::vec3(0.0f, 1.0f, 0.0f));
	transformation = glm::rotate(m, mPosition.z, glm::vec3(0.0f, 0.0f, 1.0f));
}

void nPhysics::cRigidBody::SetProperties(glm::vec3 & position, glm::vec3 & velocity, glm::vec3 & acceleration, glm::quat & orientation, glm::vec3 angularVelocity)
{
	this->mPosition = position;
	this->mVelocity = velocity;
	this->mAcceleration = acceleration;
	this->mAngularVelocity = angularVelocity;
	this->mOrientation = orientation;
}

void nPhysics::cRigidBody::GetMass(float & mass)
{
	mass = this->mMass;
}

void nPhysics::cRigidBody::GetInverseMass(float & inverseMass)
{
	inverseMass = this->mInverseMass;
}

void nPhysics::cRigidBody::GetPreviousPosition(glm::vec3 & position)
{
	position.x = this->mPreviousPosition.x;
	position.y = this->mPreviousPosition.y;
	position.z = this->mPreviousPosition.z;
}

void nPhysics::cRigidBody::SetPreviousPosition(glm::vec3 & position)
{
	this->mPreviousPosition.x = position.x;
	this->mPreviousPosition.y = position.y;
	this->mPreviousPosition.z = position.z;
}

void nPhysics::cRigidBody::GetPosition(glm::vec3 & position)
{
	position.x = this->mPosition.x;
	position.y = this->mPosition.y;
	position.z = this->mPosition.z;
}

void nPhysics::cRigidBody::GetVelocity(glm::vec3 & velocity)
{
	velocity.x = this->mVelocity.x;
	velocity.y = this->mVelocity.y;
	velocity.z = this->mVelocity.z;
}

void nPhysics::cRigidBody::GetAcceleration(glm::vec3 & acceleration)
{
	acceleration.x = this->mAcceleration.x;
	acceleration.y = this->mAcceleration.y;
	acceleration.z = this->mAcceleration.z;
}

void nPhysics::cRigidBody::GetOrientation(glm::quat & orientation)
{
	orientation = this->mOrientation;
}

void nPhysics::cRigidBody::GetAngularVelocity(glm::vec3 & angularVelocity)
{
	angularVelocity = this->mAngularVelocity;
}

bool nPhysics::cRigidBody::IsStatic(void)
{
	return mStatic;
}

nPhysics::eShapeType nPhysics::cRigidBody::GetShapeType(void)
{
	return this->mShape->GetShapeType();
}

nPhysics::iShape * nPhysics::cRigidBody::GetShape(void)
{
	return this->mShape;
}

int nPhysics::cRigidBody::GetOwnerID(void)
{
	return this->ownerID;
}

iGameObject * nPhysics::cRigidBody::GetOwnerObject(void)
{
	return nullptr;
}

void nPhysics::cRigidBody::GetSpecialScale(glm::vec3 & specialScale)
{
}

void nPhysics::cRigidBody::ApplyAngularImpulse(const glm::vec3 & impulse, const glm::vec3 & worldPoint)
{
	mAngularVelocity += glm::cross(worldPoint - mPosition, impulse * mInverseMass);
}

void nPhysics::cRigidBody::ApplyImpulseAtPoint(const glm::vec3 & impulse, const glm::vec3 & worldPoint)
{
	this->mVelocity += impulse * this->mInverseMass;
	this->mAngularVelocity += glm::cross(worldPoint - mPosition, impulse * mInverseMass);
}

void nPhysics::cRigidBody::GetVelocityAtPoint(const glm::vec3 & worldPoint, glm::vec3 & velocityOut)
{
	velocityOut = mVelocity + glm::cross(mAngularVelocity, worldPoint - mPosition);
}
