#include "bulletRigidBody.h"

glm::vec3 getVecGLM(const btVector3& vec)
{
	glm::vec3 vecout = glm::vec3((float)vec.getX(), (float)vec.getY(), (float)vec.getZ());
	return vecout;
}

btVector3 getVecBT(glm::vec3 vec)
{
	btVector3 vecout(vec.x, vec.y, vec.z);
	return vecout;
}

nPhysics::bulletRigidBody::bulletRigidBody(const cRigidBodyDef & def, iShape * shape)
{
	this->mShape = shape;
	btCollisionShape *bulletShape = this->mShape->GetShape();

	eShapeType type = shape->GetShapeType();
	if (type == SPHERE_SHAPE)
	{
		btDefaultMotionState* fallMotionState =
			new btDefaultMotionState(btTransform(btQuaternion(def.Orientation.x, def.Orientation.y, def.Orientation.z, def.Orientation.w), btVector3(def.Position.x, def.Position.y, def.Position.z)));
		btScalar mass = def.Mass;
		btVector3 fallInertia(0, 0, 0);
		bulletShape->calculateLocalInertia(mass, fallInertia);
		btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass, fallMotionState, bulletShape, fallInertia);
		fallRigidBodyCI.m_friction = def.friction;
		fallRigidBodyCI.m_restitution = def.restitution;
		fallRigidBodyCI.m_linearDamping = def.linearDamping;
		fallRigidBodyCI.m_angularDamping = def.angularDamping;
		this->rigidBody = new btRigidBody(fallRigidBodyCI);
		this->rigidBody->setUserPointer(def.ownerObject);
	}
	else if (type == PLANE_SHAPE)
	{
		btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(def.Orientation.x, def.Orientation.y, def.Orientation.z, def.Orientation.w), btVector3(def.Position.x, def.Position.y, def.Position.z)));
		btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, groundMotionState, bulletShape, btVector3(0, 0, 0));
		groundRigidBodyCI.m_friction = def.friction;
		groundRigidBodyCI.m_restitution = def.restitution;
		this->rigidBody = new btRigidBody(groundRigidBodyCI);
		this->rigidBody->setUserPointer(def.ownerObject);
	}
	else if (type == BOX_SHAPE)
	{
		//The ground is not dynamic, we set its mass to 0
		btScalar mass_(def.Mass);
/*
		btTransform groundTransform;
		groundTransform.setIdentity();
		groundTransform.setRotation(btQuaternion(def.Orientation.x, def.Orientation.y, def.Orientation.z, def.Orientation.w));
		groundTransform.setOrigin(btVector3(def.Position.x, def.Position.y, def.Position.z));
*/
		btVector3 fallInertia(0, 0, 0);

		//No need for calculating the inertia on a static object
		//btVector3 localInertia(0, 0, 0);
		//using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
		//btDefaultMotionState* groundMotionState = new btDefaultMotionState(groundTransform);
		//btRigidBody::btRigidBodyConstructionInfo rbInfo(mass_, groundMotionState, groundShape, fallInertia)

		//btRigidBody::btRigidBodyConstructionInfo info(mass_, groundMotionState, bulletShape, fallInertia);

		btDefaultMotionState* fallMotionState =
			new btDefaultMotionState(btTransform(btQuaternion(def.Orientation.x, def.Orientation.y, def.Orientation.z, def.Orientation.w), btVector3(def.Position.x, def.Position.y, def.Position.z)));
		btScalar mass = def.Mass;
		//this->rigidBody = new btRigidBody(info);

		bulletShape->calculateLocalInertia(mass, fallInertia);
		btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass, fallMotionState, bulletShape, fallInertia);
		fallRigidBodyCI.m_friction = def.friction;
		fallRigidBodyCI.m_restitution = def.restitution;
		fallRigidBodyCI.m_linearDamping = def.linearDamping;
		fallRigidBodyCI.m_angularDamping = def.angularDamping;
		this->rigidBody = new btRigidBody(fallRigidBodyCI);
		this->rigidBody->setLinearVelocity(btVector3(def.Velocity.x, def.Velocity.y, def.Velocity.z));
		this->rigidBody->setUserPointer(def.ownerObject);
	}
	else if (type == CYLINDER_SHAPE)
	{
		btDefaultMotionState* fallMotionState =
			new btDefaultMotionState(btTransform(btQuaternion(def.Orientation.x, def.Orientation.y, def.Orientation.z, def.Orientation.w), btVector3(def.Position.x, def.Position.y, def.Position.z)));
		btScalar mass = def.Mass;
		btVector3 fallInertia(0, 0, 0);
		bulletShape->calculateLocalInertia(mass, fallInertia);
		btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass, fallMotionState, bulletShape, fallInertia);
		fallRigidBodyCI.m_friction = def.friction;
		fallRigidBodyCI.m_restitution = def.restitution;
		fallRigidBodyCI.m_linearDamping = def.linearDamping;
		fallRigidBodyCI.m_angularDamping = def.angularDamping;
		this->rigidBody = new btRigidBody(fallRigidBodyCI);
		this->rigidBody->setLinearVelocity(btVector3(def.Velocity.x, def.Velocity.y, def.Velocity.z));
		this->rigidBody->setUserPointer(def.ownerObject);
	}
	else if (type == CAPSULE_SHAPE)
	{
		btDefaultMotionState* fallMotionState =
			new btDefaultMotionState(btTransform(btQuaternion(def.Orientation.x, def.Orientation.y, def.Orientation.z, def.Orientation.w), btVector3(def.Position.x, def.Position.y, def.Position.z)));
		btScalar mass = def.Mass;
		btVector3 fallInertia(0, 0, 0);
		bulletShape->calculateLocalInertia(mass, fallInertia);
		btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass, fallMotionState, bulletShape, fallInertia);
		fallRigidBodyCI.m_friction = def.friction;
		fallRigidBodyCI.m_restitution = def.restitution;
		fallRigidBodyCI.m_linearDamping = def.linearDamping;
		fallRigidBodyCI.m_angularDamping = def.angularDamping;
		this->rigidBody = new btRigidBody(fallRigidBodyCI);
		this->rigidBody->setLinearVelocity(btVector3(def.Velocity.x, def.Velocity.y, def.Velocity.z));
		this->rigidBody->setUserPointer(def.ownerObject);
	}
	

	

	this->ownerID = def.ownerID;
}


nPhysics::bulletRigidBody::~bulletRigidBody()
{
}

void nPhysics::bulletRigidBody::ClearForces()
{
	//btTransform btt;
	//Globals::phyWorld->removeRigidBody(mToolBody);
	//mToolBody->setMassProps(0, btVector3(0, 0, 0));
	//mToolBody->setCollisionFlags(mToolBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
	//btt.setIdentity();
	//btt.setOrigin(BtOgre::Convert::toBullet(cPos));
	//btt.setRotation(BtOgre::Convert::toBullet(Ori));
	//mToolBody->getMotionState()->setWorldTransform(btt);
	//mToolBody->setActivationState(DISABLE_DEACTIVATION);
	//Globals::phyWorld->addRigidBody(mToolBody);
	this->rigidBody->clearForces();
}

void nPhysics::bulletRigidBody::Teleport(glm::vec3 position)
{
	this->rigidBody->activate(true);
	this->rigidBody->translate(getVecBT(position));
}

void nPhysics::bulletRigidBody::GetTransform(glm::mat4 & transformation)
{
}

void nPhysics::bulletRigidBody::SetProperties(glm::vec3 & position, glm::vec3 & velocity, glm::vec3 & acceleration, glm::quat & orientation, glm::vec3 angularVelocity)
{
	//ball->clearForces();
	//btVector3 zeroVector(0, 0, 0);
	//ball->setLinearVelocity(zeroVector);
	//ball->setAngularVeclocity(zeroVector);
	//ball->setWorldTransform(startingTransform); // reset ball position

	//this->rigidBody->clearForces();

	//this->rigidBody->setAngularVelocity(btVector3((float)angularVelocity.x, (float)angularVelocity.y, (float)angularVelocity.z));
	//this->rigidBody->setLinearVelocity(btVector3((float)velocity.x, (float)velocity.y, (float)velocity.z));

	//btTransform trans = this->rigidBody->getWorldTransform();
	//btTransform trans;
	//this->rigidBody->getMotionState()->getWorldTransform(trans);
	
	//trans = this->rigidBody->getWorldTransform();
	//trans.setOrigin(btVector3((float)position.x, (float)position.y, (float)position.z));
	//trans.setRotation(btQuaternion((float)orientation.x, (float)orientation.y, (float)orientation.z, (float)orientation.w));
	//this->rigidBody->getMotionState()->setWorldTransform(trans);
	//this->rigidBody->setWorldTransform(trans);
	//this->rigidBody->setCenterOfMassTransform(trans);
}

void nPhysics::bulletRigidBody::GetMass(float & mass)
{
	mass = 1.0f / (float)this->rigidBody->getInvMass();
}

void nPhysics::bulletRigidBody::GetInverseMass(float & inverseMass)
{
	inverseMass = (float)this->rigidBody->getInvMass();
}

void nPhysics::bulletRigidBody::GetPreviousPosition(glm::vec3 & position)
{
}

void nPhysics::bulletRigidBody::SetPreviousPosition(glm::vec3 & position)
{
}

void nPhysics::bulletRigidBody::GetPosition(glm::vec3 & position)
{
	this->rigidBody->getWorldTransform();
}

void nPhysics::bulletRigidBody::GetVelocity(glm::vec3 & velocity)
{
	velocity = getVecGLM(this->rigidBody->getLinearVelocity());
}

void nPhysics::bulletRigidBody::GetAcceleration(glm::vec3 & acceleration)
{
	acceleration = getVecGLM(this->rigidBody->getLinearFactor());
}

void nPhysics::bulletRigidBody::GetOrientation(glm::quat & orientation)
{
	btQuaternion quat = this->rigidBody->getOrientation();
	orientation.x = quat.getX();
	orientation.y = quat.getY();
	orientation.z = quat.getZ();
	orientation.w = quat.getW();
}

void nPhysics::bulletRigidBody::GetAngularVelocity(glm::vec3 & angularVelocity)
{
	angularVelocity = getVecGLM(this->rigidBody->getAngularVelocity());
}

bool nPhysics::bulletRigidBody::IsStatic(void)
{
	return this->rigidBody->isStaticObject();
}

nPhysics::eShapeType nPhysics::bulletRigidBody::GetShapeType(void)
{
	btCollisionShape *shape = this->rigidBody->getCollisionShape();
	
	return eShapeType();
}

nPhysics::iShape * nPhysics::bulletRigidBody::GetShape(void)
{
	return nullptr;
}

btRigidBody* nPhysics::bulletRigidBody::GetBulletBody(void)
{
	return this->rigidBody;
}

int nPhysics::bulletRigidBody::GetOwnerID(void)
{
	return this->ownerID;
}

iGameObject * nPhysics::bulletRigidBody::GetOwnerObject(void)
{
	return this->ownerObject;
}

void nPhysics::bulletRigidBody::GetSpecialScale(glm::vec3 & specialScale)
{
	if (mShape->GetShapeType() == eShapeType::BOX_SHAPE)
	{
		btBoxShape *box = (btBoxShape*)mShape->GetShape();
		btVector3 scale = box->getHalfExtentsWithMargin();
		specialScale.x = scale.getX() * 2.0f;
		specialScale.y = scale.getY() * 2.0f;
		specialScale.z = scale.getZ() * 2.0f;
	}
	else if (mShape->GetShapeType() == eShapeType::CAPSULE_SHAPE)
	{
		btCapsuleShape *capsule = (btCapsuleShape*)mShape->GetShape();
		float radius = capsule->getRadius();
		float height = capsule->getHalfHeight();
		specialScale.x = radius * 2.0f;
		specialScale.y = height * 2.0f;
		specialScale.z = radius * 2.0f;
	}
	else if (mShape->GetShapeType() == eShapeType::CYLINDER_SHAPE)
	{
		btCylinderShape *cylinder = (btCylinderShape*)mShape->GetShape();
		btVector3 scale = cylinder->getHalfExtentsWithMargin();
		float radius = cylinder->getRadius();
		specialScale.x = radius * 2.0f;
		specialScale.y = scale.getY() * 2.0f;
		specialScale.z = radius * 2.0f;
	}
}

void nPhysics::bulletRigidBody::ApplyAngularImpulse(const glm::vec3 & impulse, const glm::vec3 & worldPoint)
{
	//btVector3 btPos(this->rigidBody->getWorldTransform().getOrigin());
	//glm::vec3 position(btPos.getX(), btPos.getY(), btPos.getZ());
	//glm::vec3 angularStep = glm::cross(worldPoint - position, impulse * this->rigidBody->getInvMass());
	//btVector3 angularVelocity = this->rigidBody->getAngularVelocity() + btVector3(angularStep.x, angularStep.y, angularStep.z);
	//this->rigidBody->setAngularVelocity(angularVelocity);
	this->rigidBody->activate(true);
	btVector3 btimp(impulse.x, impulse.y, impulse.z);
	this->rigidBody->applyCentralImpulse(btimp);
}

void nPhysics::bulletRigidBody::ApplyImpulseAtPoint(const glm::vec3 & impulse, const glm::vec3 & worldPoint)
{
	this->rigidBody->activate(true);
	//this->rigidBody->applyImpulse(btVector3(impulse.x, impulse.y, impulse.z), btVector3(worldPoint.x, worldPoint.y, worldPoint.z));
	//this->rigidBody->applyCentralImpulse(btVector3(impulse.x, impulse.y, impulse.z));
	//this->rigidBody->applyImpulse(btVector3(impulse.x, impulse.y, impulse.z), btVector3(worldPoint.x, worldPoint.y, worldPoint.z));
	this->rigidBody->applyCentralImpulse(btVector3(impulse.x, impulse.y, impulse.z));
}

void nPhysics::bulletRigidBody::GetVelocityAtPoint(const glm::vec3 & worldPoint, glm::vec3 & velocityOut)
{
	btVector3 velOut = this->rigidBody->getVelocityInLocalPoint(btVector3(worldPoint.x, worldPoint.y, worldPoint.z));
	velocityOut = glm::vec3(velOut.getX(), velOut.getY(), velOut.getZ());
}

