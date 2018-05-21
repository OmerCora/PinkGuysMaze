#ifndef _CSETCAMERAPOSITIONCOMMAND_HG_
#define _CSETCAMERAPOSITIONCOMMAND_HG_

#include "iCommand.h"

class cSetCameraPositionCommand : public iCommand
{
public:
	virtual void SetGameObject(iGameObject* newGameObject);
	// Move the object to this point at this speed
	virtual void Init(rapidjson::Value& initJSON);
	virtual bool Execute(float deltaTime);		// Called over and over again
	virtual bool IsDone(void);
private:
	bool executed;
	float eyePositionX;
	float eyePositionY;
	float eyePositionZ;
};


#endif //!_CSETCAMERATARGETCOMMAND_HG_
