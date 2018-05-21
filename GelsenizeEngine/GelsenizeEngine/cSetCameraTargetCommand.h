#ifndef _CSETCAMERATARGETCOMMAND_HG_
#define _CSETCAMERATARGETCOMMAND_HG_

#include "iCommand.h"

class cSetCameraTargetCommand : public iCommand
{
public:
	virtual void SetGameObject(iGameObject* newGameObject);
	// Move the object to this point at this speed
	virtual void Init(rapidjson::Value& initJSON);
	virtual bool Execute(float deltaTime);		// Called over and over again
	virtual bool IsDone(void);
private:
	iGameObject* gameObject;
	bool executed;
};


#endif //!_CSETCAMERATARGETCOMMAND_HG_
