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
	//Map& map = *new Map;

	std::vector<std::string> lines;
	std::string line;
	int width = -1;
	while (std::getline(std::cin, line)) {
		if (width < 0) width = line.size();
		if ( width != line.size() )
			bailout("Input error"); 

		lines.push_back(line);
	}

	printf("%dx%d grid\n", width, lines.size());

	Map& map = *new Map(width, lines.size());
	for (int i=0;i<lines.size(); i++) 
	{
		memcpy( &map.at(0, i), lines[i].data(), width );
	}

	return 0;
}
