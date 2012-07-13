#include <vector>

#define ROBOT 'R'
#define ROCK '*'
#define GATE 'L'
#define EARTH '.'
#define WALL '#'
#define LAMBDA '\\'
// #define OPEN_LAMBDA 'O'  // not going to use, use 'GATE'
#define EMPTY ' '

struct Update
{
	int x, y;
	char c;
};

class Map
{
public:
	std::vector<char> data;
	typedef std::vector<Update> updqueue_t;
	updqueue_t updqueue;

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
		at(x,y) = ROBOT;
	}


	inline char& at(int x, int y)
	{
		return data[n*y + x];
	}

	void update();
	void domove(char move);

	void queueupdate(int x, int y, char newcell);
	void commitupdates();
};
