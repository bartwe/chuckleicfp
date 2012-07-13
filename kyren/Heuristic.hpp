#include <vector>

class Mine;

struct Pos { int x,y; };

class SafeZone : public std::vector<char>
{
public:
	inline char& at(int x, int y)
	{
		return (*this)[nx*y+x];
	}

	int nx;
};

class Heuristic
{
public:
	static int bestCase(Mine& mine);

	static void markSafeZone(Mine& mine, SafeZone& o_safezone);


	// If you construct a class like this, it will use the static mine
	// data to make future calls faster... But we need to see in what way
	// you'll need to have the heuristic notified of changes to the mine
	explicit Heuristic(Mine& mine);
private:
	std::vector<Pos> lambdas;
};
