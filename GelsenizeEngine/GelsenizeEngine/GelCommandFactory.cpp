#include "GelCommandFactory.h"

#include "cMoveCommand.h"
#include "cWaitCommand.h"
#include "cRotateCommand.h"
#include "cSetCameraTargetCommand.h"
#include "cSetStaticCameraTargetCommand.h"
#include "cSetCameraPositionCommand.h"

iCommand* GelCommandFactory::createCommand(COMMAND_TYPE objectType)
{
	iCommand* command = 0;
	if (objectType == COMMAND_TYPE::MOVE_OBJECT_withDURATION)
	{
		command = new cMoveCommand();
	}
	else if (objectType == COMMAND_TYPE::WAIT)
	{
		command = new cWaitCommand();
	}
	else if (objectType == COMMAND_TYPE::SET_CAMERA_TARGET)
	{
		command = new cSetCameraTargetCommand();
	}
	else if (objectType == COMMAND_TYPE::SET_CAMERA_POSITION)
	{
		command = new cSetCameraPositionCommand();
	}
	else if (objectType == COMMAND_TYPE::ROTATE_OBJECT_withDURATION)
	{
		command = new cRotateCommand();
	}
	else if (objectType == COMMAND_TYPE::SET_CAMERA_TARGET_STATIC)
	{
		command = new cSetStaticCameraTargetCommand();
	}
	return command;
}

//iCommand * GelCommandFactory::createCommand(std::string objectType)
//{
//	return nullptr;
//}
