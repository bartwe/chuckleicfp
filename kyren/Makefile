CXX=g++
CXXFLAGS=-std=c++0x -O3

all: main libkyren.so

main: main.o Mine.o sha1.o Searcher.o Heuristic.o Util.o
	$(CXX) $(CXXFLAGS) $^ -o main

main.o: Searcher.hpp Mine.hpp
Mine.o: Mine.hpp
Searcher.o: Searcher.hpp Mine.hpp

libkyren.so: $(wildcard *.hpp) $(wildcard *.cpp)
	g++ $(CXXFLAGS) -fPIC *.cpp -shared -o $@

clean:
	rm -f *.o main
