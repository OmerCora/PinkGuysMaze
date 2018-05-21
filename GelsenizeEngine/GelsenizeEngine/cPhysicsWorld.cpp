#include "cPhysicsWorld.h"

#include "gelObjectManager.h"
#include "SimpleShapes.h"
#include <algorithm>    // std::find

#include "cSoftBody.h"

#include <functional>

namespace nPhysics
{
	cPhysicsWorld::cPhysicsWorld()
	{
		totalTime = 0.0f;
	}

	cPhysicsWorld::~cPhysicsWorld()
	{
	}

	bool cPhysicsWorld::RegisterCollisionListener(std::function<void(iGameObject*, iGameObject*)> listener)
	{
		return false;
	}

	bool cPhysicsWorld::AddPoint2PointConstraint(iCollisionBody * body1, glm::vec3 pivot)
	{
		return false;
	}

	bool cPhysicsWorld::AddPoint2PointConstraint(iCollisionBody * body1, iCollisionBody * body2, glm::vec3 pivot, glm::vec3 pivot2)
	{
		return false;
	}

	bool cPhysicsWorld::AddHingeConstraint(iCollisionBody * body1, glm::vec3 from)
	{
		return false;
	}

	bool cPhysicsWorld::AddSliderConstraint(iCollisionBody * body1, glm::vec3 from)
	{
		return false;
	}

	bool cPhysicsWorld::AddConeTwistConstraint(iCollisionBody * body1, glm::vec3 from)
	{
		return false;
	}

	bool cPhysicsWorld::SaveShape(iShape shapeToSave)
	{
		return false;
	}

	bool cPhysicsWorld::SaveShape(std::vector<iShape>& allShapes)
	{
		return false;
	}

	bool cPhysicsWorld::CreateCharacter(iGameObject * characterOwnerObject)
	{
		return false;
	}

	void cPhysicsWorld::GetCharacterBodyProperties(glm::vec3 & specialScale, eShapeType & shapeType)
	{
	}

	void cPhysicsWorld::WarpVehicle(glm::vec3 position)
	{
	}

	void cPhysicsWorld::GetWheelTransforms(std::vector<glm::vec3>& positions, std::vector<glm::quat>& orientations, std::vector<glm::mat4>&transformations)
	{
	}

	void cPhysicsWorld::WarpCharacter(glm::vec3 position)
	{
	}

	void cPhysicsWorld::MousePositionCallback(float xPosition, float yPosition)
	{
	}

	void cPhysicsWorld::KeyboardCallback(int key, int action)
	{
	}

	//not used
	bool cPhysicsWorld::AddBody(iCollisionBody * body)
	{
		//not actually used

		//if (body == 0) return false;
		//cRigidBody *rb = dynamic_cast<cRigidBody*>(body);
		//if (rb == 0) return false;
		//std::vector<cRigidBody*>::iterator it = std::find(mRigidBodies.begin(), mRigidBodies.end(), rb);
		//if (it != mRigidBodies.end()) return false;
		//mRigidBodies.push_back(rb);
		//return true;
		return false;
	}

	bool cPhysicsWorld::RemoveBody(iCollisionBody * body)
	{
		return false;
	}

	struct State
	{
		glm::vec3 x;      // position
		glm::vec3 v;      // velocity
	};

	struct Derivative
	{
		//by time
		glm::vec3 dx;      // dx/dt = velocity
		glm::vec3 dv;      // dv/dt = acceleration
	};

	glm::vec3 accelerationByGravity(const State &state, float t)
	{
		//gravity
		return glm::vec3(0.0f, -9.8f, 0.0f);
	}


	Derivative sample(const State &initial ,glm::vec3 forcedAcceleration,
		float t,
		float dt,
		const Derivative &d)
	{
		State state;
		state.x.x = initial.x.x + d.dx.x*dt;
		state.x.y = initial.x.y + d.dx.y*dt;
		state.x.z = initial.x.z + d.dx.z*dt;

		state.v.x = initial.v.x + d.dv.x*dt;
		state.v.y = initial.v.y + d.dv.y*dt;
		state.v.z = initial.v.z + d.dv.z*dt;

		Derivative output;
		output.dx.x = state.v.x;
		output.dx.y = state.v.y;
		output.dx.z = state.v.z;

		output.dv = accelerationByGravity(state, t + dt);
		output.dv += forcedAcceleration;
		//also add forces impact to acceleration
		return output;
	}

	
	void RK4_integrate(State &state, glm::vec3 forcedAcceleration, float t, float dt)
	{

		Derivative sampleZero, sampleHalf1, sampleHalf2, sampleFull;

		sampleZero  = sample(state, forcedAcceleration, t, 0.0f,    Derivative());
		sampleHalf1 = sample(state, forcedAcceleration, t, dt*0.5f, sampleZero);
		sampleHalf2 = sample(state, forcedAcceleration, t, dt*0.5f, sampleHalf1);
		sampleFull  = sample(state, forcedAcceleration, t, dt,      sampleHalf2);

		//add impulses here to derivations until we add other variables

		float dxdt_x = 1.0f / 6.0f *
			(sampleZero.dx.x + 2.0f*(sampleHalf1.dx.x + sampleHalf2.dx.x) + sampleFull.dx.x);
		float dxdt_y = 1.0f / 6.0f *
			(sampleZero.dx.y + 2.0f*(sampleHalf1.dx.y + sampleHalf2.dx.y) + sampleFull.dx.y);
		float dxdt_z = 1.0f / 6.0f *
			(sampleZero.dx.z + 2.0f*(sampleHalf1.dx.z + sampleHalf2.dx.z) + sampleFull.dx.z);

		float dvdt_x = 1.0f / 6.0f *
			(sampleZero.dv.x + 2.0f*(sampleHalf1.dv.x + sampleHalf2.dv.x) + sampleFull.dv.x);
		float dvdt_y = 1.0f / 6.0f *
			(sampleZero.dv.y + 2.0f*(sampleHalf1.dv.y + sampleHalf2.dv.y) + sampleFull.dv.y);
		float dvdt_z = 1.0f / 6.0f *
			(sampleZero.dv.z + 2.0f*(sampleHalf1.dv.z + sampleHalf2.dv.z) + sampleFull.dv.z);

		state.x.x = state.x.x + dxdt_x * dt;
		state.x.y = state.x.y + dxdt_y * dt;
		state.x.z = state.x.z + dxdt_z * dt;

		state.v.x = state.v.x + dvdt_x * dt;
		state.v.y = state.v.y + dvdt_y * dt;
		state.v.z = state.v.z + dvdt_z * dt;
	}

	//collision methods
	int IntersectMovingSphereSphere(iRigidBody* bodySphereLeft, iRigidBody* bodySphereRight, float &t, glm::vec3 &contactPoint)
	{
		nPhysics::iShape* s1 = bodySphereLeft->GetShape();
		cSphereShape* sphereObjectLeft = dynamic_cast<cSphereShape*>(s1);
		nShapes::sSphere sphereLeft;
		sphereObjectLeft->GetSphere(sphereLeft);

		nPhysics::iShape* s2 = bodySphereRight->GetShape();
		cSphereShape* sphereObjectRight = dynamic_cast<cSphereShape*>(s2);
		nShapes::sSphere sphereRight;
		sphereObjectRight->GetSphere(sphereRight);

		float radiusLeft  = sphereLeft.Radius;
		float radiusRight = sphereRight.Radius;

		glm::vec3 positionLeft;
		glm::vec3 positionRight;

		bodySphereLeft ->GetPosition(positionLeft );
		bodySphereRight->GetPosition(positionRight);

		float dist = glm::distance(positionLeft, positionRight);
		if (dist <= 0.1f + (radiusLeft + radiusRight)/2.0f)
		{
			t = 0.0f;
			contactPoint = (positionLeft + positionRight) / 2.0f;
			return 1;
		}

		//not using velocity and t right now
		return 0;
	}

	int IntersectMovingSpherePlane(iRigidBody* bodySphere, glm::vec3 velocity, iRigidBody* bodyPlane, float &t, glm::vec3 &contactPoint, glm::vec3 &contactNormal)
	{
		nPhysics::iShape* s = bodySphere->GetShape();
		cSphereShape* sphereObject = dynamic_cast<cSphereShape*>(s);
		nShapes::sSphere sphere;
		sphereObject->GetSphere(sphere);

		glm::vec3 sphereCenter;
		bodySphere->GetPosition(sphereCenter);

		float sphereRadius;
		sphereRadius = sphere.Radius /2.0f;

		///

		nPhysics::iShape* p = bodyPlane->GetShape();
		cPlaneShape*  planeObject  = dynamic_cast<cPlaneShape *>(p);
		nShapes::sPlane plane;
		planeObject->GetPlane(plane);

		glm::vec3 planePosition;
		bodyPlane->GetPosition(planePosition);

		glm::vec3 planeNormal;
		planeNormal = plane.Normal;
		contactNormal = planeNormal;

		// Compute distance of sphere center to plane
		float dist = glm::dot(sphereCenter - planePosition, planeNormal );
		if (abs(dist) <= sphereRadius) {
			// The sphere is already overlapping the plane. Set time of
			// intersection to zero and q to sphere center
			t = 0.0f;
			contactPoint = sphereCenter;
			return 1;
		}
		else {
			float denom = glm::dot(planeNormal, velocity);
			if (denom * dist >= 0.0f) {
				// No intersection as sphere moving parallel to or away from plane
				return 0;
			}
			else {
				// Sphere is moving towards the plane

				// Use +r in computations if sphere in front of plane, else -r
				float r = dist > 0.0f ? sphereRadius : -sphereRadius;
				t = (r - dist) / denom;
				contactPoint = sphereCenter + t * velocity - r * planeNormal;
				return 1;
			}
		}
	}



	void cPhysicsWorld::TimeStep(float dt)
	{
		contacts.clear();
		//1-) For all objects, integrate 
		std::vector<iGameObject*> gameObjects = gelObjectManager::sharedInstance().GetGameObjects();

		for (int index = 0; index != gameObjects.size(); index++)
		{
			PhysicsInfo objectPhysics = gameObjects[index]->GetPhysics();

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
				iRigidBody* rigidBody = dynamic_cast<iRigidBody*>(objectPhysics.collisionBody);

				if (rigidBody->IsStatic())
				{
					continue;
				}

				State stateOfBody;

				glm::vec3 position;
				glm::vec3 velocity;
				glm::vec3 acceleration;
				glm::quat orientation;
				glm::vec3 angularVelocity;

				//get rigid body properties			
				rigidBody->GetPosition(position);
				rigidBody->GetVelocity(velocity);
				rigidBody->GetOrientation(orientation);
				rigidBody->GetAngularVelocity(angularVelocity);

				rigidBody->SetPreviousPosition(position);

				stateOfBody.x = position;
				stateOfBody.v = velocity;
				rigidBody->GetAcceleration(acceleration);

				//integration
				RK4_integrate(stateOfBody, acceleration, this->totalTime, dt);

				RenderInfo objectRender = gameObjects[index]->GetRender();

				// Represents the change over 1 second.
				glm::quat qRotChangeZeroSeconds;		// No rotational change
				glm::quat qRotChange(angularVelocity);

				glm::quat qRotChangeThisFrame = glm::slerp(qRotChangeZeroSeconds,
					qRotChange,
					dt);

				objectRender.orientation *= qRotChangeThisFrame;

				objectRender.color = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
				objectRender.position = stateOfBody.x;
				gameObjects[index]->SetRender(objectRender);

				objectPhysics.velocity = stateOfBody.v;
				gameObjects[index]->SetPhysics(objectPhysics);

				rigidBody->SetProperties(stateOfBody.x, stateOfBody.v, acceleration, orientation, angularVelocity);
			}
			else if (bodyType == eBodyType::SOFT_BODY_TYPE)
			{
				cSoftBody* softBody = dynamic_cast<cSoftBody*>(objectPhysics.collisionBody);

				//iSoftBody* softBody = dynamic_cast<iSoftBody*>(softBody);
				
				softBody->ApplyForce(glm::vec3(0.0f, -0.8f, 0.0f));

				std::vector<cSoftBody::cConnection*>::iterator it_connection;
				for (int i = 0; i < 15; i++) // iterate over all constraints several times
				{
					for (it_connection = softBody->mConnections.begin(); it_connection != softBody->mConnections.end(); it_connection++)
					{
						cSoftBody::cConnection *connection = (*it_connection);
						connection->Update(dt); //springy stuff
					}
					int x = 0;
				}

				std::vector<cSoftBody::cNode*>::iterator it_node;
				for (it_node = softBody->mNodes.begin(); it_node != softBody->mNodes.end(); it_node++)
				{
					(*it_node)->Update(dt); // calculate the position of each particle at the next time step.
				}
			}
			
		}


		//2-) Check for collision - one combination swipe
		for (int m = 0; m != gameObjects.size(); m++)
		{
			iGameObject* objectLeft = gameObjects[m];
			PhysicsInfo objectPhysicsLeft = gameObjects[m]->GetPhysics();

			iCollisionBody *collisionBody = objectPhysicsLeft.collisionBody;

			if (!collisionBody)
			{
				continue;
			}

			eBodyType bodyType = collisionBody->GetBodyType();

			if (bodyType == eBodyType::RIGID_BODY_TYPE)
			{
				iRigidBody* rigidBodyLeft = dynamic_cast<iRigidBody*>(collisionBody);

				if (!rigidBodyLeft)
				{
					continue;
				}

				for (int n = m + 1; n != gameObjects.size(); n++)
				{
					iGameObject* objectRight = gameObjects[n];
					PhysicsInfo objectPhysicsRight = gameObjects[n]->GetPhysics();

					iCollisionBody* collisionBodyR = objectPhysicsRight.collisionBody;

					if (!collisionBodyR)
					{
						continue;
					}

					if (!collisionBodyR)
					{
						continue;
					}

					eBodyType bodyTypeR = collisionBodyR->GetBodyType();

					if (bodyTypeR == eBodyType::RIGID_BODY_TYPE)
					{
						iRigidBody* rigidBodyRight = dynamic_cast<iRigidBody*>(collisionBodyR);

						if (!rigidBodyRight)
						{
							continue;
						}

						eShapeType shapeLeft = rigidBodyLeft->GetShapeType();
						if (shapeLeft == nPhysics::eShapeType::PLANE_SHAPE)
						{
							//plane -> n
							if (rigidBodyRight->GetShapeType() == nPhysics::eShapeType::PLANE_SHAPE)
							{
								//plane -> plane
								continue;
							}
							else if (rigidBodyRight->GetShapeType() == nPhysics::eShapeType::SPHERE_SHAPE)
							{
								//plane -> sphere

								glm::vec3 rightVelocity;

								float t;
								glm::vec3 contactPoint;
								glm::vec3 contactNormal;
								int collision = IntersectMovingSpherePlane(rigidBodyRight, objectPhysicsRight.velocity, rigidBodyLeft, t, contactPoint, contactNormal);

								if (t == 0.0f && collision == 1)
								{
									cContact curContact;
									curContact.objectID = objectRight->getObjectID(); //we need ID of the sphere for response
									curContact.collisionType = cContact::WITH_PLANE;
									curContact.contactXYZ = contactPoint;
									curContact.normal = contactNormal;
									curContact.contactVelocity = objectPhysicsRight.velocity;
									this->contacts.push_back(curContact);
								}

								if (collision == 1)
								{
									int x = 0;
								}
								else
								{
									int y = 0;
								}
							}
						}
						else if (rigidBodyLeft->GetShapeType() == nPhysics::eShapeType::SPHERE_SHAPE)
						{
							//sphere -> n
							eShapeType shapeRight = rigidBodyRight->GetShapeType();
							if (shapeRight == nPhysics::eShapeType::PLANE_SHAPE)
							{
								//sphere -> plane

								glm::vec3 rightVelocity;

								float t;
								glm::vec3 contactPoint;
								glm::vec3 contactNormal;
								int collision = IntersectMovingSpherePlane(rigidBodyLeft, objectPhysicsLeft.velocity, rigidBodyRight, t, contactPoint, contactNormal);

								if (t == 0.0f && collision == 1)
								{
									cContact curContact;

									curContact.objectID = objectLeft->getObjectID(); //we need ID of the sphere for response

									curContact.collisionType = cContact::WITH_PLANE;
									curContact.contactXYZ = contactPoint;
									curContact.contactVelocity = objectPhysicsLeft.velocity;
									curContact.normal = contactNormal;

									this->contacts.push_back(curContact);
								}

								if (collision == 1)
								{
									int x = 0;
								}
								else
								{
									int y = 0;
								}
							}
							else if (rigidBodyRight->GetShapeType() == nPhysics::eShapeType::SPHERE_SHAPE)
							{
								//sphere -> sphere

								float t;
								glm::vec3 contactPoint;
								glm::vec3 contactNormal;
								int collision = IntersectMovingSphereSphere(rigidBodyLeft, rigidBodyRight, t, contactPoint);

								if (t == 0.0f && collision == 1)
								{
									cContact curContact;

									curContact.objectID = objectLeft->getObjectID(); //we need ID of the sphere for response
									curContact.objectID_other = objectRight->getObjectID();

									curContact.collisionType = cContact::WITH_SPHERE;
									curContact.contactXYZ = contactPoint;
									curContact.contactVelocity = objectPhysicsLeft.velocity;

									glm::vec3 positionLeft;
									glm::vec3 positionRight;

									rigidBodyLeft->GetPosition(positionLeft);
									rigidBodyRight->GetPosition(positionRight);

									curContact.normal = glm::normalize(positionLeft - positionRight);

									this->contacts.push_back(curContact);
								}
							}
						}
					}

				}
			}
			else if (bodyType == eBodyType::SOFT_BODY_TYPE)
			{
				//get all balls in vicinity AABB and compare them to the balls and move the cloth

				cSoftBody* softBody = dynamic_cast<cSoftBody*>(collisionBody);

				if (!softBody)
				{
					continue;
				}

				for (int n = m + 1; n != gameObjects.size(); n++)
				{
					iGameObject* objectRight = gameObjects[n];
					PhysicsInfo objectPhysicsRight = gameObjects[n]->GetPhysics();

					iCollisionBody* collisionBodyR = objectPhysicsRight.collisionBody;

					if (!collisionBodyR)
					{
						continue;
					}

					eBodyType bodyTypeR = collisionBodyR->GetBodyType();
					if (bodyTypeR == eBodyType::RIGID_BODY_TYPE)
					{
						iRigidBody *body = dynamic_cast<iRigidBody*>(collisionBodyR);

						if (body->IsStatic())
						{
							continue;
						}

						if (body->GetShapeType() == nPhysics::eShapeType::SPHERE_SHAPE)
						{
							std::vector<cSoftBody::cNode*>::iterator it_node;
							for (it_node = softBody->mNodes.begin(); it_node != softBody->mNodes.end(); it_node++)
							{
								glm::vec3 ballPosition;
								body->GetPosition(ballPosition);

								glm::vec3 difference = (*it_node)->position - ballPosition;
								float len = glm::length(difference);

								nPhysics::iShape* s1 = body->GetShape();
								cSphereShape* sphereObjectLeft = dynamic_cast<cSphereShape*>(s1);
								nShapes::sSphere sphereLeft;
								sphereObjectLeft->GetSphere(sphereLeft);
								float radius = sphereLeft.Radius*0.52f;

								if (len < radius) // if the particle is inside the ball
								{
									//move node to ball surface
									(*it_node)->AddToPosition(glm::normalize(difference) * (radius - len));
								}
							}
						}
					}
				}
			}
		}


		//3-) Apply responses
		for (int index = 0; index != contacts.size(); index++)
		{
			cContact contact = contacts[index];
			if (contact.collisionType == cContact::WITH_PLANE)
			{
				//reflect velocity with normal
				glm::vec3 reflectedVelocity = glm::reflect(contact.contactVelocity, contact.normal);
				iGameObject* object = gelObjectManager::sharedInstance().FindGameObjectWithID(contact.objectID);
				PhysicsInfo objectPhysics = object->GetPhysics();
				objectPhysics.velocity = reflectedVelocity * 0.99f;
				object->SetPhysics(objectPhysics);

				RenderInfo objectRender = object->GetRender();
				objectRender.color = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);
				if (objectPhysics.collisionBody && objectPhysics.collisionBody->GetBodyType() == eBodyType::RIGID_BODY_TYPE)
				{
					dynamic_cast<iRigidBody*>(objectPhysics.collisionBody)->GetPreviousPosition(objectRender.position);
				}
				
				object->SetRender(objectRender);
			}
			else if (contact.collisionType == cContact::WITH_SPHERE)
			{
				//response for left and right
				
				iGameObject* objectLeft = gelObjectManager::sharedInstance().FindGameObjectWithID(contact.objectID);
				iGameObject* objectRight = gelObjectManager::sharedInstance().FindGameObjectWithID(contact.objectID_other);

				PhysicsInfo objectLeftPhysics  = objectLeft ->GetPhysics();
				PhysicsInfo objectRightPhysics = objectRight->GetPhysics();

				float massLeft = 1.0f;
				if (objectLeftPhysics.collisionBody && objectLeftPhysics.collisionBody->GetBodyType() == eBodyType::RIGID_BODY_TYPE)
				{
					dynamic_cast<iRigidBody*>(objectLeftPhysics.collisionBody)->GetMass(massLeft);
				}
				glm::vec3 velocityLeft = objectLeftPhysics.velocity;

				float massRight = 1.0f;
				if (objectRightPhysics.collisionBody && objectRightPhysics.collisionBody->GetBodyType() == eBodyType::RIGID_BODY_TYPE)
				{
					dynamic_cast<iRigidBody*>(objectRightPhysics.collisionBody)->GetMass(massRight);
				}
				glm::vec3 velocityRight = objectRightPhysics.velocity;

				//new velocity - straightforward momentum transfer (get half of it)
				objectLeftPhysics.velocity  = ( ((( massLeft - massRight) / (massLeft + massRight)) * velocityLeft)  + (((2 * massRight) / (massLeft + massRight)) * velocityRight) *0.42f );
				objectRightPhysics.velocity = ( (((-massLeft + massRight) / (massLeft + massRight)) * velocityRight) + (((2 * massLeft)  / (massLeft + massRight)) * velocityLeft)  *0.42f );

				//transfer through hit vector
				objectLeftPhysics.velocity  += 0.48f * contact.normal * glm::dot(velocityRight, contact.normal);
				objectRightPhysics.velocity += 0.48f * contact.normal * glm::dot(velocityLeft, contact.normal);

				objectLeftPhysics.rotationalVelocity = glm::vec3(objectLeftPhysics.velocity.x*0.1f, objectLeftPhysics.velocity.y*0.1f, objectLeftPhysics.velocity.z*0.1f);
				objectRightPhysics.rotationalVelocity = glm::vec3(objectRightPhysics.velocity.x*0.1f, objectRightPhysics.velocity.y*0.1f, objectRightPhysics.velocity.z*0.1f);

				objectLeft->SetPhysics(objectLeftPhysics);
				objectRight->SetPhysics(objectRightPhysics);

				//unstuck if the balls are stuck
				RenderInfo renderLeft = objectLeft->GetRender();
				RenderInfo renderRight = objectRight->GetRender();

				//renderLeft.color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
				//renderRight.color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

				objectLeft->SetRender(renderLeft);
				objectRight->SetRender(renderRight);
			}
		}

		this->totalTime += dt;
	}


}

