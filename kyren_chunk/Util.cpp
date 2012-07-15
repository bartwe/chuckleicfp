#include "Util.hpp"
#include <string.h>

Moves moves;
Moves::Moves()
{
	memset(m, 0, sizeof moves);
	int i=0;
	m[i++].dx = 1;
	m[i++].dx = -1;
	m[i++].dy = 1;
	m[i++].dy = -1;
	memcpy(mcommands, "RLUD", 4);
}
