CXX=g++-4.7
CFLAGS=-std=c++11 -O2

main: main.o mine.o sha1.o searcher.o
	$(CXX) $(CFLAGS) main.o mine.o sha1.o searcher.o -o main

main.o: main.cpp Searcher.hpp Mine.hpp
	$(CXX) $(CFLAGS) main.cpp -c -o main.o

mine.o: Mine.cpp Mine.hpp
	$(CXX) $(CFLAGS) Mine.cpp -c -o mine.o

sha1.o: sha1.cpp sha1.h
	$(CXX) $(CFLAGS) sha1.cpp -c -o sha1.o

searcher.o: Searcher.hpp Searcher.cpp Mine.hpp
	$(CXX) $(CFLAGS) Searcher.cpp -c -o searcher.o

clean:
	rm -f mine.o main.o sha1.o searcher.o main
