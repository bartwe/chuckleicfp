#include <algorithm>
#include <cstdlib>
#include <map>

#include "Solvers.hpp"

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

int distanceToNextInterestingThing(Mine const& mine) {
  int x = 0;
  int y = 0;
  int dx = 0;
  int dy = -1;

  int X = mine.getProblem()->width;
  int Y = mine.getProblem()->height;
  int max = std::max(X, Y) * std::max(X, Y);

  auto rpos = mine.robotPosition();

  for (int i = 0; i < max; ++i) {
    auto pos = Position{rpos.x + x, rpos.y +y};
    if (pos.x >= 0 && pos.x < X && pos.y >= 0 && pos.y < Y) {
      auto t = mine.get(pos);
      if (mine.remainingLambdas() != 0 && (trampolineType(t) || t == Tile::Lambda))
        return std::abs(x) + std::abs(y);
      else if (mine.remainingLambdas() == 0 && (t == Tile::OpenLift))
        return std::abs(x) + std::abs(y);
    }

    if (x == y || (x < 0 && x == -y) || (x > 0 && x == 1-y)) {
      int odx = dx;
      dx = -dy;
      dy = odx;
    }

    x = x + dx;
    y = y + dy;
  }

  return max;
}

WeirdAStarSolver::WeirdAStarSolver(Best& best) : best(best) {
  maxOpenSet = 10000;
}

void WeirdAStarSolver::run(Mine mine) {
  visited.clear();
  openSet.clear();

  for (auto command : AllRobotCommands) {
    Mine m(mine);
    if (performCommand(m, command))
      insertIntoOpenSet({m, calcHeuristic(m)});
  }

  while (!openSet.empty()) {
    State top = openSet[0];
    openSet.pop_front();
    for (auto command : AllRobotCommands) {
      Mine m(top.mine);
      if (!performCommand(m, command))
        continue;

      insertIntoOpenSet({m, calcHeuristic(m)});
      if (openSet.size() > maxOpenSet)
        openSet.pop_back();
    }
  }
}

bool WeirdAStarSolver::performCommand(Mine& mine, RobotCommand command) {
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

bool WeirdAStarSolver::checkHashCode(std::string const& hash, int moves) {
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

int WeirdAStarSolver::calcHeuristic(Mine const& mine) {
  int h;
  if (mine.remainingLambdas() == 0)
    h = mine.moveCount();
  else
    h = mine.remainingLambdas() * 75 + mine.collectedLambdas() * 25 + mine.moveCount();

  h += distanceToNextInterestingThing(mine);
  return h;
}

void WeirdAStarSolver::insertIntoOpenSet(State const& state) {
  auto i = std::lower_bound(openSet.begin(), openSet.end(), state, [](State const& s1, State const& s2) {
      return s1.heuristic < s2.heuristic;
    });
  openSet.insert(i, state);
}
