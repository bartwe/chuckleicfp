#pragma once

#include <stdio.h>

// You can later turn this into an assert if needed
#define REQUIRES

#ifdef REQUIRES
#define REQUIRE(x) {if (!(x)) { fprintf(stderr, "%s%d\n", "REQUIRE failed: " #x " in file " __FILE__ " on line ", __LINE__); throw std::exception();}}
#else
#define REQUIRE(x)
#endif


struct Move { int dx, dy; };

struct Moves
{
        Move m[4];
	char mcommands[4];

        Moves();
};
extern Moves moves;

