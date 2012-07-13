CXXFLAGS = -O3

all: prog shlib

prog: main.o state.o guihook.o
	g++ -o prog $<
shlib:
	g++ $(CXXFLAGS) -fPIC *.cpp -shared -o libicfp.so

clean:
	rm *.o *.so  prog
