#include <stdio.h>
#include <stdlib.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include "luajit.h"
#include "factorial.h"

lua_State* init_lua()
{
  lua_State *L = luaL_newstate(); // open Lua 
	printf("lua_State: %p\n", L);
	if (!L)
	{
    fprintf(stderr, "%s\n", "Cannot create new Lua state!");
		exit(-1);
	}
  printf("lua_State: %p\n", L);
  luaL_openlibs(L); // load Lua libraries
	printf("loaded Lua libs.\n");
	return L;
}

void end_lua(lua_State* L)
{
  lua_close(L);
}

int factorial(int n)
{
  lua_State *L = init_lua();
	printf("lua_State: %p", L);
	
  luaL_loadfile(L, "factorial.lua");
  lua_pcall(L, 0, 0, 0); // Execute script once to create and assign functions
	
  lua_getglobal(L, "factorial"); // function to be called
  lua_pushinteger(L, n); // push argument
	
  if (lua_pcall(L, 1, 1, 0) != 0) // 1 argument, 1 return value
  {
    fprintf(stderr, "%s\n", lua_tostring(L, -1));
		return 1;
  }
  
  int result = lua_tointeger(L, -1);
  lua_pop(L, 1); // pop returned value
  printf("%d! is %d\n", n, result);  
    
  end_lua(L); 
  return result;
}
