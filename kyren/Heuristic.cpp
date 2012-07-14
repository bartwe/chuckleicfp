#include "Heuristic.hpp"
#include "Mine.hpp"
#include <string.h>
#include <deque>
#include "Util.hpp"

int Heuristic::bestCase(Mine& mine)
{
	if (mine.state != State::InProgress)
		return mine.score();

	// Assuming we can reach the exit, we still need one step for each lambda to
	// collect
	return mine.score()
		+ (mine.numInitialLambdas-mine.var.collectedLambdas)*24
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
	o_safezone.reset(mine.width, mine.height, 0);

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

void Heuristic::findPathToNearestLambda(Mine& mine, SafeZone& safezone, RobotCommands& o_commands)
{
	struct pathinfo
	{
		int dist;
		int lastmove;
	};
	Grid<pathinfo> shortest;
	std::deque<Position> todo;

	shortest.reset(mine.width, mine.height, {mine.width*mine.height, -1});

	todo.push_back({mine.var.robotX, mine.var.robotY});
	shortest.at(mine.var.robotX, mine.var.robotY) = {0, -1};

	while (!todo.empty()) {
		Position cur = todo.front();
		todo.pop_front();

		pathinfo& thispathinfo = shortest.at(cur.x, cur.y);

		for (Move& m : moves.m) {
			Position np = {cur.x + m.dx, cur.y + m.dy};
			if ( !safezone.at(np.x, np.y) ) continue;

#if 1 // if you want to short-circuit
			if  ( mine.get(np.x, np.y) == MineContent::Lambda )
			{
				// Got it!
				RobotCommands cmds;
				while ( np.x != mine.var.robotX && np.y != mine.var.robotY ){
					pathinfo& pi = shortest.at(np.x, np.y);
					cmds.push_back( Mine::charToCommand(moves.mcommands[pi.lastmove]) );
					np.x -= moves.m[pi.lastmove].dx;
					np.y -= moves.m[pi.lastmove].dy;
				}
				std::copy_backward(cmds.begin(), cmds.end(), o_commands.end());
			}
#endif
			if ( shortest.at(np.x, np.y).dist > thispathinfo.dist + 1 )
			{
				shortest.at(np.x, np.y).dist = thispathinfo.dist + 1;
				shortest.at(np.x, np.y).lastmove = (&m) - &moves.m[0];
				todo.push_back(np);
			}
		}
	}
}
