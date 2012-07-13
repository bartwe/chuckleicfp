#include "state.h"
#include "main.h"

extern "C" {

Map* Init()
{
	return readMap();
}

char* GetData(Map* m, int* width, int* height)
{
	*width = m->n;
	*height = m->m;
	return &m->data.front();
}

void DoMove(Map* m, char move)
{
	m->domove(move);
	m->update();
}

} // extern
