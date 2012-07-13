#include "state.h"

#include <string.h>

struct Move { int dx, dy; };

struct Moves
{
	Move moves[128];

	Moves()
	{
		memset(moves, 0, sizeof moves);
		moves['R'].dx = 1;
		moves['L'].dx = -1;
		moves['U'].dy = 1;
		moves['D'].dy = -1;
	}
};
static Moves moves;

void Map::domove(char move)
{
	int ok = 1;

	int nx  = rx + moves.moves[move].dx;
	int nnx = nx + moves.moves[move].dx;
	int ny  = ry + moves.moves[move].dy;

	if ( nx < 0 || nx >= n ) ok = 0;
	if ( ny < 0 || ny >= m ) ok = 0;

	char atnext = at(nx, ny);
	switch (atnext) {

	// These ones are always okay:
	case '\\': lambdacollect++;
	case ' ': // ok
	case '.': // ok
	case 'O': // todo
		   break;

	// Boulder:
	case '*':
		if (ny == ry && nnx >= 0 && nnx < n && at(nnx, ny) == ' ')
		{
			at(nnx, ny) = '*';
			//at(nx, ny) = ' '; // rover position is always //undefined...?
		}
		else
		{
			return;
		}
		break;
	default:
		// not okay, execute wait
		return;
	}

	setRover(nx, ny);
	at(rx, ry) = ' ';
}

void Map::update()
{
}
