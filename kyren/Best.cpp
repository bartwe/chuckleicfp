#include "Best.hpp"

#include <algorithm>

Best::Best() {
  dabest = std::make_shared<Solution>(Solution{{RobotCommand::Abort}, 0});
  alternate = std::make_shared<Solution>(*dabest);
}

bool Best::isImprovement(int score) {
	return score > dabest->score;
}

void Best::improveSolution(Solution solution) {
	if (!isImprovement(solution.score))
    return;

  *alternate = solution;
	__asm__ __volatile__ ("" ::: "memory");
  std::swap(alternate, dabest);
	__asm__ __volatile__ ("" ::: "memory");
}

Solution Best::solution() const {
  return *dabest;
}

int Best::maxScore() const {
  return dabest->score;
}
