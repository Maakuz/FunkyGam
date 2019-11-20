#pragma once
#include <string>
#include "Misc/ConsoleWindow.h"

extern "C"
{
#include "LUA/lua.h"
#include "LUA/lauxlib.h"
#include "LUA/lualib.h"
}

#define SCRIPT_PATH "../Resources/Scripts/"

class LuaScript
{
public:
    LuaScript(std::string filename);
    ~LuaScript();


    
    template <typename T>
    T get(std::string variable);

    void runFunc(std::string funcName, const char* args, ...);

    void reload();
private:
    lua_State* state;
    int level;
    std::string filename;

    void printErrorMsg(std::string variable, std::string error);

    bool getToStack(std::string variable);

    template <typename T>
    T lua_get(std::string variable);

    template <typename T>
    T getDef();
};

template<typename T>
inline T LuaScript::get(std::string variable)
{
    if (!state)
    {
        printfCon("Script not loaded");
        return getDef<T>();
    }
    
    T value;
    if (getToStack(variable))
        value = lua_get<T>(variable);

    else
        value = getDef<T>();

    lua_pop(state, this->level);
    return value;
}

template<typename T>
inline T LuaScript::getDef()
{
    return 0;
}

template<>
inline std::string LuaScript::getDef()
{
    return "null";
}

template<typename T>
inline T LuaScript::lua_get(std::string variable)
{
    return 0;
}

template<>
inline int LuaScript::lua_get(std::string variable)
{
    if (!lua_isnumber(state, -1))
        printfCon("%s not a number", variable.c_str());
    
    return (int)lua_tonumber(state, -1);
}

template<>
inline float LuaScript::lua_get(std::string variable)
{
    if (!lua_isnumber(state, -1))
        printfCon("%s not a number", variable.c_str());

    return (float)lua_tonumber(state, -1);
}

template<>
inline bool LuaScript::lua_get(std::string variable)
{
    return (bool)lua_toboolean(state, -1);
}

template<>
inline std::string LuaScript::lua_get(std::string variable)
{
    std::string str("null");
    if (lua_isstring(state, -1))
        str = lua_tostring(state, -1);

    else
        printfCon("%s not a string", variable.c_str());

    return str;
}
