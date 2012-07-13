#include "Heuristic.hpp"
#include "Mine.hpp"

int Heuristic::bestCase(Mine& mine)
{
	if (mine.state != State::InProgress)
		return mine.score();

	// Assuming we can reach the exit, we still need one step for each lambda to
	// collect
	return mine.score()
		+ (mine.numInitialLambdas-mine.collectedLambdas)*24
		+ mine.numInitialLambdas * 50;
}
