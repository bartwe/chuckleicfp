#include "Searcher.hpp"

Searcher::Searcher(Mine const& mine) {
  mineInitialState = mine;
}

std::pair<std::string, int> Searcher::bruteForce(int maxLength) {
  visited.clear();
  auto mine = mineInitialState;
  auto result = bruteForceSearch(maxLength, mine);
  std::string cmd;
  for (auto c : result.commands)
    cmd.push_back(Mine::commandChar(c));
  return std::make_pair(cmd, result.score);
}

Searcher::Result Searcher::bruteForceSearch(int maxLength, Mine& mine) {
  std::vector<Result> results;

  if (mine.pushMove(RobotCommand::Abort)) {
    results.push_back({mine.commands(), mine.score()});
    mine.popMove();
  }

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

  return *results.begin();
}
