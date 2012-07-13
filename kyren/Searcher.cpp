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

  // Skip if we have visited this mine state before with a better score
  auto hashcode = mine.hashcode();
  auto vi = visited.find(hashcode);
  if (vi != visited.end()) {
    if (mine.score() < vi->second)
      return {{}, 0};
  }

  visited[hashcode] = mine.score();
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
