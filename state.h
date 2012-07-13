#include <vector>

class Map
{
	std::vector<char> data;
public:
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
};
