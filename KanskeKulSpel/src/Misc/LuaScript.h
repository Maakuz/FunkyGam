#pragma once
#include <string>

extern "C"
{
#include "LUA/lua.h"
#include "LUA/lauxlib.h"
#include "LUA/lualib.h"
}

class LuaScript
{
public:
    LuaScript(std::string filename);
    ~LuaScript();

private:
    lua_State* state;
};