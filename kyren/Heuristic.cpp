#include "Heuristic.hpp"
#include "Mine.hpp"
#include <string.h>

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

Heuristic::Heuristic(Mine& mine)
{
	lambdas.reserve(mine.numInitialLambdas);
	for (int y=0;y<mine.height;y++) for (int x=0;x<mine.width;x++)
	{
		if (mine.get(x, y) == MineContent::Lambda)
		{
			lambdas.push_back({x,y});
		}
	}
}

void Heuristic::markSafeZone(Mine& mine, SafeZone& o_safezone)
{
	o_safezone.resize(mine.height * mine.width);
	o_safezone.nx = mine.width;
	memset(&o_safezone[0], 0, o_safezone.size());

	// note: we explore anything except the border, so that references are
	// always okay
	for (int y=1;y<mine.height-1;y++) for (int x=1;x<mine.width-1;x++) {
		MineContent tile = mine.get(x, y);
		if (tile == MineContent::Empty) {
			o_safezone.at(x, y) = 1;
		}
		else if (tile == MineContent::Earth || tile == MineContent::Lambda) {
			// only safe under certain conditions: no rock will
			// fall if this tile is cleared
			MineContent above = mine.get(x, y+1);
			if (above != MineContent::Empty && above != MineContent::Rock) {
				// somethign solid above, so now only if left
				// or right is a rock we have a potential problem

				if (mine.get(x-1, y) != MineContent::Rock && mine.get(x+1, y) != MineContent::Rock )
				{
					o_safezone.at(x, y) = 1;
				}
			}
		}
	}
}
