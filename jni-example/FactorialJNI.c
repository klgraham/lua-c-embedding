#include <jni.h>
#include <stdio.h>
#include "FactorialJNI.h"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include "luajit.h"
#include "luatorch.h"

JNIEXPORT jint JNICALL Java_FactorialJNI_factorial(JNIEnv *env, jobject thisObj, jint n) 
{
	lua_State* L = initlua();
  jint result;
  luaL_loadfile(L, "factorial.lua");
  lua_pcall(L, 0, 0, 0);
  lua_getglobal(L, "factorial");
  lua_pushnumber(L, (int)n);
  
  if (lua_pcall(L, 1, 1, 0) != 0) 
  {
    fprintf(stderr, "%s\n", lua_tostring(L, -1));
    return 1;
  }

  result = lua_tonumber(L, -1);
  lua_pop(L, 1);
	closeLua(L);
  return result;
}
