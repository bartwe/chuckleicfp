#include "Solvers.hpp"

#include <cstdlib>
#include <csignal>
#include <ctime>

Best m_best;
std::shared_ptr<Problem> m_problem;

static void sighandler(int signum) {
  auto solution = m_best.solution();
	printf("# score: %d\n", solution.score);
	printf("%s\n", commandString(solution.commands).c_str());
  /*
  Mine m(m_problem);
  m.evaluateAndPrint(solution.commands);
  */
	abort();
}

int main(int argc, char** argv) {
	// Now register
	signal(SIGINT, sighandler);

  srand(time(NULL));

  m_problem = Problem::read(std::cin);
  RandomDijkstraSolver solver(m_best, 10000, 2000, 1000);

  while (true)
    solver.run(Mine(m_problem));

  return 0;
}
