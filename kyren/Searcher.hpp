#ifndef SEARCHER_HPP
#define SEARCHER_HPP

#include <unordered_set>

#include "Mine.hpp"

class Searcher {
public:
  Searcher(Mine const& mine);

  struct Result {
    std::vector<RobotCommand> commands;
    int score;
  };

  Result bruteForce(int maxLength);

private:
  Result bruteForceSearch(int maxLength, Mine& mine);

  Mine mineInitialState;
  std::unordered_set<std::string> visited;
};

#endif
