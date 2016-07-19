CC=gcc 
LUAFLAGS=-I/usr/local/include -I$(TORCH_HOME)/install/include -L/usr/local/lib -L$(TORCH_HOME)/install/lib -llua -lm -shared
CFLAGS=-I$(JAVA_HOME)/include $(LUAFLAGS) -I. -fPIC

all: libfactorial.dylib

libfactorial.dylib: FactorialJNI.o 
	$(CC) -o $@ $< $(CFLAGS) 

FactorialJNI.o: FactorialJNI.c FactorialJNI.h
	$(CC) -c $< -o $@ $(CFLAGS)

#luatorch.o: luatorch.c
#	$(CC) -c $< -o $@ $(CFLAGS)

FactorialJNI.h: FactorialJNI.class
	javah FactorialJNI

FactorialJNI.class: FactorialJNI.java
	javac $<

clean:
	rm *.o *.class *.dylib FactorialJNI.h
