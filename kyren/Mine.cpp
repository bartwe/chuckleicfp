#include "Mine.hpp"
#include "sha1.hpp"

struct delta_t {
  int dx, dy;
};

static delta_t BeardDirs[] = {
    { 1, -1},
    { 1,  0},
    { 1,  1},
    { 0,  1},
    {-1,  1},
    {-1,  0},
    {-1, -1},
    { 0, -1},
  };

Mine::Mine(std::shared_ptr<Problem> p) {
  problem = p;
  state = State::InProgress;
  totalMoves = 0;
  rockBeardPositions = problem->rockBeardPositions;

  var.submergedSteps = 0;
  var.collectedLambdas = 0;
  var.curWaterLevel = problem->water.initWaterLevel;
  var.curRazors = problem->beard.initRazors;
  var.robotX = problem->robotX;
  var.robotY = problem->robotY;

  chunks.reset(problem->tiles.width() / ChunkSize + 1, problem->tiles.height() / ChunkSize + 1, std::shared_ptr<Chunk>());
  for (int y = 0; y < chunks.height(); ++y) {
    for (int x = 0; x < chunks.width(); ++x) {
      int cwidth;
      int cheight;
      if (x == chunks.width() - 1)
        cwidth = problem->tiles.width() - (chunks.width() - 1) * ChunkSize;
      else
        cwidth = ChunkSize;

      if (y == chunks.height() - 1)
        cheight = problem->tiles.height() - (chunks.height() - 1) * ChunkSize;
      else
        cheight = ChunkSize;

      chunks.at(x, y) = std::make_shared<Chunk>(cwidth, cheight);
    }
  }

  for (int y = 0; y < problem->tiles.height(); ++y)
    for (int x = 0; x < problem->tiles.width(); ++x)
      set(x, y, problem->tiles.at(x, y));
}

Mine::Mine() {}

std::shared_ptr<Problem> Mine::getProblem() const {
  return problem;
}

Tile Mine::get(int x, int y) const {
  int xc = x / ChunkSize;
  int yc = y / ChunkSize;

  int xi = x - xc * ChunkSize;
  int yi = y - yc * ChunkSize;

  auto const& chunk = chunks.at(xc, yc);
  return chunk->tiles.at(xi, yi);
}

void Mine::set(int x, int y, Tile c) {
  int xc = x / ChunkSize;
  int yc = y / ChunkSize;

  int xi = x - xc * ChunkSize;
  int yi = y - yc * ChunkSize;

  auto& chunk = chunks.at(xc, yc);
  if (!chunk.unique())
    chunk = std::make_shared<Chunk>(*chunk);

  Tile& tile = chunk->tiles.at(xi,yi);
  if (tile != c)
    chunk->hashDirty = true;

  if (rockType(tile) || tile == Tile::Beard)
    rockBeardPositions.erase({x, y});
  tile = c;
  if (rockType(tile) || tile == Tile::Beard)
    rockBeardPositions.insert({x, y});
}

Tile Mine::get(Position const& pos) const {
  return get(pos.x, pos.y);
}

void Mine::set(Position const& pos, Tile c) {
  set(pos.x, pos.y, c);
}

bool Mine::ended() const {
  return state != State::InProgress;
}

State Mine::currentState() const {
  return state;
}

Mine::VariadicState const& Mine::currentVariadicState() const {
  return var;
}

int Mine::score() const {
  int s = var.collectedLambdas * 25 - totalMoves;

  if (state == State::Aborted)
    s += var.collectedLambdas * 25;
  else if (state == State::Win)
    s += var.collectedLambdas * 50;

  return s;
}

void Mine::evaluate(RobotCommands commandList) {
  for (auto c : commandList)
    doCommand(c);
}

void Mine::evaluateAndPrint(RobotCommands commandList) {
  print();
  for (auto c : commandList) {
    if (!doCommand(c))
      std::cout << "Illegal command: " << commandName(c) << std::endl;

    print();
    if (state != State::InProgress)
      break;
  }
  std::cout << "Final state: " << stateToString(state) << std::endl;
  std::cout << "Final score: " << score() << std::endl;
}

bool Mine::doCommand(RobotCommand command) {
  if (state != State::InProgress)
    return false;

  if (command == RobotCommand::Abort) {
    state = State::Aborted;
    commandHistory.push_back(command);
    return true;
  }

  int newRobotX = var.robotX;
  int newRobotY = var.robotY;

  std::vector<MineUpdate> updateQueue;

  int dx=0, dy=0;
  switch (command) {
	  case RobotCommand::Left:  dx=-1; break;
	  case RobotCommand::Right: dx=+1; break;
	  case RobotCommand::Up:    dy=+1; break;
	  case RobotCommand::Down:  dy=-1; break;
    default: break;
  }

  if (dx != 0 || dy != 0) {
    int nx = var.robotX + dx;
    int ny = var.robotY + dy;
    auto c = get(nx, ny);
    if (c == Tile::Empty || 
        c == Tile::Earth || 
        c == Tile::Lambda || 
        c == Tile::OpenLift || 
        c == Tile::Razor) {
      newRobotX = nx;
      newRobotY = ny;
    } else if (rockType(c) && dy == 0 && get(nx + dx, var.robotY) == Tile::Empty) {
      updateQueue.push_back({nx+dx, var.robotY, c});
      newRobotX = nx;
    } else if (c >= Tile::TrampolineA && c <= Tile::TrampolineI) {
      Tile target = problem->getTargetForTramp(c);
      Position jumpTo = problem->targetLoc[problem->indexOfTrampTarget(target)];
      newRobotX = jumpTo.x;
      newRobotY = jumpTo.y;
      for (auto i : problem->getTrampLocsForTarget(target)) {
        updateQueue.push_back({i.x, i.y, Tile::Empty});
      }
    } 

    // If we have been commanded to move, but haven't moved, command must not
    // have been valid, do nothing.
    if (newRobotX == var.robotX && newRobotY == var.robotY)
      return false;
  }

  if (command == RobotCommand::Slice) {
    if (var.curRazors > 0) {
      var.curRazors--;
      for (auto i : BeardDirs) {
        if (get(var.robotX + i.dx, var.robotY + i.dy) == Tile::Beard) {
          updateQueue.push_back({var.robotX + i.dx, var.robotY + i.dy, Tile::Empty});
        }
      }
    } else {
      return false;
    }
  }

  commandHistory.push_back(command);

  auto nr = get(newRobotX, newRobotY);
  if (nr == Tile::Lambda)
    ++var.collectedLambdas;
  else if (nr == Tile::Razor)
    ++var.curRazors;
  else if (nr == Tile::OpenLift)
    state = State::Win;

  updateQueue.push_back({var.robotX, var.robotY, Tile::Empty});
  updateQueue.push_back({newRobotX, newRobotY, Tile::Robot});

  // Apply all of the robot moves before computing the world update.
  for (auto update : updateQueue)
    set(update.x, update.y, update.c);
  updateQueue.clear();

  var.robotX = newRobotX;
  var.robotY = newRobotY;

  bool beardGrowTime = (moveCount()+1) % problem->beard.growthrate == 0;
  for (auto const& rockbeard : rockBeardPositions) {
    int x = rockbeard.x;
    int y = rockbeard.y;

    if (rockType(get(rockbeard))) {
      Tile thisRock = get(rockbeard);
      if (get(x, y - 1) == Tile::Empty) {
        updateQueue.push_back({x, y, Tile::Empty});
        if (thisRock == Tile::HigherOrderRock && get(x, y - 2) != Tile::Empty) {
          updateQueue.push_back({x, y - 1, Tile::Lambda});
        } else {
          updateQueue.push_back({x, y - 1, thisRock});
        }
      } else if (rockType(get(x, y - 1)) && get(x + 1, y) == Tile::Empty && get(x + 1, y - 1) == Tile::Empty) {
        updateQueue.push_back({x, y, Tile::Empty});
        if (thisRock == Tile::HigherOrderRock && get(x + 1, y - 2) != Tile::Empty) {
          updateQueue.push_back({x + 1, y - 1, Tile::Lambda});
        } else {
          updateQueue.push_back({x + 1, y - 1, thisRock});
        }
      } else if (rockType(get(x, y - 1)) && get(x - 1, y) == Tile::Empty && get(x - 1, y - 1) == Tile::Empty) {
        updateQueue.push_back({x, y, Tile::Empty});
        if (thisRock == Tile::HigherOrderRock && get(x - 1, y - 2) != Tile::Empty) {
          updateQueue.push_back({x - 1, y - 1, Tile::Lambda});
        } else {
          updateQueue.push_back({x - 1, y - 1, thisRock});
        }
      } else if (get(x, y - 1) == Tile::Lambda && get(x + 1, y) == Tile::Empty && get(x + 1, y - 1) == Tile::Empty) {
        updateQueue.push_back({x, y, Tile::Empty});
        if (thisRock == Tile::HigherOrderRock && get(x + 1, y - 2) != Tile::Empty) {
          updateQueue.push_back({x + 1, y - 1, Tile::Lambda});
        } else {
          updateQueue.push_back({x + 1, y - 1, thisRock});
        }
      }
    } else {
      assert(get(rockbeard) == Tile::Beard);
      if (beardGrowTime) {
        for ( auto dir : BeardDirs ) {
          int nx = x + dir.dx;
          int ny = y + dir.dy;
          if ( get(nx, ny) == Tile::Empty ) {
            updateQueue.push_back({nx, ny, Tile::Beard});
          }
        }
      }
    }
  }

  var.curWaterLevel = waterLevel(totalMoves + 1);

  if (var.curWaterLevel > var.robotY) {
    var.submergedSteps += 1;
  } else {
    var.submergedSteps = 0;
  }

  if (var.submergedSteps > problem->water.waterproof) {
    state = State::Lose;
  }

  if (var.collectedLambdas == problem->numInitialLambdas) {
    for (auto lift : problem->liftLoc)
      updateQueue.push_back({lift.x, lift.y, Tile::OpenLift});
  }

  for (auto update : updateQueue) {
    if (rockType(update.c) && update.x == var.robotX && update.y == var.robotY + 1) {
      // Robot was hit on the head by rock
      state = State::Lose;
    }

    set(update.x, update.y, update.c);
  }

  ++totalMoves;
  return true;
}

int Mine::waterLevel(int turn) const {
  if (problem->water.floodingFreq == 0) {
    return problem->water.initWaterLevel;
  } else {
    return turn / problem->water.floodingFreq + problem->water.initWaterLevel;
  }
}

int Mine::moveCount() const {
  return totalMoves;
}

int Mine::collectedLambdas() const {
  return var.collectedLambdas;
}

int Mine::remainingLambdas() const {
  return problem->numInitialLambdas - var.collectedLambdas;
}

bool Mine::dead() const {
  if (state != State::InProgress)
    return true;

  if (canMove(1, 0) || canMove(-1, 0) || canMove(0, 1) || canMove(0, -1))
    return false;

  if (problem->getTileCount(Tile::Beard) != 0 && var.curRazors != 0)
    return false;

  return true;
}

RobotCommands const& Mine::commands() const {
  return commandHistory;
}

Solution Mine::solution() const {
  return {commandHistory, score()};
}

void Mine::print() const {
  for (int y = problem->height - 1; y >= 0; --y) {
    for (int x = 0; x < problem->width; ++x)
      std::cout << charFromTile(get(x, y));
    std::cout << std::endl;
  }
  std::cout << std::endl;
}

std::string Mine::hashcode() const {
  std::string totalHash(20, '\0');
  for (int y = 0; y < chunks.height(); ++y) {
    for (int x = 0; x < chunks.width(); ++x) {
      auto const& chunk = chunks.at(x, y);
      if (chunk->hashDirty)
        chunk->computeHash();
      for (int i = 0; i < 20; ++i)
        totalHash[i] ^= chunk->hashCode[i];
    }
  }

  return totalHash;
}

Mine::Chunk::Chunk(int width, int height) {
  tiles.reset(width, height, Tile::Wall);
  hashDirty = true;
}

void Mine::Chunk::computeHash() {
  unsigned char hash[20];
  sha1::calc(tiles.getGrid(), tiles.gridSize(), hash);
  hashCode = std::string((char const*)hash, 20);
  hashDirty = false;
}

bool Mine::canMove(int dx, int dy) const {
  if (dx == 0 && dy == 0)
    return true;

  int nx = var.robotX + dx;
  int ny = var.robotY + dy;
  auto c = get(nx, ny);
  if (c == Tile::Empty || 
      c == Tile::Earth || 
      c == Tile::Lambda || 
      c == Tile::OpenLift || 
      c == Tile::Razor) {
    return true;
  } else if (rockType(c) && dy == 0 && get(nx + dx, var.robotY) == Tile::Empty) {
    return true;
  } else if (c >= Tile::TrampolineA && c <= Tile::TrampolineI) {
    return true;
  } else {
    return false;
  }
}
