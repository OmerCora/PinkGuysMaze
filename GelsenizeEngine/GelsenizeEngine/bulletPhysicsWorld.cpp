#include "bulletPhysicsWorld.h"
#include "bulletVehicleRaycaster.h"

#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include "gelObjectManager.h"
#include "GelPhysicsManager.h"
#include "gelCameraManager.h"

#include "GelsenizeCore.h"
#include "externals.h"
#include "gelObjectFactory.h"

#include "GelMazeWorldManager.h"
#include "gelSoundManager.h"

nPhysics::bulletPhysicsWorld::bulletPhysicsWorld()
{
	btVector3 worldMin(-600, -600, -600);
	btVector3 worldMax(600, 600, 600);
	//broadphase = new btAxisSweep3(worldMin, worldMax);

	broadphase = new btDbvtBroadphase();

	btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
	btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);

	btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;

	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);

	dynamicsWorld->setGravity(btVector3(0, -10.0f, 0));

	dynamicsWorld->getDispatchInfo().m_allowedCcdPenetration = 0.0001f;
	dynamicsWorld->getSolverInfo().m_splitImpulse = true;//
	dynamicsWorld->getSolverInfo().m_numIterations = 20;
	dynamicsWorld->getDispatchInfo().m_useContinuous = true;
	dynamicsWorld->getSolverInfo().m_erp2 = 0.9;

	CreateVehicle();
	drivingVehicle = false;
}

nPhysics::bulletPhysicsWorld::~bulletPhysicsWorld()
{
}

bool nPhysics::bulletPhysicsWorld::RegisterCollisionListener(std::function<void(iGameObject*, iGameObject*)> listener)
{
	this->collisionListener = listener;
	return true;
}

bool nPhysics::bulletPhysicsWorld::AddBody(iCollisionBody * body)
{
	if (body == 0) return false;
	bulletRigidBody *rb = dynamic_cast<bulletRigidBody*>(body);
	if (rb == 0) return false;
	
	btRigidBody *rigidBody = rb->GetBulletBody();

	dynamicsWorld->addRigidBody(rigidBody);
}

bool nPhysics::bulletPhysicsWorld::RemoveBody(iCollisionBody * body)
{
	if (body == 0) return false;
	bulletRigidBody *rb = dynamic_cast<bulletRigidBody*>(body);
	if (rb == 0) return false;

	btRigidBody *rigidBody = rb->GetBulletBody();
	dynamicsWorld->removeRigidBody(rigidBody);
	return false;
}

bool nPhysics::bulletPhysicsWorld::AddSliderConstraint(iCollisionBody * body1, glm::vec3 from)
{
	btTransform transform(btTransform::getIdentity());
	transform.setOrigin(btVector3(from.x, from.y, from.z));
	
	btRigidBody *b1 = dynamic_cast<bulletRigidBody*>(body1)->GetBulletBody();
	
	//btRigidBody *b2 = dynamic_cast<bulletRigidBody*>(body2)->GetBulletBody();
	
	btSliderConstraint* bt_constraint = new btSliderConstraint(*b1, transform, true);

	bt_constraint->setLowerLinLimit(-10.0f);
	bt_constraint->setUpperLinLimit(10.0f);

	//this enabled me to spring back from the limit
	bt_constraint->setTargetLinMotorVelocity(10.0f);
	bt_constraint->setDampingLimLin(0.8f);

	bt_constraint->setPoweredLinMotor(true);

	bt_constraint->setLowerAngLimit(0.0f);
	bt_constraint->setUpperAngLimit(0.0f);

	dynamicsWorld->addConstraint(bt_constraint, false);

	characterShapeSpecialScale = glm::vec3(0.8f, 1.85f, 0.6f);
	characterShapeType = BOX_SHAPE;

	return true;
}

bool nPhysics::bulletPhysicsWorld::AddConeTwistConstraint(iCollisionBody * body1, glm::vec3 from)
{
	btTransform transform(btTransform::getIdentity());
	transform.setOrigin(btVector3(from.x, from.y, from.z));

	btRigidBody *b1 = dynamic_cast<bulletRigidBody*>(body1)->GetBulletBody();

	//btRigidBody *b2 = dynamic_cast<bulletRigidBody*>(body2)->GetBulletBody();

	btConeTwistConstraint* bt_constraint = new btConeTwistConstraint(*b1, transform);

	bt_constraint->setLimit(1.0f, 1.0f, 1.0f, 1.0f, 0.3f, 1.0f);

	dynamicsWorld->addConstraint(bt_constraint, false);

	return true;
}

bool nPhysics::bulletPhysicsWorld::SaveShape(iShape shapeToSave)
{
	for (int i = 0; i != this->shapes.size(); i++)
	{

	}
	
	return true;
}

bool nPhysics::bulletPhysicsWorld::SaveShape(std::vector<iShape>& allShapes)
{
	return false;
}

//---------------------------------------------------------------------------------------
enum EPhysicsCollisionMask {

	E_Static = 1 << 0,
	E_Rigid = 1 << 1,
	E_Actor = 1 << 2,
	E_Trigger = 1 << 3,

	E_StaticGroup =  E_Rigid  | E_Actor,
	E_ActorGroup =   E_Static | E_Rigid | E_Actor | E_Trigger,
	E_RigidGroup =   E_Static | E_Rigid | E_Actor | E_Trigger,
	E_TriggerGroup = E_Rigid  | E_Actor
};

bool nPhysics::bulletPhysicsWorld::CreateCharacter(iGameObject *characterOwnerObject)
{
	//------------------------------ ghost trigger ------------------------------
	ghostShape = new btBoxShape(btVector3(2, 1, 2));
	//shapes.push_back(ghostShape);

	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(btVector3(192.0f, 2.0f, 375.0f));

	ghostObject = new btPairCachingGhostObject();
	ghostObject->setWorldTransform(transform);

	ghostPairCallback = new btGhostPairCallback();
	broadphase->getOverlappingPairCache()->setInternalGhostPairCallback(ghostPairCallback);

	ghostObject->setCollisionShape(ghostShape);
	ghostObject->setCollisionFlags(ghostObject->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);

	//------------------------------ rigid box ------------------------------
	characterShape = new btBoxShape(btVector3(characterShapeSpecialScale.x, characterShapeSpecialScale.y, characterShapeSpecialScale.z));
	//characterShape = new btCylinderShape(btVector3(characterShapeSpecialScale.x, characterShapeSpecialScale.y, characterShapeSpecialScale.z));
	//shapes.push_back(fallShape);

	btDefaultMotionState* fallMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(5, 5, 0)));
	btVector3 fallInertia(0, 0, 0);
	characterShape->calculateLocalInertia(1.0f, fallInertia);

	btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(1.0f, fallMotionState, characterShape, fallInertia); // może być wspólne
	characterRigidBody = new btRigidBody(fallRigidBodyCI);
	//characterRigidBody->setUserPointer(NULL);

	//------------------------------ character ------------------------------
	btTransform startTransform;
	startTransform.setIdentity();
	startTransform.setOrigin(btVector3(0, 10, -7)); // check
	
	actorGhost = new btPairCachingGhostObject();
	actorGhost->setUserPointer(characterOwnerObject);
	actorGhost->setWorldTransform(startTransform);

	btGhostPairCallback* actorGhostPairCallback = new btGhostPairCallback();
	broadphase->getOverlappingPairCache()->setInternalGhostPairCallback(actorGhostPairCallback);

	actorGhost->setCollisionShape(characterShape);
	actorGhost->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);

	OiJE::GelKinematicCharacterController* character = new OiJE::GelKinematicCharacterController(actorGhost, static_cast<btConvexShape*>(characterShape), 0.5f);
	
	//------------------------------ add actor to the world ------------------------------
	dynamicsWorld->addCollisionObject(actorGhost, E_Actor, E_Static | E_Rigid | E_Actor | E_Trigger);
	dynamicsWorld->addAction(character);

	//------------------------------ add rigid to the world ------------------------------
	dynamicsWorld->addRigidBody(characterRigidBody, E_Rigid, E_Static | E_Rigid | E_Actor | E_Trigger);

	//------------------------------ add ghost to the world ------------------------------
	dynamicsWorld->addCollisionObject(ghostObject, E_Trigger, E_Rigid | E_Actor);


	//Character Controller Properties

	btVector3 gravity(0.0, 10.0f, 0.0f);// dynamicsWorld->getGravity() * -1.0f;
	btScalar maxSlope = 1.5f;
	btScalar fallSpeed = 40.0f; //5.0f;
	btScalar jumpSpeed = 10.0f;
	btScalar maxJump = 2.0f;

	this->characterController = character;
	this->characterController->setGravity(gravity.getY());
	this->characterController->setMaxSlope(maxSlope);
	this->characterController->setFallSpeed(fallSpeed);
	this->characterController->setJumpSpeed(jumpSpeed);
	this->characterController->setMaxJumpHeight(maxJump);

	this->characterController->characterOwner = characterOwnerObject;

	jumpControlLock = false;
	jumpControlLock_internal = 1.6f;
	jumpControlLockAcc = 0.0f;

	jumpOngoing = false;
	jumpOngoing_internal = false;
	jumpAcc = 0.0f;
	jumpAcc_internal = 0.0f;
	jumpLock = 0.80f;
	jumpLock_internal = 0.08f;

	fallOngoing = false;
	fallAcc = 0.0f;
	fallAcc_internal = 0.15f;

	skillOngoing = false;
	skillAcc = 0.0f;
	skillAcc_internal = 1.0f;

	return true;
}

void nPhysics::bulletPhysicsWorld::GetCharacterBodyProperties(glm::vec3 & specialScale, eShapeType & shapeType)
{
	specialScale = characterShapeSpecialScale * 2.0f;
	shapeType = characterShapeType;
	return;
}

void nPhysics::bulletPhysicsWorld::WarpCharacter(glm::vec3 position)
{
	this->characterController->warp(btVector3(position.x, position.y, position.z));
}

void nPhysics::bulletPhysicsWorld::WarpVehicle(glm::vec3 position)
{
	btTransform newTrans;
	newTrans.setIdentity();
	newTrans.setOrigin(btVector3(position.x, position.y, position.z));
	this->vehicle->getRigidBody()->setWorldTransform(newTrans);
}


glm::mat4 bulletToGlm(const btTransform& t)
{
	glm::mat4 m(1.0f);
	const btMatrix3x3& basis = t.getBasis();
	// rotation
	for (int r = 0; r < 3; r++)
	{
		for (int c = 0; c < 3; c++)
		{
			m[c][r] = basis[r][c];
		}
	}
	// traslation
	btVector3 origin = t.getOrigin();
	m[3][0] = origin.getX();
	m[3][1] = origin.getY();
	m[3][2] = origin.getZ();
	// unit scale
	m[0][3] = 0.0f;
	m[1][3] = 0.0f;
	m[2][3] = 0.0f;
	m[3][3] = 1.0f;
	return m;
}

void nPhysics::bulletPhysicsWorld::GetWheelTransforms(std::vector<glm::vec3>& positions, std::vector<glm::quat>& orientations, std::vector<glm::mat4>&transformations)
{
	positions.resize(vehicle->getNumWheels());
	orientations.resize(vehicle->getNumWheels());
	transformations.resize(vehicle->getNumWheels());

	/*for (int i = 0; i < vehicle->getNumWheels(); i++)
	{
		this->vehicle->updateWheelTransform(i, false);
		btTransform transform = this->vehicle->getWheelTransformWS(i);
		glm::vec3 v3Origin = glm::vec3(transform.getOrigin().getX(), transform.getOrigin().getY(), transform.getOrigin().getZ());
		glm::quat qRot = glm::quat(transform.getRotation().getX(), transform.getRotation().getY(), transform.getRotation().getZ(), transform.getRotation().getW());

		positions[i] =  v3Origin;

		btWheelInfo whInfo = this->vehicle->getWheelInfo(i);
		glm::vec3 v3PosLS = glm::vec3(whInfo.m_chassisConnectionPointCS.getX(), whInfo.m_chassisConnectionPointCS.getY(), whInfo.m_chassisConnectionPointCS.getZ());
		glm::quat qRotator = (v3PosLS.x >= 0.0 ? glm::quat(glm::vec3(0.0f, 0.0f, -1.57f)) : glm::quat(glm::vec3(0.0f, 0.0f, 1.57f)));
		orientations[i] = qRot * qRotator;
	}
	return;*/
	

	for (int i = 0; i < vehicle->getNumWheels(); i++)
	{
		//this->vehicle->updateWheelTransform(i);
		//btWheelInfo& wheel = vehicle->getWheelInfo(i);
		btTransform transform = vehicle->getWheelInfo(i).m_worldTransform;// getWheelTransformWS(i);

		btVector3 position = transform.getOrigin();
		btQuaternion orientation = transform.getRotation();

		positions[i] = glm::vec3(position.getX(), position.getY(), position.getZ());
		orientations[i] = glm::quat(orientation.getY(), orientation.getX(), orientation.getZ(), orientation.getW());

		//glm::quat orientTurn(glm::vec3(0.0f, 0.0f, 1.5708f));		// No rotational change
		//orientations[i] *= orientTurn;

		glm::mat4 trans = bulletToGlm(transform);
		transformations[i] = trans;

	}
}

bool nPhysics::bulletPhysicsWorld::AddHingeConstraint(iCollisionBody * body1, glm::vec3 from)
{
	btTransform transform(btTransform::getIdentity());
	transform.setOrigin(btVector3(from.x, from.y, from.z));

	btRigidBody *b1 = dynamic_cast<bulletRigidBody*>(body1)->GetBulletBody();

	//btRigidBody *b2 = dynamic_cast<bulletRigidBody*>(body2)->GetBulletBody();

	btHingeConstraint* bt_constraint = new btHingeConstraint(*b1, transform, true);

	bt_constraint->setLimit(-3.14f, 3.14f, 0.9f, 0.3f, 1.0f);

	dynamicsWorld->addConstraint(bt_constraint, false);

	return true;
}

bool nPhysics::bulletPhysicsWorld::AddPoint2PointConstraint(iCollisionBody * body1, iCollisionBody * body2, glm::vec3 pivot, glm::vec3 pivot2)
{
	btVector3 bulletPivot1(pivot.x, pivot.y, pivot.z);
	btVector3 bulletPivot2(pivot2.x, pivot2.y, pivot2.z);

	//btVector3 bulletPivot1(0.0, 0.0f, +0.3f);
	//btVector3 bulletPivot2(0.0, 0.0f, -0.3f);

	btRigidBody *b1 = dynamic_cast<bulletRigidBody*>(body1)->GetBulletBody();
	btRigidBody *b2 = dynamic_cast<bulletRigidBody*>(body2)->GetBulletBody();

	btPoint2PointConstraint* bt_constraint = new btPoint2PointConstraint(*b1, *b2, bulletPivot1, bulletPivot2);

	dynamicsWorld->addConstraint(bt_constraint, false);

	return true;
}

bool nPhysics::bulletPhysicsWorld::AddPoint2PointConstraint(iCollisionBody * body1, glm::vec3 pivot)
{
	btVector3 bulletPivot1(pivot.x, pivot.y, pivot.z);

	btRigidBody *b1 = dynamic_cast<bulletRigidBody*>(body1)->GetBulletBody();

	btPoint2PointConstraint* bt_constraint = new btPoint2PointConstraint(*b1, bulletPivot1);

	//bt_constraint->setBreakingImpulseThreshold(9999999.0f);

	dynamicsWorld->addConstraint(bt_constraint, false);

	return true;
}




void nPhysics::bulletPhysicsWorld::TimeStep(float dt)
{
	//this->characterController->preStep(dynamicsWorld);
	this->ProcessCharacterInputs(dt);

	//this->characterController->playerStep(dynamicsWorld, dt);
	this->characterController->updateAction(dynamicsWorld, dt);
	
	//this->vehicle->updateAction(dynamicsWorld, dt);

	dynamicsWorld->stepSimulation(1 / 60.f, 10);

	// --- check ghost collision
	CheckGhost(ghostObject);

	//http://stackoverflow.com/questions/11175694/bullet-physics-simplest-collision-example
	//collision
	int numManifolds = dynamicsWorld->getDispatcher()->getNumManifolds();
	for (int i = 0; i<numManifolds; i++)
	{
		btPersistentManifold* contactManifold = dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
		btCollisionObject* obA = (btCollisionObject*)(contactManifold->getBody0());
		btCollisionObject* obB = (btCollisionObject*)(contactManifold->getBody1());

		//get contacts
		int numContacts = contactManifold->getNumContacts();
		for (int j = 0; j<numContacts; j++)
		{
			btManifoldPoint& pt = contactManifold->getContactPoint(j);
			if (pt.getDistance()<0.f)
			{
				const btVector3& ptA = pt.getPositionWorldOnA();
				const btVector3& ptB = pt.getPositionWorldOnB();
				const btVector3& normalOnB = pt.m_normalWorldOnB;

				//get our custom pointer that references the owner game object
				iGameObject* objectA = (iGameObject*)obA->getUserPointer();
				iGameObject* objectB = (iGameObject*)obB->getUserPointer();

				if (objectA && objectB)
				{
					this->collisionListener(objectA, objectB);
				}
			}
		}
	}


	//update gameobjects except player
	std::vector<iGameObject*> gameObjects = gelObjectManager::sharedInstance().GetGameObjects();

	iGameObject* vehicleObject = gelObjectManager::sharedInstance().GetVehicle();

	for (int index = 0; index != gameObjects.size(); index++)
	{
		iGameObject *gameObject = gameObjects[index];
		if (gameObject == this->characterController->characterOwner || 
			gameObject == vehicleObject)
		{
			continue; //skip player and vehicle, it is updated by character controller;
		}

		PhysicsInfo objectPhysics = gameObject->GetPhysics();
		RenderInfo objectRender = gameObject->GetRender();

		if (!objectPhysics.enabled)
		{
			continue;
		}

		iCollisionBody* collisionBody = objectPhysics.collisionBody;

		if (!collisionBody)
		{
			continue;
		}

		eBodyType bodyType = collisionBody->GetBodyType();

		if (bodyType == eBodyType::RIGID_BODY_TYPE)
		{
			iRigidBody *rigidBody = dynamic_cast<iRigidBody*>(collisionBody);

			/*if (rigidBody->IsStatic())
			{
				continue;
			}*/

			btRigidBody *rb = rigidBody->GetBulletBody();

			btTransform trans;
			rb->getMotionState()->getWorldTransform(trans);
			objectRender.previousPosition = objectRender.position;
			objectRender.position.x = float(trans.getOrigin().getX());
			objectRender.position.y = float(trans.getOrigin().getY());
			objectRender.position.z = float(trans.getOrigin().getZ());
			
			if(gameObject->GetObjectName() == "mazeObject")
			{
				glm::vec3 playerPosition = this->characterController->characterOwner->GetRender().position;
				playerPosition.y = 0.0f;

				glm::vec3 objectPosition = objectRender.position;
				objectRender.position.y = 0.0f;

				

				float distance = glm::distance(playerPosition, objectPosition);

				float minDistance = 30.0;
				float maxDistance = 70.0f;

				float groundOffsetFull = -120.0f;

				float stepWeight = glm::smoothstep(minDistance, maxDistance, distance);
				objectRender.position.y = groundOffsetFull * stepWeight;

				if (stepWeight <= 0.9f)
				{
					GelMazeWorldManager::MazeNode node;
					GelMazeWorldManager::sharedInstance().GetNodeAt(objectPosition.x, objectPosition.z, node);
					if (node.type == GelMazeWorldManager::MazeNodeType::JUMP_TILE)
					{
						objectRender.position.y += 3.0f;
					}
					else if (node.type == GelMazeWorldManager::MazeNodeType::JUMP_TILE_LEVEL_2)
					{
						objectRender.position.y += 9.0f;
					}
					else if (node.type == GelMazeWorldManager::MazeNodeType::EMPTY_TILE_LEVEL_2)
					{
						objectRender.position.y += 6.0f;
					}
					else if (node.type == GelMazeWorldManager::MazeNodeType::EMPTY_TILE_LEVEL_3)
					{
						objectRender.position.y += 12.0f;
					}
				}

				//objectRender.alpha = 1.0f * (1.0f-stepWeight);

				rb->getWorldTransform().setOrigin(btVector3(objectRender.position.x, objectRender.position.y, objectRender.position.z));
			}


			btQuaternion quat = trans.getRotation();

			objectRender.orientation.x = (float)quat.getX();
			objectRender.orientation.y = (float)quat.getY();
			objectRender.orientation.z = (float)quat.getZ();
			objectRender.orientation.w = (float)quat.getW();

			btVector3 velocity = rb->getLinearVelocity();
			//btVector3 acceleration = rb->getLocalInertia();
			btVector3 angularVelocity = rb->getAngularVelocity();

			objectPhysics.velocity.x = (float)velocity.getX();
			objectPhysics.velocity.y = (float)velocity.getY();
			objectPhysics.velocity.z = (float)velocity.getZ();

			//objectPhysics.acceleration.x = (float)acceleration.getX();
			//objectPhysics.acceleration.y = (float)acceleration.getY();
			//objectPhysics.acceleration.z = (float)acceleration.getZ();

			objectPhysics.rotationalVelocity.x = (float)angularVelocity.getX();
			objectPhysics.rotationalVelocity.y = (float)angularVelocity.getY();
			objectPhysics.rotationalVelocity.z = (float)angularVelocity.getZ();

			gameObject->SetRender(objectRender);
			gameObject->SetPhysics(objectPhysics);

		}
		else if (bodyType == eBodyType::SOFT_BODY_TYPE)
		{

		}
		
	}

	///////////////////////
	///////////////////////

	//update player
	iGameObject *playerObject = this->characterController->characterOwner;
	RenderInfo  playerRender = playerObject->GetRender();
	PhysicsInfo	playerPhysics = playerObject->GetPhysics();

	btTransform trans = actorGhost->getWorldTransform();
	playerRender.previousPosition = playerRender.position;
	playerRender.position.x = float(trans.getOrigin().getX());
	playerRender.position.y = float(trans.getOrigin().getY());
	playerRender.position.z = float(trans.getOrigin().getZ());

	btQuaternion quat = trans.getRotation();

	playerRender.orientation.x = (float)quat.getX();
	playerRender.orientation.y = (float)quat.getY();
	playerRender.orientation.z = (float)quat.getZ();
	playerRender.orientation.w = (float)quat.getW();

	btVector3 velocity = characterRigidBody->getLinearVelocity();
	btVector3 angularVelocity = characterRigidBody->getAngularVelocity();

	playerPhysics.velocity.x = (float)velocity.getX();
	playerPhysics.velocity.y = (float)velocity.getY();
	playerPhysics.velocity.z = (float)velocity.getZ();

	playerPhysics.rotationalVelocity.x = (float)angularVelocity.getX();
	playerPhysics.rotationalVelocity.y = (float)angularVelocity.getY();
	playerPhysics.rotationalVelocity.z = (float)angularVelocity.getZ();

	playerObject->SetRender(playerRender);
	playerObject->SetPhysics(playerPhysics);


	//////////////////
	//////////////////

	//update vehicle
	RenderInfo  vehicleRender = vehicleObject->GetRender();
	PhysicsInfo	vehiclePhysics = vehicleObject->GetPhysics();

	trans = vehicle->getRigidBody()->getWorldTransform();
	vehicleRender.previousPosition = playerRender.position;
	vehicleRender.position.x = float(trans.getOrigin().getX());
	vehicleRender.position.y = float(trans.getOrigin().getY());
	vehicleRender.position.z = float(trans.getOrigin().getZ());

	quat = trans.getRotation();

	vehicleRender.orientation.x = (float)quat.getX();
	vehicleRender.orientation.y = (float)quat.getY();
	vehicleRender.orientation.z = (float)quat.getZ();
	vehicleRender.orientation.w = (float)quat.getW();

	velocity = vehicle->getRigidBody()->getLinearVelocity();
	angularVelocity = vehicle->getRigidBody()->getAngularVelocity();

	vehiclePhysics.velocity.x = (float)velocity.getX();
	vehiclePhysics.velocity.y = (float)velocity.getY();
	vehiclePhysics.velocity.z = (float)velocity.getZ();

	vehiclePhysics.rotationalVelocity.x = (float)angularVelocity.getX();
	vehiclePhysics.rotationalVelocity.y = (float)angularVelocity.getY();
	vehiclePhysics.rotationalVelocity.z = (float)angularVelocity.getZ();

	vehicleObject->SetRender(vehicleRender);
	vehicleObject->SetPhysics(vehiclePhysics);
}

//---------------------------------------------------------------------------------------
void bulletPhysicsWorld::CheckGhost(btPairCachingGhostObject* ghostObject) {

	btManifoldArray   manifoldArray;
	btBroadphasePairArray& pairArray = ghostObject->getOverlappingPairCache()->getOverlappingPairArray();
	int numPairs = pairArray.size();

	for (int i = 0; i<numPairs; i++)
	{
		manifoldArray.clear();

		const btBroadphasePair& pair = pairArray[i];

		//unless we manually perform collision detection on this pair, the contacts are in the dynamics world paircache:
		btBroadphasePair* collisionPair = dynamicsWorld->getPairCache()->findPair(pair.m_pProxy0, pair.m_pProxy1);
		if (!collisionPair)
			continue;
		if (collisionPair->m_algorithm)
			collisionPair->m_algorithm->getAllContactManifolds(manifoldArray);

		for (int j = 0; j<manifoldArray.size(); j++) {
			btPersistentManifold* manifold = manifoldArray[j];
			if (manifold->getNumContacts()) {
				iGameObject* objectInGhost= (iGameObject*)((manifold->getBody0() != ghostObject) ? (btCollisionObject*)(manifold->getBody0())->getUserPointer() :
					(btCollisionObject*)(manifold->getBody1())->getUserPointer());

				if (objectInGhost == this->actorGhost->getUserPointer())
				{
					//actor collided
					//trigger car event
					/*
					iGameObject* tile = gelObjectManager::sharedInstance().FindGameObjectWithObjectName("triggerTile");
					PhysicsInfo physics = tile->GetPhysics();
					GelPhysicsManager::sharedInstance().GetWorld()->RemoveBody(physics.collisionBody);

					RenderInfo render = tile->GetRender();
					render.position.y -= -500.0f;

					tile->SetRender(render);
					tile->SetPhysics(physics);*/

					iGameObject* player = gelObjectManager::sharedInstance().GetMainPlayer();
					RenderInfo playerrender = player->GetRender();
					playerrender.exploding = true;
					player->SetRender(playerrender);
					gelSoundManager::sharedInstance().PlaySoundWithName("explode");

					btTransform trans = this->ghostObject->getWorldTransform();
					trans.setOrigin(btVector3(0.0f, 100.0f, 0.0f));
					this->ghostObject->setWorldTransform(trans);

/*
					iGameObject* light0 = gelObjectManager::sharedInstance().FindGameObjectWithObjectName("LIGHT_0");
					LightInfo lightInfo0 = light0->GetLight();
					lightInfo0.bIsEnabled = false;

					iGameObject* light1 = gelObjectManager::sharedInstance().FindGameObjectWithObjectName("LIGHT_1");
					LightInfo lightInfo1 = light1->GetLight();
					lightInfo1.bIsEnabled = true;

					light0->SetLight(lightInfo0);
					light1->SetLight(lightInfo1);
*/
					//toggleDrivingMode();
				}

				//std::cout << manifold->getNumContacts() << ") : " << ((id == 1) ? "box" : "actor") << std::endl;
			}
		}
	}
}

void nPhysics::bulletPhysicsWorld::ProcessCharacterInputs(float deltaTime)
{
	if (skillOngoing)
	{
		skillAcc += deltaTime;
		if (skillAcc >= skillAcc_internal)
		{
			skillOngoing = false;
		}
		else
		{
			characterController->setWalkDirection(btVector3(0.0f, 0.0f, 0.0f));
			return;
		}
	}

	//Player Controls
	bool pressW = glfwGetKey(GelsenizeCore::sharedInstance().GetWindow(), GLFW_KEY_W) == GLFW_PRESS;
	bool pressS = glfwGetKey(GelsenizeCore::sharedInstance().GetWindow(), GLFW_KEY_S) == GLFW_PRESS;

	bool pressA = glfwGetKey(GelsenizeCore::sharedInstance().GetWindow(), GLFW_KEY_A) == GLFW_PRESS;
	bool pressD = glfwGetKey(GelsenizeCore::sharedInstance().GetWindow(), GLFW_KEY_D) == GLFW_PRESS;

	bool pressE = glfwGetKey(GelsenizeCore::sharedInstance().GetWindow(), GLFW_KEY_E) == GLFW_PRESS;
	bool pressQ = glfwGetKey(GelsenizeCore::sharedInstance().GetWindow(), GLFW_KEY_Q) == GLFW_PRESS;

	bool pressLeft = glfwGetKey(GelsenizeCore::sharedInstance().GetWindow(), GLFW_KEY_LEFT) == GLFW_PRESS;
	bool pressRight = glfwGetKey(GelsenizeCore::sharedInstance().GetWindow(), GLFW_KEY_RIGHT) == GLFW_PRESS;

	bool pressLeftShift = glfwGetKey(GelsenizeCore::sharedInstance().GetWindow(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;

	bool pressSpace = glfwGetKey(GelsenizeCore::sharedInstance().GetWindow(), GLFW_KEY_SPACE) == GLFW_PRESS;

	float goForward = pressW ? 1.6f  : -1.6f;
	float goUpward  = pressE ? 18.0f : -18.0f;
	float turnLeft  = pressD ? -1.0f : +1.0f;

	glm::vec4 up     (0.0f, goUpward, 0.0f,      0.0f);
	glm::vec4 forward(0.0f, 0.0f,     goForward, 0.0f);

	iGameObject *playerObject = this->characterController->characterOwner;

	MeshInfo    playerMesh    = playerObject->GetMesh();
	RenderInfo  playerRender  = playerObject->GetRender();
	PhysicsInfo	playerPhysics = playerObject->GetPhysics();
	
	glm::mat4x4 playerTransform = glm::mat4x4(1.0f);
	
	btVector3 walkDirection(0.0f,0.0f,0.0f);

	if (jumpControlLock)
	{
		if (jumpOngoing_internal)
		{
			jumpAcc_internal += deltaTime;
			if (jumpAcc_internal >= jumpLock_internal)
			{
				jumpOngoing_internal = false;

				btVector3 jumpVector(0.0f, 5.0f, 0.0f);
				characterController->jump(jumpVector);
				gelSoundManager::sharedInstance().PlaySoundWithName("jump");
			}
		}

		jumpAcc += deltaTime;
		if (jumpAcc >= jumpLock)
		{
			jumpOngoing = false;
		}

		jumpControlLockAcc += deltaTime;
		if (jumpControlLockAcc > jumpControlLock_internal)
		{
			jumpControlLock = false;
		}
	}

	if (pressE != pressQ)
	{
		/*glm::vec4 playerDirection = glm::normalize(playerRender.transform * up);

		walkDirection.x = playerDirection.x * deltaTime * 0.8f;
		walkDirection.y = playerDirection.y * deltaTime * 0.8f;
		walkDirection.z = playerDirection.z * deltaTime * 0.8f;
*/
	}

	if ((pressW != pressS))
	{
		glm::vec4 playerDirection = glm::normalize(playerRender.transform * forward);
		
		float sprintMod = 4.0f;
		
		AnimationClipInfo animationInfo;
		if (pressLeftShift)
		{
			animationInfo.rate = 1.5f;
			animationInfo.name = "CHAR_RUN";
		}
		else
		{
			animationInfo.rate = 1.5f;
			sprintMod = 1.5f;
			animationInfo.name = "CHAR_WALK";
		}

		if (!pressW)
		{
			//animationInfo.rate *= -1.0f;
			animationInfo.name = "CHAR_WALK_BACKWARDS";
		}

		animationInfo.loopCount = -1;
		if (characterController->onGround() && !jumpOngoing)//!jump
		{
			playerMesh.animationController->BlendIntoAnimation(animationInfo, 0.24f);
		}

		walkDirection = btVector3(
			playerDirection.x * deltaTime * 2.2f * sprintMod,
			playerDirection.y * deltaTime * 2.2f * sprintMod,
			playerDirection.z * deltaTime * 2.2f * sprintMod);

	}
	else //if (!(pressW || pressS))
	{
	//set walk direction to 0.0.0
	walkDirection = btVector3(0.0f, 0.0f, 0.0f);

	//float yDirection = playerRender.position.y - playerRender.previousPosition.y;
	//positive = ascending
	//negative = descending
	if (characterController->onGround() && !jumpOngoing)//!jump
	{
		AnimationClipInfo animationInfo;

		animationInfo.name = "CHAR_IDLE_COMBAT";
		animationInfo.rate = 1.5f;
		animationInfo.loopCount = -1;
		playerMesh.animationController->BlendIntoAnimation(animationInfo, 0.24f);
	}
	}

	if (pressA != pressD)
	{
		// Represents the change over 1 second.
		glm::quat qRotChangeZeroSeconds;		// No rotational change
		glm::quat qRotChange(glm::vec3(0.0f, (turnLeft * 1.4f), 0.0f));
		glm::quat qRotChangeThisFrame = glm::slerp(qRotChangeZeroSeconds, qRotChange, deltaTime);

		glm::quat finalOrientationGLM = playerRender.orientation * qRotChangeThisFrame;
		btQuaternion finalOrientation(
			finalOrientationGLM.x,
			finalOrientationGLM.y,
			finalOrientationGLM.z,
			finalOrientationGLM.w);

		btTransform ghostTransform = actorGhost->getWorldTransform();
		ghostTransform.setRotation(finalOrientation);
		actorGhost->setWorldTransform(ghostTransform);
	}
	else
	{

	}

	/*if (pressSpace && 0)
	{
		if (characterController->onGround())
		{
			AnimationClipInfo animationInfo;
			animationInfo.name = "CHAR_JUMP";
			animationInfo.rate = 1.5f;
			animationInfo.loopCount = -1;
			playerMesh.animationController->BlendIntoAnimation(animationInfo, 0.10f);

			btVector3 jumpVector(0.0f, 4.0f, 0.0f);
			characterController->jump(jumpVector);
		}
	}*/

	if (!characterController->onGround() && !jumpOngoing)
	{
		fallAcc += deltaTime;

		if (fallAcc >= fallAcc_internal)
		{
			fallAcc = 0.0f;

			AnimationClipInfo animationInfo;

			animationInfo.name = "CHAR_FALL";
			animationInfo.rate = 1.5f;
			animationInfo.loopCount = -1;
			playerMesh.animationController->BlendIntoAnimation(animationInfo, 0.25f);
		}
	}


	//turn with mouse
	turnLeft = leftSide ? +1.0f : -1.0f;
	if (mousePercentX > 0.8f)
	{
		iGameObject *playerObject = this->characterController->characterOwner;
		RenderInfo  playerRender = playerObject->GetRender();

		// Represents the change over 1 second.
		glm::quat qRotChangeZeroSeconds;		// No rotational change
		glm::quat qRotChange(glm::vec3(0.0f, (turnLeft * 1.2f), 0.0f));
		glm::quat qRotChangeThisFrame = glm::slerp(qRotChangeZeroSeconds, qRotChange, 0.02f);

		glm::quat finalOrientationGLM = playerRender.orientation * qRotChangeThisFrame;
		btQuaternion finalOrientation(
			finalOrientationGLM.x,
			finalOrientationGLM.y,
			finalOrientationGLM.z,
			finalOrientationGLM.w);

		btTransform ghostTransform = actorGhost->getWorldTransform();
		ghostTransform.setRotation(finalOrientation);
		actorGhost->setWorldTransform(ghostTransform);
	}


	characterController->setWalkDirection(walkDirection);

	playerObject->SetRender(playerRender);
	playerObject->SetPhysics(playerPhysics);
}


void nPhysics::bulletPhysicsWorld::MousePositionCallback(float xPosition, float yPosition)
{
	std::cout << "Mouse position:  " << xPosition << ", " << yPosition << std::endl;

	int window_Width;
	int window_Height;
	gelCameraManager::sharedInstance().mainCamera->GetWindowSize(window_Width, window_Height);

	float xLeftPercent = xPosition / window_Width;
	float yTopPercent = yPosition / window_Height;

	leftSide = xLeftPercent <= 0.5f;
	topSide = yTopPercent <= 0.5;

	const glm::vec3 maxOffsetLookat = glm::vec3(4.0f, 2.0f, 4.0f);
	glm::vec3 adjustedOffsetLookat = maxOffsetLookat;

	bool invertControls = false;
	if(invertControls)
	{
		adjustedOffsetLookat.x *= -1.0f;
		adjustedOffsetLookat.y *= -1.0f;
	}


	//adjust x
	if (leftSide)
	{
		xLeftPercent = 0.5f - xLeftPercent;
		xLeftPercent *= 2; //total contribute when 50% of whole screen represents 100%
	}
	else
	{
		xLeftPercent = xLeftPercent - 0.5f;
		xLeftPercent *= 2; //total contribute when 50% of whole screen represents 100%

		adjustedOffsetLookat.x *= -1.0f;
	}

	if (topSide)
	{
		yTopPercent = 0.5f - yTopPercent;
		yTopPercent *= 2; //total contribute when 50% of whole screen represents 100%
	}
	else
	{
		yTopPercent = yTopPercent - 0.5f;
		yTopPercent *= 2; //total contribute when 50% of whole screen represents 100%

		adjustedOffsetLookat.y *= -1.0f;
	}

	adjustedOffsetLookat.x *= xLeftPercent;
	adjustedOffsetLookat.y *= yTopPercent;
	gelCameraManager::sharedInstance().mainCamera->SetOffsetLookAt(adjustedOffsetLookat);

	mousePercentX = xLeftPercent;
	mousePercentY = yTopPercent;
}


void nPhysics::bulletPhysicsWorld::KeyboardCallback(int key, int action)
{
	iGameObject *playerObject = this->characterController->characterOwner;

	MeshInfo    playerMesh = playerObject->GetMesh();
	
	if (skillOngoing)
	{
		return;
	}


	if (drivingVehicle)
	{
		bool handled = false;

		//Key pressed events
		if (action == GLFW_PRESS)
		{
			/*if (key == GLFW_KEY_E)
			{
				toggleDrivingMode();
				return;
			}*/

			if (key == GLFW_KEY_A)
			{
				this->vehicle->setSteeringValue(btScalar(0.3), 2);
				this->vehicle->setSteeringValue(btScalar(0.3), 3);
				handled = true;
			}

			if (key == GLFW_KEY_D)
			{
				this->vehicle->setSteeringValue(btScalar(-0.3), 2);
				this->vehicle->setSteeringValue(btScalar(-0.3), 3);
				handled = true;
			}

			if (key == GLFW_KEY_W)
			{
				this->vehicle->applyEngineForce(3000, 0);
				this->vehicle->applyEngineForce(3000, 1);
				handled = true;
			}

			if (key == GLFW_KEY_S)
			{
				this->vehicle->applyEngineForce(-2000, 0);
				this->vehicle->applyEngineForce(-2000, 1);
				handled = true;
			}

			//Handbrake
			if (key == GLFW_KEY_LEFT_CONTROL)
			{
				this->vehicle->setBrake(500, 0);
				this->vehicle->setBrake(500, 1);
				handled = true;
			}

		}
		//Key released events
		else if (action == GLFW_RELEASE)
		{
			if (key == GLFW_KEY_A || key == GLFW_KEY_D)
			{
				this->vehicle->setSteeringValue(0, 2);
				this->vehicle->setSteeringValue(0, 3);
				handled = true;
			}

			if (key == GLFW_KEY_W || key == GLFW_KEY_S)
			{
				this->vehicle->applyEngineForce(0, 0);
				this->vehicle->applyEngineForce(0, 1);

				//Default braking force, always added otherwise there is no friction on the wheels
				this->vehicle->setBrake(10, 0);
				this->vehicle->setBrake(10, 1);
				handled = true;
			}

			if (key == GLFW_KEY_LEFT_CONTROL)
			{
				this->vehicle->setBrake(0, 0);
				this->vehicle->setBrake(0, 1);
				handled = true;
			}
		}
	}
	else
	{
		if (action == GLFW_PRESS)
		{
			if (key == GLFW_KEY_E)
			{
				AnimationClipInfo animationInfo;
				animationInfo.name = "CHAR_CAST_1";
				animationInfo.rate = 1.5f;
				animationInfo.loopCount = 1;
				animationInfo.overtake = true;
				playerMesh.animationController->BlendWithAnimation(animationInfo, 0.20f);
				skillOngoing = true;
				skillAcc = 0.0f;

				//iGameObject* player = this->characterController->characterOwner;
				//RenderInfo render = player->GetRender();
				//render.exploding = true;
				//player->SetRender(render);
			}
			else if (key == GLFW_KEY_W)
			{
			}
			else if (key == GLFW_KEY_A)
			{
			}
			else if (key == GLFW_KEY_S)
			{
			}
			else if (key == GLFW_KEY_D)
			{
			}
			else if (key == GLFW_KEY_SPACE)
			{
				if (characterController->onGround() && !jumpControlLock)
				{
					jumpControlLockAcc = 0.0f;
					jumpControlLock = true;
					jumpAcc = 0.0f;
					jumpAcc_internal = 0.0f;
					jumpOngoing = true;
					jumpOngoing_internal = true;
					//start counting internal clock now before adding jump impulse

					AnimationClipInfo animationInfo;
					animationInfo.name = "CHAR_JUMP";
					animationInfo.rate = 1.0f;
					animationInfo.loopCount = -1;
					playerMesh.animationController->BlendIntoAnimation(animationInfo, 0.20f);


				}
			}
		}
		else if (action == GLFW_RELEASE)
		{
			if (key == GLFW_KEY_W)
			{
			}
			else if (key == GLFW_KEY_A)
			{
			}
			else if (key == GLFW_KEY_S)
			{
			}
			else if (key == GLFW_KEY_D)
			{
			}
		}
	}

}



// VEHICLE //

btRigidBody* nPhysics::bulletPhysicsWorld::CreateChassisRigidBodyFromShape(btCollisionShape* chassisShape)
{
	btTransform chassisTransform;
	chassisTransform.setIdentity();
	chassisTransform.setOrigin(btVector3(0.0f, 0.0f, 0.0f));

	{
		//chassis mass 
		btScalar mass(480);

		//since it is dynamic, we calculate its local inertia
		btVector3 localInertia(0, 0, 0);
		chassisShape->calculateLocalInertia(mass, localInertia);

		//using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
		btDefaultMotionState* groundMotionState = new btDefaultMotionState(chassisTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, groundMotionState, chassisShape, localInertia);

		return new btRigidBody(rbInfo);
	}
}

btRigidBody* nPhysics::bulletPhysicsWorld::CreateGroundRigidBodyFromShape(btCollisionShape* groundShape)
{
	btTransform groundTransform;
	groundTransform.setIdentity();
	groundTransform.setOrigin(btVector3(0, 1.4, 0));

	{
		//The ground is not dynamic, we set its mass to 0
		btScalar mass(0);

		//No need for calculating the inertia on a static object
		btVector3 localInertia(0, 0, 0);

		//using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
		btDefaultMotionState* groundMotionState = new btDefaultMotionState(groundTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, groundMotionState, groundShape, localInertia);

		return new btRigidBody(rbInfo);
	}
}

void nPhysics::bulletPhysicsWorld::CreateVehicle()
{
	{
		////make sure to re-use collision shapes among rigid bodies whenever possible!
		//{
		//	//Creates the ground shape
		//	btCollisionShape* groundShape = new btBoxShape(btVector3(1000, 1, 1000));

		//	//Stores on an array for reusing
		//	//collisionShapes.push_back(groundShape);

		//	//Creates the ground rigidbody
		//	btRigidBody* groundRigidBody = this->CreateGroundRigidBodyFromShape(groundShape);

		//	//Adds it to the world
		//	this->dynamicsWorld->addRigidBody(groundRigidBody);
		//}


		//the dimensions for the boxShape are half extents, so 0,5 equals to 
		//btVector3 halfExtends(3.4f, 1.4f, 6.6f);
		btVector3 halfExtends(2.5f, 1.4f, 5.0f);

		//The btBoxShape is centered at the origin
		btCollisionShape* chassisShape = new btBoxShape(halfExtends);

		//Stores on an array for reusing
		//collisionShapes.push_back(chassisShape);

		//A compound shape is used so we can easily shift the center of gravity of our vehicle to its bottom
		//This is needed to make our vehicle more stable
		btCompoundShape* compound = new btCompoundShape();

		//collisionShapes.push_back(compound);

		glm::vec3 origin(0.0f, 0.0f, -0.6f);
		
		btTransform localTransform;
		localTransform.setIdentity();
		localTransform.setOrigin(btVector3(origin.x, origin.y, origin.z));

		//The center of gravity of the compound shape is the origin. When we add a rigidbody to the compound shape
		//it's center of gravity does not change. This way we can add the chassis rigidbody one unit above our center of gravity
		//keeping it under our chassis, and not in the middle of it
		compound->addChildShape(localTransform, chassisShape);

		//Creates a rigid body
		btRigidBody* chassisRigidBody = this->CreateChassisRigidBodyFromShape(compound);
		

		//Adds the vehicle chassis to the world
		this->dynamicsWorld->addRigidBody(chassisRigidBody);//, E_Rigid, E_Static | E_Rigid | E_Actor | E_Trigger);
		
		btVehicleRaycaster* vehicleRayCaster = new btDefaultVehicleRaycaster(this->dynamicsWorld);
		//bulletVehicleRaycaster* vehicleRayCaster = new bulletVehicleRaycaster(this->dynamicsWorld);
		//vehicleRayCaster->useFilter = true;
		//vehicleRayCaster->collisionFilterMask = E_Static | E_Rigid | E_Actor | E_Trigger;
		//vehicleRayCaster->collisionFilterGroup = E_Rigid;

		

		btRaycastVehicle::btVehicleTuning tuning;
		tuning.m_suspensionStiffness = 6.00;
		tuning.m_suspensionCompression = 0.85f;
		tuning.m_suspensionDamping = 0.9f;	   
		tuning.m_frictionSlip = 10.15f;
		//wheel.m_rollInfluence = 1;
		tuning.m_maxSuspensionForce = 7500.0f;
		tuning.m_maxSuspensionTravelCm = 240.0f;

		//Creates a new instance of the raycast vehicle
		this->vehicle = new btRaycastVehicle(tuning, chassisRigidBody, vehicleRayCaster);

		this->vehicle->setCoordinateSystem(0, 1, 2);

		//Never deactivate the vehicle
		chassisRigidBody->setActivationState(DISABLE_DEACTIVATION);

		//Adds the vehicle to the world
		//dynamicsWorld->addCollisionObject(this->vehicle, E_Actor, E_Static | E_Rigid | E_Actor | E_Trigger);
		dynamicsWorld->addAction(this->vehicle);
		//this->dynamicsWorld->addVehicle(this->vehicle);


		//Adds the wheels to the vehicle
		this->AddWheels(&halfExtends, this->vehicle, tuning);

		RenderInfo render;
		render.position = GelPosition(20.0f, 8.0f, 30.0f);

		render.color = GelColor(0.0f, 0.8f, 0.8f, 1.0f);
		MeshInfo mesh;
		mesh.name = "cube";

		PhysicsInfo physics;
		physics.enabled = true;

		iGameObject* gameObject = gelObjectFactory::sharedInstance().createGameObject("object");
		if (gameObject != 0)
		{
			chassisRigidBody->setUserPointer(gameObject);

			gameObject->SetObjectName("vehicle");
			gameObject->SetMesh(mesh);
			gameObject->SetRender(render);
			gameObject->SetPhysics(physics);
			gelObjectManager::sharedInstance().AddGameObject(gameObject);
			gelObjectManager::sharedInstance().SetVehicleID(gameObject->getObjectID());
		}
	}
}

void nPhysics::bulletPhysicsWorld::toggleDrivingMode()
{
	drivingVehicle = true; //alternate for e
	if (drivingVehicle)
	{
		glm::vec3 vehiclePos = this->characterController->characterOwner->GetRender().position;
		vehiclePos.y += 3.0f;
		this->vehicle->getRigidBody()->setLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));
		this->vehicle->getRigidBody()->setGravity(btVector3(0.0f, -10.0f, 0.0f));
		this->characterRigidBody->setGravity(btVector3(0.0f, 0.0f, 0.0f));
		WarpCharacter(glm::vec3(-100.0f, -100.0f, 0.0f));
		WarpVehicle(vehiclePos);

		gelCameraManager::sharedInstance().AttachToGameObject(gelObjectManager::sharedInstance().GetVehicle());
		

		//this->characterController->getGhostObject()->get
		//this->characterController->
	}
	else
	{
		btVector3 characterPos = this->vehicle->getRigidBody()->getWorldTransform().getOrigin();
		this->characterRigidBody->setLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));
		this->vehicle->getRigidBody()->setGravity(btVector3(0.0f, 0.0f, 0.0f));
		this->characterRigidBody->setGravity(btVector3(0.0f, -10.0f, 0.0f));
		WarpVehicle(glm::vec3(100.0f, -100.0f, 0.0f));
		WarpCharacter(glm::vec3(characterPos.getX(), characterPos.getY(), characterPos.getZ()));

		gelCameraManager::sharedInstance().AttachToGameObject(this->characterController->characterOwner);
		
	}
}


void nPhysics::bulletPhysicsWorld::AddWheels(
	btVector3* halfExtents,
	btRaycastVehicle* vehicle,
	btRaycastVehicle::btVehicleTuning tuning)
{
	//The direction of the raycast, the btRaycastVehicle uses raycasts instead of simiulating the wheels with rigid bodies
	btVector3 wheelDirectionCS0(0.0f, -1.0f, 0.0f);

	//The axis which the wheel rotates arround
	btVector3 wheelAxleCS(-1.0f, 0.0f, 0.0f);

	btScalar suspensionRestLength(0.62f);

	//btScalar wheelWidth(0.8f);

	btScalar wheelRadius(1.0f);

	//The height where the wheels are connected to the chassis
	//btScalar connectionHeight(1.6f);

	//All the wheel configuration assumes the vehicle is centered at the origin and a right handed coordinate system is used
	btVector3 wheelConnectionPoint(-1.0f, 0.0f, -2.6f);

	//btVector3 wheelConnectionPoint(halfExtents->x() - wheelWidth, connectionHeight, halfExtents->z() - (wheelRadius));
	//btVector3 vector2 = wheelConnectionPoint * btVector3(-1, 1, 1);
	//btVector3 halfExtends(1.6f, 1.2f, 3.0f);

	//Adds the front wheels
	vehicle->addWheel(wheelConnectionPoint * btVector3(1.4, 1, 1), wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, tuning, true);

	vehicle->addWheel(wheelConnectionPoint * btVector3(-1.4, 1, 1), wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, tuning, true);

	//Adds the rear wheels
	vehicle->addWheel(wheelConnectionPoint* btVector3(0.9, 1, -1), wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, tuning, false);

	vehicle->addWheel(wheelConnectionPoint * btVector3(-0.9, 1, -1), wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, tuning, false);


	//Configures each wheel of our vehicle, setting its friction, damping compression, etc.
	//For more details on what each parameter does, refer to the docs
	for (int i = 0; i < vehicle->getNumWheels(); i++)
	{
		btWheelInfo& wheel = vehicle->getWheelInfo(i);
		wheel.m_suspensionStiffness = 20;
		wheel.m_wheelsDampingCompression = 4.0f;// btScalar(0.3) * 2 * btSqrt(wheel.m_suspensionStiffness);//btScalar(0.8);
		wheel.m_wheelsDampingRelaxation = 2.0f;// btScalar(0.5) * 2 * btSqrt(wheel.m_suspensionStiffness);//1;
											   //Larger friction slips will result in better handling
		wheel.m_frictionSlip = 1000.0f;// btScalar(1.2);
		wheel.m_rollInfluence = 0.0001f;

	}
}