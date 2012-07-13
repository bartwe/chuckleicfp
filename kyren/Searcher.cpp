#include "Searcher.hpp"

Searcher::Searcher(Mine const& mine) {
  mineInitialState = mine;
}

Searcher::Result Searcher::bruteForce(int maxLength) {
  visited.clear();
  auto mine = mineInitialState;
  return bruteForceSearch(maxLength, mine);
}

Searcher::Result Searcher::bruteForceSearch(int maxLength, Mine& mine) {
  std::vector<Result> results;

  if (mine.pushMove(RobotCommand::Abort)) {
    results.push_back({mine.commands(), mine.score()});
    mine.popMove();
  }

  if (mine.moveCount() >= maxLength) {
    if (!results.empty())
      return results[0];
    else
      return {{}, 0};
  }

  if (mine.pushMove(RobotCommand::Right)) {
    results.push_back(bruteForceSearch(maxLength, mine));
    mine.popMove();
  }

  if (mine.pushMove(RobotCommand::Left)) {
    results.push_back(bruteForceSearch(maxLength, mine));
    mine.popMove();
  }

  if (mine.pushMove(RobotCommand::Up)) {
    results.push_back(bruteForceSearch(maxLength, mine));
    mine.popMove();
  }

  if (mine.pushMove(RobotCommand::Down)) {
    results.push_back(bruteForceSearch(maxLength, mine));
    mine.popMove();
  }

  if (mine.pushMove(RobotCommand::Wait)) {
    results.push_back(bruteForceSearch(maxLength, mine));
    mine.popMove();
  }

  std::sort(results.begin(), results.end(), [](Result const& ra, Result const& rb) {
      return rb.score < ra.score;
    });

  if (!results.empty())
    return results[0];
  else
    return {{}, 0};
}
