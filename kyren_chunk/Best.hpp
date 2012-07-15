#pragma once

#include <memory>

#include "Types.hpp"

// Utility class to (in an atomic / thread safe way) update a best solution and score.
class Best {
public:
  struct Solution {
    RobotCommands commands;
    int score;
  };

  Best();

  bool isImprovement(int score);
  void improveSolution(Solution solution);

  Solution solution() const;

private:
  std::shared_ptr<Solution> dabest;
  std::shared_ptr<Solution> alternate;
};
