#include "gelObjectManager.h"

#include "gelObjectFactory.h"

int gelObjectManager::gameObjectCounter = 0;
int gelObjectManager::GetNewGameObjectID(void)
{
	return this->gameObjectCounter++;
}

//void gelObjectManager::AddParticleEmitter(cParticleEmitter * emitter)
//{
//
//
//	this->vParticles.push_back(emitter);
//	std::vector< cParticle > particles;
//	this->vVecParticlesToRender.push_back(particles);
//}

void gelObjectManager::GetParticleVectorToRender(int objectID, std::vector<cParticle>& vParticle)
{
	// Perform a lookup into the map
	std::map< int, std::vector<cParticle> >::iterator itParticle
		= this->mIDtoParticlesToRender.find(objectID);

	// If the iterator ISN'T set to end(), then it found it
	if (itParticle == this->mIDtoParticlesToRender.end())
	{	// On no! Didn't find it
		return;
	}

	std::vector<cParticle> particleArray = itParticle->second;
	vParticle = particleArray;
	//if ((unsigned int) index < this->vVecParticlesToRender.size())
	//{
	//	vParticle = this->vVecParticlesToRender[index];
	//}
}

iGameObject* gelObjectManager::GetMainPlayer(void)
{
	iGameObject* gameObject = 0;
	for (int index = 0; index != vGameObjects.size(); index++)
	{
		unsigned int objectID = vGameObjects[index]->getObjectID();
		//return the game object with the ID that matches this->mainPlayerID
		if (objectID == mainPlayerID)
		{
			gameObject = vGameObjects[index];
			break;
		}
	}
	return gameObject;
}

iGameObject * gelObjectManager::GetVehicle(void)
{
	iGameObject* gameObject = 0;
	for (int index = 0; index != vGameObjects.size(); index++)
	{
		unsigned int objectID = vGameObjects[index]->getObjectID();
		//return the game object with the ID that matches this->mainPlayerID
		if (objectID == vehicleID)
		{
			gameObject = vGameObjects[index];
			break;
		}
	}
	return gameObject;
}

iGameObject * gelObjectManager::GetSkyboxObject(void)
{
	return this->skyboxObject;
}

void gelObjectManager::SetMainPlayerID(unsigned int playerID)
{
	this->mainPlayerID = playerID;
}

void gelObjectManager::SetVehicleID(unsigned int vehicleID_)
{
	this->vehicleID = vehicleID_;
}

std::vector<iGameObject*> gelObjectManager::GetGameObjects(void)
{
	return this->vGameObjects;
}
std::vector<iWorldObject*> gelObjectManager::GetWorldObjects(void)
{
	return this->vWorldObjects;
}
//
//std::vector<cParticleEmitter*> gelObjectManager::GetParticleEmitters(void)
//{
//	return this->vParticles;
//}

iGameObject * gelObjectManager::FindGameObjectWithID(unsigned int gameObjectID)
{
	for (int index = 0; index != vGameObjects.size(); index++)
	{
		if (vGameObjects[index]->getObjectID() == gameObjectID)
		{	// Found it! 
			return vGameObjects[index];
		}
	}
	// Didn't find it
	return 0;
}

iGameObject* gelObjectManager::FindGameObjectWithObjectName(std::string name)
{
	for (int index = 0; index != vGameObjects.size(); index++)
	{
		if (vGameObjects[index]->GetObjectName() == name)
		{	// Found it! 
			return vGameObjects[index];
		}
	}
	// Didn't find it
	return 0;
}


//int GelObjectManager::GetGameObjectCount(void)
//{
//	return this->vGameObjects.size();
//}

int gelObjectManager::AddGameObject(iGameObject* gameObject)
{
	this->vGameObjects.push_back(gameObject);

	int gameObjectID = gameObject->getObjectID();

	std::vector< cParticle > particles;
	this->mIDtoParticlesToRender[gameObjectID] = particles;

	return gameObjectID;
}

void gelObjectManager::AddWorldObject(iGameObject * gameObject)
{
	this->vWorldObjects.push_back(dynamic_cast<iWorldObject*>(gameObject));
	this->vGameObjects.push_back((gameObject));
}


void gelObjectManager::LoadGameObjectFile(std::string fileName)
{
}

void gelObjectManager::Setup(void)
{
	//create skybox object
	this->skyboxObject = gelObjectFactory::sharedInstance().createGameObject("object");
	MeshInfo mesh = this->skyboxObject->GetMesh();
	RenderInfo render = this->skyboxObject->GetRender();
	mesh.name = "cube_inverted";
	render.scale = 9000.0f;
	this->skyboxObject->SetMesh(mesh);
	this->skyboxObject->SetRender(render);
}

void gelObjectManager::Update(float deltaTime)
{
	UpdatePlayer(deltaTime);
	UpdateGameObjects(deltaTime);



	//Update particles
	//UpdateParticles(deltaTime); //now done inside ugo
}

void gelObjectManager::UpdateGameObjects(float deltaTime)
{
	for (int index = 0; index != this->vGameObjects.size(); index++)
	{
		iGameObject *gameObject = this->vGameObjects[index];
		gameObject->Update(deltaTime);

		MeshInfo mesh = gameObject->GetMesh();
		RenderInfo render = gameObject->GetRender();
		PhysicsInfo physics = gameObject->GetPhysics();

		//tick its skinned mesh animation controller's internal clock
		
		if (mesh.isASkinnedMesh && !render.exploding)
		{
			mesh.animationController->Tick();
		}

		if (physics.collisionBody && physics.collisionBody->GetBodyType() == eBodyType::RIGID_BODY_TYPE)
		{
			iRigidBody *body = dynamic_cast<iRigidBody*>(physics.collisionBody);
			body->SetProperties(render.position, physics.velocity, physics.acceleration, render.orientation, physics.rotationalVelocity);
		}

		if (render.exploding)
		{
			render.explodeTime += deltaTime;
			if (render.explodeTime >= 4.0f)
			{
				render.exploding = false;
				render.explodeTime = 0.0f;
			}
		}
		gameObject->SetRender(render);
		gameObject->SetPhysics(physics);
	}
	int x = 0;
}

//void gelObjectManager::UpdateParticles(float deltaTime)
//{
//	typedef std::map<int, std::vector<cParticle> >::iterator amk;
//	for (amk iterator = mIDtoParticlesToRender.begin(); iterator != mIDtoParticlesToRender.end(); iterator++) {
//		// iterator->first = key
//		// iterator->second = value
//		// Repeat if you also want to iterate through the second map.
//		std::vector<cParticle> particeVector = iterator->second;
//	}
//
///*
//	for (int index = 0; index != this->vParticles.size(); index++)
//	{
//		this->vParticles[index]->Update(deltaTime);
//	}*/
//}

void gelObjectManager::UpdatePlayer(float deltaTime)
{
	this->GetMainPlayer()->Update(deltaTime);
}
