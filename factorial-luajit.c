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
  int n = atoi(argv[1]);

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
    
  lua_close(L); 
  return 0;
}