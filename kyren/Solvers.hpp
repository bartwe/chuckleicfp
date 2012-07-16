#ifndef SEARCHER_HPP
#define SEARCHER_HPP

#include <unordered_map>
#include <functional>
#include <deque>

#include "Mine.hpp"
#include "Best.hpp"

bool reachable(Mine const& mine, Position const& destination);
int distanceToNextInterestingThing(Mine const& mine);

class WeirdAStarSolver {
public:
  WeirdAStarSolver(Best& best);

  void run(Mine mine);

private:
  struct State {
    Mine mine;
    int heuristic;
  };

  // Returns true if should continue adding moves
  bool performCommand(Mine& mine, RobotCommand command);
  bool checkHashCode(std::string const& hash, int moves);
  int calcHeuristic(Mine const& mine);
  void insertIntoOpenSet(State const& state);

  Best& best;
  int maxOpenSet;

  std::unordered_map<std::string, int> visited;
  std::deque<State> openSet;
};

#endif
