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

  auto hashcode = mine.hashcode();
  if (visited.find(hashcode) != visited.end())
    return {{}, 0};

  visited.insert(hashcode);
  results.push_back({mine.commands(), mine.score()});

  if (mine.moveCount() >= maxLength)
    return results[0];

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

  return results[0];
}
