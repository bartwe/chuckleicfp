#include <vector>

class Map
{
public:
	std::vector<char> data;
	int n, m;
	int rx, ry;

	Map(int n_, int m_)
		: n(n_), m(m_)
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
};
