#include "cSetCameraTargetCommand.h"

#include "gelCameraManager.h"

void cSetCameraTargetCommand::SetGameObject(iGameObject * newGameObject)
{
	this->gameObject = newGameObject;
}

void cSetCameraTargetCommand::Init(rapidjson::Value & initJSON)
{
	executed = false;
}

bool cSetCameraTargetCommand::Execute(float deltaTime)
{
	if (this->gameObject)
	{
		gelCameraManager::sharedInstance().mainCamera->SetCameraMode(cCamera::MODE::FOLLOWING_TARGET);
		gelCameraManager::sharedInstance().mainCamera->SetTargetObject(this->gameObject);
	}

	executed = true;

	return executed;
}

bool cSetCameraTargetCommand::IsDone(void)
{
	return executed;
}
