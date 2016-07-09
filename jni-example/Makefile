CC=gcc 
LUAFLAGS=-I/usr/local/include/luajit-2.1 -L/usr/local/lib -lluajit -lm -ldl -shared
CFLAGS=-I$(JAVA_HOME)/include $(LUAFLAGS) -I.


# gcc -o libfactorial.dylib -I"$JAVA_HOME/include" -shared  -I/usr/local/include/luajit-2.1 -L/usr/local/lib -lluajit -lm -ldl FactorialJNI.c

all: libfactorial.dylib

libfactorial.dylib: FactorialJNI.o luatorch.o luatorch.h
	$(CC) -o $@ $< $(LUAFLAGS) -I.

FactorialJNI.o: FactorialJNI.c FactorialJNI.h luatorch.h
	$(CC) -c $< -o $@ $(CFLAGS)

luatorch.o: luatorch.c
	$(CC) -c $< -o $@ $(LUAFLAGS)

FactorialJNI.h: FactorialJNI.class
	javah FactorialJNI

FactorialJNI.class: FactorialJNI.java
	javac $<

clean:
	rm *.o FactorialJNI.h *.class *.dylib
