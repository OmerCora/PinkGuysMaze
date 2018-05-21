//
//
//
//
//float getRandFloat(float LO, float HI)
//{
//	float r3 = LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)));
//	return r3;
//}
//
//float distanceBetweenPoints(glm::vec3 A, glm::vec3 B)
//{
//	float deltaX = A.x - B.x;
//	float deltaY = A.y - B.y;
//	float deltaZ = A.z - B.z;
//
//	return sqrt(deltaX*deltaX + deltaY*deltaY + deltaZ*deltaZ);
//
//	// ***************************************************
//	// Note that glm already has a function for this:
//	//return glm::distance( A, B );
//	// ***************************************************
//}
//
//float distanceBetweenSpheres(iGameObject* pBallA, iGameObject* pBallB)
//{
//	RenderInfo renderA = pBallA->GetRender();
//	RenderInfo renderB = pBallB->GetRender();
//
//	float deltaX = renderA.position.x - renderB.position.x;
//	float deltaY = renderA.position.y - renderB.position.y;
//	float deltaZ = renderA.position.z - renderB.position.z;
//
//	return sqrt(deltaX*deltaX + deltaY*deltaY + deltaZ*deltaZ);
//}
//
//// Narrow phase 
//bool testSphereSphereCollision(iGameObject* pBallA, iGameObject* pBallB)
//{
//	RenderInfo renderA = pBallA->GetRender();
//	RenderInfo renderB = pBallB->GetRender();
//
//	float totalRadius = renderA.GetRadius() + renderB.GetRadius();
//	if (distanceBetweenSpheres(pBallA, pBallB) <= totalRadius)
//	{
//		return true;
//	}
//	return false;
//}
//
//glm::vec3 getTransformedVertex(glm::vec3 vertex, iGameObject* gameObject)
//{
//	RenderInfo render = gameObject->GetRender();
//	//glm::vec4 transform = render.transform * glm::vec4(vertex, 0.0f);
//	glm::mat4x4 transform = glm::translate(render.transform, glm::vec3(vertex.x, vertex.y, vertex.z));
//	return glm::vec3(transform[3].x, transform[3].y, transform[3].z);
//}
//
////abstraction soon
//void CollisionStep(float deltaTime)
//{
//	// Clear the contact list toggle back colors
//	for (int index = 0; index != ::g_vec_Contacts.size(); index++)
//	{
//		// Change colour of any balls that are touching...
//		unsigned int ID = ::g_vec_Contacts[index].objectID;
//		iGameObject* pCurGO = gelObjectManager::sharedInstance().FindGameObjectWithID(ID);
//		if (pCurGO)
//		{
//			RenderInfo renderCurrent = pCurGO->GetRender();
//			renderCurrent.ResetToggleColor();
//			pCurGO->SetRender(renderCurrent);
//		}
//	}
//	g_vec_Contacts.clear();
//
//	// Brute force narrow phase detection
//
//	std::vector<iGameObject*> gameObjects = gelObjectManager::sharedInstance().GetGameObjects();
//
//	//lets assume there is a special object for spherical objects, we can use the useSphericalBody flag here,
//	//since the "sphere" mesh is relatively close to a perfect sphere... :p
//
//	//lets do a trick here for this time to filter out the balls and "non ball" triangle objects
//	std::vector<iGameObject*> balls;
//	std::vector<iGameObject*> environmentObjects; //only ground right now
//
//	for (int index = 0; index != gameObjects.size(); index++)
//	{
//		iGameObject* gameObject = gameObjects[index];
//		PhysicsInfo physics = gameObject->GetPhysics();
//		if (physics.useSphericalBody)
//		{
//			//ball
//			//will be subject to:
//			//   - Sphere -> Sphere
//			//   - Sphere -> Triangle
//			balls.push_back(gameObject);
//		}
//		else
//		{
//			//triangle
//			//will be subject to:
//			//   - Sphere -> Triangle
//			//   * Triangle -> Triangle (*:maybe not just yet)
//			environmentObjects.push_back(gameObject);
//		}
//	}
//
//	//make a map of all objects and sort them out by using a map,
//	//then iterate through the map and get a single triangle array.
//	std::map< std::string, std::vector< iGameObject* > > meshToObjectMap;
//
//	for (int i = 0; i != environmentObjects.size(); i++)
//	{
//		iGameObject* triangleObject = environmentObjects[i];
//		std::string meshName = triangleObject->GetMesh().name;
//
//		//search
//		std::map< std::string, std::vector< iGameObject* > >::iterator mapIterator =
//			meshToObjectMap.find(meshName);
//
//		if (mapIterator == meshToObjectMap.end())
//		{
//			//not found, add an array
//			std::vector<iGameObject*> vecObjects;
//			vecObjects.push_back(triangleObject);
//			meshToObjectMap[meshName] = vecObjects;
//		}
//		else
//		{
//			//found
//			std::vector<iGameObject*> vecObjects = mapIterator->second;
//			vecObjects.push_back(triangleObject);
//			meshToObjectMap[meshName] = vecObjects;
//		}
//	}
//
//	// 1. For each ball, check for collision with other balls
//	for (int indexOut = 0; indexOut != balls.size(); indexOut++)
//	{
//		iGameObject* pOuterObject = balls[indexOut];
//		RenderInfo renderOut = pOuterObject->GetRender();
//
//		for (int indexIn = 0; indexIn != balls.size(); indexIn++)
//		{
//			iGameObject* pInnerObject = balls[indexIn];
//			RenderInfo renderIn = pInnerObject->GetRender();
//			GelVelocity innerVelocity = pInnerObject->GetPhysics().velocity;
//			// Is this the same object
//			if (pOuterObject != pInnerObject)
//			{
//				if (testSphereSphereCollision(pOuterObject, pInnerObject))
//				{
//					//let the contact point be the point between these two spheres
//					cContact curContact;
//					curContact.objectID = pOuterObject->getObjectID();
//					curContact.collisionType = cContact::WITH_SPHERE;
//
//					//Get mid point, a simple "contact" point
//					glm::vec4 contactPoint = (pOuterObject->GetRender().transform[3] + pInnerObject->GetRender().transform[3]) * 0.5f;
//					curContact.contactXYZ = glm::vec3(contactPoint.x, contactPoint.y, contactPoint.z);
//
//					curContact.contactVelocity = glm::vec3(innerVelocity.x, innerVelocity.y, innerVelocity.z);
//
//					//when they just touch distance is R1+R2, calculate penetration depth from that
//					float zeroDepthCollisionDistance = pOuterObject->GetRender().GetRadius() + pInnerObject->GetRender().GetRadius();
//					curContact.penetrationDepth = zeroDepthCollisionDistance - distanceBetweenSpheres(pOuterObject, pInnerObject);
//
//					//calculate normal
//					glm::vec3 contactNormal(
//						renderOut.position.x - renderIn.position.x,
//						renderOut.position.y - renderIn.position.y,
//						renderOut.position.z - renderIn.position.z);
//					contactNormal = glm::normalize(contactNormal);
//					curContact.normal = contactNormal;
//
//					::g_vec_Contacts.push_back(curContact);
//				}
//			}
//		}
//	}
//
//	// 2. For each ball, check for collision with triangles (environment)
//	for (int indexOut = 0; indexOut != balls.size(); indexOut++)
//	{
//		iGameObject* ball = balls[indexOut];
//		RenderInfo ballRender = ball->GetRender();
//
//		//http://stackoverflow.com/questions/4844886/how-to-loop-through-a-c-map
//		//loop through map,   ("bunny") ->  (gameObjects that have "bunny" as meshName)
//		typedef std::map< std::string, std::vector< iGameObject* > >::iterator it_type;
//		for (it_type iterator = meshToObjectMap.begin(); iterator != meshToObjectMap.end(); iterator++) {
//
//			//list of triangle objects that are under the same mesh model; grouped for the same array of triangles
//			std::string meshName = iterator->first;
//			std::vector<iGameObject*> vecObjects = iterator->second;
//
//			//get triangle array for the model
//			std::vector<GelTriangle> meshTriangles;
//			if (!gelMeshManager::sharedInstance().GetTriangleArrayForMesh(meshName, meshTriangles))
//			{
//				//got false
//				continue;//just for this model.
//			}
//
//			//for each triangle of the triangle_array, check collision for each object
//			for (int triangleIndex = 0; triangleIndex != meshTriangles.size(); triangleIndex++)
//			{
//				GelTriangle triangle = meshTriangles[triangleIndex];
//
//				for (int index = 0; index != vecObjects.size(); index++)
//				{
//					iGameObject* gameObject = vecObjects[index];
//					RenderInfo render = gameObject->GetRender();
//
//					glm::vec3 transformedVertex1 = getTransformedVertex(triangle.v1, gameObject);
//					glm::vec3 transformedVertex2 = getTransformedVertex(triangle.v2, gameObject);
//					glm::vec3 transformedVertex3 = getTransformedVertex(triangle.v3, gameObject);
//
//					glm::vec3 ballCentre(
//						ballRender.position.x,
//						ballRender.position.y,
//						ballRender.position.z);
//
//					//get the closest point of the ball centre, out of the 3 transformed vertices
//					/*glm::vec3 closestPoint = ClosestPtPointTriangle(
//					ballCentre,
//					transformedVertex1,
//					transformedVertex2,
//					transformedVertex3);*/
//					glm::vec3 closestPoint;
//					//DONT FORGET UNCOMMENT ABOVE IF YOU WISH TO USE THIS AGAIN
//
//					//check distance between ball and closest point to see if there is a penetration
//					float distanceToCenterOfClosestPoint = distanceBetweenPoints(ballCentre, closestPoint);
//					float radius = ball->GetRender().GetRadius();
//
//					if (distanceToCenterOfClosestPoint <= radius)
//					{
//						//found a contact
//						cContact curContact;
//						curContact.objectID = ball->getObjectID();
//						curContact.collisionType = cContact::WITH_TRIANGLE;
//						curContact.contactXYZ = closestPoint;
//						curContact.penetrationDepth = radius - distanceToCenterOfClosestPoint;
//
//						//calculate normal
//						curContact.normal = glm::normalize(ballCentre - closestPoint);
//
//						::g_vec_Contacts.push_back(curContact);
//					}
//				}// for each object of a mesh name group
//			}// for each triangle
//		}// for each available mesh name
//	}// for each ball
//
//	 // Calculate reponse....
//	if (!::g_vec_Contacts.empty())
//	{
//		//make a new map
//		std::map< unsigned int, std::vector< cContact > > idToContactVecMap;
//		for (int i = 0; i != ::g_vec_Contacts.size(); i++)
//		{
//			cContact contact = ::g_vec_Contacts[i];
//
//			//search
//			std::map< unsigned int, std::vector< cContact > >::iterator mapIterator =
//				idToContactVecMap.find(contact.objectID);
//
//			if (mapIterator == idToContactVecMap.end())
//			{
//				//not found, add an array
//				std::vector<cContact> vecObjects;
//				vecObjects.push_back(contact);
//				idToContactVecMap[contact.objectID] = vecObjects;
//			}
//			else
//			{
//				//found
//				std::vector<cContact> vecObjects = mapIterator->second;
//				vecObjects.push_back(contact);
//				idToContactVecMap[contact.objectID] = vecObjects;
//			}
//		}
//
//		//loop through map,   ("bunny") ->  (gameObjects that have "bunny" as meshName)
//		typedef std::map< unsigned int, std::vector< cContact > >::iterator it_contact;
//		for (it_contact iterator = idToContactVecMap.begin(); iterator != idToContactVecMap.end(); iterator++) {
//			unsigned int ID = iterator->first;
//			std::vector<cContact> contactsOfID = iterator->second;
//
//			iGameObject* pCurGO = gelObjectManager::sharedInstance().FindGameObjectWithID(ID);
//			if (pCurGO)
//			{
//				RenderInfo renderCurrent = pCurGO->GetRender();
//				PhysicsInfo physicsCurrent = pCurGO->GetPhysics();
//				renderCurrent.ToggleColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
//
//				bool triangleReflection = false;
//				bool sphereReflection = false;
//
//				for (int index = 0; index != contactsOfID.size(); index++)
//				{
//					//for each contact
//					cContact contact = contactsOfID[index];
//					if (contact.collisionType == cContact::WITH_SPHERE)
//					{
//
//						glm::vec3 velocityVec;
//						velocityVec = glm::vec3(physicsCurrent.velocity.x, physicsCurrent.velocity.y, physicsCurrent.velocity.z);
//						glm::vec3 reflectedVelocity;
//						reflectedVelocity = glm::reflect(velocityVec, contact.normal);
//
//						//lose some energy while reflecting
//						//half of it goes to other sphere, half of it reflects.
//						physicsCurrent.velocity.x = reflectedVelocity.x * 0.5f + contact.contactVelocity.x * 0.5f;
//						physicsCurrent.velocity.y = reflectedVelocity.y * 0.5f + contact.contactVelocity.y * 0.5f;
//						physicsCurrent.velocity.z = reflectedVelocity.z * 0.5f + contact.contactVelocity.z * 0.5f;
//
//						renderCurrent.position.x += reflectedVelocity.x * deltaTime * 0.9f;
//						renderCurrent.position.y += reflectedVelocity.y * deltaTime * 0.9f;
//						renderCurrent.position.z += reflectedVelocity.z * deltaTime * 0.9f;
//					}
//					else if (contact.collisionType == cContact::WITH_TRIANGLE)
//					{
//						//only allowed to reflect once in each cycle with a triangle
//						if (triangleReflection)
//						{
//							continue;
//						}
//						triangleReflection = true;
//
//						glm::vec3 velocityVec;
//						velocityVec = glm::vec3(physicsCurrent.velocity.x, physicsCurrent.velocity.y, physicsCurrent.velocity.z);
//						glm::vec3 reflectedVelocity;
//						reflectedVelocity = glm::reflect(velocityVec, contact.normal);
//
//						//lose some energy while reflecting
//						physicsCurrent.velocity.x = reflectedVelocity.x * 0.9f;
//						physicsCurrent.velocity.y = reflectedVelocity.y * 0.9f;
//						physicsCurrent.velocity.z = reflectedVelocity.z * 0.9f;
//
//						renderCurrent.position.x += reflectedVelocity.x * deltaTime * 0.9f;
//						renderCurrent.position.y += reflectedVelocity.y * deltaTime * 0.9f;
//						renderCurrent.position.z += reflectedVelocity.z * deltaTime * 0.9f;
//					}
//				}
//
//				pCurGO->SetRender(renderCurrent);
//				pCurGO->SetPhysics(physicsCurrent);
//			}
//		}
//	}
//	return;
//}
//
//
////
//void PhysicsStep(float deltaTime)		// 24 Hz  30 Hz  60 Hz  100 Hz 60Hz  50,000 fps
//{
//	std::vector<iGameObject*> gameObjects = gelObjectManager::sharedInstance().GetGameObjects();
//
//	glm::vec3 gravityForce(0.0f, 0.0f, 0.0f);
//
//
//	// Euler... 
//	for (int index = 0; index != gameObjects.size(); index++)
//	{
//		// f = m*a
//		// Velocity = 1 m per second
//		// x = x + velocity*time
//		// v = v + accel*time
//
//		// each second 
//		iGameObject* pCurGO = gameObjects[index];
//
//		if (pCurGO->ObjectType() == "flock")
//		{
//			std::vector<iGameObject*> flockMembers;
//			((cFlock*)pCurGO)->GetMembers(flockMembers);
//			for (int k = 0; k != flockMembers.size(); k++)
//			{
//				iGameObject* member = flockMembers[k];
//
//				RenderInfo renderCurrent = member->GetRender();
//				PhysicsInfo physicsCurrent = member->GetPhysics();
//
//				physicsCurrent.velocity.x += (physicsCurrent.acceleration.x + gravityForce.x) * deltaTime;
//				physicsCurrent.velocity.y += (physicsCurrent.acceleration.y + gravityForce.y) * deltaTime;
//				physicsCurrent.velocity.z += (physicsCurrent.acceleration.z + gravityForce.z) * deltaTime;
//
//				renderCurrent.position.x += physicsCurrent.velocity.x * deltaTime;
//				renderCurrent.position.y += physicsCurrent.velocity.y * deltaTime;
//				renderCurrent.position.z += physicsCurrent.velocity.z * deltaTime;
//
//				// Represents the change over 1 second.
//				glm::quat qRotChangeZeroSeconds;		// No rotational change
//				glm::quat qRotChange(physicsCurrent.rotationalVelocity);
//
//				glm::quat qRotChangeThisFrame = glm::slerp(qRotChangeZeroSeconds,
//					qRotChange,
//					deltaTime);
//
//				renderCurrent.orientation *= qRotChangeThisFrame;
//
//				member->SetRender(renderCurrent);
//				member->SetPhysics(physicsCurrent);
//			}
//			continue;
//		}
//
//		RenderInfo renderCurrent = pCurGO->GetRender();
//		PhysicsInfo physicsCurrent = pCurGO->GetPhysics();
//
//		if (physicsCurrent.enabled)
//		{
//
//			physicsCurrent.velocity.x += (physicsCurrent.acceleration.x + gravityForce.x) * deltaTime;
//			physicsCurrent.velocity.y += (physicsCurrent.acceleration.y + gravityForce.y) * deltaTime;
//			physicsCurrent.velocity.z += (physicsCurrent.acceleration.z + gravityForce.z) * deltaTime;
//
//			renderCurrent.position.x += physicsCurrent.velocity.x * deltaTime;
//			renderCurrent.position.y += physicsCurrent.velocity.y * deltaTime;
//			renderCurrent.position.z += physicsCurrent.velocity.z * deltaTime;
//
//			// Represents the change over 1 second.
//			glm::quat qRotChangeZeroSeconds;		// No rotational change
//			glm::quat qRotChange(physicsCurrent.rotationalVelocity);
//
//			glm::quat qRotChangeThisFrame = glm::slerp(qRotChangeZeroSeconds,
//				qRotChange,
//				deltaTime);
//
//			renderCurrent.orientation *= qRotChangeThisFrame;
//
//			pCurGO->SetRender(renderCurrent);
//			pCurGO->SetPhysics(physicsCurrent);
//
//		}//if ( pCurShip->bIsUpdatedByPhysics )
//	}
//
//	std::vector<cContact> contactVec;
//	GelCollisionManager::sharedInstance().GetContacts(contactVec);
//
//	for (int i = 0; i != contactVec.size(); i++)
//	{
//		cContact contact = contactVec[i];
//		iGameObject* contactObject = gelObjectManager::sharedInstance().FindGameObjectWithID(contact.objectID);
//		PhysicsInfo physics = contactObject->GetPhysics();
//		RenderInfo render = contactObject->GetRender();
//
//		//stop acceleration
//		physics.acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
//
//		//reverse and clamp velocity
//		//physics.velocity *= -0.85f;
//
//		//or even better
//		physics.velocity = glm::reflect(physics.velocity, contact.normal);
//
//		//the after collision behaviour you mentioned at the class
//		physics.rotationalVelocity = glm::vec3(getRandFloat(-1.8f, 1.8f), getRandFloat(-1.8f, 1.8f), getRandFloat(-1.8f, 1.8f));
//
//		render.color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
//
//		contactObject->SetPhysics(physics);
//		contactObject->SetRender(render);
//	}
//
//	return;
//}