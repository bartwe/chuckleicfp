main: main.o mine.o sha1.o
	g++-4.7 -std=c++11 main.o mine.o sha1.o -o main

main.o: main.cpp
	g++-4.7 -std=c++11 main.cpp -c -o main.o

mine.o: Mine.cpp Mine.hpp
	g++-4.7 -std=c++11 Mine.cpp -c -o mine.o

sha1.o: sha1.cpp sha1.h
	g++-4.7 -std=c++11 sha1.cpp -c -o sha1.o

clean:
	rm -f mine.o main.o sha1.o main
