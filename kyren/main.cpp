#include "Solvers.hpp"

#include <cstdlib>
#include <csignal>
#include <ctime>

#include <sys/types.h>
#include <signal.h>
#include <unistd.h>

// Holds global data for the signal handler, so the signal handler can exit(0)
// without destructors being run.. kind of blech, but it's fine.
struct RunInfo {
  Best best;
  std::shared_ptr<Problem> problem;
};

RunInfo* g_info;

static void sighandler(int signum) {
  auto solution = g_info->best.solution();
#ifndef NDEBUG
	printf("Score: %d\n", solution.score);
#endif
	printf("%s\n", commandString(solution.commands).c_str());
  exit(0);
}

int main(int argc, char** argv) {
  g_info = new RunInfo;
	signal(SIGINT, sighandler);
  srand(time(NULL));

  g_info->problem = Problem::read(std::cin);
  WeirdAStarSolver solver(g_info->best);
  while (true)
    solver.run(Mine(g_info->problem));
}
