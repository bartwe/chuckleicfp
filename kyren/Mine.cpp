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
    default:
      return ' ';
  }
}

RobotCommand Mine::charToCommand(char command)
{
	switch (command) {
		case 'L': return RobotCommand::Left;
		case 'R': return RobotCommand::Right;
		case 'U': return RobotCommand::Up;
		case 'D': return RobotCommand::Down;
		case 'W': return RobotCommand::Wait;
		case 'A': return RobotCommand::Abort;
		default: // eh
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
    default:
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
  collectedLambdas = 0;
  totalMoves = 0;

  std::vector<std::vector<MineContent>> readContent;
  int maxRow = 0;
  while (!is.eof()) {
    char buf[4096];
    is.getline(buf, 4096);

    if (buf[0] == '\0') { //blank line means weather params
      break;
    }

    std::vector<MineContent> row;
    char* c = buf;
    while (*c)
      row.push_back(tileFromChar(*c++));
    maxRow = std::max<int>(maxRow, row.size());

    if (is.eof())
      break;

    readContent.push_back(row);
  }

  initWaterLevel = 0;
  floodingFreq = 0;
  waterproof = 10;
  while (!is.eof()) {
    char command[4096];
    char parameter[4096];
    is.getline(command, 4096, ' ');
    is.getline(parameter, 4096);

    if (strcmp(command, "Water") == 0) {
      initWaterLevel = atoi(parameter);
    } else if (strcmp(command, "Flooding") == 0) {
      floodingFreq = atoi(parameter);
    } else if (strcmp(command, "Waterproof") == 0) {
      waterproof = atoi(parameter);
    } else if (strcmp(command, "Trampoline") == 0) {
      trampMapping.push_back({tileFromChar(parameter[0]), tileFromChar(parameter[strlen(parameter)-1])});
    }
  }
  curWaterLevel = initWaterLevel;

  width = maxRow;
  height = readContent.size();

  Best::ReserveSpace(width * height);

  content.resize(width * height, MineContent::Empty);
  // Turn upside down so (0, 0) is bottom left
  std::reverse(readContent.begin(), readContent.end());

  numInitialLambdas = 0;

  for (int i = 0; i < readContent.size(); ++i) {
    auto const& row = readContent[i];
    for (int j = 0; j < row.size(); ++j) {
      auto c = row[j];
      if (c == MineContent::Robot) {
        robotX = j;
        robotY = i;
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
  if (x < 0 || x >= width || y < 0 || y >= width)
    return MineContent::Wall;
  else
    return content[y * width + x];
}

State Mine::currentState() {
  return state;
}

int Mine::score() {
  int s = collectedLambdas * 25 - totalMoves;
  if (state == State::Aborted)
    s += collectedLambdas * 25;
  else if (state == State::Win)
    s += collectedLambdas * 50;
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
  if (state != State::InProgress)
    return false;

  if (command == RobotCommand::Abort) {
    state = State::Aborted;
    commandHistory.push_back(command);
    historyList.push_back({{}, robotX, robotY, collectedLambdas});
    ++totalMoves;
    return true;
  }

  int newRobotX = robotX;
  int newRobotY = robotY;

  std::vector<MineUpdate> updateQueue;

  if (command == RobotCommand::Left) {
    auto c = get(robotX - 1, robotY);
    if (c == MineContent::Empty || c == MineContent::Earth || c == MineContent::Lambda || c == MineContent::OpenLift) {
      newRobotX = robotX - 1;
    } else if (c == MineContent::Rock && get(robotX - 2, robotY) == MineContent::Empty) {
      updateQueue.push_back({robotX - 2, robotY, MineContent::Rock});
      newRobotX = robotX - 1;
    } else if (c >= MineContent::TrampolineA && c <= MineContent::TrampolineI) {
      MineContent target = getTargetForTramp(c);
      Coord jumpTo = targetLoc[indexOfTrampTarget(target)];
      newRobotX = jumpTo.x;
      newRobotY = jumpTo.y;
      for (auto i : getTrampLocsForTarget(target)) {
        updateQueue.push_back({i.x, i.y, MineContent::Empty});
      }
    }
  } else if (command == RobotCommand::Right) {
    auto c = get(robotX + 1, robotY);
    if (c == MineContent::Empty || c == MineContent::Earth || c == MineContent::Lambda || c == MineContent::OpenLift) {
      newRobotX = robotX + 1;
    } else if (c == MineContent::Rock && get(robotX + 2, robotY) == MineContent::Empty) {
      updateQueue.push_back({robotX + 2, robotY, MineContent::Rock});
      newRobotX = robotX + 1;
    } else if (c >= MineContent::TrampolineA && c <= MineContent::TrampolineI) {
      MineContent target = getTargetForTramp(c);
      Coord jumpTo = targetLoc[indexOfTrampTarget(target)];
      newRobotX = jumpTo.x;
      newRobotY = jumpTo.y;
      for (auto i : getTrampLocsForTarget(target)) {
        updateQueue.push_back({i.x, i.y, MineContent::Empty});
      }
    }

  } else if (command == RobotCommand::Up) {
    auto c = get(robotX, robotY + 1);
    if (c == MineContent::Empty || c == MineContent::Earth || c == MineContent::Lambda || c == MineContent::OpenLift) {
      newRobotY = robotY + 1;
    } else if (c >= MineContent::TrampolineA && c <= MineContent::TrampolineI) {
      MineContent target = getTargetForTramp(c);
      Coord jumpTo = targetLoc[indexOfTrampTarget(target)];
      newRobotX = jumpTo.x;
      newRobotY = jumpTo.y;
      for (auto i : getTrampLocsForTarget(target)) {
        updateQueue.push_back({i.x, i.y, MineContent::Empty});
      }
    }
  } else if (command == RobotCommand::Down) {
    auto c = get(robotX, robotY - 1);
    if (c == MineContent::Empty || c == MineContent::Earth || c == MineContent::Lambda || c == MineContent::OpenLift) {
      newRobotY = robotY - 1;
    } else if (c >= MineContent::TrampolineA && c <= MineContent::TrampolineI) {
      MineContent target = getTargetForTramp(c);
      Coord jumpTo = targetLoc[indexOfTrampTarget(target)];
      newRobotX = jumpTo.x;
      newRobotY = jumpTo.y;
      for (auto i : getTrampLocsForTarget(target)) {
        updateQueue.push_back({i.x, i.y, MineContent::Empty});
      }
    }
  }

  // If we have been commanded to move, but haven't moved, command must not
  // have been valid, do nothing.
  if (command != RobotCommand::Wait && newRobotX == robotX && newRobotY == robotY)
    return false;

  commandHistory.push_back(command);

  // Record history for this state before updating.
  MineHistory historyEntry;
  historyEntry.collectedLambdas = collectedLambdas;
  historyEntry.robotX = robotX;
  historyEntry.robotY = robotY;
  historyEntry.submergedSteps = submergedSteps;

  auto nr = get(newRobotX, newRobotY);
  if (nr == MineContent::Lambda)
    ++collectedLambdas;
  else if (nr == MineContent::OpenLift)
    state = State::Win;

  updateQueue.push_back({robotX, robotY, MineContent::Empty});
  updateQueue.push_back({newRobotX, newRobotY, MineContent::Robot});

  // Apply all of the robot moves before computing the world update.
  for (auto update : updateQueue) {
    // Add the required update to go *backwards* onto the history entry.
    historyEntry.updates.push_back({update.x, update.y, get(update.x, update.y)});
    set(update.x, update.y, update.c);
  }
  updateQueue.clear();

  robotX = newRobotX;
  robotY = newRobotY;

  bool allLambdasCollected = true;

  for (int x = 0; x < width; ++x) {
    for (int y = 0; y < height; ++y) {
      if (get(x, y) == MineContent::Lambda) {
        allLambdasCollected = false;
      } else if (get(x, y) == MineContent::Rock && get(x, y - 1) == MineContent::Empty) {
        updateQueue.push_back({x, y, MineContent::Empty});
        updateQueue.push_back({x, y - 1, MineContent::Rock});
      } else if (get(x, y) == MineContent::Rock && get(x, y - 1) == MineContent::Rock && get(x + 1, y) == MineContent::Empty && get(x + 1, y - 1) == MineContent::Empty) {
        updateQueue.push_back({x, y, MineContent::Empty});
        updateQueue.push_back({x + 1, y - 1, MineContent::Rock});
      } else if (get(x, y) == MineContent::Rock && get(x, y - 1) == MineContent::Rock && get(x - 1, y) == MineContent::Empty && get(x - 1, y - 1) == MineContent::Empty) {
        updateQueue.push_back({x, y, MineContent::Empty});
        updateQueue.push_back({x - 1, y - 1, MineContent::Rock});
      } else if (get(x, y) == MineContent::Rock && get(x, y - 1) == MineContent::Lambda && get(x + 1, y) == MineContent::Empty && get(x + 1, y - 1) == MineContent::Empty) {
        updateQueue.push_back({x, y, MineContent::Empty});
        updateQueue.push_back({x + 1, y - 1, MineContent::Rock});
      }
    }
  }

  curWaterLevel = waterLevel(totalMoves + 1);

  if (curWaterLevel > robotY) {
    submergedSteps += 1;
  } else {
    submergedSteps = 0;
  }

  if (submergedSteps > waterproof) {
    state = State::Lose;
  }

  if (allLambdasCollected) {
    for (auto lift : liftLoc) {
      updateQueue.push_back({lift.x, lift.y, MineContent::OpenLift});
    }
  }

  for (auto update : updateQueue) {
    // Add the required update to go *backwards* onto the history entry.
    historyEntry.updates.push_back({update.x, update.y, get(update.x, update.y)});

    if (update.c == MineContent::Rock && update.x == robotX && update.y == robotY + 1) {
      // Robot was hit on the head by rock
      state = State::Lose;
    }
    set(update.x, update.y, update.c);
  }

  historyList.push_back(historyEntry);

  ++totalMoves;
    

  return true;
}

int Mine::waterLevel(int turn) const {
  if (floodingFreq == 0) {
    return initWaterLevel;
  } else {
    return turn / floodingFreq + initWaterLevel;
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
  robotX = history.robotX;
  robotY = history.robotY;
  submergedSteps = history.submergedSteps;
  collectedLambdas = history.collectedLambdas;
  for (auto const& update : history.updates)
    set(update.x, update.y, update.c);

  historyList.pop_back();
  commandHistory.pop_back();
  
  curWaterLevel = waterLevel(totalMoves - 1);
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

std::string Mine::hashcode() const {
  unsigned char hash[20];
  sha1::calc(&content[0], content.size(), hash);
  return std::string((char const*)hash, 20);
}

void Mine::set(int x, int y, MineContent c) {
  if (x < 0 || x >= width || y < 0 || y >= width)
    return;
  else
    content[y * width + x] = c;
}

void Mine::updateMine() {
}
