#ifndef MINE_HPP
#define MINE_HPP

#include <iostream>
#include <vector>
#include <algorithm>

enum class RobotCommand : uint8_t {
  Left,
  Right,
  Up,
  Down,
  Wait,
  Abort
};

enum class MineContent : uint8_t {
  Robot,
  Wall,
  Rock,
  Lambda,
  ClosedLift,
  OpenLift,
  Earth,
  Empty
};

enum class State : uint8_t {
  InProgress,
  Win,
  Lose,
  Aborted
};

class Mine {
public:
  static MineContent contentFromChar(char c);
  static char charFromContent(MineContent c);
  static std::string stateToString(State s);

  void read(std::istream& is);

  MineContent get(int x, int y);

  State currentState();

  int score();

  // Do a series of commands until state != InProgress
  void evaluate(std::vector<RobotCommand> commandList);

  void evaluateAndPrint(std::vector<RobotCommand> commandList);

  // If done, returns true.
  bool move(RobotCommand command);

  void print();

  // Returns 20 char (binary) SHA-1 hash of map state.
  std::string hashcode() const;

private:
  void set(int x, int y, MineContent c);

  void updateMine();

  struct QueuedUpdate {
    int x, y;
    MineContent c;
  };

  std::vector<MineContent> content;
  std::vector<QueuedUpdate> updateQueue;

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

#endif
