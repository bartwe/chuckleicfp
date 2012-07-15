#ifndef SEARCHER_HPP
#define SEARCHER_HPP

#include <unordered_map>
#include <functional>

#include "Mine.hpp"

class Searcher {
public:
  typedef std::function<void (RobotCommands const& commands, int score)> SolutionCallback;

  void bruteForce(Mine mine, int maxLength, SolutionCallback callback);

private:
  void bruteForceSearch(Mine& mine, int maxLength, SolutionCallback callback);

  std::unordered_map<std::string, int> visited;
};

#endif
