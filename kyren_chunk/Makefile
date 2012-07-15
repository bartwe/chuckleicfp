CXX=g++
CXXFLAGS=-std=c++0x -O3

CPPFILES = $(wildcard *.cpp)
OFILES = $(patsubst %.cpp,%.o,$(CPPFILES))
HFILES = $(wildcard *.h) $(wildcard *.hpp)

all: main

main: $(OFILES)
	$(CXX) $(CXXFLAGS) -static $^ -o main

$(OFILES): $(HFILES)

clean:
	rm -f *.o *.so main
