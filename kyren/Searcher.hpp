#ifndef SEARCHER_HPP
#define SEARCHER_HPP

#include <unordered_map>
#include <functional>

#include "Mine.hpp"
#include "Best.hpp"

class Searcher {
public:
  void bruteForce(Mine mine, Best& best, int maxLength);

private:
  void bruteForceSearch(Mine& mine, Best& best, int maxLength);

  std::unordered_map<std::string, int> visited;
};

#endif
