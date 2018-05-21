#include "GelCommandBuilder.h"

#include "GelCommandFactory.h"
#include "managers.h"

iCommand* GelCommandBuilder::createCommand_MoveObjectWithDuration(int objectID, glm::vec3 destination, float duration, float easeInRatio, float easeOutRatio)
{
	rapidjson::Document initDoc;

	rapidjson::Value initData;
	initData.SetObject();

	rapidjson::Value durationVal    (rapidjson::kNumberType);
	rapidjson::Value easeInRatioVal (rapidjson::kNumberType);
	rapidjson::Value easeOutRatioVal(rapidjson::kNumberType);

	//clamp ease in
	if (easeInRatio > 0.3f)
	{
		easeInRatio = 0.3f;
	}
	else if (easeInRatio < 0.0f)
	{
		easeInRatio = 0.0f;
	}

	//clamp ease out
	if (easeOutRatio > 0.3f)
	{
		easeOutRatio = 0.3f;
	}
	else if (easeOutRatio < 0.0f)
	{
		easeOutRatio = 0.0f;
	}

	durationVal    .SetFloat(duration);
	easeInRatioVal .SetFloat(easeInRatio);
	easeOutRatioVal.SetFloat(easeOutRatio);

	rapidjson::Value destinationVal(rapidjson::kObjectType);
	{
		rapidjson::Value destination_x(rapidjson::kNumberType);
		rapidjson::Value destination_y(rapidjson::kNumberType);
		rapidjson::Value destination_z(rapidjson::kNumberType);

		destination_x.SetFloat(destination.x);
		destination_y.SetFloat(destination.y);
		destination_z.SetFloat(destination.z);

		// adding elements to contacts array.
		destinationVal.AddMember("x", destination_x, initDoc.GetAllocator());
		destinationVal.AddMember("y", destination_y, initDoc.GetAllocator());
		destinationVal.AddMember("z", destination_z, initDoc.GetAllocator());
	}
	initData.AddMember("destination",  destinationVal,  initDoc.GetAllocator());
	initData.AddMember("easeInRatio",  easeInRatioVal,  initDoc.GetAllocator());
	initData.AddMember("easeOutRatio", easeOutRatioVal, initDoc.GetAllocator());
	initData.AddMember("duration",     durationVal,     initDoc.GetAllocator());

	iGameObject* object = gelObjectManager::sharedInstance().FindGameObjectWithID(objectID);

	iCommand* command = GelCommandFactory::sharedInstance().createCommand(GelCommandFactory::MOVE_OBJECT_withDURATION);
	command->SetGameObject(object);
	command->Init(initData);

	return command;
}

iCommand * GelCommandBuilder::createCommand_RotateObjectWithDuration(int objectID, glm::vec3 rotationalVelocity, float duration)
{
	rapidjson::Document initDoc;

	rapidjson::Value initData;
	initData.SetObject();

	rapidjson::Value durationVal(rapidjson::kNumberType);

	durationVal.SetFloat(duration);

	rapidjson::Value rotationalVelocityVal(rapidjson::kObjectType);
	{
		rapidjson::Value rotational_x(rapidjson::kNumberType);
		rapidjson::Value rotational_y(rapidjson::kNumberType);
		rapidjson::Value rotational_z(rapidjson::kNumberType);

		rotational_x.SetFloat(rotationalVelocity.x);
		rotational_y.SetFloat(rotationalVelocity.y);
		rotational_z.SetFloat(rotationalVelocity.z);

		// adding elements to contacts array.
		rotationalVelocityVal.AddMember("x", rotational_x, initDoc.GetAllocator());
		rotationalVelocityVal.AddMember("y", rotational_y, initDoc.GetAllocator());
		rotationalVelocityVal.AddMember("z", rotational_z, initDoc.GetAllocator());
	}
	initData.AddMember("rotationalVelocity", rotationalVelocityVal, initDoc.GetAllocator());
	initData.AddMember("duration", durationVal, initDoc.GetAllocator());

	iGameObject* object = gelObjectManager::sharedInstance().FindGameObjectWithID(objectID);

	iCommand* command = GelCommandFactory::sharedInstance().createCommand(GelCommandFactory::ROTATE_OBJECT_withDURATION);
	command->SetGameObject(object);
	command->Init(initData);

	return command;
}

iCommand* GelCommandBuilder::createCommand_WaitObject(int objectID, float duration)
{
	rapidjson::Document initDoc;

	rapidjson::Value initData;
	initData.SetObject();

	rapidjson::Value durationVal(rapidjson::kNumberType);
	durationVal.SetFloat(duration);

	initData.AddMember("duration", durationVal, initDoc.GetAllocator());

	iGameObject* object = gelObjectManager::sharedInstance().FindGameObjectWithID(objectID);

	iCommand* command = GelCommandFactory::sharedInstance().createCommand(GelCommandFactory::WAIT);
	command->SetGameObject(object);
	command->Init(initData);

	return command;
}

iCommand * GelCommandBuilder::createCommand_SetCameraTarget(int objectID)
{
	rapidjson::Value emptyData;

	iGameObject* object = gelObjectManager::sharedInstance().FindGameObjectWithID(objectID);

	iCommand* command = GelCommandFactory::sharedInstance().createCommand(GelCommandFactory::SET_CAMERA_TARGET);
	command->SetGameObject(object);
	command->Init(emptyData);

	return command;
}

iCommand * GelCommandBuilder::createCommand_SetCameraTargetStatic(glm::vec3 position)
{
	rapidjson::Document initDoc;

	rapidjson::Value initData;
	initData.SetObject();

	rapidjson::Value positionVal(rapidjson::kObjectType);
	{
		rapidjson::Value destination_x(rapidjson::kNumberType);
		rapidjson::Value destination_y(rapidjson::kNumberType);
		rapidjson::Value destination_z(rapidjson::kNumberType);

		destination_x.SetFloat(position.x);
		destination_y.SetFloat(position.y);
		destination_z.SetFloat(position.z);

		// adding elements to contacts array.
		positionVal.AddMember("x", destination_x, initDoc.GetAllocator());
		positionVal.AddMember("y", destination_y, initDoc.GetAllocator());
		positionVal.AddMember("z", destination_z, initDoc.GetAllocator());
	}
	initData.AddMember("position", positionVal, initDoc.GetAllocator());

	iCommand* command = GelCommandFactory::sharedInstance().createCommand(GelCommandFactory::SET_CAMERA_TARGET_STATIC);
	command->Init(initData);

	return command;
}

iCommand * GelCommandBuilder::createCommand_SetCameraPosition(glm::vec3 position)
{
	rapidjson::Document initDoc;

	rapidjson::Value initData;
	initData.SetObject();

	rapidjson::Value positionVal(rapidjson::kObjectType);
	{
		rapidjson::Value destination_x(rapidjson::kNumberType);
		rapidjson::Value destination_y(rapidjson::kNumberType);
		rapidjson::Value destination_z(rapidjson::kNumberType);

		destination_x.SetFloat(position.x);
		destination_y.SetFloat(position.y);
		destination_z.SetFloat(position.z);

		// adding elements to contacts array.
		positionVal.AddMember("x", destination_x, initDoc.GetAllocator());
		positionVal.AddMember("y", destination_y, initDoc.GetAllocator());
		positionVal.AddMember("z", destination_z, initDoc.GetAllocator());
	}
	initData.AddMember("position", positionVal, initDoc.GetAllocator());

	iCommand* command = GelCommandFactory::sharedInstance().createCommand(GelCommandFactory::SET_CAMERA_POSITION);
	command->Init(initData);

	return command;
}
