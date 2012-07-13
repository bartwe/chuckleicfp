#include "Mine.hpp"

MineContent Mine::contentFromChar(char c) {
  switch (c) {
    case 'R':
      return Robot;
    case '#':
      return Wall;
    case '*':
      return Rock;
    case '\\':
      return Lambda;
    case 'L':
      return ClosedLift;
    case 'O':
      return OpenLift;
    case '.':
      return Earth;
    case ' ':
      return Empty;
    default:
      return Wall;
  }
}

char Mine::charFromContent(MineContent c) {
  switch (c) {
    case Robot:
      return 'R';
    case Wall:
      return '#';
    case Rock:
      return '*';
    case Lambda:
      return '\\';
    case ClosedLift:
      return 'L';
    case OpenLift:
      return 'O';
    case Earth:
      return '.';
    case Empty:
      return ' ';
    default:
      return '#';
  }
}

std::string Mine::stateToString(State s) {
  switch (s) {
    case InProgress:
      return "InProgress";
    case Win:
      return "Win";
    case Lose:
      return "Lose";
    case Aborted:
      return "Aborted";
    default:
      return "Unknown";
  }
}

void Mine::read(std::istream& is) {
  state = InProgress;
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

  content.resize(width * height, Empty);
  // Turn upside down so (0, 0) is bottom left
  std::reverse(readContent.begin(), readContent.end());

  for (size_t i = 0; i < readContent.size(); ++i) {
    auto const& row = readContent[i];
    for (size_t j = 0; j < row.size(); ++j) {
      auto c = row[j];
      if (c == Robot) {
        robotX = j;
        robotY = i;
      } else if (c == ClosedLift) {
        liftX = j;
        liftY = i;
      }

      set(j, i, c);
    }
  }
}

MineContent Mine::get(int x, int y) {
  if (x < 0 || x >= width || y < 0 || y >= width)
    return Wall;
  else
    return content[x * width + y];
}

State Mine::currentState() {
  return state;
}

int Mine::score() {
  int s = collectedLambdas * 25 - totalMoves;
  if (state == Aborted)
    s += collectedLambdas * 25;
  else if (state == Win)
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
  if (state != InProgress)
    return true;

  if (command == Abort) {
    state = Aborted;
    return true;
  }

  int newRobotX = robotX;
  int newRobotY = robotY;

  if (command == Left) {
    auto c = get(robotX - 1, robotY);
    if (c == Empty || c == Earth || c == Lambda || c == OpenLift) {
      newRobotX = robotX - 1;
    } else if (c == Rock && get(robotX - 2, robotY) == Empty) {
      set(robotX - 2, robotY, Rock);
      newRobotX = robotX - 1;
    }
  } else if (command == Right) {
    auto c = get(robotX + 1, robotY);
    if (c == Empty || c == Earth || c == Lambda || c == OpenLift) {
      newRobotX = robotX + 1;
    } else if (c == Rock && get(robotX + 2, robotY) == Empty) {
      set(robotX + 2, robotY, Rock);
      newRobotX = robotX + 1;
    }
  } else if (command == Up) {
    auto c = get(robotX, robotY + 1);
    if (c == Empty || c == Earth || c == Lambda || c == OpenLift)
      newRobotY = robotY + 1;
  } else if (command == Down) {
    auto c = get(robotX, robotY - 1);
    if (c == Empty || c == Earth || c == Lambda || c == OpenLift)
      newRobotY = robotY - 1;
  }

  auto nr = get(newRobotX, newRobotY);
  if (nr == Lambda)
    ++collectedLambdas;
  else if (nr == OpenLift)
    state = Win;

  set(robotX, robotY, Empty);
  set(newRobotX, newRobotY, Robot);
  robotX = newRobotX;
  robotY = newRobotY;

  updateMine();
  ++totalMoves;

  return state != InProgress;
}

void Mine::print() {
  for (int y = height - 1; y >= 0; --y) {
    for (int x = 0; x < height; ++x)
      std::cout << charFromContent(get(x, y));
    std::cout << std::endl;
  }
  std::cout << std::endl;
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
      if (get(x, y) == Lambda) {
        allLambdasCollected = false;
      } else if (get(x, y) == Rock && get(x, y - 1) == Empty) {
        updateQueue.push_back({x, y, Empty});
        updateQueue.push_back({x, y - 1, Rock});
      } else if (get(x, y) == Rock && get(x, y - 1) == Rock && get(x + 1, y) == Empty && get(x + 1, y - 1) == Empty) {
        updateQueue.push_back({x, y, Empty});
        updateQueue.push_back({x + 1, y - 1, Rock});
      } else if (get(x, y) == Rock && get(x, y - 1) == Rock && get(x - 1, y) == Empty && get(x - 1, y - 1) == Empty) {
        updateQueue.push_back({x, y, Empty});
        updateQueue.push_back({x - 1, y - 1, Rock});
      } else if (get(x, y) == Rock && get(x, y - 1) == Lambda && get(x + 1, y) == Empty && get(x + 1, y - 1) == Empty) {
        updateQueue.push_back({x, y, Empty});
        updateQueue.push_back({x + 1, y - 1, Rock});
      }
    }
  }

  if (allLambdasCollected)
    updateQueue.push_back({liftX, liftY, OpenLift});

  for (auto update : updateQueue) {
    if (update.c == Rock && update.x == robotX && update.y == robotY + 1) {
      // Robot was hit on the head by rock
      state = Lose;
    }
    set(update.x, update.y, update.c);
  }

  updateQueue.clear();
}
