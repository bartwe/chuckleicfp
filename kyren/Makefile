CXX=g++
#CXXFLAGS=-std=c++0x -g
#CXXFLAGS=-std=c++0x -O3
CXXFLAGS=-std=c++0x -O3 -DNDEBUG

CPPFILES = $(wildcard *.cpp)
OFILES = $(patsubst %.cpp,%.o,$(CPPFILES))
HFILES = $(wildcard *.h) $(wildcard *.hpp)

<<<<<<< HEAD
all: clean main libkyren.so release


=======
all: clean lifter
>>>>>>> 64986bd24cbcdc0c845534187b35b1ebabda2be4

lifter: $(OFILES)
	$(CXX) $(CXXFLAGS) -static $^ -o lifter

$(OFILES): $(HFILES)

<<<<<<< HEAD
release: lifter
lifter: $(HFILES) $(CPPFILES)
	g++ -DRELEASE $(CXXFLAGS) $(CPPFILES) -static -o release

libkyren.so: $(HFILES) $(CPPFILES)
	g++ -DGUI $(CXXFLAGS) -fpic $(CPPFILES) -shared -o $@

=======
>>>>>>> 64986bd24cbcdc0c845534187b35b1ebabda2be4
clean:
	rm -f *.o *.so lifter
