#include "Mine.hpp"
#include "sha1.h"
#include "BestSoFar.h"

std::string Mine::stateToString(State s) {
  switch (s) {
    case State::InProgress:
      return "InProgress";
    case State::Win:
      return "Win";
    case State::Lose:
      return "Lose";
    case State::Aborted:
      return "Aborted";
    default:
      REQUIRE( !"Unknown state" );
      return "Unknown";
  }
}

char Mine::commandChar(RobotCommand command) {
  switch (command) {
    case RobotCommand::Left:
      return 'L';
    case RobotCommand::Right:
      return 'R';
    case RobotCommand::Up:
      return 'U';
    case RobotCommand::Down:
      return 'D';
    case RobotCommand::Wait:
      return 'W';
    case RobotCommand::Abort:
      return 'A';
    case RobotCommand::Slice:
      return 'S';
    default:
      REQUIRE( !"Unknown command" );
      return ' ';
  }
}

struct delta_t
{
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

RobotCommand Mine::charToCommand(char command)
{
	switch (command) {
		case 'L': return RobotCommand::Left;
		case 'R': return RobotCommand::Right;
		case 'U': return RobotCommand::Up;
		case 'D': return RobotCommand::Down;
		case 'W': return RobotCommand::Wait;
		case 'A': return RobotCommand::Abort;
    case 'S': return RobotCommand::Slice;
		default: // eh
			  REQUIRE( !"Unknown command" );
			return RobotCommand::Wait;
	}
}

std::string Mine::commandName(RobotCommand command) {
  switch (command) {
    case RobotCommand::Left:
      return "Left";
    case RobotCommand::Right:
      return "Right";
    case RobotCommand::Up:
      return "Up";
    case RobotCommand::Down:
      return "Down";
    case RobotCommand::Wait:
      return "Wait";
    case RobotCommand::Abort:
      return "Abort";
    case RobotCommand::Slice:
      return "Slice";
    default:
      REQUIRE( !"Unknown command" );
      return "";
  }
}

std::string Mine::commandString(RobotCommands commands) {
  std::string str;
  for (auto c : commands)
    str.push_back(commandChar(c));
  return str;
}

void Mine::read(std::istream& is) {
  state = State::InProgress;
  var.collectedLambdas = 0;
  totalMoves = 0;

  std::vector<std::vector<MineContent>> readContent;
  int maxRow = 0;
  std::string line;
  while (std::getline(is, line)) {
    if (line.size() == 0) break;

    std::vector<MineContent> row;
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
      trampMapping.push_back({tileFromChar(parameter[0]), tileFromChar(parameter[strlen(parameter)-1])});
    }
  }
  var.curWaterLevel = problem.water.initWaterLevel;
  var.curRazors = problem.beard.initRazors;

  width = maxRow;
  height = readContent.size();

  Best::ReserveSpace(width * height);

  content.reset(width, height, Tile::Empty);
  // Turn upside down so (0, 0) is bottom left
  std::reverse(readContent.begin(), readContent.end());

  numInitialLambdas = 0;

  for (int i = 0; i < readContent.size(); ++i) {
    auto const& row = readContent[i];
    for (int j = 0; j < row.size(); ++j) {
      auto c = row[j];
      if (c == MineContent::Robot) {
        var.robotX = j;
        var.robotY = i;
      } else if (c == MineContent::ClosedLift) {
        liftLoc.push_back({j, i});
      } else if (c == MineContent::Lambda) {
	      numInitialLambdas++;
      } else if (c >= MineContent::TrampolineA && c <= MineContent::TrampolineI) {
        int index = indexOfTrampTarget(c);
        if ((int)c >= trampLoc.size()) {
          trampLoc.resize(index+1);
        }
        trampLoc[index] = {j, i};
      } else if (c >= MineContent::Target1 && c <= MineContent::Target9) {
        int index = indexOfTrampTarget(c);
        if ((int)c >= targetLoc.size()) {
          targetLoc.resize(index+1);
        }
        targetLoc[index] = {j, i};
      }

      set(j, i, c);
    }
  }

  memset(initialTileHistogram, 0, sizeof initialTileHistogram);

  for (int i=0; i<content.gridSize(); i++) {
    Tile tile = content.atidx(i);
    initialTileHistogram[charFromTile(tile)]++;

    if (tile==Tile::Rock || tile==Tile::Beard)
      rockbeardpositions.insert(i);
  }
  REQUIRE( initialTileHistogram[charFromTile(Tile::Robot)] == 1 );
  REQUIRE( initialTileHistogram[charFromTile(Tile::ClosedLift)] == 1 );
  REQUIRE( initialTileHistogram[charFromTile(Tile::OpenLift)] == 0 );
  REQUIRE( initialTileHistogram[charFromTile(Tile::Lambda)] == numInitialLambdas );
}

int Mine::indexOfTrampTarget(MineContent c) const {
  if (c >= MineContent::TrampolineA && c <= MineContent::TrampolineI)
    return (int)c - (int)MineContent::TrampolineA;
  else if (c >= MineContent::Target1 && c <= MineContent::Target9)
    return (int)c - (int)MineContent::Target1;
  else
    return -1; //error
}

MineContent Mine::getTargetForTramp(MineContent c) const {
  for (auto i : trampMapping) {
    if (i.first == c) {
      return i.second;
    }
  }
  return MineContent::Empty;
}

std::vector<Coord> Mine::getTrampLocsForTarget(MineContent c) const {
  std::vector<Coord> res;
  for (auto i : trampMapping) {
    if (i.second == c) {
      res.push_back(trampLoc[indexOfTrampTarget(i.first)]);
    }
  }
  return res;
}

MineContent Mine::get(int x, int y) {
  return content.at(x,y);
}

State Mine::currentState() {
  return state;
}

int Mine::score() {
  int s = var.collectedLambdas * 25 - totalMoves;
  if (state == State::Aborted)
    s += var.collectedLambdas * 25;
  else if (state == State::Win)
    s += var.collectedLambdas * 50;
  return s;
}

void Mine::evaluate(RobotCommands commandList) {
  for (auto c : commandList) {
    pushMove(c);
    if (state != State::InProgress)
      return;
  }
}

void Mine::evaluateAndPrint(RobotCommands commandList) {
  print();
  for (auto c : commandList) {
    if (!pushMove(c))
      std::cout << "Illegal command: " << commandName(c) << std::endl;

    print();
    if (state != State::InProgress)
      break;
  }
  std::cout << "Final state: " << stateToString(state) << std::endl;
  std::cout << "Final score: " << score() << std::endl;
}

bool Mine::pushMove(RobotCommand command) {
  checkConsistency();
  if (state != State::InProgress)
    return false;

  if (command == RobotCommand::Abort) {
    state = State::Aborted;
    commandHistory.push_back(command);
    historyList.push_back({{}, var});
    ++totalMoves;
    checkConsistency();
    return true;
  }

  int newRobotX = var.robotX;
  int newRobotY = var.robotY;

  std::vector<MineUpdate> updateQueue;

  int dx=0, dy=0;
  switch (command)
  {
	  case RobotCommand::Left:  dx=-1; break;
	  case RobotCommand::Right: dx=+1; break;
	  case RobotCommand::Up:    dy=+1; break;
	  case RobotCommand::Down:  dy=-1; break;
  }
  if (dx != 0 || dy != 0) {
    int nx = var.robotX + dx;
    int ny = var.robotY + dy;
    auto c = get(nx, ny);
    if (c == MineContent::Empty || 
        c == MineContent::Earth || 
        c == MineContent::Lambda || 
        c == MineContent::OpenLift || 
        c == MineContent::Razor) {
      newRobotX = nx;
      newRobotY = ny;
    } else if (c == MineContent::Rock && dy == 0 && get(nx + dx, var.robotY) == MineContent::Empty) {
      updateQueue.push_back({nx+dx, var.robotY, MineContent::Rock});
      newRobotX = nx;
    } else if (c >= MineContent::TrampolineA && c <= MineContent::TrampolineI) {
      MineContent target = getTargetForTramp(c);
      Coord jumpTo = targetLoc[indexOfTrampTarget(target)];
      newRobotX = jumpTo.x;
      newRobotY = jumpTo.y;
      for (auto i : getTrampLocsForTarget(target)) {
        updateQueue.push_back({i.x, i.y, MineContent::Empty});
      }
    } 

    // If we have been commanded to move, but haven't moved, command must not
    // have been valid, do nothing.
    if (newRobotX == var.robotX && newRobotY == var.robotY)
    {
      checkConsistency();
      return false;
    }
  }

  if (command == RobotCommand::Slice) {
    var.curRazors--;
    for (auto i : BeardDirs) {
      if (get(var.robotX + i.dx, var.robotY + i.dy) == MineContent::Beard) {
        updateQueue.push_back({var.robotX + i.dx, var.robotY + i.dy, MineContent::Empty});
      }
    }
  }

  commandHistory.push_back(command);

  // Record history for this state before updating.
  MineHistory historyEntry = {{}, var};

  auto nr = get(newRobotX, newRobotY);
  if (nr == MineContent::Lambda)
    ++var.collectedLambdas;
  else if (nr == MineContent::Razor)
    ++var.curRazors;
  else if (nr == MineContent::OpenLift)
    state = State::Win;

  updateQueue.push_back({var.robotX, var.robotY, MineContent::Empty});
  updateQueue.push_back({newRobotX, newRobotY, MineContent::Robot});

  // Apply all of the robot moves before computing the world update.
  for (auto update : updateQueue) {
    // Add the required update to go *backwards* onto the history entry.
    historyEntry.updates.push_back({update.x, update.y, get(update.x, update.y)});
    set(update.x, update.y, update.c);
  }
  updateQueue.clear();

  var.robotX = newRobotX;
  var.robotY = newRobotY;

  bool beardGrowTime = (moveCount()+1) % problem.beard.growthrate == 0;
  for ( PosIdx rockbeardpos : rockbeardpositions ) {
    Position rockbeard = content.idx2pos(rockbeardpos);
    int x = rockbeard.x;
    int y = rockbeard.y;
    
    if (content.atidx(rockbeardpos) == Tile::Rock) {
      if (get(x, y - 1) == MineContent::Empty) {
        updateQueue.push_back({x, y, MineContent::Empty});
        updateQueue.push_back({x, y - 1, MineContent::Rock});
      } else if (get(x, y - 1) == MineContent::Rock && get(x + 1, y) == MineContent::Empty && get(x + 1, y - 1) == MineContent::Empty) {
        updateQueue.push_back({x, y, MineContent::Empty});
        updateQueue.push_back({x + 1, y - 1, MineContent::Rock});
      } else if (get(x, y - 1) == MineContent::Rock && get(x - 1, y) == MineContent::Empty && get(x - 1, y - 1) == MineContent::Empty) {
        updateQueue.push_back({x, y, MineContent::Empty});
        updateQueue.push_back({x - 1, y - 1, MineContent::Rock});
      } else if (get(x, y - 1) == MineContent::Lambda && get(x + 1, y) == MineContent::Empty && get(x + 1, y - 1) == MineContent::Empty) {
        updateQueue.push_back({x, y, MineContent::Empty});
        updateQueue.push_back({x + 1, y - 1, MineContent::Rock});
      }
    } else {
      REQUIRE( content.atidx(rockbeardpos) == Tile::Beard);
      if ( beardGrowTime )
      {
	for ( auto dir : BeardDirs ) {
	  int nx = x + dir.dx;
	  int ny = y + dir.dy;
	  if ( get(nx, ny) == Tile::Empty ) {
	    updateQueue.push_back({nx, ny, MineContent::Beard});
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

  if (var.collectedLambdas == numInitialLambdas) {
    for (auto lift : liftLoc) {
      updateQueue.push_back({lift.x, lift.y, MineContent::OpenLift});
    }
  }

  for (auto update : updateQueue) {
    // Add the required update to go *backwards* onto the history entry.
    historyEntry.updates.push_back({update.x, update.y, get(update.x, update.y)});

    if (update.c == MineContent::Rock && update.x == var.robotX && update.y == var.robotY + 1) {
      // Robot was hit on the head by rock
      state = State::Lose;
    }
    set(update.x, update.y, update.c);
  }

  historyList.push_back(historyEntry);

  ++totalMoves;
    

  checkConsistency();
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

bool Mine::popMove() {
  if (historyList.empty())
    return false;

  auto const& history = historyList[historyList.size() - 1];
  memcpy(&var, &history.prevvarstate, sizeof var);
  for (auto const& update : history.updates)
    set(update.x, update.y, update.c);

  historyList.pop_back();
  commandHistory.pop_back();
  
  var.curWaterLevel = waterLevel(totalMoves - 1);
  state = State::InProgress;
  --totalMoves;
  return true;
}

void Mine::print() {
  for (int y = height - 1; y >= 0; --y) {
    for (int x = 0; x < width; ++x)
      std::cout << charFromTile(get(x, y));
    std::cout << std::endl;
  }
  std::cout << std::endl;
}

std::string Mine::hashcode() {
  unsigned char hash[20];
  sha1::calc(content.getGrid(), content.gridSize(), hash);
  return std::string((char const*)hash, 20);
}

void Mine::set(int x, int y, MineContent c)
{
  Tile& tile = content.at(x,y);
  if ( tile == Tile::Rock || tile == Tile::Beard )
    rockbeardpositions.erase(content.pos2idx(x,y));
  tile = c;
  if ( tile == Tile::Rock || tile == Tile::Beard )
    rockbeardpositions.insert(content.pos2idx(x,y));
}

void Mine::checkConsistency() {
	// TODO: check whether the collectedLambdas is correct
	// TODO: check the rockindex
	// etc
  REQUIRE( moveCount() == historyList.size() );
}
