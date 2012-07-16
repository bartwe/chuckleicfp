#ifndef SEARCHER_HPP
#define SEARCHER_HPP

#include <unordered_map>
#include <functional>
#include <deque>

#include "Mine.hpp"
#include "Best.hpp"

bool reachable(Mine const& mine, Position const& destination);

class RandomDijkstraSolver {
public:
  RandomDijkstraSolver(Best& best, int maxOpenSet, int sortNumber, int sortBuff);

  void run(Mine mine);

private:
  // Returns true if should continue adding moves
  bool performCommand(Mine& mine, RobotCommand command);
  bool checkHashCode(std::string const& hash, int moves);
  int simpleScoreHeuristic(Mine const& mine);

  Best& best;
  int maxOpenSet;
  int sortNumber;
  int sortBuff;

  struct State {
    Mine mine;
    int heuristic;
  };

  std::unordered_map<std::string, int> visited;
  std::vector<State> openSet;
};

#endif
