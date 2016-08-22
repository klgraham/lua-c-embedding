#include <jni.h>
#include <stdio.h>
#include "LuaJNI.h"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <luaT.h>

const char* convert_jstring(JNIEnv *env, jstring s);


lua_State *L;


JNIEXPORT void JNICALL Java_LuaJNI_startLua(JNIEnv *env, jobject thisObj)
{
    L = luaL_newstate();

    if (!L) {
        fprintf(stderr, "%s\n", "Cannot create new Lua state!");
        exit(-1);
    }
    luaL_openlibs(L);
}


JNIEXPORT void JNICALL Java_LuaJNI_closeLua(JNIEnv *env, jobject thisObj)
{
    lua_close(L);
}


JNIEXPORT jint JNICALL Java_LuaJNI_apply(JNIEnv *env, jobject thisObj, jint functionRef, jint n)
{
    // push function on the stack from the registry
    lua_rawgeti(L, LUA_REGISTRYINDEX, (int)functionRef);
    lua_pushinteger(L, (int)n); // push parameter onto stack
    
    if (lua_pcall(L, 1, 1, 0) != 0) {
        fprintf(stderr, "%s\n", lua_tostring(L, -1));
        luaT_stackdump(L);
        exit(-1);
    }
    
    int result = lua_tointeger(L, -1);
    lua_pop(L, 1); // pop returned value
    return (jint)result;
}


JNIEXPORT jint JNICALL Java_LuaJNI_getFunctionRef(JNIEnv *env, jobject thisObj, jstring scriptPath)
{
    const char *script = convert_jstring(env, scriptPath);
    
    luaL_dofile(L, script); // Load the function
    lua_getglobal(L, "factorial"); // place it on the stack
    
    // store function in the registry and return reference
    int ref = luaL_ref(L, LUA_REGISTRYINDEX);
  
    return (jint)ref;
}

/* converts a JNI string into a char pointer */
const char* convert_jstring(JNIEnv *env, jstring s)
{
    const char *c_string = (*env)->GetStringUTFChars(env, s, NULL);
    if (!c_string)
    {
        fprintf(stderr, "%s\n", "Failure converting JNI string to C string");
        return NULL;
    }
    return c_string;
}
