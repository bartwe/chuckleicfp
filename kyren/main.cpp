#include <iostream>
#include <vector>
#include <algorithm>

enum RobotCommand {
  Left,
  Right,
  Up,
  Down,
  Wait,
  Abort
};

enum MineContent {
  Robot,
  Wall,
  Rock,
  Lambda,
  ClosedLift,
  OpenLift,
  Earth,
  Empty
};

enum State {
  InProgress,
  Win,
  Lose,
  Aborted
};

class Mine {
public:
  static MineContent contentFromChar(char c) {
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

  static char charFromContent(MineContent c) {
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

  static std::string stateToString(State s) {
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

  void read(std::istream& is) {
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
      readContent.push_back(row);

      if (is.eof())
        break;
    }

    width = maxRow;
    height = readContent.size();

    content.resize(width * height, Empty);
    nextContent.resize(width * height, Empty);
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

  MineContent get(int x, int y) {
    if (x < 0 || x >= width || y < 0 || y >= width)
      return Wall;
    else
      return content[x * width + y];
  }

  State currentState() {
    return state;
  }

  int score() {
    int s = collectedLambdas * 25 - totalMoves;
    if (state == Aborted)
      s += collectedLambdas * 25;
    else if (state == Win)
      s += collectedLambdas * 50;
    return s;
  }

  // Do a series of commands until state != InProgress
  void evaluate(std::vector<RobotCommand> commandList) {
    for (auto c : commandList)
      if (move(c))
        return;
  }

  void evaluateAndPrint(std::vector<RobotCommand> commandList) {
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

  // If done, returns true.
  bool move(RobotCommand command) {
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

  void print() {
    for (int y = height - 1; y >= 0; --y) {
      for (int x = 0; x < height; ++x)
        std::cout << charFromContent(get(x, y));
      std::cout << std::endl;
    }
    std::cout << std::endl;
  }

private:
  MineContent getNext(int x, int y) {
    if (x < 0 || x >= width || y < 0 || y >= width)
      return Wall;
    else
      return nextContent[x * width + y];
  }


  void setNext(int x, int y, MineContent c) {
    if (x < 0 || x >= width || y < 0 || y >= width)
      return;
    else
      nextContent[x * width + y] = c;
  }

  void set(int x, int y, MineContent c) {
    if (x < 0 || x >= width || y < 0 || y >= width)
      return;
    else
      content[x * width + y] = c;
  }

  void updateMine() {
    std::copy(content.begin(), content.end(), nextContent.begin());

    bool allLambdasCollected = true;

    for (int x = 0; x < width; ++x) {
      for (int y = 0; y < height; ++y) {
        if (get(x, y) == Lambda) {
          allLambdasCollected = false;
        } else if (get(x, y) == Rock && get(x, y - 1) == Empty) {
          setNext(x, y, Empty);
          setNext(x, y - 1, Rock);
        } else if (get(x, y) == Rock && get(x, y - 1) == Rock && get(x + 1, y) == Empty && get(x + 1, y - 1) == Empty) {
          setNext(x, y, Empty);
          setNext(x + 1, y - 1, Rock);
        } else if (get(x, y) == Rock && get(x, y - 1) == Rock && get(x - 1, y) == Empty && get(x - 1, y - 1) == Empty) {
          setNext(x, y, Empty);
          setNext(x - 1, y - 1, Rock);
        } else if (get(x, y) == Rock && get(x, y - 1) == Lambda && get(x + 1, y) == Empty && get(x + 1, y - 1) == Empty) {
          setNext(x, y, Empty);
          setNext(x + 1, y - 1, Rock);
        }
      }
    }

    if (allLambdasCollected)
      setNext(liftX, liftY, OpenLift);

    if (get(robotX, robotY + 1) != Rock && getNext(robotX, robotY + 1) == Rock)
      state = Lose;

    std::swap(content, nextContent);
  }

  std::vector<MineContent> content;
  std::vector<MineContent> nextContent;

  int width;
  int height;

  int robotX;
  int robotY;
  State state;

  int liftX;
  int liftY;

  int collectedLambdas;
  int totalMoves;
};

int main(int argc, char** argv) {
  Mine mine;
  mine.read(std::cin);

  // Run sample for contest1.map

  mine.evaluateAndPrint({Down, Left, Left, Left, Down, Down, Right, Right, Right, Left, Up, Left, Left, Down, Left});
  return 0;
}
