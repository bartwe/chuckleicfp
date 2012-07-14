#ifdef GUI

#include "Mine.hpp"
#include <iostream>
#include <fstream>
#include "Heuristic.hpp"
#include "Heuristic.hpp"
#include "BestSoFar.h"
#include "Searcher.hpp"

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
	return sz.getGrid();
}

int DoMove(Mine* m, char move)
{
	m->pushMove(Mine::charToCommand(move));
	//printf("Score: %d, state=%s\n", m->score(), Mine::stateToString(m->state).c_str());
	return m->score();
}

static void* docalc(void* arg)
{
        Mine* mine = (Mine*)arg;

        // Copied from main... todo: share code
        Searcher searcher(*mine);;
        auto result = searcher.bruteForce(24);

        return 0;
}

void GoForIt(Mine* m)
{
	Mine* workingcopy = new Mine(*m);
	pthread_t thread;
	pthread_create(&thread, 0, docalc, workingcopy);

        // we leak the thread, whatever
}

char* GetBest()
{
  return Best::GetBest();
}

} // extern

#endif
