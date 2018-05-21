#ifndef _CROTATECOMMAND_HG_
#define _CROTATECOMMAND_HG_

#include "iCommand.h"

class cRotateCommand : public iCommand
{
public:
	virtual void SetGameObject(iGameObject* newGameObject);
	// Move the object to this point at this speed
	virtual void Init(rapidjson::Value& initJSON);
	virtual bool Execute(float deltaTime);		// Called over and over again
	virtual bool IsDone(void);
private:
	glm::vec3 rotationalVelocity;
	float duration;
	float durationAcc;
	glm::quat finalOrientation;
	bool setOrientation;
	iGameObject* gameObject;
};


#endif
