#include <stdio.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include "luajit.h"

int main(int argc, char *argv[])
{
	int status;
	lua_State *L;
	
	L = luaL_newstate(); // open Lua
  if (!L) {
		return -1; // Checks that Lua started up
	}
	
	luaL_openlibs(L); // load Lua libraries
		
	if (argc > 1) {
		status = luaL_loadfile(L, argv[1]);  // load Lua script
    int ret = lua_pcall(L, 0, 0, 0); // tell Lua to run the script
		if (ret != 0) {
			fprintf(stderr, "%s\n", lua_tostring(L, -1)); // tell us what mistake we made
			return 1;
		}
	}
	
	lua_close(L); // Close Lua
	return 0;
}

