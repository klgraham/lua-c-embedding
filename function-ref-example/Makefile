CC=gcc
LUAFLAGS=-I/usr/local/include -I$(TORCH_HOME)/include -I$(TORCH_HOME)/include/TH  -L/usr/local/lib -L$(TORCH_HOME)/lib -llua -lluaT -lm -shared
CFLAGS=-I$(JAVA_HOME)/include $(LUAFLAGS) -I. -fPIC

all: libfact.dylib

libfact.dylib: LuaJNI.o
	$(CC) -o $@ $< $(CFLAGS)

LuaJNI.o: LuaJNI.c LuaJNI.h
	$(CC) -c $< -o $@ $(CFLAGS)

LuaJNI.h: LuaJNI.class
	javah LuaJNI

LuaJNI.class: LuaJNI.java
	javac $<

clean:
	rm *.o *.class *.dylib LuaJNI.h
