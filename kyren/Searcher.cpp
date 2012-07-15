#include "Searcher.hpp"

void Searcher::bruteForce(Mine mine, Best& best, int maxLength) {
  visited.clear();
  return bruteForceSearch(mine, best, maxLength);
}

void Searcher::bruteForceSearch(Mine& mine, Best& best, int maxLength) {
  // If we haven't won, need to abort before determining the moves / score we are considering.
  if (mine.currentState() == State::InProgress)
    mine.pushMove(RobotCommand::Abort);

  int score = mine.score();
  auto commands = mine.commands();
  auto hashcode = mine.hashcode();

  if (mine.currentState() == State::Aborted)
    mine.popMove();

  // Skip if we have visited this mine state before with a better score
  auto vi = visited.find(hashcode);
  if (vi != visited.end()) {
    if (score < vi->second)
      return;
  }
  visited[hashcode] = score;

  if (best.isImprovement(score))
    best.improveSolution(score, commandString(commands).c_str());

  // Now proceed down the tree in DFS

  if (mine.moveCount() >= maxLength)
    return;

  if (mine.pushMove(RobotCommand::Right)) {
    bruteForceSearch(mine, best, maxLength);
    mine.popMove();
  }

  if (mine.pushMove(RobotCommand::Left)) {
    bruteForceSearch(mine, best, maxLength);
    mine.popMove();
  }

  if (mine.pushMove(RobotCommand::Up)) {
    bruteForceSearch(mine, best, maxLength);
    mine.popMove();
  }

  if (mine.pushMove(RobotCommand::Down)) {
    bruteForceSearch(mine, best, maxLength);
    mine.popMove();
  }

  if (mine.pushMove(RobotCommand::Wait)) {
    bruteForceSearch(mine, best, maxLength);
    mine.popMove();
  }
}
