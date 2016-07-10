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

## Using Lua from Java

We're going to adapt the earlier Lua example where we compute the factorial of a number. This is on the jni-example branch. **Am currently having the problem where all the LuaJIT/Torch integration examples looked at thus far work perfectly well, but the JNI examples do not. For some reason, the ```lua_newstate()``` command is not working when using the JNI.**

Have tried using the JNI explicitly, in jni-example/jni. You can build and run (though the run fails) by using this:

```make && java FactorialJNI <full path to libfactorial.dylib> <int of your choice>```

Have also tried using Swig, in jni-example/swig, which is a bit simpler. You can build and run (though the run fails) by using this:

```./build.sh && java main <full path to libfactorial.dylib> <int of your choice>```

Some notes about other users who have gotten a similar problem, though only on the C-side. Note that I get the pure C examples working fine:

- http://comments.gmane.org/gmane.comp.lang.lua.luajit/4817
- http://stackoverflow.com/questions/14840569/sigsegv-error-in-some-lua-c-code
- http://stackoverflow.com/questions/13400660/binding-lua-in-static-library-segfault?rq=1
- http://nticformation.com/solutions_problems.php?tuto=59091&subCategory=c+lua+jni+luajit&Category=C+Language


Based on the last of the above links, I think the issue is that ```-pagezero_size 10000 -image_base 100000000``` is not included when compiling the JNI stuff. The problem is that when it is included, the compiler says 
>-pagezero_size option can only be used when linking a main executable
which means those params can't be used for shared libraries (which JNI needs).