#include "LuaScript.h"
#include "Misc/ConsoleWindow.h"

LuaScript::LuaScript(std::string filename)
{
    this->state = luaL_newstate();

    if (luaL_loadfile(state, filename.c_str()) || lua_pcall(state, 0, 0, 0))
        printfCon("Script %s could not be loaded.", filename.c_str());

}

LuaScript::~LuaScript()
{
}
