#ifndef _GELOBJECTMANAGER_HG_
#define _GELOBJECTMANAGER_HG_

#include "iManager.h"
#include "iGameObject.h"
#include <vector>
#include <string>
#include <map>
#include "iWorldObject.h"

#include "cParticleEmitter.h"

class gelObjectManager : iManager
{
public:
	//Singleton  
	static gelObjectManager& sharedInstance()
	{
		static gelObjectManager instance;
		return instance;
	}

	void Setup(void);
	virtual void Update(float deltaTime);

	std::vector< iGameObject* > GetGameObjects(void);
	std::vector< iWorldObject* > GetWorldObjects(void);
	//std::vector< cParticleEmitter* > GetParticleEmitters(void);
	iGameObject* FindGameObjectWithID(unsigned int gameObjectID);
	iGameObject* FindGameObjectWithObjectName(std::string name);

	//convinience
	int AddGameObject(iGameObject* gameObject);
	void AddWorldObject(iGameObject * gameObject);
	int GetNewGameObjectID(void);

	//void AddParticleEmitter(cParticleEmitter * emitter);
	void GetParticleVectorToRender(int objectID, std::vector<cParticle> &vParticle);
	//player
	iGameObject* GetMainPlayer(void);
	iGameObject* GetVehicle(void);
	iGameObject* GetSkyboxObject(void);
	void SetMainPlayerID(unsigned int playerID);
	void SetVehicleID(unsigned int vehicleID);

	//maybe abstract this later
	void LoadGameObjectFile(std::string fileName);

private:
	gelObjectManager() {};
	gelObjectManager(gelObjectManager const&); //dont implement - to build
	void operator=(gelObjectManager const&); //dont implement - to build

	std::vector< iGameObject* > vGameObjects;
	//std::vector< cParticleEmitter* > vParticles;
	std::vector< iWorldObject* > vWorldObjects;

	void UpdateGameObjects(float deltaTime);
	void UpdatePlayer(float deltaTime);

	//particles
	//void UpdateParticles(float deltaTime);
	std::vector< std::vector< cParticle > > vVecParticlesToRender;
	std::map <int, std::vector< cParticle > > mIDtoParticlesToRender;
	iGameObject *skyboxObject;

	unsigned int mainPlayerID;
	unsigned int vehicleID;
	static int gameObjectCounter;
};


#endif // !_GELOBJECTMANAGER_
