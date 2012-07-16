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
      openSet.push_back({m, simpleScoreHeuristic(m)});
  }

  int numSets = 0;
  while (!openSet.empty()) {
    std::vector<State> newOpenSet;
    for (auto const& s : openSet) {
      for (auto command : AllRobotCommands) {
        Mine m(s.mine);
        if (!performCommand(m, command))
          continue;

        State state = {m, simpleScoreHeuristic(m)};

        if (newOpenSet.size() < maxOpenSet + sortBuff) {
          newOpenSet.push_back(state);
        } else {
          newOpenSet[rand() % newOpenSet.size()] = state;
        }
        ++numSets;
        if (numSets >= sortNumber && newOpenSet.size() > maxOpenSet) {
          numSets = 0;
          std::sort(newOpenSet.begin(), newOpenSet.end(), [](State const& s1, State const& s2) {
              return s1.heuristic < s2.heuristic;
            });
          newOpenSet.erase(newOpenSet.begin() + maxOpenSet, newOpenSet.end());
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

  if (!checkHashCode(mine.hashcode(), mine.moveCount()))
    return false;

  return true;
}

bool RandomDijkstraSolver::checkHashCode(std::string const& hash, int moves) {
  // Skip if we have visited this mine state before with a better or equal set
  // of moves.
  auto vi = visited.find(hash);
  if (vi != visited.end()) {
    if (moves >= vi->second)
      return false;
  }
  visited[hash] = moves;
  return true;
}

int RandomDijkstraSolver::simpleScoreHeuristic(Mine const& mine) {
  if (mine.remainingLambdas() == 0)
    return mine.moveCount();
  else
    return mine.remainingLambdas() * 75 + mine.collectedLambdas() * 25 + mine.moveCount();
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
