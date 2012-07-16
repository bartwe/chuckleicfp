CXX=g++
#CXXFLAGS=-std=c++0x -g
#CXXFLAGS=-std=c++0x -O3
CXXFLAGS=-std=c++0x -O3 -DNDEBUG

CPPFILES = $(wildcard *.cpp)
OFILES = $(patsubst %.cpp,%.o,$(CPPFILES))
HFILES = $(wildcard *.h) $(wildcard *.hpp)

all: clean lifter

lifter: $(OFILES)
	$(CXX) $(CXXFLAGS) -static $^ -o lifter

$(OFILES): $(HFILES)

clean:
	rm -f *.o *.so lifter
