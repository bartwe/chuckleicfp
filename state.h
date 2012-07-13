#include <vector>

class Map
{
public:
	std::vector<char> data;
	int n, m;

	Map(int n_, int m_)
		: n(n_), m(m_)
	{
		data.resize(n*m);
	}


	inline char& at(int x, int y)
	{
		return data[n*y + x];
	}

	void update();
};
