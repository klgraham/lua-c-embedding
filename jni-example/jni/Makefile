CC=gcc 
LUAFLAGS=-I/usr/local/include/luajit-2.1 -L/usr/local/lib -lluajit -lm -ldl -shared
CFLAGS=-I$(JAVA_HOME)/include $(LUAFLAGS) -I. -fPIC


# gcc -o libfactorial.dylib -I"$JAVA_HOME/include" -shared  -I/usr/local/include/luajit-2.1 -L/usr/local/lib -lluajit -lm -ldl FactorialJNI.c

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
