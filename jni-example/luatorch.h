#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include "luajit.h"

lua_State* initlua();
void closeLua(lua_State* L);
