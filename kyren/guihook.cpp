#include "Mine.hpp"
#include <iostream>
#include <fstream>

extern "C" {

Mine* Init(const char* map)
{
	Mine* res = new Mine;
	if ( map )
	{
		std::ifstream is(map);
		res->read(is);
	}
	else
	{
		res->read(std::cin);
	}

	return res;
}

char* GetData(Mine* m, int* width, int* height)
{
	*width = m->width;
	*height = m->height;
	return (char*)&m->content[0];
}

void DoMove(Mine* m, char move)
{
	m->pushMove(Mine::charToCommand(move));
	printf("Score: %d\n", m->score());
}

} // extern
