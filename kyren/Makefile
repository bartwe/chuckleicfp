main: main.o mine.o
	g++-4.7 -std=c++11 main.o mine.o -o main

main.o: main.cpp
	g++-4.7 -std=c++11 main.cpp -c -o main.o

mine.o: Mine.cpp Mine.hpp
	g++-4.7 -std=c++11 Mine.cpp -c -o mine.o

clean:
	rm -f mine.o main.o main
