#include <algorithm>

#include "Solver.hpp"

BDFSSearcher::BDFSSearcher(int maxDepth, int maxSolutions) : maxDepth(maxDepth), maxSolutions(maxSolutions) {}

Solutions BDFSSearcher::operator()(Best& best, Mine mine) {
  visited.clear();
  solutions.clear();

  bruteForceSearch(best, mine, RobotCommand::Right);
  bruteForceSearch(best, mine, RobotCommand::Left);
  bruteForceSearch(best, mine, RobotCommand::Up);
  bruteForceSearch(best, mine, RobotCommand::Down);
  bruteForceSearch(best, mine, RobotCommand::Wait);
  bruteForceSearch(best, mine, RobotCommand::Abort);

  return Solutions(solutions.begin(), solutions.end());
}

void BDFSSearcher::bruteForceSearch(Best& best, Mine mine, RobotCommand command) {
  if (!mine.doCommand(command))
    return;

  visitSolution(best, mine);

  if (mine.moveCount() >= maxDepth || mine.ended())
    return;

  // We should only update the visited set if the game is not in an ending
  // state.
  auto hashcode = mine.hashcode();
  auto score = mine.score();
  // Skip if we have visited this mine state before with a better score.
  auto vi = visited.find(hashcode);
  if (vi != visited.end()) {
    if (mine.score() < vi->second)
      return;
  }
  visited[hashcode] = mine.score();

  // Now proceed down the tree in DFS

  bruteForceSearch(best, mine, RobotCommand::Right);
  bruteForceSearch(best, mine, RobotCommand::Left);
  bruteForceSearch(best, mine, RobotCommand::Up);
  bruteForceSearch(best, mine, RobotCommand::Down);
  bruteForceSearch(best, mine, RobotCommand::Slice);
  bruteForceSearch(best, mine, RobotCommand::Wait);
  bruteForceSearch(best, mine, RobotCommand::Abort);
}

void BDFSSearcher::visitSolution(Best& best, Mine const& mine) {
  auto solution = mine.solution();

  if (best.isImprovement(solution.score))
    best.improveSolution(solution);

  if (mine.dead() || maxSolutions <= 0)
    return;

  if (solutions.empty()) {
    solutions.push_back(solution);
  } else if (solution.score > solutions[0].score) {
    solutions.push_back(solution);
    std::sort(solutions.begin(), solutions.end());
    if (solutions.size() > maxSolutions)
      solutions.pop_front();
  }
}

RoundSolver::RoundSolver(Best& best, std::shared_ptr<Problem> problem, std::vector<Searcher> const& searchers, int maxTotalBranches) :
  best(best), problem(problem), searchers(searchers), maxTotalBranches(maxTotalBranches) {}

void RoundSolver::doRound() {
  if (currentSolutions.empty())
    currentSolutions.push_back({{}, 0});

  Mine startingMine(problem);

  Solutions nextSolutions;

  for (auto solution : currentSolutions) {
    Mine mine = startingMine;
    mine.evaluate(solution.commands);
    for (auto searcher : searchers) {
      auto solutions = searcher(best, mine);
      nextSolutions.insert(nextSolutions.end(), solutions.begin(), solutions.end());
    }
  }
  currentSolutions = nextSolutions;

  std::sort(currentSolutions.begin(), currentSolutions.end());
  if (currentSolutions.size() > maxTotalBranches)
    currentSolutions.erase(currentSolutions.begin(), currentSolutions.begin() + currentSolutions.size() - maxTotalBranches);
}
