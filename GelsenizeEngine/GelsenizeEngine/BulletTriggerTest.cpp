///**
//* BulletTriggerTest.cpp
//*
//* 2010 Marzena Gasidło (Ocelot).
//* http://www.ocelotsjungle.republika.pl/
//*
//* -----------------------------------------------------------------------------
//*
//* REVISION HISTORY:
//*
//*  12/07(Jul)/2010: Ocelot - Original creation
//*
//* -----------------------------------------------------------------------------
//*
//* Licence:
//*
//*   Bullet Continuous Collision Detection and Physics Library
//*   Copyright (c) 2003-2008 Erwin Coumans  http://bulletphysics.com
//*
//*   This software is provided 'as-is', without any express or implied warranty.
//*   In no event will the authors be held liable for any damages arising from the use of this software.
//*   Permission is granted to anyone to use this software for any purpose,
//*   including commercial applications, and to alter it and redistribute it freely,
//*   subject to the following restrictions:
//*
//*   1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
//*   2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
//*   3. This notice may not be removed or altered from any source distribution.
//*/
//
//#pragma warning( disable : 4100 4127 )
//#include <btBulletDynamicsCommon.h>
//#include <BulletCollision/CollisionDispatch/btGhostObject.h>
//#include "GelKinematicCharacterController.h"
//#pragma warning( default : 4100 4127 )
//#include <iostream>
//#include <vector>
//
//#define LOG(t) std::cout<<t<<std::endl;
//
//btDiscreteDynamicsWorld* dynamicsWorld = 0;
//
////---------------------------------------------------------------------------------------
//enum EPhysicsCollisionMask {
//
//	E_Static = 1 << 0,
//	E_Rigid = 1 << 1,
//	E_Actor = 1 << 2,
//	E_Trigger = 1 << 3,
//
//	E_StaticGroup = E_Rigid | E_Actor,
//	E_ActorGroup = E_Static | E_Rigid | E_Actor | E_Trigger,
//	E_RigidGroup = E_Static | E_Rigid | E_Actor | E_Trigger,
//	E_TriggerGroup = E_Rigid | E_Actor
//};
////---------------------------------------------------------------------------------------
//struct FilterCallback : public btOverlapFilterCallback {
//
//	// return true when pairs need collision
//	virtual bool needBroadphaseCollision(btBroadphaseProxy* proxy0, btBroadphaseProxy* proxy1) const override
//	{
//		bool collides = (proxy0->m_collisionFilterGroup & proxy1->m_collisionFilterMask) &&
//			(proxy1->m_collisionFilterGroup & proxy0->m_collisionFilterMask);
//		//add some additional logic here that modified 'collides'
//		return collides;
//	}
//};
////---------------------------------------------------------------------------------------
//void checkGhost(btPairCachingGhostObject* ghostObject) {
//
//	btManifoldArray   manifoldArray;
//	btBroadphasePairArray& pairArray = ghostObject->getOverlappingPairCache()->getOverlappingPairArray();
//	int numPairs = pairArray.size();
//
//	for (int i = 0; i<numPairs; i++)
//	{
//		manifoldArray.clear();
//
//		const btBroadphasePair& pair = pairArray[i];
//
//		//unless we manually perform collision detection on this pair, the contacts are in the dynamics world paircache:
//		btBroadphasePair* collisionPair = dynamicsWorld->getPairCache()->findPair(pair.m_pProxy0, pair.m_pProxy1);
//		if (!collisionPair)
//			continue;
//		if (collisionPair->m_algorithm)
//			collisionPair->m_algorithm->getAllContactManifolds(manifoldArray);
//
//		for (int j = 0; j<manifoldArray.size(); j++) {
//			btPersistentManifold* manifold = manifoldArray[j];
//			if (manifold->getNumContacts()) {
//				int id = reinterpret_cast<int>((manifold->getBody0() != ghostObject) ? (btCollisionObject*)(manifold->getBody0())->getUserPointer() :
//					(btCollisionObject*)(manifold->getBody1())->getUserPointer());
//				LOG("\tcontact (" << manifold->getNumContacts() << ") : " << ((id == 1) ? "box" : "actor"));
//			}
//		}
//	}
//}
////---------------------------------------------------------------------------------------
//int main(int argc, char* argv[]) {
//
//	btDefaultCollisionConfiguration* conf = new btDefaultCollisionConfiguration();
//	btCollisionDispatcher* dispatcher = new btCollisionDispatcher(conf);
//	btBroadphaseInterface* overlappingPairCache = new btDbvtBroadphase();
//	btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;
//
//	//world
//	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, conf);
//	dynamicsWorld->setGravity(btVector3(0, -10, 0));
//
//	// broadphase filter callback
//	btOverlapFilterCallback * filterCallback = new FilterCallback();
//	dynamicsWorld->getPairCache()->setOverlapFilterCallback(filterCallback);
//
//	std::vector<btCollisionShape*> shapes;
//
//	//------------------------------ ground - static plane ------------------------------
//	btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0, 1, 0), 1);
//	shapes.push_back(groundShape);
//	btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, -1, 0)));
//	btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, groundMotionState, groundShape, btVector3(0, 0, 0));
//	btRigidBody* groundRigidBody = new btRigidBody(groundRigidBodyCI);
//	dynamicsWorld->addRigidBody(groundRigidBody, E_Static, E_Rigid | E_Actor);
//
//	//------------------------------ ghost ------------------------------
//	btCollisionShape* ghostShape = new btBoxShape(btVector3(10, 1, 10));
//	shapes.push_back(ghostShape);
//
//	btTransform transform;
//	transform.setIdentity();
//	transform.setOrigin(btVector3(0.0f, 1.0f, 0.0f));
//
//	btPairCachingGhostObject* ghostObject = new btPairCachingGhostObject();
//	ghostObject->setWorldTransform(transform);
//
//	btGhostPairCallback* ghostPairCallback = new btGhostPairCallback();
//	overlappingPairCache->getOverlappingPairCache()->setInternalGhostPairCallback(ghostPairCallback);
//
//	ghostObject->setCollisionShape(ghostShape);
//	ghostObject->setCollisionFlags(ghostObject->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
//
//	//------------------------------ riggid box ------------------------------
//	btCollisionShape* fallShape = new btBoxShape(btVector3(1.0, 1.0, 1.0));
//	shapes.push_back(fallShape);
//
//	btDefaultMotionState* fallMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(5, 5, 0)));
//	btVector3 fallInertia(0, 0, 0);
//	fallShape->calculateLocalInertia(1.0f, fallInertia);
//
//	btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(1.0f, fallMotionState, fallShape, fallInertia); // może być wspólne
//	btRigidBody* fallRigidBody = new btRigidBody(fallRigidBodyCI);
//	fallRigidBody->setUserPointer((void*)1);
//
//	//------------------------------ character ------------------------------
//	btTransform startTransform;
//	startTransform.setIdentity();
//	startTransform.setOrigin(btVector3(0, 5, 0)); // check
//
//	btPairCachingGhostObject* actorGhost = new btPairCachingGhostObject();
//	actorGhost->setUserPointer((void*)2);
//	actorGhost->setWorldTransform(startTransform);
//
//	btGhostPairCallback* actorGhostPairCallback = new btGhostPairCallback();
//	overlappingPairCache->getOverlappingPairCache()->setInternalGhostPairCallback(actorGhostPairCallback);
//
//	actorGhost->setCollisionShape(fallShape);
//	actorGhost->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);
//
//	OiJE::GelKinematicCharacterController* character = new OiJE::GelKinematicCharacterController(actorGhost, static_cast<btConvexShape*>(fallShape), 0.5f);
//
//	//------------------------------ add actor to the world ------------------------------
//	dynamicsWorld->addCollisionObject(actorGhost, E_Actor, E_Static | E_Rigid | E_Actor | E_Trigger);
//	dynamicsWorld->addAction(character);
//
//	//------------------------------ add rigid to the world ------------------------------
//	dynamicsWorld->addRigidBody(fallRigidBody, E_Rigid, E_Static | E_Rigid | E_Actor | E_Trigger);
//
//	//------------------------------ add ghost to the world ------------------------------
//	dynamicsWorld->addCollisionObject(ghostObject, E_Trigger, E_Rigid | E_Actor);
//
//	//------------------------------------ simulation ------------------------------------
//	for (int i = 0; i<230; i++) {
//		dynamicsWorld->stepSimulation(1 / 60.f, 10);
//		LOG(i);
//		// --- check ghost
//		checkGhost(ghostObject);
//		btTransform trans = actorGhost->getWorldTransform();
//	}
//	//------------------------------------------------------------------------------------
//
//	LOG("\nAfter simulation");
//	btTransform trans;
//	fallRigidBody->getMotionState()->getWorldTransform(trans);
//	LOG("box position    should be (5, 1) is : " << trans.getOrigin().getX() << ", " << trans.getOrigin().getY());
//	trans = actorGhost->getWorldTransform();
//	LOG("actor position  should be (0, 1) is : " << trans.getOrigin().getX() << ", " << trans.getOrigin().getY());
//
//	//************* cleanup in the reverse order of creation/initialization
//	//remove the rigidbodies from the dynamics world and delete them
//	for (int i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--) {
//		btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
//		btRigidBody* body = btRigidBody::upcast(obj);
//		if (body && body->getMotionState())
//			delete body->getMotionState();
//		dynamicsWorld->removeCollisionObject(obj);
//		delete obj;
//	}
//	//delete collision shapes
//	for (int i = 0; i<shapes.size(); ++i)
//		delete shapes[i];
//	delete dynamicsWorld;
//	delete solver;
//	delete overlappingPairCache;
//	delete dispatcher;
//	delete conf;
//
//	LOG("Press <Enter> to end..."); std::cin.get();
//	return 0;
//}
////---------------------------------------------------------------------------------------