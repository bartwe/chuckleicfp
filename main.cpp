#include <stdio.h> 
#include <string>
#include <list>
#include <iostream>

#include "state.h"

int main(int argc, char** argv)
{
	//Map& map = *new Map;

	std::list<std::string> lines;
	std::string line;
	while (std::getline(std::cin, line)) {
		lines.push_back(line);
	}

	printf("%d lines\n", line.size());

	return 0;
}
