#pragma once

#include <vector>
#include "Mine.hpp"
#include "Grid.hpp"

typedef Grid<char> SafeZone;

class Heuristic
{
public:
	static int bestCase(Mine& mine);

	static void markSafeZone(Mine& mine, SafeZone& o_safezone);

	// will append path
	void findPathToNearestLambda(Mine& mine, SafeZone& safezone, RobotCommands& o_commands);

	// If you construct a class like this, it will use the static mine
	// data to make future calls faster... But we need to see in what way
	// you'll need to have the heuristic notified of changes to the mine
	explicit Heuristic(Mine& mine);
private:
	std::vector<Position> lambdas;
};
