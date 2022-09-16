//
// Created by yll20 on 2022/9/16.
//

#ifndef HOOKDEMO_LUAENGINE_H
#define HOOKDEMO_LUAENGINE_H

#include <cstdlib>
#include <cstring>
#include <string>
#include <Windows.h>

extern "C"
{
#include "lua/lua.h"
#include "lua/lauxlib.h"
#include "lua/lualib.h"
}

lua_State *L;

int paramNum = 0;

template<typename T>
void _LuaEnter(const T &param) {
    void *pointer = (void *) &param;
    lua_pushnumber(L, (int)pointer);
    lua_pcall(L, paramNum, 0, 0);
    paramNum = 0;
}

template<typename T, typename... Args>
void _LuaEnter(const T &param, const Args &...rest) {
    if (paramNum==0){
        paramNum = sizeof...(rest) + 1;
    }
    void *pointer = (void *) &param;
    lua_pushnumber(L, (int)pointer);
    _LuaEnter(rest...);
}

#define LuaEnter(luaFuncName, ...) \
    lua_getglobal(L, luaFuncName); \
    _LuaEnter(__VA_ARGS__);

#define LuaLeave(funcName, result) \
    lua_getglobal(L, (funcName));  \
    lua_pushnumber(L, (int)&(result));\
    lua_pcall(L, 1, 0, 0);

int luaGetBytesByPointer(lua_State *L) {
    int pointer = lua_tointeger(L, 1);
    int length = lua_tointeger(L, 2);
    char *bytes = (char *) pointer;
    std::string result;
    for (int i = 0; i < length; i++) {
        result += bytes[i];
    }
    lua_pushstring(L, result.c_str());
    return 1;
}

int luaGetPointerByPointer(lua_State *L) {
    int *pointer = (int *) lua_tointeger(L, 1);
    int *result = (int *) (*pointer);
    lua_pushinteger(L, (int) result);
    return 1;
}

int luaGetCStringByPointer(lua_State *L) {
    int pointer = lua_tointeger(L, 1);
    char *bytes = (char *) pointer;
    std::string result;
    for (int i = 0; bytes[i] != '\0'; i++) {
        result += bytes[i];
    }
    lua_pushstring(L, result.c_str());
    return 1;
}

int luaGetIntByPointer(lua_State *L) {
    int pointer = lua_tointeger(L, 1);
    int *result = (int *) pointer;
    lua_pushinteger(L, *result);
    return 1;
}

int luaAlloc(lua_State *L) {
    int size = lua_tointeger(L, 1);
    void *result = malloc(size);
    lua_pushinteger(L, (int) result);
    return 1;
}

int luaFree(lua_State *L) {
    int pointer = lua_tointeger(L, 1);
    free((void *) pointer);
    return 0;
}

int luaNewCString(lua_State *L) {
    const char *str = lua_tostring(L, 1);
    int length = strlen(str);
    char *result = (char *) malloc(length + 1);
    for (int i = 0; i < length; i++) {
        result[i] = str[i];
    }
    result[length] = '\0';
    lua_pushinteger(L, (int) result);
    return 1;
}

int luaSetNBytesByPointer(lua_State *L) {
    int pointer = lua_tointeger(L, 1);
    const char *str = lua_tostring(L, 2);
    int length = lua_tointeger(L, 3);
    char *bytes = (char *) pointer;
    for (int i = 0; i < length; i++) {
        bytes[i] = str[i];
    }
    return 0;
}

int luaSetPointerByPointer(lua_State *L) {
    int *pointer = (int *) lua_tointeger(L, 1);
    int value = lua_tointeger(L, 2);
    *pointer = value;
    return 0;
}

#define LUA_FILE "LuaEngine.lua"

BOOL initLuaEngine() {
    L = luaL_newstate();
    if (L == nullptr) {
        return FALSE;
    }
    luaL_openlibs(L);
    luaL_dofile(L, LUA_FILE);
    lua_register(L, "luaGetBytesByPointer", luaGetBytesByPointer);
    lua_register(L, "luaGetPointerByPointer", luaGetPointerByPointer);
    lua_register(L, "luaGetCStringByPointer", luaGetCStringByPointer);
    lua_register(L, "luaGetIntByPointer", luaGetIntByPointer);
    lua_register(L, "luaAlloc", luaAlloc);
    lua_register(L, "luaFree", luaFree);
    lua_register(L, "luaNewCString", luaNewCString);
    lua_register(L, "luaSetNBytesByPointer", luaSetNBytesByPointer);
    lua_register(L, "luaSetPointerByPointer", luaSetPointerByPointer);
    return TRUE;
}


#endif //HOOKDEMO_LUAENGINE_H
