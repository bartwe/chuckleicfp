#include "Mine.hpp"
#include "sha1.h"

MineContent Mine::contentFromChar(char c) {
  switch (c) {
    case 'R':
      return MineContent::Robot;
    case '#':
      return MineContent::Wall;
    case '*':
      return MineContent::Rock;
    case '\\':
      return MineContent::Lambda;
    case 'L':
      return MineContent::ClosedLift;
    case 'O':
      return MineContent::OpenLift;
    case '.':
      return MineContent::Earth;
    case ' ':
      return MineContent::Empty;
    default:
      return MineContent::Wall;
  }
}

char Mine::charFromContent(MineContent c) {
  switch (c) {
    case MineContent::Robot:
      return 'R';
    case MineContent::Wall:
      return '#';
    case MineContent::Rock:
      return '*';
    case MineContent::Lambda:
      return '\\';
    case MineContent::ClosedLift:
      return 'L';
    case MineContent::OpenLift:
      return 'O';
    case MineContent::Earth:
      return '.';
    case MineContent::Empty:
      return ' ';
    default:
      return '#';
  }
}

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

void Mine::read(std::istream& is) {
  state = State::InProgress;
  collectedLambdas = 0;
  totalMoves = 0;

  std::vector<std::vector<MineContent>> readContent;
  size_t maxRow = 0;
  while (!is.eof()) {
    char buf[4096];
    is.getline(buf, 4096);

    std::vector<MineContent> row;
    char* c = buf;
    while (*c)
      row.push_back(contentFromChar(*c++));
    maxRow = std::max(maxRow, row.size());

    if (is.eof())
      break;

    readContent.push_back(row);
  }

  width = maxRow;
  height = readContent.size();

  content.resize(width * height, MineContent::Empty);
  // Turn upside down so (0, 0) is bottom left
  std::reverse(readContent.begin(), readContent.end());

  for (size_t i = 0; i < readContent.size(); ++i) {
    auto const& row = readContent[i];
    for (size_t j = 0; j < row.size(); ++j) {
      auto c = row[j];
      if (c == MineContent::Robot) {
        robotX = j;
        robotY = i;
      } else if (c == MineContent::ClosedLift) {
        liftX = j;
        liftY = i;
      }

      set(j, i, c);
    }
  }
}

MineContent Mine::get(int x, int y) {
  if (x < 0 || x >= width || y < 0 || y >= width)
    return MineContent::Wall;
  else
    return content[x * width + y];
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

void Mine::evaluate(std::vector<RobotCommand> commandList) {
  for (auto c : commandList)
    if (move(c))
      return;
}

void Mine::evaluateAndPrint(std::vector<RobotCommand> commandList) {
  print();
  for (auto c : commandList) {
    bool done = move(c);
    print();
    if (done)
      break;
  }
  std::cout << "Final state: " << stateToString(state) << std::endl;
  std::cout << "Final score: " << score() << std::endl;
}

bool Mine::move(RobotCommand command) {
  if (state != State::InProgress)
    return true;

  if (command == RobotCommand::Abort) {
    state = State::Aborted;
    return true;
  }

  int newRobotX = robotX;
  int newRobotY = robotY;

  if (command == RobotCommand::Left) {
    auto c = get(robotX - 1, robotY);
    if (c == MineContent::Empty || c == MineContent::Earth || c == MineContent::Lambda || c == MineContent::OpenLift) {
      newRobotX = robotX - 1;
    } else if (c == MineContent::Rock && get(robotX - 2, robotY) == MineContent::Empty) {
      set(robotX - 2, robotY, MineContent::Rock);
      newRobotX = robotX - 1;
    }
  } else if (command == RobotCommand::Right) {
    auto c = get(robotX + 1, robotY);
    if (c == MineContent::Empty || c == MineContent::Earth || c == MineContent::Lambda || c == MineContent::OpenLift) {
      newRobotX = robotX + 1;
    } else if (c == MineContent::Rock && get(robotX + 2, robotY) == MineContent::Empty) {
      set(robotX + 2, robotY, MineContent::Rock);
      newRobotX = robotX + 1;
    }
  } else if (command == RobotCommand::Up) {
    auto c = get(robotX, robotY + 1);
    if (c == MineContent::Empty || c == MineContent::Earth || c == MineContent::Lambda || c == MineContent::OpenLift)
      newRobotY = robotY + 1;
  } else if (command == RobotCommand::Down) {
    auto c = get(robotX, robotY - 1);
    if (c == MineContent::Empty || c == MineContent::Earth || c == MineContent::Lambda || c == MineContent::OpenLift)
      newRobotY = robotY - 1;
  }

  auto nr = get(newRobotX, newRobotY);
  if (nr == MineContent::Lambda)
    ++collectedLambdas;
  else if (nr == MineContent::OpenLift)
    state = State::Win;

  set(robotX, robotY, MineContent::Empty);
  set(newRobotX, newRobotY, MineContent::Robot);
  robotX = newRobotX;
  robotY = newRobotY;

  updateMine();
  ++totalMoves;

  return state != State::InProgress;
}

void Mine::print() {
  for (int y = height - 1; y >= 0; --y) {
    for (int x = 0; x < height; ++x)
      std::cout << charFromContent(get(x, y));
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
    content[x * width + y] = c;
}

void Mine::updateMine() {
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

  if (allLambdasCollected)
    updateQueue.push_back({liftX, liftY, MineContent::OpenLift});

  for (auto update : updateQueue) {
    if (update.c == MineContent::Rock && update.x == robotX && update.y == robotY + 1) {
      // Robot was hit on the head by rock
      state = State::Lose;
    }
    set(update.x, update.y, update.c);
  }

  updateQueue.clear();
}
