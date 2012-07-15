#include "Solvers.hpp"

#include <cstdlib>
#include <csignal>
#include <ctime>

Best m_best;

static void sighandler(int signum) {
  auto solution = m_best.solution();
	printf("# score: %d\n", solution.score);
	printf("%s\n", commandString(solution.commands).c_str());
	abort();
}

int main(int argc, char** argv) {
	// Now register
	signal(SIGINT, sighandler);

  srand(time(NULL));

  auto problem = Problem::read(std::cin);
  RandomDijkstraSolver solver(m_best, 10000, 2000, 1000);

  while (true)
    solver.run(Mine(problem));

  return 0;
}
