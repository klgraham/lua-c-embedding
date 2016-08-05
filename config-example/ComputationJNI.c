#include <jni.h>
#include <stdio.h>
#include "ComputationJNI.h"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <luaT.h>

lua_State *L;


JNIEXPORT void JNICALL Java_ComputationJNI_startLua(JNIEnv *env, jobject thisObj)
{
  L = luaL_newstate();

  if (!L)
  {
    fprintf(stderr, "%s\n", "Cannot create new Lua state!");
    exit(-1);
  }
  luaL_openlibs(L);
}


JNIEXPORT void JNICALL Java_ComputationJNI_closeLua(JNIEnv *env, jobject thisObj)
{
  lua_close(L);
}


JNIEXPORT jstring JNICALL Java_ComputationJNI_factorial(JNIEnv *env, jobject thisObj, jint n)
{
  // load factorial function from script onto stack
  luaL_loadfile(L, "computation.lua");
  lua_pcall(L, 0, 0, 0);
  lua_getglobal(L, "factorial");

  lua_pushnumber(L, (int)n); // push parameter onto stack

  if (lua_pcall(L, 1, 1, 0) != 0)
  {
    fprintf(stderr, "%s\n", lua_tostring(L, -1));
    luaT_stackdump(L);
    exit(-1);
  }

  // pop output off stack and store as a global variable
  lua_setglobal(L, "N");

  jstring result = (*env)->NewStringUTF(env, "N"); // pass global var name to Java
  printf("Lua global var: %s", "N");
  return result;
}


JNIEXPORT jint JNICALL Java_ComputationJNI_sum(JNIEnv *env, jobject thisObj, jint m)
{
  // convert name of global var holding function input to C string
  // const char *var_name = (*env)->GetStringUTFChars(env, n, NULL);
  // fprintf(stderr, "Global name: %s", var_name);

  // load factorial function from script onto stack
  luaL_loadfile(L, "computation.lua");
  lua_pcall(L, 0, 0, 0);
  lua_getglobal(L, "sum");
  lua_pushnumber(L, (int)m);

  if (lua_pcall(L, 1, 1, 0) != 0)
  {
    fprintf(stderr, "%s\n", lua_tostring(L, -1));
    luaT_stackdump(L);
    return 1;
  }

  // convert result to Java int
  jint result = (jint)lua_tonumber(L, -1);
  lua_pop(L, 1); // remove from stack

  // (*env)->ReleaseStringUTFChars(env, n, var_name); // release mem from C string
  return result; // return int to Java
}
