#ifndef SEARCHER_HPP
#define SEARCHER_HPP

#include <unordered_map>
#include <functional>
#include <deque>

#include "Mine.hpp"
#include "Best.hpp"

// Bounded depth first search
class BDFSSearcher {
public:
  BDFSSearcher(int maxDepth, int maxSolutions);

  // Returns most promising solutions which are not dead ends
  Solutions operator()(Best& best, Mine mine);

private:
  void bruteForceSearch(Best& best, Mine mine, RobotCommand command);
  void visitSolution(Best& best, Mine const& mine);

  std::unordered_map<std::string, int> visited;
  int maxDepth;
  int maxSolutions;
  std::deque<Solution> solutions;
};

class RoundSolver {
public:
  typedef std::function<Solutions (Best& best, Mine mine)> Searcher;

  RoundSolver(Best& best, std::shared_ptr<Problem> problem, std::vector<Searcher> const& searchers, int maxTotalBranches);

  void doRound();

private:
  Best& best;
  std::shared_ptr<Problem> problem;
  std::vector<Searcher> searchers;
  int maxTotalBranches;
  Solutions currentSolutions;
};

#endif
