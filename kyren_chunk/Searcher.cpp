#include "Searcher.hpp"

void Searcher::bruteForce(Best& best, Mine mine, int maxLength) {
  visited.clear();

  bruteForceSearch(best, mine, RobotCommand::Right, maxLength);
  bruteForceSearch(best, mine, RobotCommand::Left, maxLength);
  bruteForceSearch(best, mine, RobotCommand::Up, maxLength);
  bruteForceSearch(best, mine, RobotCommand::Down, maxLength);
  bruteForceSearch(best, mine, RobotCommand::Wait, maxLength);
  bruteForceSearch(best, mine, RobotCommand::Abort, maxLength);
}

void Searcher::bruteForceSearch(Best& best, Mine mine, RobotCommand command, int maxLength) {
  if (!mine.doCommand(command))
    return;

  if (best.isImprovement(mine.score()))
    best.improveSolution({mine.commands(), mine.score()});

  if (mine.moveCount() >= maxLength || mine.ended())
    return;

  // We should only update the visited set if the game is not in an ending
  // state.
  auto hashcode = mine.hashcode();
  auto score = mine.score();
  // Skip if we have visited this mine state before with a better score.
  auto vi = visited.find(hashcode);
  if (vi != visited.end()) {
    if (mine.score() < vi->second)
      return;
  }
  visited[hashcode] = mine.score();

  // Now proceed down the tree in DFS

  bruteForceSearch(best, mine, RobotCommand::Right, maxLength);
  bruteForceSearch(best, mine, RobotCommand::Left, maxLength);
  bruteForceSearch(best, mine, RobotCommand::Up, maxLength);
  bruteForceSearch(best, mine, RobotCommand::Down, maxLength);
  bruteForceSearch(best, mine, RobotCommand::Wait, maxLength);
  bruteForceSearch(best, mine, RobotCommand::Abort, maxLength);
}
