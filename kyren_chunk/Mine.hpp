#ifndef MINE_HPP
#define MINE_HPP

#include "Problem.hpp"

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

  Mine(std::shared_ptr<Problem> problem);

  std::shared_ptr<Problem> getProblem() const;

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

private:
  struct MineUpdate {
    int x, y;
    Tile c;
  };

  void set(int x, int y, Tile c);

  std::shared_ptr<Problem> problem;
  Grid<Tile, Problem::PosIdx> content;

  // any tile type that can trigger an update
  std::set<Problem::PosIdx> rockbeardpositions;

  RobotCommands commandHistory;

  int totalMoves;
  State state;

  VariadicState var;
};

#endif
