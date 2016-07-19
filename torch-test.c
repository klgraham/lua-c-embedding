#include <stdio.h>
#include <stdlib.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

int main(int argc, char *argv[])
{
  lua_State *L;
  
  L = luaL_newstate(); // open Lua 
  luaL_openlibs(L); // load Lua libraries
  float radians = atof(argv[1]);

  luaL_loadfile(L, "torch-test.lua");
  lua_pcall(L, 0, 0, 0); // Execute script once to create and assign functions
	
  lua_getglobal(L, "cosine"); // function to be called
  lua_pushnumber(L, radians); // push argument
	
  if (lua_pcall(L, 1, 1, 0) != 0) // 1 argument, 1 return value
  {
    fprintf(stderr, "%s\n", lua_tostring(L, -1));
		return 1;
  }
  
  float result = lua_tonumber(L, -1);
  lua_pop(L, 1); // pop returned value
  printf("Cos(%f) = %f\n", radians, result);  
    
  lua_close(L); 
  return 0;
}
