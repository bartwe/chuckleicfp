#include <stdio.h> 
#include <stdlib.h> 
#include <string>
#include <list>
#include <iostream>
#include <string.h>

#include "state.h"

void bailout( const char* w )
{
	printf( "%s\n", w );
	exit(1);
}

int main(int argc, char** argv)
{
	std::vector<std::string> lines;
	std::string line;
	int width = -1;
	while (std::getline(std::cin, line)) {
		width = std::max<int>(width, line.size());
		lines.push_back(line);
	}

	printf("%dx%d grid\n", width, lines.size());

	Map& map = *new Map(width, lines.size());
	memset(&map.at(0, 0), ' ', map.n*map.m);
	for (int i=0;i<lines.size(); i++) 
	{
		char* mapline = &map.at(0, i);
		memcpy( mapline, lines[i].data(), width );
		void* pos = memchr(mapline, 'R', width);
		if (pos) map.setRover((char*)pos-mapline, i);
	}

	return 0;
}
