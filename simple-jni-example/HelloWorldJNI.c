#include <jni.h>
#include <stdio.h>
#include "HelloWorldJNI.h"
 
JNIEXPORT void JNICALL Java_HelloWorldJNI_hello(JNIEnv *env, jobject thisObj)
{
  printf("Hello, World!\n");
  return;
}
