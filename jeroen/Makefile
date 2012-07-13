all: prog shlib

prog: main.o state.o guihook.o
	g++ -o prog $<
shlib:
	g++ -fPIC *.cpp -shared -o libicfp.so

