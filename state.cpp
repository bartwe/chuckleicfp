#include <state.h>

struct Move { int dx, dy; };

class Moves
{
	Move moves[128];

	Moves()
	{
		memset(char2move, 0, sizeof char2move);
		char2move['R'].x = 1;
		char2move['L'].x = -1;
		char2move['U'].y = 1;
		char2move['D'].y = -1;
	}
};
static Moves moves;

bool Map::validateMove(char move)
{
	int ok = 1;

	int nx  = rx + moves[move].dx;
	int nnx = nx + moves[move].dx;
	int ny  = ry + moves[move].dy;

	if ( nx < 0 || nx >= n ) ok = 0;
	if ( ny < 0 || ny >= m ) ok = 0;

	char atnext = map.at(nx, ny);
	if (atnext == ' ' || atnext == '.' || atnext == '\\' || atnext == 'O')
		return ok;

	if (atnext == '*' && ny == y && nnx >= 0 && nnx < n && at(nnx, ny) == ' ')
		return ok;

	return 0;
}

Map::update()
{
}
