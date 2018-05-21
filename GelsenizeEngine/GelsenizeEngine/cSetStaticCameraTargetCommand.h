#ifndef _CSETSTATICCAMERAPOSITIONCOMMAND_HG_
#define _CSETSTATICCAMERAPOSITIONCOMMAND_HG_

#include "iCommand.h"

class cSetStaticCameraTargetCommand : public iCommand
{
public:
	virtual void SetGameObject(iGameObject* newGameObject);
	// Move the object to this point at this speed
	virtual void Init(rapidjson::Value& initJSON);
	virtual bool Execute(float deltaTime);		// Called over and over again
	virtual bool IsDone(void);
private:
	bool executed;
	float atPositionX;
	float atPositionY;
	float atPositionZ;
};


#endif //!_CSETCAMERATARGETCOMMAND_HG_
