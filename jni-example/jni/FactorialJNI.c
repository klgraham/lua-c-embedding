#include <jni.h>
#include <stdio.h>
#include "FactorialJNI.h"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
// #include "luatorch.h"

// lua_State* initlua();
void handler(int sig);

void handler(int sig) {
  void *array[10];
  size_t size;

  // get void*'s for all entries on the stack
  size = backtrace(array, 10);

  // print out all the frames to stderr
  fprintf(stderr, "Error: signal %d:\n", sig);
  backtrace_symbols_fd(array, size, STDERR_FILENO);
  exit(1);
}

// lua_State* init_lua()
// {
//   lua_State *L;
//   L = luaL_newstate();
//   luaL_openlibs(L);
//   return L;
// }

JNIEXPORT jint JNICALL Java_FactorialJNI_factorial(JNIEnv *env, jobject thisObj, jint n) 
{
	signal(SIGSEGV, handler);
	
  lua_State *L;
  L = luaL_newstate();
	printf("lua_State: %p\n", L);
	if (!L)
	{
    fprintf(stderr, "%s\n", "Cannot create new Lua state!");
		exit(-1);
	}
  printf("lua_State: %p\n", L);
  luaL_openlibs(L); 
	// lua_State* L = init_lua();
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

  result = (jint)lua_tonumber(L, -1);
  lua_pop(L, 1);
	lua_close(L);
  return result;
}
