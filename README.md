# lua-c-embedding

Here are some notes/instructions for embedding Lua in C. Initially, we follow along with [Marek Vavrusa's blog post](https://en.blog.nic.cz/2015/08/12/embedding-luajit-in-30-minutes-or-so/). Later, we move beyond and see how to interact with things in Torch.

Note: This was initially setup to embed the LuaJIT, but I then discovered the issue with building 64-bit LuaJIT library to use with the JNI. So, I switched to plain Lua instead.

### Install Torch and Lua

This repo includes the Torch distribution. Installation instrutions follow:

- ```git clone https://github.com/klgraham/luajit-c-embedding.git ~/lua-c-embedding --recursive```
- ```cd ~/lua-c-embedding/torch-distro```
- install dependencies: ```bash install-deps```
- install Lua, LuaRocks, and Torch: ```TORCH_LUA_VERSION=LUA51 ./install.sh```
- ```cd ..```
- creating an environment variable ```$TORCH_HOME``` pointing to your torch installation, which is ```~/lua-c-embedding/torch-distro/install```. Note that if you skip this step, you'll need to update the makefiles in the JNI examples.

## Hello, World!

In the hello-world directory you'll notice a Lua script named hello.lua which contains ```print('Hello, World!')``` and a C file, hello.c, which looks like this (modified from the above blog post):

```
#include <stdio.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

int main()
{
  int status;
  lua_State *L;
  
  L = luaL_newstate(); // open Lua
  if (!L) {
    return -1; // Checks that Lua started up
  }
  
  luaL_openlibs(L); // load Lua libraries

  status = luaL_loadfile(L, argv[1]);  // load Lua script
  int ret = lua_pcall(L, 0, 0, 0); // tell Lua to run the script
  if (ret != 0) {
    fprintf(stderr, "%s\n", lua_tostring(L, -1)); // tell us what mistake we made
    return 1;
  }

  lua_close(L); // Close Lua
  return 0;
}
```

To compile the C, use:

```gcc hello.c -pagezero_size 10000 -image_base 100000000 -I$TORCH_HOME/include -I/usr/local/include -L/usr/local/lib -L$TORCH_HOME/lib -llua -lm -o hello.out```, noting that ```-pagezero_size 10000 -image_base 100000000``` only needs to be included on macOS systems. Also note how the LuaJIT include directory has been included in the compiler search path. 

You can now execute the Lua script with ```./hello.out hello.lua```.

### Understanding the C Code

To interact with Lua from C we will use [Lua's C API](http://www.lua.org/pil/24.1.html). See http://pgl.yoyo.org/luai/i/_ for great documentation on the functions in Lua's C API. Let's look at the each part of hello-luajit.c. 

- ```luaL_newstate```: Starts up Lua and returns a new Lua state
- ```luaL_openlibs```: loads the Lua standard libraries
- ```luaL_loadfile```: Loads the Lua script
- ```lua_pcall```: runs the Lua script and returns an error code
- ```lua_close```: closes Lua

## Using the Lua Stack

Lua uses a stack to exchange values with C. To get a value from Lua, you call Lua to push the value onto the stack. To pass a value from C to Lua, you call Lua to push the value onto the stack and then pop the value. When pushing a value onto the stack, it's important to know if the stack has room for the new value. The lua-users wiki has [a nice example of this](http://lua-users.org/wiki/SimpleLuaApiExample). Let's look a couple of examples.

### Example 1

Suppose we want to compute the factorial of a number in Lua and make the result of the computation available in C. Go into the ```factorial``` directory. The Lua code is in the file factorial.lua:

```
function factorial(n)
  local result = 1
  for i = 1, n do
    result = result * i
  end
  return result
end
```

From C, we'll call factorial.lua and then get the result from the Lua stack and print to stdout.

```
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
  lua_pushnumber(L, n); // push argument
	
  if (lua_pcall(L, 1, 1, 0) != 0) // 1 argument, 1 return value
  {
    fprintf(stderr, "%s\n", lua_tostring(L, -1));
    return 1;
  }
  
  int result = lua_tonumber(L, -1);
  lua_pop(L, 1); // pop returned value
  printf("%d! is %d\n", n, result);  
    
  lua_close(L); 
  return 0;
}
```

This example is simple:

1. First, we load the Lua file containing the factorial function
- Call ```lua_pcall``` to execute the script and create the functions
- Call ```lua_getglobal``` to specify which function you wish to call
- Push the function arguments to the stack, in order
- Call ```lua_pcall``` again, specifying the number of arguments and return values
- Use ```lua_tointeger``` to get the return value from the top of the stack

You'll notice that at the end we pop the returned value from the stack with ```lua_pop```. For such a simple example, this is not needed since we're killing Lua afterwards, but for more complex uses you'll definitely want to clean up the stack before moving on.

You can compile with ```gcc factorial.c -pagezero_size 10000 -image_base 100000000 -I$TORCH_HOME/include -I/usr/local/include -L/usr/local/lib -L$TORCH_HOME/lib -llua -lm -o fact.out```. You can run with ```./fact.out <some integer>```

### Example 2

Now we'll use Torch to compute the trace of a matrix. For this, we'll simply execute a Lua script that requires Torch:

```
require('torch')

function cosine(theta)
  return torch.cos(theta)
end
```

This example is functionally identical to the previous one. You should be able to do this on your own now. If you run into any trouble, just look at torch-test.c.

## Using Lua From Java



This requires the JNI. So, first, we'll look at how to use it for a simple example.

### Hello, World, v2.0

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

### Using Torch from Java

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

### Configuring Lua State

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

### Returning Function References

Above, we stored a value in a global variable so that we could use it again. That would be find for storing a constant, such as Avogadro's number. But what if you want to call a function multiple times? Is a global variable the best way to go? Probably not. Lua lets you create a reference to a function. Lua's C API has a function ```luaL_ref`` that will store the value on top of the stack in the registry and return an integer that refers to the value in the registry. So, you can load the function onto the stack, call ```lua_ref``` to store it in the registry, and then use ```lua_rawgeti``` to push the function onto the stack again. See the example in the ```function-ref-example``` directory. Build with the ```make``` and run with ```java LuaJNI <path to compiled library>.



