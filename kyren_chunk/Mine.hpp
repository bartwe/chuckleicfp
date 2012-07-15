#ifndef MINE_HPP
#define MINE_HPP

#include <vector>
#include <set>
#include <algorithm>
#include <iostream>
#include <cstring>

#include "Types.hpp"
#include "Grid.hpp"

struct Problem {
  int initialTileHistogram[128];
  std::vector<Position> liftLoc;
  std::vector<Position> trampLoc;
  std::vector<Position> targetLoc;
  std::vector<std::pair<Tile, Tile>> trampMapping;

  int width;
  int height;
  int numInitialLambdas;

  struct {
    int initWaterLevel;
    int waterproof;
    int floodingFreq;
  } water;
  struct {
    int growthrate;
    int initRazors;
  } beard;
};

class Mine {
public:
  // State that changes every move, and can be trivially copied
  struct VariadicState {
	  int robotX;
	  int robotY;
	  int collectedLambdas;
	  int curWaterLevel;
	  int submergedSteps;
    int curRazors;
  };

  void read(std::istream& is);

  Problem const& getProblem() const;

  Tile get(int x, int y) const;

  bool ended() const;
  State currentState() const;
  VariadicState const& currentVariadicState() const;

  int score() const;

  // Do a series of commands until state != InProgress
  void evaluate(RobotCommands commandList);
  void evaluateAndPrint(RobotCommands commandList);

  // Returns false if illegal command (and does nothing)
  bool doCommand(RobotCommand command);

  RobotCommands const& commands() const;
  int moveCount() const;
  int collectedLambdas() const;
  int submergedSteps() const;
  int currentWaterLevel() const;

  void print() const;

  // Returns 20 char (binary) SHA-1 hash of map state.
  std::string hashcode() const;

  int waterLevel(int turn) const;
  int indexOfTrampTarget(Tile c) const;
  Tile getTargetForTramp(Tile c) const;
  std::vector<Position> getTrampLocsForTarget(Tile c) const;

private:
  struct MineUpdate {
    int x, y;
    Tile c;
  };
  typedef uint32_t PosIdx;

  void set(int x, int y, Tile c);

  Grid<Tile, PosIdx> content;

  // any tile type that can trigger an update
  std::set<PosIdx> rockbeardpositions;

  RobotCommands commandHistory;

  int totalMoves;
  State state;

  VariadicState var;

  // These are all problem-static fields -- they do not change anywhere except
  // in read()
  Problem problem;
};

#endif
