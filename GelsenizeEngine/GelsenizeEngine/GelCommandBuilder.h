#ifndef _GELCOMMANDBUILDER_HG_
#define _GELCOMMANDBUILDER_HG_

#include "iCommand.h"
#include "externals.h"

class GelCommandBuilder
{
public:
	//SIGNLETON
	static GelCommandBuilder& sharedInstance()
	{
		static GelCommandBuilder instance;
		return instance;
	}

	//duration is in seconds, ratios 0.0 to 0.5, 0.1 easeIn meaning 10% of duration spent in easeIn 
	iCommand* createCommand_MoveObjectWithDuration(int objectID, glm::vec3 destination, float duration, float easeInRatio, float easeOutRatio);
	iCommand* createCommand_RotateObjectWithDuration(int objectID, glm::vec3 rotationalVelocity, float duration);
	iCommand* createCommand_WaitObject(int objectID, float duration);
	iCommand* createCommand_SetCameraTarget(int objectID);
	iCommand* createCommand_SetCameraTargetStatic(glm::vec3 position);
	iCommand* createCommand_SetCameraPosition(glm::vec3 position);

private:
	GelCommandBuilder() {};
	GelCommandBuilder(GelCommandBuilder const&); //dont implement - to build
	void operator=(GelCommandBuilder const&); //dont implement - to build

};

#endif // !_GELCOMMANDBUILDER_HG_
