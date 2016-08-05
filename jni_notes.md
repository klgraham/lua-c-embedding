# Using Lua from Java

This requires the JNI. So, first, we'll look at how to use it for a simple example.

## Hello, World

First, we make a Java class, HelloWorldJNI.java, that loads a native (C) library, helloworld, that contains a method ```hello()```.

```
public class HelloWorldJNI 
{
  private native void hello();
 
  public static void main(String[] args) 
  {
    System.load(args[0]);
    new HelloWorldJNI().hello();  
  }
}
```

Next steps are to compile the java code with ```javac HelloWorldJNI.java``` to get the a class file, HelloWorld.class, and then generate the JNI header file with ```javah HelloWorldJNI```.

Then we create the JNI C implementation, HelloWorldJNI.c:

```
#include <jni.h>
#include <stdio.h>
#include "HelloWorldJNI.h"

JNIEXPORT void JNICALL Java_HelloWorldJNI_hello(JNIEnv *env, jobject thisObj)
{
  printf("Hello, World!\n");
  return;
}
```

Now we need to compile the C code and link everything up. For this we can use:

```gcc -I"$JAVA_HOME/include" -shared -o libhelloworld.dylib HelloWorldJNI.c```

Notice that we're including the location of JAVA_HOME/include, are creating a shared library, and are naming the library libhelloworld.dylib. To run the java application, you use ```java HelloWorldJNI <full path to helloworld.dylib>```.

## Using Torch from Java

We're going to adapt the earlier example where we compute the factorial of a number. This is on the jni-example branch. You can build and run in the ```jni-example``` directory by using this:

```make && java FactorialJNI <full path to libfactorial.dylib> <int of your choice>```


Note, there is an issue using 64-bit LuaJIT on macOS:

- http://comments.gmane.org/gmane.comp.lang.lua.luajit/4817
- http://stackoverflow.com/questions/14840569/sigsegv-error-in-some-lua-c-code
- http://stackoverflow.com/questions/13400660/binding-lua-in-static-library-segfault?rq=1
- http://nticformation.com/solutions_problems.php?tuto=59091&subCategory=c+lua+jni+luajit&Category=C+Language

Based on the last of the above links, I think the issue is that ```-pagezero_size 10000 -image_base 100000000``` is not included when compiling the JNI stuff. The problem is that when it is included, the compiler says 
>-pagezero_size option can only be used when linking a main executable
which means those params can't be used for shared libraries (which JNI needs).

I've confirmed that the above is the problem via [a thread](http://www.freelists.org/post/luajit/luaL-newstate-fails-on-64bit-Mac-cant-set-linker-flags) involving Lua's maintainer, Mike Pall. This is specifically an issue on 64-bit macOS. If I compile a 32-bit LuaJIT from source, then I should be able to get things to work. But, a 32-bit version might not be as useful for our purposes. To build the 32-bit LuaJIT binary, you can run the following commands inside the LuaJIT directory:

```CFLAGS="-arch i386" GCCFLAGS="-arch i386" LDFLAGS="-arch i386" make && sudo make install```

## Configuring Lua State

Let's say that you want to set a variable to a constant and then use that constant multiple times in a calculation. One way to do this in Lua is to set a global variable and then refer to that global by name in later calculations. A simple example of this is in the directory ```config-example```. It computes the factorial of a number and then computes the sum of integers from some ```m``` up to that number. 

The Lua code uses the earlier factorial function and adds a sum function. This time, you'll notice that the sum references a variable ```N```. We'll set that variable from Java.

```
function factorial(n)
  local result = 1
  for i = 1, n do
    result = result * i
  end
  return result
end

-- computes $\sum_{i=m}^N i$
function sum(m)
  local result = 0
  for i = m, N do
    result = result + i
  end
  return result
end
```

The Java code is shown below. Notice that the constructor has methods that start and close Lua.
```

import java.lang.Throwable;

public class ComputationJNI
{
  private native int sum(int m);
  private native String factorial(int n);
  private native void startLua();
  private native void closeLua();

  public ComputationJNI()
  {
    this.startLua();
  }

  @Override
  protected void finalize() throws Throwable
  {
    this.closeLua();
  }

  public static void main(String[] args)
  {
    System.load(args[0]);
    int n = Integer.valueOf(args[1]);

    ComputationJNI jni = new ComputationJNI();
	String fact = jni.factorial(n);
    System.out.println(n + "! = " + fact + "\n");
    int sum = -1;

    for (int m = 0; m < 10; m++)
    {
      sum = jni.sum(m);
      System.out.println("Sum(" + m + ", " + fact + ") = " + sum);
    }
  }
}
```

The JNI code has functions for starting and closing Lua, which are nice to have on the Java side when instantiating and garbage-collecting JNI instances.
```
#include <jni.h>
#include <stdio.h>
#include "ComputationJNI.h"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <luaT.h>

lua_State *L;


JNIEXPORT void JNICALL Java_ComputationJNI_startLua(JNIEnv *env, jobject thisObj)
{
  L = luaL_newstate();

  if (!L)
  {
    fprintf(stderr, "%s\n", "Cannot create new Lua state!");
    exit(-1);
  }
  luaL_openlibs(L);
}


JNIEXPORT void JNICALL Java_ComputationJNI_closeLua(JNIEnv *env, jobject thisObj)
{
  lua_close(L);
}


JNIEXPORT jstring JNICALL Java_ComputationJNI_factorial(JNIEnv *env, jobject thisObj, jint n)
{
  // load factorial function from script onto stack
  luaL_loadfile(L, "computation.lua");
  lua_pcall(L, 0, 0, 0);
  lua_getglobal(L, "factorial");

  lua_pushnumber(L, (int)n); // push parameter onto stack

  if (lua_pcall(L, 1, 1, 0) != 0)
  {
    fprintf(stderr, "%s\n", lua_tostring(L, -1));
    luaT_stackdump(L);
    exit(-1);
  }

  // pop output off stack and store as a global variable
  lua_setglobal(L, "N");

  jstring result = (*env)->NewStringUTF(env, "N"); // pass global var name to Java
  printf("Lua global var: %s", "N");
  return result;
}


JNIEXPORT jint JNICALL Java_ComputationJNI_sum(JNIEnv *env, jobject thisObj, jint m)
{
  // convert name of global var holding function input to C string
  // const char *var_name = (*env)->GetStringUTFChars(env, n, NULL);
  // fprintf(stderr, "Global name: %s", var_name);

  // load factorial function from script onto stack
  luaL_loadfile(L, "computation.lua");
  lua_pcall(L, 0, 0, 0);
  lua_getglobal(L, "sum");
  lua_pushnumber(L, (int)m);

  if (lua_pcall(L, 1, 1, 0) != 0)
  {
    fprintf(stderr, "%s\n", lua_tostring(L, -1));
    luaT_stackdump(L);
    return 1;
  }

  // convert result to Java int
  jint result = (jint)lua_tonumber(L, -1);
  lua_pop(L, 1); // remove from stack

  // (*env)->ReleaseStringUTFChars(env, n, var_name); // release mem from C string
  return result; // return int to Java
}
```