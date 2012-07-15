#ifdef GUI

#include "Mine.hpp"
#include <iostream>
#include <fstream>
#include "Problem.hpp"
#include "Best.hpp"
#include "Searcher.hpp"

Best g_best;
Grid<Tile> g_grid;
std::string g_bestCommands;
Mine g_mine;

extern "C" {

Mine* Init(const char* map)
{
	auto problem = std::make_shared<Problem>();
	if ( map )
	{
		std::ifstream is(map);
		problem = problem->read(is);
	}
	else
	{
		problem = problem->read(std::cin);
	}
  
  g_mine = Mine(problem);

	return &g_mine;
}

uint8_t* GetData(Mine* m, int* width, int* height)
{
	*width = m->getProblem()->width;
	*height = m->getProblem()->height;
  g_grid.reset(*width, *height, Tile::Wall);
  for (int y = 0; y < *height; ++y)
    for (int x = 0; x < *width; ++x)
      g_grid.at(x, y) = m->get(x, y);

	return (uint8_t*)g_grid.getGrid();
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

char const* GetBest()
{
  g_bestCommands = commandString(g_best.solution().commands);
  return g_bestCommands.c_str();
}

} // extern

#endif
