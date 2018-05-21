#include "cSetCameraPositionCommand.h"

#include "gelCameraManager.h"

#include "externals.h"

void cSetCameraPositionCommand::SetGameObject(iGameObject * newGameObject)
{
}

void cSetCameraPositionCommand::Init(rapidjson::Value & initJSON)
{
	this->executed = false;

	if (initJSON.HasMember("position"))
	{
		this->eyePositionX = initJSON["position"]["x"].GetFloat();
		this->eyePositionY = initJSON["position"]["y"].GetFloat();
		this->eyePositionZ = initJSON["position"]["z"].GetFloat();
	}
	else
	{
		this->eyePositionX = 0.0f;
		this->eyePositionY = 0.0f;
		this->eyePositionZ = 0.0f;
	}
}

bool cSetCameraPositionCommand::Execute(float deltaTime)
{
	gelCameraManager::sharedInstance().mainCamera->SetEyePosition(glm::vec4(this->eyePositionX, this->eyePositionY, this->eyePositionZ, 1.0f));
	executed = true;
	return executed;
}

bool cSetCameraPositionCommand::IsDone(void)
{
	return executed;
}
