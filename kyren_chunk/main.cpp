#include "Searcher.hpp"
#include "Best.hpp"

#include <signal.h>

Best m_best;

static void sighandler(int signum) {
  auto solution = m_best.solution();
	printf("# score: %d\n", solution.score);
	printf("%s\n", commandString(solution.commands).c_str());
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
