CC=gcc 
CFLAGS=-I$(JAVA_HOME)/include -I/usr/local/include/luajit-2.1 -L/usr/local/lib -lluajit -lm -ldl -shared -I. -fPIC


# gcc -o libfactorial.dylib -I"$JAVA_HOME/include" -shared  -I/usr/local/include/luajit-2.1 -L/usr/local/lib -lluajit -lm -ldl FactorialJNI.c

all: libfactorial.dylib

libfactorial.dylib: FactorialJNI.o 
	$(CC) -o $@ $< $(CFLAGS) 

factorial.o: FactorialJNI.c FactorialJNI.h
	$(CC) -c $< -o $@ $(CFLAGS)

#luatorch.o: luatorch.c
#	$(CC) -c $< -o $@ $(CFLAGS)

FactorialJNI.h: FactorialJNI.class
	javah FactorialJNI

FactorialJNI.class: FactorialJNI.java
	javac $<

clean:
	rm *.o *.class *.dylib *.h
