#!/bin/bash

CC=gcc

swig -java factorial.i && #make && javac main.java

$CC -c factorial.c factorial_wrap.c -fPIC -I. -I$JAVA_HOME/include -I include -I/usr/local/include/luajit-2.1 -L/usr/local/lib -lluajit -lm -ldl  &&

$CC -shared factorial.o factorial_wrap.o -o libfactorial.dylib -I include -I/usr/local/include/luajit-2.1 -L/usr/local/lib -lluajit -lm -ldl &&

javac main.java