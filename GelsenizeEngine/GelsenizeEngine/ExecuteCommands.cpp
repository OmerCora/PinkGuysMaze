#include "ExecuteCommands.h"

void ExecuteCommands_beforeGameLoop()
{
	{
		cCommandGroup commandGroup;

		iCommand *initCamera = GelCommandBuilder::sharedInstance().createCommand_SetCameraTargetStatic(glm::vec3(5.0f, 5.0f, 5.0f));
		commandGroup.AddSerialCommand(initCamera);

		iCommand *positionCamera = GelCommandBuilder::sharedInstance().createCommand_SetCameraPosition(glm::vec3(5.0f, 5.0f, 0.0f));
		commandGroup.AddSerialCommand(positionCamera);

		iGameObject *podObject = gelObjectManager::sharedInstance().FindGameObjectWithObjectName("pod");
		int podID = podObject->getObjectID();

		iGameObject *odyObject = gelObjectManager::sharedInstance().FindGameObjectWithObjectName("odyssey");
		int odyID = odyObject->getObjectID();

		iGameObject *frankObject = gelObjectManager::sharedInstance().FindGameObjectWithObjectName("frank");
		int frankID = frankObject->getObjectID();

		iCommand* moveCommand_init1 = GelCommandBuilder::sharedInstance().createCommand_MoveObjectWithDuration(podID, glm::vec3(3.84, 5.67, 2.70f), 0.0f, 0.0f, 0.0f);
		commandGroup.AddSerialCommand(moveCommand_init1);
		iCommand* moveCommand_init2 = GelCommandBuilder::sharedInstance().createCommand_MoveObjectWithDuration(odyID, glm::vec3(10.5f, 5.66f, 18.5f), 0.0f, 0.0f, 0.0f);
		commandGroup.AddSerialCommand(moveCommand_init2);
		iCommand* moveCommand_init3 = GelCommandBuilder::sharedInstance().createCommand_MoveObjectWithDuration(frankID, glm::vec3(4.0, 5.55f, 7.0f), 0.0f, 0.0f, 0.0f);
		commandGroup.AddSerialCommand(moveCommand_init3);

		iCommand* rotateCommand_init1 = GelCommandBuilder::sharedInstance().createCommand_RotateObjectWithDuration(podID, glm::vec3(2.0f, 3.0f, -0.4f), 0.0f);
		commandGroup.AddSerialCommand(rotateCommand_init1);
		iCommand* rotateCommand_init2 = GelCommandBuilder::sharedInstance().createCommand_RotateObjectWithDuration(odyID, glm::vec3(-1.2f, 0.0f, -1.0f), 0.0f);
		commandGroup.AddSerialCommand(rotateCommand_init2);
		iCommand* rotateCommand_init3 = GelCommandBuilder::sharedInstance().createCommand_RotateObjectWithDuration(frankID, glm::vec3(0.0f, 0.0f, 0.0f), 0.0f);
		commandGroup.AddSerialCommand(rotateCommand_init3);

		/*iCommand* waitCommand = GelCommandBuilder::sharedInstance().createCommand_WaitObject(objectID, 3.0f);
		commandGroup.AddSerialCommand(waitCommand);

		iCommand* moveCommand2 = GelCommandBuilder::sharedInstance().createCommand_MoveObjectWithDuration(podObject->getObjectID(), glm::vec3(10.0f, 4.0f, 15.0f), 5, 0.1f, 0.1f);
		commandGroup.AddSerialCommand(moveCommand2);

		iCommand* rotateCommand = GelCommandBuilder::sharedInstance().createCommand_RotateObjectWithDuration(podObject->getObjectID(), glm::vec3(0.0f, 0.65f, 0.0f), 2.0f);
		commandGroup.AddSerialCommand(rotateCommand);

		iCommand* moveCommand3 = GelCommandBuilder::sharedInstance().createCommand_MoveObjectWithDuration(podObject->getObjectID(), glm::vec3(20.0f, 4.0f, 15.0f), 5, 0.1f, 0.1f);
		commandGroup.AddSerialCommand(moveCommand3);*/

		GelCommandManager::sharedInstance().AddCommandGroup(commandGroup);
	}
}

void ExecuteCommands_scene1()
{
	//hmm missing keyframes

	iGameObject *podObject = gelObjectManager::sharedInstance().FindGameObjectWithObjectName("pod");
	int podID = podObject->getObjectID();

	iGameObject *odyObject = gelObjectManager::sharedInstance().FindGameObjectWithObjectName("odyssey");
	int odyID = odyObject->getObjectID();

	iGameObject *frankObject = gelObjectManager::sharedInstance().FindGameObjectWithObjectName("frank");
	int frankID = frankObject->getObjectID();

	cCommandGroup commandGroup2;
	{
		iCommand* moveCommand = GelCommandBuilder::sharedInstance().createCommand_MoveObjectWithDuration(frankID, glm::vec3(10.5f, 6.66f, 25.0f), 20.0f, 0.3f, 0.3f);
		commandGroup2.AddParallelCommand(moveCommand);

		iCommand* rotateCommand = GelCommandBuilder::sharedInstance().createCommand_RotateObjectWithDuration(frankID, glm::vec3(0.0f, 0.1f, -0.2f), 10.0f);
		commandGroup2.AddParallelCommand(rotateCommand);

		iCommand* waitRotate = GelCommandBuilder::sharedInstance().createCommand_WaitObject(podID, 5.0f);
		commandGroup2.AddSerialCommand(waitRotate);

		iCommand* rotateCommand2 = GelCommandBuilder::sharedInstance().createCommand_RotateObjectWithDuration(podID, glm::vec3(0.0f, 0.0f, -0.6f), 4.0f);
		commandGroup2.AddSerialCommand(rotateCommand2);

		iCommand* cameraCut = GelCommandBuilder::sharedInstance().createCommand_SetCameraPosition(glm::vec3(3.84, 5.67, 6.70f));
		commandGroup2.AddSerialCommand(cameraCut);

		iCommand* cameraCutTarget = GelCommandBuilder::sharedInstance().createCommand_SetCameraTargetStatic(glm::vec3(3.84, 5.67, 2.70f));
		commandGroup2.AddSerialCommand(cameraCutTarget);

		iCommand* rotateCommand3 = GelCommandBuilder::sharedInstance().createCommand_RotateObjectWithDuration(podID, glm::vec3(0.0f, 0.0f, -0.6f), 1.0f);
		commandGroup2.AddSerialCommand(rotateCommand3);

		iCommand* movePodCommand = GelCommandBuilder::sharedInstance().createCommand_MoveObjectWithDuration(podID, glm::vec3(3.88, 5.70, 10.70f), 12.0f, 0.3f, 0.3f);
		commandGroup2.AddSerialCommand(movePodCommand);

		GelCommandManager::sharedInstance().AddCommandGroup(commandGroup2);
	}
}

void ExecuteCommands_scene2()
{
	iGameObject *podObject = gelObjectManager::sharedInstance().FindGameObjectWithObjectName("pod");
	int podID = podObject->getObjectID();

	iGameObject *odyObject = gelObjectManager::sharedInstance().FindGameObjectWithObjectName("odyssey");
	int odyID = odyObject->getObjectID();

	iGameObject *frankObject = gelObjectManager::sharedInstance().FindGameObjectWithObjectName("frank");
	int frankID = frankObject->getObjectID();

	{
		cCommandGroup commandGroup;

		cCommandGroup subGroup0;
		cCommandGroup subGroup1;
		cCommandGroup subGroup2;

		iCommand *initCamera = GelCommandBuilder::sharedInstance().createCommand_SetCameraTargetStatic(glm::vec3(5.5f, 6.66f, 15.0f));
		subGroup0.AddSerialCommand(initCamera);

		iCommand *positionCamera = GelCommandBuilder::sharedInstance().createCommand_SetCameraPosition(glm::vec3(5.5f, 6.66f, 25.0f));
		subGroup0.AddSerialCommand(positionCamera);

		iGameObject *podObject = gelObjectManager::sharedInstance().FindGameObjectWithObjectName("pod");
		int podID = podObject->getObjectID();

		iGameObject *odyObject = gelObjectManager::sharedInstance().FindGameObjectWithObjectName("odyssey");
		int odyID = odyObject->getObjectID();

		iGameObject *frankObject = gelObjectManager::sharedInstance().FindGameObjectWithObjectName("frank");
		int frankID = frankObject->getObjectID();

		iCommand* moveCommand_init1 = GelCommandBuilder::sharedInstance().createCommand_MoveObjectWithDuration(podID, glm::vec3(-6.0, 5.67, 8.70f), 0.0f, 0.0f, 0.0f);
		subGroup0.AddSerialCommand(moveCommand_init1);
		iCommand* moveCommand_init2 = GelCommandBuilder::sharedInstance().createCommand_MoveObjectWithDuration(odyID, glm::vec3(-10000, -10000, -10000), 0.0f, 0.0f, 0.0f);
		subGroup0.AddSerialCommand(moveCommand_init2);
		iCommand* moveCommand_init3 = GelCommandBuilder::sharedInstance().createCommand_MoveObjectWithDuration(frankID, glm::vec3(6.0f, 6.0f, 6.0f), 0.0f, 0.0f, 0.0f);
		subGroup0.AddSerialCommand(moveCommand_init3);

		iCommand* rotateCommand_init1 = GelCommandBuilder::sharedInstance().createCommand_RotateObjectWithDuration(podID, glm::vec3(2.0f, 3.0f, -0.4f), 0.0f);
		subGroup0.AddSerialCommand(rotateCommand_init1);
		iCommand* rotateCommand_init2 = GelCommandBuilder::sharedInstance().createCommand_RotateObjectWithDuration(odyID, glm::vec3(-1.2f, 0.0f, -1.0f), 0.0f);
		subGroup0.AddSerialCommand(rotateCommand_init2);
		iCommand* rotateCommand_init3 = GelCommandBuilder::sharedInstance().createCommand_RotateObjectWithDuration(frankID, glm::vec3(0.0f, 0.0f, 0.0f), 0.0f);
		subGroup0.AddSerialCommand(rotateCommand_init3);
		iCommand* wait0 = GelCommandBuilder::sharedInstance().createCommand_WaitObject(podID, 1.0f);
		subGroup0.AddSerialCommand(wait0);

		iCommand* moveCommand = GelCommandBuilder::sharedInstance().createCommand_MoveObjectWithDuration(frankID, glm::vec3(5.5f, 6.66f, 15.0f), 3.0f, 0.0f, 0.0f);
		subGroup1.AddParallelCommand(moveCommand);

		iCommand* moveCommand2 = GelCommandBuilder::sharedInstance().createCommand_MoveObjectWithDuration(podID, glm::vec3(5.5f, 6.66f, 15.0f), 3.0f, 0.0f, 0.0f);
		subGroup1.AddParallelCommand(moveCommand2);

		//subgroup 2


		iCommand* moveCommand5 = GelCommandBuilder::sharedInstance().createCommand_MoveObjectWithDuration(frankID, glm::vec3(25.5f, -16.66f, 55.0f), 15.0f, 0.0f, 0.21f);
		subGroup2.AddParallelCommand(moveCommand5);

		iCommand* moveCommand6 = GelCommandBuilder::sharedInstance().createCommand_MoveObjectWithDuration(podID, glm::vec3(-20.0f, 3.66f, 60.0f), 15.0f, 0.0f, 0.21f);
		subGroup2.AddParallelCommand(moveCommand6);

		iCommand* rotateCommand1 = GelCommandBuilder::sharedInstance().createCommand_RotateObjectWithDuration(frankID, glm::vec3(0.2f, 1.0f, -0.4f), 15.0f);
		subGroup2.AddParallelCommand(rotateCommand1);

		iCommand* rotateCommand2 = GelCommandBuilder::sharedInstance().createCommand_RotateObjectWithDuration(podID, glm::vec3(-0.3f, 0.1f, -0.2f), 15.0f);
		subGroup2.AddParallelCommand(rotateCommand2);

		commandGroup.AddCommandGroup(subGroup0);
		commandGroup.AddCommandGroup(subGroup1);
		commandGroup.AddCommandGroup(subGroup2);

		GelCommandManager::sharedInstance().AddCommandGroup(commandGroup);
	}
}
