#ifndef MINE_HPP
#define MINE_HPP

#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <cstdint>
#include <cstring>

#include "Tile.hpp"
#include "Grid.hpp"

enum class RobotCommand : uint8_t {
  Left,
  Right,
  Up,
  Down,
  Wait,
  Abort,
  Slice
};

typedef Tile MineContent;

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
  std::string hashcode();
  int waterLevel(int turn) const;
  int indexOfTrampTarget(MineContent c) const;
  MineContent getTargetForTramp(MineContent c) const;
  std::vector<Coord> getTrampLocsForTarget(MineContent c) const;

private:
  void set(int x, int y, MineContent c);

  void checkConsistency();

  struct MineUpdate {
    int x, y;
    MineContent c;
  };

  // State that changes every move, and can be trivially copied
  struct variadicstate {
	  int robotX;
	  int robotY;
	  int collectedLambdas;
	  int curWaterLevel;
	  int submergedSteps;
    int curRazors;
  };

  struct MineHistory {
    // The updates it takes to go back to the previous state.
    std::vector<MineUpdate> updates;

    struct variadicstate prevvarstate;
  };

public:
  Grid<Tile> content;
private:
  // any tile type that can trigger an update
  std::set<PosIdx> rockbeardpositions;

  std::vector<MineHistory> historyList;
  RobotCommands commandHistory;

  // These are all problem-static fields -- they do not change anywhere except
  // in read()
  int initialTileHistogram[128];
  std::vector<Coord> liftLoc;
  std::vector<Coord> trampLoc;
  std::vector<Coord> targetLoc;
  std::vector<std::pair<MineContent, MineContent>> trampMapping;

public:

  int totalMoves;
  State state;


  variadicstate var;


  // These are all problem-static fields -- they do not change anywhere except
  // in read()
  int width;
  int height;
  int numInitialLambdas;
  struct {
	  struct {
		  int initWaterLevel;
		  int waterproof;
		  int floodingFreq;
	  } water;
	  struct {
		  int growthrate;
		  int initRazors;
	  } beard;
  } problem;
};

#endif
