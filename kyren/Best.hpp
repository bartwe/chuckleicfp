#pragma once

#include <memory>

#include "Types.hpp"

// Utility class to (in an atomic / thread safe way) update a best solution and score.
class Best {
public:
  Best();

  bool isImprovement(int score);
  void improveSolution(Solution solution);

  Solution solution() const;
  int maxScore() const;

private:
  std::shared_ptr<Solution> dabest;
  std::shared_ptr<Solution> alternate;
};
