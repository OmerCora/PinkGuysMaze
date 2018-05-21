#include "LuaBrain.h"

lua_State* L;
std::map<std::string, std::vector<cCommandGroup> > luaCommandGroups;
cCommandGroup currentCommandGroup;

lua_State* luaStart(void)
{
	L = luaL_newstate();		/* opens Lua 5.3.3 */
	if (L == NULL)
	{	// Out of memory
		return false;
	}
	// No way to check if this is working or not
	luaL_openlibs(L);					/* Lua 5.3.3 */

	return L;
}


void luaShutDown(void)
{
	// Note: if you shut down the thing with ctrl-c, then
	//	you will likely get an exception here, which 
	//	can be ignored. Keep in mind that you won't be 
	//	doing this kind of shutdown, normally.
	lua_close(L);
	return;
}

std::string decodeLuaErrorToString(int error)
{
	switch (error)
	{
	case 0:
		return "Lua: no error";
		break;
	case LUA_ERRSYNTAX:
		return "Lua: syntax error";
		break;
	case LUA_ERRMEM:
		return "Lua: memory allocation error";
		break;
	case LUA_ERRRUN:
		return "Lua: Runtime error";
		break;
	case LUA_ERRERR:
		return "Lua: Error while running the error handler function";
		break;
	}//switch ( error )

	 // Who knows what this error is?
	return "Lua: UNKNOWN error";
}


int GelsenizeLuaShutDown(void)
{
	luaShutDown();
	return 1;
}

int GelsenizeLuaStartUp(void)
{
	L = luaStart();
	if (L == NULL)
	{
		std::cout << "All is lost, forever LOST!! (because Lua didn't start)" << std::endl;
		return -1;
	}

	int error = 0;
	char buffer[256] = { 0 };

	std::cout << "Lua interpreter has started. crtl-C or crtl-Z+enter to exit." << std::endl;
	std::cout << "------------------------------------------------------------" << std::endl;

	lua_register(L, "InitCommandGroup", InitCommandGroup);
	lua_register(L, "SubmitCommandGroup", SubmitCommandGroup);
	lua_register(L, "MoveTo", MoveTo);
	lua_register(L, "GetObjectPosition", GetObjectPosition);
	lua_register(L, "LookAtOnce", LookAtOnce);
	lua_register(L, "FollowObject", SetCameraTarget);
	lua_register(L, "PlaceCameraAt", SetCameraPosition);
	lua_register(L, "Wait", DelayCommandChain);
	lua_register(L, "RotateObject", RotateObject);

	std::ifstream inputFile("resources/gameFile/gameFile.lua");

	if (!inputFile) {
		std::cout << "Cannot open input file.\n";
		return 1;
	}

	while (inputFile)
	{
		inputFile.getline(buffer, 255);
		if (inputFile) std::cout << buffer << std::endl; //default \n

		error = luaL_loadbuffer(L, buffer, strlen(buffer), "line");
		if (error != 0 /*no error*/)
		{
			std::cout << "Lua: There was an error..." << std::endl;
			std::cout << decodeLuaErrorToString(error) << std::endl;
			continue;
		}

		// execute funtion in "protected mode", where problems are 
		//  caught and placed on the stack for investigation
		error = lua_pcall(L,	/* lua state */
			0,	/* nargs: number of arguments pushed onto the lua stack */
			0,	/* nresults: number of results that should be on stack at end*/
			0);	/* errfunc: location, in stack, of error function.
				if 0, results are on top of stack. */
		if (error != 0 /*no error*/)
		{
			std::cout << "Lua: There was an error..." << std::endl;
			std::cout << decodeLuaErrorToString(error) << std::endl;

			std::string luaError;
			// Get error information from top of stack (-1 is top)
			luaError.append(lua_tostring(L, -1));
			std::cout << luaError << std::endl;
			// We passed zero (0) as errfunc, so error is on stack)
			lua_pop(L, 1);  /* pop error message from the stack */

			continue;
		}
	}//where there are more lines

	 //maybe shut down too here

	return 1;
}

///////////////////////////////////////




/*

iCommand* createCommand_MoveObjectWithDuration  ( int objectID, glm::vec3 destination,        float duration, float easeInRatio, float easeOutRatio );
iCommand* createCommand_RotateObjectWithDuration( int objectID, glm::vec3 rotationalVelocity, float duration );
iCommand* createCommand_WaitObject              ( int objectID,                               float duration );
iCommand* createCommand_SetCameraTarget         ( int objectID );
iCommand* createCommand_SetCameraTargetStatic   ( glm::vec3 position );
iCommand* createCommand_SetCameraPosition       ( glm::vec3 position );

*/
extern "C" int InitCommandGroup(lua_State* L)
{
	currentCommandGroup = cCommandGroup();
	return 0;
}

extern "C" int SubmitCommandGroup(lua_State* L)
{
	GelCommandManager::sharedInstance().AddCommandGroup(currentCommandGroup);
	return 0;
}

extern "C" int MoveTo(lua_State* L)
{
	bool serialCommand = lua_toboolean(L, 1);

	std::string objectName = lua_tostring(L, 2);
	float destination_x    = lua_tonumber(L, 3);
	float destination_y    = lua_tonumber(L, 4);
	float destination_z    = lua_tonumber(L, 5);
	float duration         = lua_tonumber(L, 6);
	float easeInRatio      = lua_tonumber(L, 7);
	float easeOutRatio     = lua_tonumber(L, 8);

	iGameObject* gameObject = gelObjectManager::sharedInstance().FindGameObjectWithObjectName(objectName);
	if (!gameObject)
	{
		std::cout << "Object with given name not found" << std::endl;
		return 0;
	}
	int objectID = gameObject->getObjectID();

	if (serialCommand)
	{
		currentCommandGroup.AddSerialCommand(GelCommandBuilder::sharedInstance().
			createCommand_MoveObjectWithDuration(
				objectID,
				glm::vec3(destination_x, destination_y, destination_z),
				duration,
				easeInRatio,
				easeOutRatio));
	}
	else
	{
		currentCommandGroup.AddParallelCommand(GelCommandBuilder::sharedInstance().
			createCommand_MoveObjectWithDuration(
				objectID,
				glm::vec3(destination_x, destination_y, destination_z),
				duration,
				easeInRatio,
				easeOutRatio));
	}

	return 0; // Count of returned values
}

extern "C" int GetObjectPosition(lua_State* L)
{
	std::string objectName = lua_tostring(L, 1);

	iGameObject* gameObject = gelObjectManager::sharedInstance().FindGameObjectWithObjectName(objectName);
	if (!gameObject)
	{
		std::cout << "Object with given name not found" << std::endl;
		return 0;
	}
	//int objectID = gameObject->getObjectID();
	glm::vec3 objectPosition = gameObject->GetRender().position;

	lua_pushnumber(L, objectPosition.x);
	lua_pushnumber(L, objectPosition.y);
	lua_pushnumber(L, objectPosition.z);

	return 3;
}


extern "C" int LookAtOnce(lua_State* L)
{
	bool serialCommand = lua_toboolean(L, 1);

	std::string objectName = lua_tostring(L, 2);

	iGameObject* gameObject = gelObjectManager::sharedInstance().FindGameObjectWithObjectName(objectName);
	if (!gameObject)
	{
		std::cout << "Object with given name not found" << std::endl;
		return -1;
	}
	glm::vec3 targetPosition = gameObject->GetRender().position;

	if (serialCommand)
	{
		currentCommandGroup.AddSerialCommand(GelCommandBuilder::sharedInstance().
			createCommand_SetCameraTargetStatic(targetPosition));
	}
	else
	{
		currentCommandGroup.AddParallelCommand(GelCommandBuilder::sharedInstance().
			createCommand_SetCameraTargetStatic(targetPosition));
	}
	return 0;
}

extern "C" int SetCameraPosition(lua_State* L)
{
	bool serialCommand  = lua_toboolean(L, 1);

	float destination_x = lua_tonumber(L, 2);
	float destination_y = lua_tonumber(L, 3);
	float destination_z = lua_tonumber(L, 4);

	if (serialCommand)
	{
		currentCommandGroup.AddSerialCommand(GelCommandBuilder::sharedInstance().
			createCommand_SetCameraPosition(
				glm::vec3(destination_x, destination_y, destination_z)));
	}
	else
	{
		currentCommandGroup.AddParallelCommand(GelCommandBuilder::sharedInstance().
			createCommand_SetCameraPosition(
				glm::vec3(destination_x, destination_y, destination_z)));
	}
	return 0;
}

extern "C" int SetCameraTarget(lua_State* L)
{
	bool serialCommand = lua_toboolean(L, 1);

	std::string objectName = lua_tostring(L, 2);

	iGameObject* gameObject = gelObjectManager::sharedInstance().FindGameObjectWithObjectName(objectName);
	if (!gameObject)
	{
		std::cout << "Object with given name not found" << std::endl;
		return -1;
	}
	int objectID = gameObject->getObjectID();

	if (serialCommand)
	{
		currentCommandGroup.AddSerialCommand(GelCommandBuilder::sharedInstance().
			createCommand_SetCameraTarget(objectID));
	}
	else
	{
		currentCommandGroup.AddParallelCommand(GelCommandBuilder::sharedInstance().
			createCommand_SetCameraTarget(objectID));
	}
	return 0;
}

extern "C" int DelayCommandChain(lua_State* L) //effective for serials
{
	bool serialCommand = lua_toboolean(L, 1);

	float duration = lua_tonumber(L, 2);

	currentCommandGroup.AddSerialCommand(GelCommandBuilder::sharedInstance().
		createCommand_WaitObject(0, duration));

	return 0;
}

extern "C" int RotateObject(lua_State* L) //effective for serials
{
	bool serialCommand = lua_toboolean(L, 1);

	std::string objectName = lua_tostring(L, 2);
	float destination_x    = lua_tonumber(L, 3);
	float destination_y    = lua_tonumber(L, 4);
	float destination_z    = lua_tonumber(L, 5);
	float duration         = lua_tonumber(L, 6);


	iGameObject* gameObject = gelObjectManager::sharedInstance().FindGameObjectWithObjectName(objectName);
	if (!gameObject)
	{
		std::cout << "Object with given name not found" << std::endl;
		return 0;
	}
	int objectID = gameObject->getObjectID();

	if (serialCommand)
	{
		currentCommandGroup.AddSerialCommand(GelCommandBuilder::sharedInstance().
			createCommand_RotateObjectWithDuration(
				objectID,
				glm::vec3(destination_x, destination_y, destination_z),
				duration));
	}
	else
	{
		currentCommandGroup.AddParallelCommand(GelCommandBuilder::sharedInstance().
			createCommand_RotateObjectWithDuration(
				objectID,
				glm::vec3(destination_x, destination_y, destination_z),
				duration));
	}

	return 0;
}




////////////////////////////////////////
