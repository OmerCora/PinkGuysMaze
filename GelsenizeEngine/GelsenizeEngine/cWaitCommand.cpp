#include "cWaitCommand.h"

void cWaitCommand::SetGameObject(iGameObject * newGameObject)
{
	this->gameObject = newGameObject;
}

void cWaitCommand::Init(rapidjson::Value & initJSON)
{
	//total duration
	if (initJSON.HasMember("duration"))
	{
		duration = initJSON["duration"].GetFloat();
	}
	else
	{
		duration = 1.0f;
	}

	durationAcc = duration;

	return;
}

bool cWaitCommand::Execute(float deltaTime)
{
	durationAcc -= deltaTime;
	return true;
}

bool cWaitCommand::IsDone(void)
{
	if (durationAcc <= 0.0f)
	{
		return true;
	}
	return false;
}
