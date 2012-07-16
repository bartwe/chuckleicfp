CXX=g++
#CXXFLAGS=-std=c++0x -g
CXXFLAGS=-std=c++0x -O3
#CXXFLAGS=-std=c++0x -O3 -DNDEBUG

CPPFILES = $(wildcard *.cpp)
OFILES = $(patsubst %.cpp,%.o,$(CPPFILES))
HFILES = $(wildcard *.h) $(wildcard *.hpp)

all: clean main libkyren.so release



main: $(OFILES)
	$(CXX) $(CXXFLAGS) -static $^ -o main

$(OFILES): $(HFILES)

release: lifter
lifter: $(HFILES) $(CPPFILES)
	g++ -DRELEASE $(CXXFLAGS) $(CPPFILES) -static -o release

libkyren.so: $(HFILES) $(CPPFILES)
	g++ -DGUI $(CXXFLAGS) -fpic $(CPPFILES) -shared -o $@

clean:
	rm -f *.o *.so main
