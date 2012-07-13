#include "Heuristic.hpp"
#include "Mine.hpp"

int Heuristic::bestCase(Mine& mine)
{
	return mine.score();
}
