struct Move { int dx, dy; };

struct Moves
{
        Move m[4];
	char mcommands[4];

        Moves();
};
extern Moves moves;

