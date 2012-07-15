#include "Searcher.hpp"
#include "BestSoFar.hpp"

#include <signal.h>

static void sighandler(int signum) {
	printf("# score: %d\n", Best::getBestScore());
	printf("%s\n", Best::getBest());
	exit(0);
}

int main(int argc, char** argv) {
	// Now register
	signal(SIGINT, sighandler);

  Mine mine;
  mine.read(std::cin);

  Best::reserveSpace(4 * 1024 * 1024);

  Searcher searcher;
  searcher.bruteForce(mine, 24, [](RobotCommands const& commands, int score) {
      if (Best::isImprovement(score))
        Best::improveSolution(score, commandString(commands).c_str());
    });

  sighandler(SIGINT);

  return 0;
}
