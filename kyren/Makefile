CXX=g++-4.7
CXXFLAGS=-std=c++11 -O3

main: main.o Mine.o sha1.o Searcher.o Heuristic.o
	$(CXX) $(CXXFLAGS) $^ -o main

main.o: Searcher.hpp Mine.hpp
Mine.o: Mine.hpp
Searcher.o: Searcher.hpp Mine.hpp

clean:
	rm -f *.o main
