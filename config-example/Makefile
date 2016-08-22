CC=gcc
LUAFLAGS=-I/usr/local/include -I$(TORCH_HOME)/include -I$(TORCH_HOME)/include/TH  -L/usr/local/lib -L$(TORCH_HOME)/lib -llua -lluaT -lm -shared
CFLAGS=-I$(JAVA_HOME)/include $(LUAFLAGS) -I. -fPIC

all: libcomputation.dylib

libcomputation.dylib: ComputationJNI.o
	$(CC) -o $@ $< $(CFLAGS)

ComputationJNI.o: ComputationJNI.c ComputationJNI.h
	$(CC) -c $< -o $@ $(CFLAGS)

ComputationJNI.h: ComputationJNI.class
	javah ComputationJNI

ComputationJNI.class: ComputationJNI.java
	javac $<

clean:
	rm *.o *.class *.dylib ComputationJNI.h
