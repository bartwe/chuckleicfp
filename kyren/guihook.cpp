#include "Mine.hpp"
#include <iostream>
#include <fstream>
#include "Heuristic.hpp"

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

char* GetSafeZone(Mine* m)
{
	static SafeZone sz;
	Heuristic::markSafeZone(*m, sz);
	return &sz[0];
}

int DoMove(Mine* m, char move)
{
	m->pushMove(Mine::charToCommand(move));
	//printf("Score: %d, state=%s\n", m->score(), Mine::stateToString(m->state).c_str());
	return m->score();
}

} // extern
