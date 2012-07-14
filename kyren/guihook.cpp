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
	Tile* grid = m->content.getGrid();
	return (char*)grid;
}

void GetInfo(Mine* m, char* str, int buflen, int* waterlevel)
{
	snprintf(str, buflen-1, "SCORE=%d (state=%s, move %d, submerged for %d)",
			m->score(), Mine::stateToString(m->state).c_str(),
			m->totalMoves, m->submergedSteps);

	*waterlevel = m->curWaterLevel;
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
