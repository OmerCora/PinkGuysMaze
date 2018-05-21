#include "GelCommandManager.h"

void GelCommandManager::AddCommandGroup(cCommandGroup CG)
{
	this->commandGroupQueue.push(CG);
	this->commandGroupVector.push_back(CG);
}

void GelCommandManager::Update(float deltaTime)
{
	//serial execution
	// Pick the "top" command group, if there is one 
	if (!this->commandGroupQueue.empty())
	{
		this->commandGroupQueue.front().Update(deltaTime);
		// Is command group done? 
		if (this->commandGroupQueue.front().IsDone())
		{
			this->commandGroupQueue.pop();
		}
	}


	//for (int i = 0; i != commandGroupVector.size(); i++)
	//{
	//	// Is command active?
	//	cCommandGroup currentGroup = commandGroupVector[i];
	//	if (!currentGroup.IsDone())
	//	{	// There's at least one active parallel command
	//		//anotherCommandActive = true;
	//		currentGroup.Update(deltaTime);
	//	}
	//}

	return;
}

void GelCommandManager::FlushCommands(void)
{
	std::queue<cCommandGroup> empty;
	std::swap(commandGroupQueue, empty);
	commandGroupVector.clear();
}
