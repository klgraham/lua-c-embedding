%module factorial
%{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include "luajit.h"
#include "factorial.h"
%}

extern int factorial(int n);
extern lua_State* init_lua();
extern void end_lua(lua_State* L);
