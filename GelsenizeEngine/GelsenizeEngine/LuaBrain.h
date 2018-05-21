#ifndef _LUABRAIN_HG_
#define _LUABRAIN_HG_



extern "C" {
#include <Lua5.3.3\lua.h>
#include <Lua5.3.3\lua.h>
#include <Lua5.3.3\lauxlib.h>
#include <Lua5.3.3/lualib.h>
}

#include <string>
#include <iostream>

#include "GelCommandBuilder.h"
#include <map>
#include "managers.h"
#include "fstream"


extern "C" int InitCommandGroup(lua_State* L);
extern "C" int SubmitCommandGroup(lua_State* L);
extern "C" int MoveTo(lua_State* L);
extern "C" int GetObjectPosition(lua_State* L);
extern "C" int LookAtOnce(lua_State* L);
extern "C" int SetCameraPosition(lua_State* L);
extern "C" int SetCameraTarget(lua_State* L);
extern "C" int DelayCommandChain(lua_State* L);
extern "C" int RotateObject(lua_State* L);

lua_State* luaStart(void);
void luaShutDown(void);
std::string decodeLuaErrorToString(int error);


int GelsenizeLuaShutDown(void);

int GelsenizeLuaStartUp(void);


#endif // ! _LUABRAIN_HG_


