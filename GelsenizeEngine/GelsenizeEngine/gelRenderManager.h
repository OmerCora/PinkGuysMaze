#ifndef _GELRENDERMANAGER_HG_
#define _GELRENDERMANAGER_HG_

#include "iManager.h"
#include "iGameObject.h"

#include "cParticleEmitter.h"
#include <vector>

#include "cSoftBody.h"

#include "cCamera.h"

class gelRenderManager : iManager
{
public:
	//Singleton
	static gelRenderManager& sharedInstance()
	{
		static gelRenderManager instance;
		return instance;
	}

	int getAABBID(void);
	void DrawFullScreenQuad(void);
	void DrawAABB(glm::vec3 position, glm::vec4 colour, float scale);
	virtual void Update(float deltaTime);
	void Setup(void);

	void SetDirectionalShadowPass(bool pass) { this->directionalShadowPass = pass; };

	//~GelRenderManager() {}
private:
	gelRenderManager() {};
	gelRenderManager(gelRenderManager const&); //dont implement - to build
	void operator=(gelRenderManager const&); //dont implement -  to build

	//void DrawScene(void);
	void DrawGameObjects(void);
	void DrawParticle(cParticle particle);
	void DrawPlayer(void);
	void DrawGameObject(iGameObject* gameObject);
	void DrawSkyboxObject(void);
	void DrawParticleEmitter(cParticleEmitter* emitter); //which refers to all particles in an emitter
	
	void DrawCloth(iGameObject* clothObject, cSoftBody* softBody);
	void DrawGameObjectCelShaded(iGameObject* gameObject);

	//a more generic draw method
	void DrawWithInfos(RenderInfo render, MeshInfo mesh, cCamera* camera, glm::mat4x4 initialTransform, bool useSpecialScale = false, glm::vec3 specialScale = glm::vec3(1.0f, 1.0f, 1.0f));

	iGameObject* aabb;
	int aabbID;

	float globalAlpha;
	bool useGlobalAlpha;
	float textureTimeAcc;

	float currentDeltaTime;
	float deltaAcc_1; bool accFlag_1;
	float deltaAcc_2; bool accFlag_2;

	float bonePositionDifference;
	glm::vec3 boneVec_1;
	glm::vec3 boneVec_2;
	glm::vec3 boneVec_3;

	bool directionalShadowPass;
};


#endif // !_GELRENDERMANAGER_HG_