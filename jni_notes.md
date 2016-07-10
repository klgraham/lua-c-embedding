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

We're going to adapt the earlier Lua example where we compute the factorial of a number. This is on the jni-example branch.

Have tried using the JNI explicitly, in jni-example/jni. You can build and run (though the run fails) by using this:

```make && java FactorialJNI <full path to libfactorial.dylib> <int of your choice>```

Have also tried using Swig, in jni-example/swig, which is a bit simpler. You can build and run (though the run fails) by using this:

```./build.sh && java main <full path to libfactorial.dylib> <int of your choice>```

