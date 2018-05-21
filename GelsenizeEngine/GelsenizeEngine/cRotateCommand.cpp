#include "cRotateCommand.h"

void cRotateCommand::SetGameObject(iGameObject * newGameObject)
{
	this->gameObject = newGameObject;
}

void cRotateCommand::Init(rapidjson::Value & initJSON)
{
	setOrientation = false;
	glm::vec3 objectPosition = this->gameObject->GetRender().position;

	if (initJSON.HasMember("rotationalVelocity"))
	{
		this->rotationalVelocity.x = initJSON["rotationalVelocity"]["x"].GetFloat();
		this->rotationalVelocity.y = initJSON["rotationalVelocity"]["y"].GetFloat();
		this->rotationalVelocity.z = initJSON["rotationalVelocity"]["z"].GetFloat();
	}
	else
	{
		this->rotationalVelocity = glm::vec3(0.0f, 0.0f, 0.0f);
	}

	float duration = 0.0f;

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

	if (duration <= 0.0f)
	{
		setOrientation = true;
		this->finalOrientation = this->rotationalVelocity;
	}
}

bool cRotateCommand::Execute(float deltaTime)
{
	durationAcc -= deltaTime;
	if (durationAcc > 0.0f)
	{
		RenderInfo renderCurrent = this->gameObject->GetRender();
		// Represents the change over 1 second.
		glm::quat rotationalChangeZeroSeconds;		// No rotational change
		glm::quat rotationalChange(this->rotationalVelocity);

		glm::quat rotationalChangeThisFrame = glm::slerp(
			rotationalChangeZeroSeconds,
			rotationalChange,
			deltaTime);

		renderCurrent.orientation *= rotationalChangeThisFrame;

		this->gameObject->SetRender(renderCurrent);

		return true;
	}
	return false;
}

bool cRotateCommand::IsDone(void)
{
	if (this->durationAcc <= 0.0f)
	{
		if (setOrientation)
		{
			RenderInfo render = this->gameObject->GetRender();
			render.orientation = finalOrientation;
			this->gameObject->SetRender(render);
		}
		return true;
	}
	return false;
}
