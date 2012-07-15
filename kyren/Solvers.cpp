#include <algorithm>
#include <cstdlib>
#include <map>

#include "Solvers.hpp"

RandomDijkstraSolver::RandomDijkstraSolver(Best& best, int maxOpenSet, int sortNumber, int sortBuff) :
  best(best), maxOpenSet(maxOpenSet), sortNumber(sortNumber), sortBuff(sortBuff) {}

void RandomDijkstraSolver::run(Mine mine) {
  visited.clear();
  openSet.clear();

  for (auto command : AllRobotCommands) {
    Mine m(mine);
    if (performCommand(m, command))
      openSet.push_back(m);
  }

  int numSets = 0;
  while (!openSet.empty()) {
    std::deque<Mine> newOpenSet;
    for (auto const& mine : openSet) {
      for (auto command : AllRobotCommands) {
        Mine m(mine);
        if (!performCommand(m, command))
          continue;

        if (newOpenSet.size() < maxOpenSet + sortBuff) {
          newOpenSet.push_back(m);
        } else {
          newOpenSet[rand() % newOpenSet.size()] = m;
        }
        ++numSets;
        if (numSets >= sortNumber && newOpenSet.size() > maxOpenSet) {
          numSets = 0;
          std::sort(newOpenSet.begin(), newOpenSet.end(), [](Mine const& m1, Mine const& m2) {
              return m1.score() < m2.score();
            });
          newOpenSet.erase(newOpenSet.begin(), newOpenSet.begin() + newOpenSet.size() - maxOpenSet);
        }
      }
    }

    openSet = newOpenSet;
  }
}

bool RandomDijkstraSolver::performCommand(Mine& mine, RobotCommand command) {
  if (!mine.doCommand(command))
    return false;

  int score = mine.score();
  if (best.isImprovement(score))
    best.improveSolution(mine.solution());

  if (mine.dead())
    return false;

  if (!checkHashCode(mine.hashcode(), score))
    return false;

  return true;
}

bool RandomDijkstraSolver::checkHashCode(std::string const& hash, int score) {
  // Skip if we have visited this mine state before with a better score.
  auto vi = visited.find(hash);
  if (vi != visited.end()) {
    if (score < vi->second)
      return false;
  }
  visited[hash] = score;
  return true;
}

BDFSSearcher::BDFSSearcher(int maxDepth, int maxSolutions) : maxDepth(maxDepth), maxSolutions(maxSolutions) {}

Solutions BDFSSearcher::operator()(Best& best, Mine mine) {
  visited.clear();
  solutions.clear();

  bruteForceSearch(best, mine, RobotCommand::Right);
  bruteForceSearch(best, mine, RobotCommand::Left);
  bruteForceSearch(best, mine, RobotCommand::Up);
  bruteForceSearch(best, mine, RobotCommand::Down);
  bruteForceSearch(best, mine, RobotCommand::Slice);
  bruteForceSearch(best, mine, RobotCommand::Wait);
  bruteForceSearch(best, mine, RobotCommand::Abort);

  return Solutions(solutions.begin(), solutions.end());
}

void BDFSSearcher::bruteForceSearch(Best& best, Mine mine, RobotCommand command) {
  if (!mine.doCommand(command))
    return;

  visitSolution(best, mine);

  if (mine.moveCount() >= maxDepth || mine.ended())
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

  bruteForceSearch(best, mine, RobotCommand::Right);
  bruteForceSearch(best, mine, RobotCommand::Left);
  bruteForceSearch(best, mine, RobotCommand::Up);
  bruteForceSearch(best, mine, RobotCommand::Down);
  bruteForceSearch(best, mine, RobotCommand::Slice);
  bruteForceSearch(best, mine, RobotCommand::Wait);
  bruteForceSearch(best, mine, RobotCommand::Abort);
}

void BDFSSearcher::visitSolution(Best& best, Mine const& mine) {
  auto solution = mine.solution();

  if (best.isImprovement(solution.score))
    best.improveSolution(solution);

  if (mine.dead() || maxSolutions <= 0)
    return;

  if (solutions.empty()) {
    solutions.push_back(solution);
  } else if (solution.score > solutions[0].score) {
    solutions.push_back(solution);
    std::sort(solutions.begin(), solutions.end());
    if (solutions.size() > maxSolutions)
      solutions.pop_front();
  }
}

RoundSolver::RoundSolver(Best& best, std::shared_ptr<Problem> problem, std::vector<Searcher> const& searchers, int maxTotalBranches) :
  best(best), problem(problem), searchers(searchers), maxTotalBranches(maxTotalBranches) {}

void RoundSolver::doRound() {
  if (currentSolutions.empty())
    currentSolutions.push_back({{}, 0});

  Mine startingMine(problem);

  Solutions nextSolutions;

  for (auto solution : currentSolutions) {
    Mine mine = startingMine;
    mine.evaluate(solution.commands);
    for (auto searcher : searchers) {
      auto solutions = searcher(best, mine);
      nextSolutions.insert(nextSolutions.end(), solutions.begin(), solutions.end());
    }
  }
  currentSolutions = nextSolutions;

  std::sort(currentSolutions.begin(), currentSolutions.end());
  if (currentSolutions.size() > maxTotalBranches)
    currentSolutions.erase(currentSolutions.begin(), currentSolutions.begin() + currentSolutions.size() - maxTotalBranches);
}

bool reachable(Mine const& mine, Position const& destination) { //start is assumed to be robot location
  std::vector<Position> toVisit;
  std::vector<bool> visited;
  const int mineWidth = mine.getProblem()->width;
  const int mineHeight = mine.getProblem()->height;
  visited.resize(mineWidth * mineHeight, false);
  toVisit.push_back({mine.currentVariadicState().robotX, mine.currentVariadicState().robotY});
  Position consider;

  const Position LookDirs[] = {
    { 1,  0},
    { 0,  1},
    {-1,  0},
    { 0, -1},
  };

  auto addToVisited = [&](Position toAdd) {
    visited[mineWidth * toAdd.y + toAdd.x] = true;
  };

  auto hasVisited = [&](Position toCheck)->bool {
    return visited[mineWidth * toCheck.y + toCheck.x];
  };

  while (toVisit.size() > 0) {
    consider = toVisit.back();
    if (hasVisited(consider)) {
      continue;
    }
    toVisit.pop_back();
    addToVisited(consider);
    if (consider == destination) {
      return true;
    }
    
    for (auto i : LookDirs) {
      auto dest = mine.get(i + consider);
      if (dest == Tile::Empty || 
          dest == Tile::Lambda || 
          dest == Tile::Earth ||
          dest == Tile::Razor ||
          dest == Tile::OpenLift ||
          dest == Tile::Beard) {
        toVisit.push_back(i + consider);
      } else if (dest == Tile::Rock || dest == Tile::HigherOrderRock) {
        Position delta = {i.x, 0};
        if (i.y == 0 && mine.get(i + consider + delta) == Tile::Empty) { //if I can push it, assume that it's not blocking me
          toVisit.push_back(i + consider);
        }
      } else if (dest >= Tile::TrampolineA && dest <= Tile::TrampolineI) {
        Tile target = mine.getProblem()->getTargetForTramp(dest);
        Position jumpTo = mine.getProblem()->targetLoc[mine.getProblem()->indexOfTrampTarget(target)];
        toVisit.push_back(jumpTo);
      }
    }

    return false;
  }
}
