#ifndef _GELCOMMANDFACTORY_HG_
#define _GELCOMMANDFACTORY_HG_

#include "iGameObject.h"

#include "iCommand.h"

class GelCommandFactory
{
public:
	//SIGNLETON
	static GelCommandFactory& sharedInstance()
	{
		static GelCommandFactory instance;
		return instance;
	}

	typedef enum {
		MOVE_OBJECT_withDURATION,
		ROTATE_OBJECT_withDURATION,
		SET_CAMERA_TARGET, //object
		SET_CAMERA_TARGET_STATIC, //vec3
		SET_CAMERA_POSITION,
		WAIT
		//PARTICLE //maybe emitter
	} COMMAND_TYPE;

	iCommand* createCommand(COMMAND_TYPE objectType);
	//iCommand* createCommand(std::string objectType);
private:
	GelCommandFactory() {};
	GelCommandFactory(GelCommandFactory const&); //dont implement - to build
	void operator=(GelCommandFactory const&); //dont implement - to build

};

#endif // !_GELCOMMANDFACTORY_HG_
