#ifndef _GELPHYSICSMANAGER_HG_
#define _GELPHYSICSMANAGER_HG_

#include <string>
#include <map>
#include "CTextureFromBMP.h"
#include "iManager.h"

//Physics 2
#include "iPhysicsWorld.h"
#include "cPhysicsFactory.h"
#include "bulletPhysicsFactory.h"

class GelPhysicsManager : public iManager
{
public:
	//Singleton 
	static GelPhysicsManager & sharedInstance()
	{
		static GelPhysicsManager  instance;
		return instance;
	}

	iPhysicsWorld* GetWorld(void) { return this->world; };
	iPhysicsFactory* GetFactory(void) { return this->factory; };

	virtual void Update(float deltaTime);
	void Setup(void);

private:
	GelPhysicsManager() {};
	GelPhysicsManager(GelPhysicsManager  const&); //dont implement - to build
	void operator=(GelPhysicsManager  const&); //dont implement - to build

	nPhysics::iPhysicsWorld* world;
	nPhysics::iPhysicsFactory* factory;
};

#endif
