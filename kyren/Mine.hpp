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
  static char commandChar(RobotCommand command);
  static std::string commandString(RobotCommand command);

  void read(std::istream& is);

  MineContent get(int x, int y);

  State currentState();

  int score();

  // Do a series of commands until state != InProgress
  void evaluate(std::vector<RobotCommand> commandList);

  void evaluateAndPrint(std::vector<RobotCommand> commandList);

  // Returns false if illegal move (and does nothing)
  bool pushMove(RobotCommand command);
  // Revert the most recent move action.  Returns false if no more moves to revert.
  bool popMove();

  int moveCount() const;
  std::vector<RobotCommand> const& commands() const;

  void print();

  // Returns 20 char (binary) SHA-1 hash of map state.
  std::string hashcode() const;

private:
  void set(int x, int y, MineContent c);

  void updateMine();

  struct MineUpdate {
    int x, y;
    MineContent c;
  };

  struct MineHistory {
    // The updates it takes to go back to the previous state.
    std::vector<MineUpdate> updates;
    // Old robot position
    int robotX;
    int robotY;
    // Old collectedLambdas
    int collectedLambdas;
  };

  std::vector<MineContent> content;
  std::vector<MineHistory> historyList;
  std::vector<RobotCommand> commandHistory;

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
