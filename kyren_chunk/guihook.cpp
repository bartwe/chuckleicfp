#ifdef GUI

#include "Mine.hpp"
#include <iostream>
#include <fstream>
#include "Problem.hpp"
#include "Best.hpp"
#include "Searcher.hpp"

Best g_best;

extern "C" {

Mine* Init(const char* map)
{
	auto problem = std::make_shared<Problem>();
	if ( map )
	{
		std::ifstream is(map);
		problem->read(is);
	}
	else
	{
		problem->read(std::cin);
	}
  
  Mine* mine = new Mine(problem);

	return mine;
}

uint32_t* GetData(Mine* m, int* width, int* height)
{
	*width = m->getProblem()->width;
	*height = m->getProblem()->height;
	Tile* grid = m->getContent()->getGrid();
	return (uint32_t*)grid;
}

void GetInfo(Mine* m, char* str, int buflen, int* waterlevel)
{
	snprintf(str, buflen-1, "SCORE=%d (state=%s, move %d, L=%d/%d, submerged for %d)",
			m->score(), stateToString(m->currentState()).c_str(),
			m->moveCount(), m->currentVariadicState().collectedLambdas, m->getProblem()->numInitialLambdas,
			m->currentVariadicState().submergedSteps);

	*waterlevel = m->currentVariadicState().curWaterLevel;
}

int DoMove(Mine* m, char move)
{
  m->doCommand(charToCommand(move));
	//printf("Score: %d, state=%s\n", m->score(), stateToString(m->state).c_str());
	return m->score();
}

char* GetBest()
{
  return g_best.getBest();
}

} // extern

#endif