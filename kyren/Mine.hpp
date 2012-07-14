#ifndef MINE_HPP
#define MINE_HPP

#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdint>
#include <cstring>

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

struct Coord {
  int x;
  int y;
};

typedef std::vector<RobotCommand> RobotCommands;

class Mine {
public:
  static MineContent contentFromChar(char c);
  static char charFromContent(MineContent c);
  static std::string stateToString(State s);
  static char commandChar(RobotCommand command);
  static RobotCommand charToCommand(char command);
  static std::string commandName(RobotCommand command);
  static std::string commandString(RobotCommands commands);

  void read(std::istream& is);

  MineContent get(int x, int y);

  State currentState();

  int score();

  // Do a series of commands until state != InProgress
  void evaluate(RobotCommands commandList);

  void evaluateAndPrint(RobotCommands commandList);

  // Returns false if illegal move (and does nothing)
  bool pushMove(RobotCommand command);
  // Revert the most recent move action.  Returns false if no more moves to revert.
  bool popMove();

  int moveCount() const;
  RobotCommands const& commands() const;

  void print();

  // Returns 20 char (binary) SHA-1 hash of map state.
  std::string hashcode() const;
  int waterLevel(int turn) const;

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
    int submergedSteps;
  };

public:
  std::vector<MineContent> content;
private:
  std::vector<MineHistory> historyList;
  RobotCommands commandHistory;

  // NOTE: there can be more than one lift, this is wrong!!! --Jeroen
  std::vector<Coord> liftLoc;

public:
  int totalMoves;
  int width;
  int height;

  State state;
  int robotX;
  int robotY;

  int collectedLambdas;
  int numInitialLambdas;
  int initWaterLevel;
  int curWaterLevel;
  int floodingFreq;
  int waterproof;
  int submergedSteps;
};

#endif
