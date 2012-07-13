all: prog

prog: main.o
	g++ -o prog $<
