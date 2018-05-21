#include "cMoveCommand.h"

#include <iostream>
#include "externals.h"

//JSON
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <iostream>

#include "managers.h"

#include "OCHelper.h"


void cMoveCommand::SetGameObject(iGameObject * newGameObject)
{
	this->gameObject = newGameObject;
}


void cMoveCommand::Init(rapidjson::Value &initJSON)
{
	setPosition = false;
	calculated = false;
	glm::vec3 objectPosition = this->gameObject->GetRender().position;

	if (initJSON.HasMember("destination"))
	{
		this->destination.x = initJSON["destination"]["x"].GetFloat();
		this->destination.y = initJSON["destination"]["y"].GetFloat();
		this->destination.z = initJSON["destination"]["z"].GetFloat();

		/*for (rapidjson::SizeType i = 0; i < initJSON["destination"].Size(); i++)
		{
			
		}*/
	}
	else 
	{
		this->destination = objectPosition;
	}

	float totalDistance = glm::distance(this->destination, objectPosition);
	float easeInRatio = 0.0f;
	float easeOutRatio = 0.0f;

	//total duration
	if (initJSON.HasMember("duration"))
	{
		duration = initJSON["duration"].GetFloat();
	}
	else
	{
		duration = 1.0f;
	}

	// percentage of total duration 0.1f means 10% of total duration
	if (initJSON.HasMember("easeInRatio"))
	{
		easeInRatio = initJSON["easeInRatio"].GetFloat();
		this->easeInDuration = easeInRatio * duration;
	}
	else
	{
		this->easeInDuration = 0.01f;
	}

	// percentage of total duration 0.1f means 10% of total duration
	if (initJSON.HasMember("easeOutRatio"))
	{
		easeOutRatio = initJSON["easeOutRatio"].GetFloat();
		this->easeOutDuration = easeOutRatio * duration; 
	}
	else
	{
		this->easeOutDuration = 0.01f;
	}

	if (duration <= 0.0f)
	{
		setPosition = true;
	}

	return;
}

bool cMoveCommand::Execute(float deltaTime)	// Called over and over again
{	
	//
	if (!calculated)
	{
		this->duration -= (easeInDuration + easeOutDuration); //subtract ease in/out durations

		durationAcc = duration;
		easeInDurationAcc = easeInDuration;
		easeOutDurationAcc = easeOutDuration;

		glm::vec3 objectPosition = this->gameObject->GetRender().position;
		float totalDistance = glm::distance(this->destination, objectPosition);
		//magic math //integral of 2 smootherstep + constant speed movement
		speed = totalDistance / (easeInDuration / 2.0f + easeOutDuration / 2.0f + this->duration);

		//find velocity
		velocity = glm::normalize(destination - objectPosition) * speed;
		calculated = true;
	}
	//

	float deltaTimeInSeconds = deltaTime;

	//check object speed first in reverse order, 
	  //does current speed it enough s need to slow down?
	  //does it already move at max_speed
	
	// It could be that the physics updater would handle this movement
	// Or this is where you might do the ease in and ease out... 

	PhysicsInfo objectPhysics = this->gameObject->GetPhysics();

	if (easeInDurationAcc > 0.0f)
	{
		easeInDurationAcc -= deltaTimeInSeconds; //deplete easeIn

		glm::vec3 desiredVelocity = velocity;

		// Calculate the relative speeds for each axis (could be moving fast on one axis and slow on another)
		//	using the smooth step calcualtion (GT max distance = max speed, LT min distance = 0);
		float velStep;
		velStep = OCHelper::smootherStep(0.0f, easeInDuration, easeInDuration - easeInDurationAcc);
		//velStep.y = 1 - OCHelper::smootherStep(0.0f, velocity.y, objectPhysics.velocity.y);
		//velStep.z = 1 - OCHelper::smootherStep(0.0f, velocity.z, objectPhysics.velocity.z);

		// Scale for time step
		//velStep *= speed * deltaTimeInSeconds;
		//velStep.y *= speed * deltaTime;
		//velStep.z *= speed * deltaTime;

		// If camera is LT position, reverse the moving direction (it's positive because of ABS distance)
		//if (velocity < objectPhysics.velocity) { velStep.x = -velStep.x; }
		//if (velocity.y < objectPhysics.velocity.y) { velStep.y = -velStep.y; }
		//if (velocity.z < objectPhysics.velocity.z) { velStep.z = -velStep.z; }

		objectPhysics.velocity = glm::normalize(velocity) * velStep * speed;
		//objectPhysics.velocity.y += velStep.y;
		//objectPhysics.velocity.z += velStep.z;

		this->gameObject->SetPhysics(objectPhysics);

	}
	else if (durationAcc > 0.0f) //constant speed
	{
		durationAcc -= deltaTimeInSeconds;

		glm::vec3 desiredVelocity = velocity;
		
		objectPhysics.velocity = desiredVelocity;

		this->gameObject->SetPhysics(objectPhysics);
	}
	else if (easeOutDurationAcc > 0.0f)
	{
		easeOutDurationAcc -= deltaTimeInSeconds;

		glm::vec3 desiredVelocity = velocity;

		// Calculate the relative speeds for each axis (could be moving fast on one axis and slow on another)
		//	using the smooth step calcualtion (GT max distance = max speed, LT min distance = 0);
		float velStep;
		velStep = 1 - OCHelper::smootherStep(0.0f, easeOutDuration, easeOutDuration - easeOutDurationAcc);
		//velStep.y = 1 - OCHelper::smootherStep(0.0f, velocity.y, objectPhysics.velocity.y);
		//velStep.z = 1 - OCHelper::smootherStep(0.0f, velocity.z, objectPhysics.velocity.z);

		// Scale for time step
		//velStep *= speed * deltaTimeInSeconds;
		//velStep.y *= speed * deltaTime;
		//velStep.z *= speed * deltaTime;

		// If camera is LT position, reverse the moving direction (it's positive because of ABS distance)
		//if (velocity < objectPhysics.velocity) { velStep.x = -velStep.x; }
		//if (velocity.y < objectPhysics.velocity.y) { velStep.y = -velStep.y; }
		//if (velocity.z < objectPhysics.velocity.z) { velStep.z = -velStep.z; }

		objectPhysics.velocity = glm::normalize(velocity) * velStep * speed;
		//objectPhysics.velocity += velStep;
		//objectPhysics.velocity.y += velStep.y;
		//objectPhysics.velocity.z += velStep.z;

		this->gameObject->SetPhysics(objectPhysics);

	}
	else
	{
		
	}

	return true;
}

bool cMoveCommand::IsDone(void)
{
	if (!calculated)
	{
		this->duration -= (easeInDuration + easeOutDuration); //subtract ease in/out durations

		durationAcc = duration;
		easeInDurationAcc = easeInDuration;
		easeOutDurationAcc = easeOutDuration;

		glm::vec3 objectPosition = this->gameObject->GetRender().position;
		float totalDistance = glm::distance(this->destination, objectPosition);
		//magic math //integral of 2 smootherstep + constant speed movement
		speed = totalDistance / (easeInDuration / 2.0f + easeOutDuration / 2.0f + this->duration);

		//find velocity
		velocity = glm::normalize(destination - objectPosition) * speed;
		calculated = true;
	}

	float distanceLeftToTravel = glm::length(this->destination - this->gameObject->GetRender().position);
	if (distanceLeftToTravel < 0.1f || (easeOutDurationAcc+durationAcc+easeInDurationAcc)<=0.0f)
	{
		glm::vec3 desiredVelocity = glm::vec3(0.0f,0.0f,0.0f);
		PhysicsInfo objectPhysics = this->gameObject->GetPhysics();
		objectPhysics.velocity = desiredVelocity;
		this->gameObject->SetPhysics(objectPhysics);

		if (setPosition)
		{
			RenderInfo render = this->gameObject->GetRender();
			render.position = this->destination;
			this->gameObject->SetRender(render);
		}

		return true;
	}
	return false;
}
