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
  static void insertSorted(std::deque<State>& list, State const& state);

  void reset(Mine mine);
  void doLoopSet();

  // Returns true if should continue adding moves
  bool performCommand(Mine& mine, RobotCommand command);
  bool checkHashCode(std::string const& hash, int moves);
  int distanceHeuristic(Mine const& mine);
  int scoreHeuristic(Mine const& mine);
  void mixSets();

  Best& best;
  int maxDistanceSet;
  int maxScoreSet;
  int maxRandomSet;

  int distanceRuns;
  int scoreRuns;
  int randomRuns;

  int mixAmount;
  int maxNumSinceScoreImprovement;
  int ourMaxScore;

  std::unordered_map<std::string, int> visited;
  std::deque<State> distanceSet;
  std::deque<State> scoreSet;
  std::deque<Mine> randomSet;
};

#endif
