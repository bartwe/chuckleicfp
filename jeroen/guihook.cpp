#include "state.h"
#include "main.h"
#include <iostream>
#include <fstream>

extern "C" {

Map* Init(const char* map)
{
	if ( map )
	{
		std::ifstream is(map);
		return readMap(is);
	}
	else
	{
		return readMap(std::cin);
	}
}

char* GetData(Map* m, int* width, int* height)
{
	*width = m->n;
	*height = m->m;
	return &m->data.front();
}

void DoMove(Map* m, char move)
{
	m->domove(move);
	m->update();
}

} // extern
