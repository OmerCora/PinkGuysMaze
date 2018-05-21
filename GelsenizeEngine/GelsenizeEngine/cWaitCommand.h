#ifndef _CWAITCOMMAND_HG_
#define _CWAITCOMMAND_HG_

#include "iCommand.h"

class cWaitCommand : public iCommand
{
public:
	virtual void SetGameObject(iGameObject* newGameObject);
	// Move the object to this point at this speed
	virtual void Init(rapidjson::Value& initJSON);
	virtual bool Execute(float deltaTime);		// Called over and over again
	virtual bool IsDone(void);
private:
	float duration;
	float durationAcc;
	iGameObject* gameObject;
};


#endif //!_CWAITCOMMAND_HG_
