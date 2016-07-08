#include <stdio.h>
#include <stdlib.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include "luajit.h"

int main(int argc, char *argv[])
{
  lua_State *L;
  
  L = luaL_newstate(); // open Lua 
  luaL_openlibs(L); // load Lua libraries
  float theta = atof(argv[1]);

  luaL_loadfile(L, "torch-test.lua");
  lua_pushnumber(L, theta);
	lua_setglobal(L, "angleInRadians");
	
  // 0 arguments, 0 return value (just setting a global this time)
  if (lua_pcall(L, 0, 0, 0) != 0) 
  {
    fprintf(stderr, "%s\n", lua_tostring(L, -1));
		return 1;
  }
      
  lua_close(L); 
  return 0;
}
