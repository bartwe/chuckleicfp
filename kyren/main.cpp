#include "Solvers.hpp"

#include <cstdlib>
#include <csignal>
#include <ctime>

#include <sys/types.h>
#include <signal.h>
#include <unistd.h>


Best m_best;
std::shared_ptr<Problem> m_problem;

static void sighandler(int signum) {
  auto solution = m_best.solution();
#ifndef RELEASE
	printf("# score: %d\n", solution.score);
#endif
	printf("%s\n", commandString(solution.commands).c_str());
	fflush(stdout);
	kill(getpid(), 9);
}

int main(int argc, char** argv) {
	// Now register
	signal(SIGINT, sighandler);

  srand(time(NULL));

  m_problem = Problem::read(std::cin);
  WeirdAStarSolver solver(m_best);

  while (true)
    solver.run(Mine(m_problem));
}
