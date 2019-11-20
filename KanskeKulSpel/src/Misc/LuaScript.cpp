#include "LuaScript.h"
#include <vector>
#include <stdarg.h>

LuaScript::LuaScript(std::string filename)
{
    this->state = luaL_newstate();
    this->level = 0;
    this->filename = filename;

    if (luaL_loadfile(state, filename.c_str()) || lua_pcall(state, 0, 0, 0))
    {
        printfCon("Script %s could not be loaded.", filename.c_str());
        printfCon("Error: %s", lua_tostring(state, -1));
        lua_pop(state, 1);
    }
}

LuaScript::~LuaScript()
{
    if (state)
        lua_close(state);
}

LuaScript::LuaScript(const LuaScript& other)
{
    *this = other;
}

void LuaScript::operator=(const LuaScript& other)
{
    this->filename = other.filename;
    this->level = other.level;

    reload();
}

void LuaScript::runFunc(std::string funcName, const char* args, ...)
{
    if (!state)
    {
        printfCon("Script not loaded");
        return;
    }

    va_list list;

    va_start(list, args);

    lua_getglobal(state, funcName.c_str());

    if (lua_isnil(state, -1))
    {
        printErrorMsg(funcName, " is undefined");
        return;
    }

    int count = 0;
    std::string str(args);
    for (const char& c : str)
    {
        if (c == 'f')
            lua_pushnumber(state, float(va_arg(list, double)));

        else if (c == 'd')
            lua_pushnumber(state, int(va_arg(list, int)));

        else if (c == 'b')
            lua_pushboolean(state, va_arg(list, bool));

        else if (c == 's')
            lua_pushstring(state, va_arg(list, const char*));

        count++;
    }

    lua_pcall(state, count, 0, 0);
}

void LuaScript::reload()
{
    if (state)
        lua_close(state);

    this->state = luaL_newstate();

    if (luaL_loadfile(state, filename.c_str()) || lua_pcall(state, 0, 0, 0))
        printfCon("Script %s could not be loaded.", filename.c_str());
}

void LuaScript::printErrorMsg(std::string variable, std::string error)
{
    printfCon("Can't find %s, %s", variable.c_str(), error.c_str());
}

bool LuaScript::getToStack(std::string variable)
{
    std::vector<std::string> path;
    std::string str;
    this->level = 0;

    for (const char& c : variable)
    {
        if (c == '.')
        {
            path.push_back(str);
            str = "";
        }

        else
            str += c;
    }
    path.push_back(str);

    for (std::string& p : path)
    {
        if (level == 0)
            lua_getglobal(state, p.c_str());

        else
            lua_getfield(state, -1, p.c_str());

        if (lua_isnil(state, -1))
        {
            printErrorMsg(variable, p + " is undefined");
            return false;
        }

        else
            level++;
    }

    return true;
}