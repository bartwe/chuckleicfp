#include "Searcher.hpp"

void Searcher::bruteForce(Mine mine, int maxLength, SolutionCallback callback) {
  visited.clear();
  return bruteForceSearch(mine, maxLength, callback);
}

void Searcher::bruteForceSearch(Mine& mine, int maxLength, SolutionCallback callback) {
  // Skip if we have visited this mine state before with a better score
  auto hashcode = mine.hashcode();
  auto vi = visited.find(hashcode);
  if (vi != visited.end()) {
    if (mine.score() < vi->second)
      return;
  }

  visited[hashcode] = mine.score();

  if (mine.currentState() == State::InProgress) {
    mine.pushMove(RobotCommand::Abort);
    callback(mine.commands(), mine.score());
    mine.popMove();
  } else {
    callback(mine.commands(), mine.score());
  }

  if (mine.moveCount() >= maxLength)
    return;

  if (mine.pushMove(RobotCommand::Right)) {
    bruteForceSearch(mine, maxLength, callback);
    mine.popMove();
  }

  if (mine.pushMove(RobotCommand::Left)) {
    bruteForceSearch(mine, maxLength, callback);
    mine.popMove();
  }

  if (mine.pushMove(RobotCommand::Up)) {
    bruteForceSearch(mine, maxLength, callback);
    mine.popMove();
  }

  if (mine.pushMove(RobotCommand::Down)) {
    bruteForceSearch(mine, maxLength, callback);
    mine.popMove();
  }

  if (mine.pushMove(RobotCommand::Wait)) {
    bruteForceSearch(mine, maxLength, callback);
    mine.popMove();
  }
}
