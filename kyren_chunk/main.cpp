#include "Searcher.hpp"
#include "Best.hpp"

#include <signal.h>

Best m_best(4 * 1024 * 1024);

static void sighandler(int signum) {
	printf("# score: %d\n", m_best.getBestScore());
	printf("%s\n", m_best.getBest());
	exit(0);
}

int main(int argc, char** argv) {
	// Now register
	signal(SIGINT, sighandler);

  auto problem = Problem::read(std::cin);

  Mine mine(problem);

  Searcher searcher;
  searcher.bruteForce(m_best, mine, 24);

  sighandler(SIGINT);

  return 0;
}
