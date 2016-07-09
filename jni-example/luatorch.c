#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include "luajit.h"
#include "luatorch.h"

lua_State* initlua() 
{
  lua_State *L;
  L = luaL_newstate();
  luaL_openlibs(L); 
  return L;
}

void closeLua(lua_State* L)
{
  lua_close(L);
}
