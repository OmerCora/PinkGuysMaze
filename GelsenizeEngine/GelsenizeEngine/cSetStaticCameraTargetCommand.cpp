#include "cSetStaticCameraTargetCommand.h"

#include "gelCameraManager.h"

#include "externals.h"

void cSetStaticCameraTargetCommand::SetGameObject(iGameObject * newGameObject)
{
}

void cSetStaticCameraTargetCommand::Init(rapidjson::Value & initJSON)
{
	this->executed = false;

	if (initJSON.HasMember("position"))
	{
		this->atPositionX = initJSON["position"]["x"].GetFloat();
		this->atPositionY = initJSON["position"]["y"].GetFloat();
		this->atPositionZ = initJSON["position"]["z"].GetFloat();
	}
	else
	{
		this->atPositionX = 0.0f;
		this->atPositionY = 0.0f;
		this->atPositionZ = 0.0f;
	}
}

bool cSetStaticCameraTargetCommand::Execute(float deltaTime)
{
	gelCameraManager::sharedInstance().mainCamera->SetCameraMode(cCamera::MODE::STATIONARY);
	gelCameraManager::sharedInstance().mainCamera->SetAtPosition(glm::vec3(this->atPositionX, this->atPositionY, this->atPositionZ));
	executed = true;
	return executed;
}

bool cSetStaticCameraTargetCommand::IsDone(void)
{
	return executed;
}
