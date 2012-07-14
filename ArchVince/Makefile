CXX=g++
CXXFLAGS=-std=c++0x -O3

all: main

main: main.o Mine.o NodeMap.o Pathfinder.o
	$(CXX) $(CXXFLAGS) $^ -o main

main.o: Mine.h NodeMap.h Pathfinder.h
Mine.o: Mine.h
NodeMap.o: Pathfinder.h
Pathfinder.o: NodeMap.h Pathfinder.h

clean:
	rm -f *.o main
