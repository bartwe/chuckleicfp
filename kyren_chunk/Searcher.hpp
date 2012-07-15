#ifndef SEARCHER_HPP
#define SEARCHER_HPP

#include <unordered_map>
#include <functional>

#include "Mine.hpp"
#include "Best.hpp"

class Searcher {
public:
  void bruteForce(Best& best, Mine mine, int maxLength);

private:
  void bruteForceSearch(Best& best, Mine mine, RobotCommand command, int maxLength);

  std::unordered_map<std::string, int> visited;
};

#endif
