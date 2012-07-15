#ifndef SEARCHER_HPP
#define SEARCHER_HPP

#include <unordered_map>
#include <functional>
#include <deque>

#include "Mine.hpp"
#include "Best.hpp"

class RandomDijkstraSolver {
public:
  RandomDijkstraSolver(Best& best, int maxOpenSet, int sortNumber, int sortBuff);

  void run(Mine mine);

private:
  // Returns true if should continue adding moves
  bool performCommand(Mine& mine, RobotCommand command);
  bool checkHashCode(std::string const& hash, int score);

  Best& best;
  int maxOpenSet;
  int sortNumber;
  int sortBuff;

  std::unordered_map<std::string, int> visited;
  std::deque<Mine> openSet;
};

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

bool reachable(Mine mine, Position destination);

#endif
