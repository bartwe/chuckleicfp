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

void Mine::read(std::istream& is) {
  state = State::InProgress;
  var.collectedLambdas = 0;
  totalMoves = 0;

  std::vector<std::vector<Tile>> readContent;
  int maxRow = 0;
  std::string line;
  while (std::getline(is, line)) {
    if (line.size() == 0) break;

    std::vector<Tile> row;
    row.resize(line.size());
    memcpy(&row[0], &line[0], line.size());

    maxRow = std::max<int>(maxRow, row.size());

    readContent.push_back(std::move(row));
  }

  problem.water.initWaterLevel = 0;
  problem.water.floodingFreq = 0;
  problem.water.waterproof = 10;

  problem.beard.growthrate = 25;
  problem.beard.initRazors = 0;

  while (std::getline(is, line)) {
    char* cline = (char*)line.c_str();
    char* eocmd = strchr(cline, ' ');
    if (!eocmd) continue;

    const char* command = cline;
    *eocmd = 0;
    const char* parameter = eocmd + 1;

    if (strcmp(command, "Water") == 0) {
      problem.water.initWaterLevel = atoi(parameter);
    } else if (strcmp(command, "Flooding") == 0) {
      problem.water.floodingFreq = atoi(parameter);
    } else if (strcmp(command, "Waterproof") == 0) {
      problem.water.waterproof = atoi(parameter);
    } else if (strcmp(command, "Growth") == 0) {
      problem.beard.growthrate = atoi(parameter);
    } else if (strcmp(command, "Razors") == 0) {
      problem.beard.initRazors = atoi(parameter);
    } else if (strcmp(command, "Trampoline") == 0) {
      problem.trampMapping.push_back({tileFromChar(parameter[0]), tileFromChar(parameter[strlen(parameter)-1])});
    }
  }
  var.curWaterLevel = problem.water.initWaterLevel;
  var.curRazors = problem.beard.initRazors;

  problem.width = maxRow;
  problem.height = readContent.size();

  content.reset(problem.width, problem.height, Tile::Empty);
  // Turn upside down so (0, 0) is bottom left
  std::reverse(readContent.begin(), readContent.end());

  problem.numInitialLambdas = 0;

  for (int i = 0; i < readContent.size(); ++i) {
    auto const& row = readContent[i];
    for (int j = 0; j < row.size(); ++j) {
      auto c = row[j];
      if (c == Tile::Robot) {
        var.robotX = j;
        var.robotY = i;
      } else if (c == Tile::ClosedLift) {
        problem.liftLoc.push_back({j, i});
      } else if (c == Tile::Lambda) {
	      problem.numInitialLambdas++;
      } else if (c >= Tile::TrampolineA && c <= Tile::TrampolineI) {
        int index = indexOfTrampTarget(c);
        if ((int)c >= problem.trampLoc.size()) {
          problem.trampLoc.resize(index+1);
        }
        problem.trampLoc[index] = {j, i};
      } else if (c >= Tile::Target1 && c <= Tile::Target9) {
        int index = indexOfTrampTarget(c);
        if ((int)c >= problem.targetLoc.size()) {
          problem.targetLoc.resize(index+1);
        }
        problem.targetLoc[index] = {j, i};
      }

      set(j, i, c);
    }
  }

  memset(problem.initialTileHistogram, 0, sizeof problem.initialTileHistogram);

  for (int i=0; i<content.gridSize(); i++) {
    Tile tile = content.atidx(i);
    problem.initialTileHistogram[charFromTile(tile)]++;

    if (tile==Tile::Rock || tile==Tile::Beard)
      rockbeardpositions.insert(i);
  }
  assert( problem.initialTileHistogram[charFromTile(Tile::Robot)] == 1 );
  assert( problem.initialTileHistogram[charFromTile(Tile::ClosedLift)] == 1 );
  assert( problem.initialTileHistogram[charFromTile(Tile::OpenLift)] == 0 );
  assert( problem.initialTileHistogram[charFromTile(Tile::Lambda)] == problem.numInitialLambdas );
}

Problem const& Mine::getProblem() const {
  return problem;
}

int Mine::indexOfTrampTarget(Tile c) const {
  if (c >= Tile::TrampolineA && c <= Tile::TrampolineI)
    return (int)c - (int)Tile::TrampolineA;
  else if (c >= Tile::Target1 && c <= Tile::Target9)
    return (int)c - (int)Tile::Target1;
  else
    return -1; //error
}

Tile Mine::getTargetForTramp(Tile c) const {
  for (auto i : problem.trampMapping) {
    if (i.first == c) {
      return i.second;
    }
  }
  return Tile::Empty;
}

std::vector<Position> Mine::getTrampLocsForTarget(Tile c) const {
  std::vector<Position> res;
  for (auto i : problem.trampMapping) {
    if (i.second == c) {
      res.push_back(problem.trampLoc[indexOfTrampTarget(i.first)]);
    }
  }
  return res;
}

Tile Mine::get(int x, int y) const {
  return content.at(x,y);
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
  for (auto c : commandList) {
    doCommand(c);
    if (state != State::InProgress)
      return;
  }
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
    } else if (c == Tile::Rock && dy == 0 && get(nx + dx, var.robotY) == Tile::Empty) {
      updateQueue.push_back({nx+dx, var.robotY, Tile::Rock});
      newRobotX = nx;
    } else if (c >= Tile::TrampolineA && c <= Tile::TrampolineI) {
      Tile target = getTargetForTramp(c);
      Position jumpTo = problem.targetLoc[indexOfTrampTarget(target)];
      newRobotX = jumpTo.x;
      newRobotY = jumpTo.y;
      for (auto i : getTrampLocsForTarget(target)) {
        updateQueue.push_back({i.x, i.y, Tile::Empty});
      }
    } 

    // If we have been commanded to move, but haven't moved, command must not
    // have been valid, do nothing.
    if (newRobotX == var.robotX && newRobotY == var.robotY)
      return false;
  }

  if (command == RobotCommand::Slice) {
    var.curRazors--;
    for (auto i : BeardDirs) {
      if (get(var.robotX + i.dx, var.robotY + i.dy) == Tile::Beard) {
        updateQueue.push_back({var.robotX + i.dx, var.robotY + i.dy, Tile::Empty});
      }
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

  bool beardGrowTime = (moveCount()+1) % problem.beard.growthrate == 0;
  for (PosIdx rockbeardpos : rockbeardpositions) {
    Position rockbeard = content.idx2pos(rockbeardpos);
    int x = rockbeard.x;
    int y = rockbeard.y;

    if (content.atidx(rockbeardpos) == Tile::Rock) {
      if (get(x, y - 1) == Tile::Empty) {
        updateQueue.push_back({x, y, Tile::Empty});
        updateQueue.push_back({x, y - 1, Tile::Rock});
      } else if (get(x, y - 1) == Tile::Rock && get(x + 1, y) == Tile::Empty && get(x + 1, y - 1) == Tile::Empty) {
        updateQueue.push_back({x, y, Tile::Empty});
        updateQueue.push_back({x + 1, y - 1, Tile::Rock});
      } else if (get(x, y - 1) == Tile::Rock && get(x - 1, y) == Tile::Empty && get(x - 1, y - 1) == Tile::Empty) {
        updateQueue.push_back({x, y, Tile::Empty});
        updateQueue.push_back({x - 1, y - 1, Tile::Rock});
      } else if (get(x, y - 1) == Tile::Lambda && get(x + 1, y) == Tile::Empty && get(x + 1, y - 1) == Tile::Empty) {
        updateQueue.push_back({x, y, Tile::Empty});
        updateQueue.push_back({x + 1, y - 1, Tile::Rock});
      }
    } else {
      assert(content.atidx(rockbeardpos) == Tile::Beard);
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

  if (var.submergedSteps > problem.water.waterproof) {
    state = State::Lose;
  }

  if (var.collectedLambdas == problem.numInitialLambdas) {
    for (auto lift : problem.liftLoc)
      updateQueue.push_back({lift.x, lift.y, Tile::OpenLift});
  }

  for (auto update : updateQueue) {
    if (update.c == Tile::Rock && update.x == var.robotX && update.y == var.robotY + 1) {
      // Robot was hit on the head by rock
      state = State::Lose;
    }

    set(update.x, update.y, update.c);
  }

  ++totalMoves;
  return true;
}

int Mine::waterLevel(int turn) const {
  if (problem.water.floodingFreq == 0) {
    return problem.water.initWaterLevel;
  } else {
    return turn / problem.water.floodingFreq + problem.water.initWaterLevel;
  }
}

int Mine::moveCount() const {
  return totalMoves;
}

RobotCommands const& Mine::commands() const {
  return commandHistory;
}

void Mine::print() const {
  for (int y = problem.height - 1; y >= 0; --y) {
    for (int x = 0; x < problem.width; ++x)
      std::cout << charFromTile(get(x, y));
    std::cout << std::endl;
  }
  std::cout << std::endl;
}

std::string Mine::hashcode() const {
  unsigned char hash[20];
  sha1::calc(content.getGrid(), content.gridSize(), hash);
  return std::string((char const*)hash, 20);
}

void Mine::set(int x, int y, Tile c)
{
  Tile& tile = content.at(x,y);
  if ( tile == Tile::Rock || tile == Tile::Beard )
    rockbeardpositions.erase(content.pos2idx(x,y));
  tile = c;
  if ( tile == Tile::Rock || tile == Tile::Beard )
    rockbeardpositions.insert(content.pos2idx(x,y));
}
