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
	case LAMBDA: lambdacollect++;
	case EMPTY: // ok
	case EARTH: // ok
		   break;

	// Special one...
	case GATE: // todo: check whether open, do stuff then
		   break;

	// Boulder:
	case ROCK:
		if (ny == ry && nnx >= 0 && nnx < n && at(nnx, ny) == EMPTY)
		{
			at(nnx, ny) = ROCK;
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
	at(rx, ry) = EMPTY;
}

static char& rightof(char& pos)
{
	return *( (&pos) + 1 );
}
static char& leftof(char& pos)
{
	return *( (&pos) - 1);
}

void Map::update()
{
	for (int y = 1; y<m; y++) for (int x = 0; x<n; x++)
	{
		char& self = at(x, y);
		if (self != ROCK) continue;
		char& under = at(x, y-1);

		if (under == EMPTY) {
			self = EMPTY;
			under = ROCK;
		}
		else if ( under == ROCK || under == LAMBDA ) {
			if (x+1 < n && rightof(self) == EMPTY && rightof(under) == EMPTY) {
				queueupdate(x, y, EMPTY);
				queueupdate(x+1, y-1, self);
			}
			else if ( under == ROCK && x>0 && leftof(self) == EMPTY && leftof(under) == EMPTY ) {
				queueupdate(x, y, EMPTY);
				queueupdate(x-1, y-1, self);
			}
		}
	}

	commitupdates();
}

void Map::queueupdate(int x, int y, char newcell)
{
	Update u = {x, y, newcell};
	updqueue.push_back( u );
}
void Map::commitupdates()
{
	for ( updqueue_t::iterator it = updqueue.begin(); it != updqueue.end(); ++it )
	{
		at(it->x, it->y) = it->c;
	}
	updqueue.clear();
}

