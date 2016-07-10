#!/bin/bash

CC=gcc

swig -java factorial.i &&

$CC -c factorial.c factorial_wrap.c -fPIC -I. -I$JAVA_HOME/include -I/usr/local/include/luajit-2.1 -L/usr/local/lib -lluajit -lm -ldl  &&

$CC -shared factorial.o factorial_wrap.o -o libfactorial.dylib -I/usr/local/include/luajit-2.1 -L/usr/local/lib -lluajit -lm -ldl &&

javac main.java