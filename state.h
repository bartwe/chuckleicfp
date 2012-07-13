#include <vector>

class Map
{
public:
	std::vector<char> data;
	int n, m;
	int rx, ry;

	int lambdacollect;

	Map(int n_, int m_)
		: n(n_), m(m_)
		  , lambdacollect(0)
	{
		data.resize(n*m);
	}

	void setRover(int x, int y)
	{
		rx = x; ry = y;
	}


	inline char& at(int x, int y)
	{
		return data[n*y + x];
	}

	void update();
	void domove(char move);
};
