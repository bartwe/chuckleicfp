#include <vector>
#include "Mine.hpp"

struct Pos { int x,y; };

template<typename what>
class CellBasedData : public std::vector<what>
{
public:
	void init(int width, int height, what initialvalue)
	{
		nx = width;
		this->clear();
		this->resize(width*height, initialvalue);
	}
	inline what& at(int x, int y)
	{
		return (*this)[nx*y+x];
	}

	int nx;
};

typedef CellBasedData<char> SafeZone;

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
	std::vector<Pos> lambdas;
};
