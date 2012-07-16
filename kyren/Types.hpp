#pragma once

#include <cstdint>
#include <cassert>
#include <string>
#include <vector>

enum class Tile : uint8_t {
  Robot = 'R',
  Wall = '#',
  Rock = '*',
  Lambda = '\\',
  ClosedLift = 'L',
  OpenLift = 'O',
  Earth = '.',
  Beard = 'W',
  Razor = '!',
  HigherOrderRock = '@',
  TrampolineA = 'A',
  TrampolineB = 'B',
  TrampolineC = 'C',
  TrampolineD = 'D',
  TrampolineE = 'E',
  TrampolineF = 'F',
  TrampolineG = 'G',
  TrampolineH = 'H',
  TrampolineI = 'I',
  Target1 = '1',
  Target2 = '2',
  Target3 = '3',
  Target4 = '4',
  Target5 = '5',
  Target6 = '6',
  Target7 = '7',
  Target8 = '8',
  Target9 = '9',
  Empty = ' '
};
typedef Tile MineContent;

enum class RobotCommand : uint8_t {
  Left,
  Right,
  Up,
  Down,
  Wait,
  Abort,
  Slice
};
typedef std::vector<RobotCommand> RobotCommands;
static const auto AllRobotCommands = {
    RobotCommand::Left,
    RobotCommand::Right,
    RobotCommand::Up,
    RobotCommand::Down,
    RobotCommand::Wait,
    RobotCommand::Abort,
    RobotCommand::Slice
  };

enum class State : uint8_t {
  InProgress,
  Win,
  Lose,
  Aborted
};

struct Solution {
  RobotCommands commands;
  int score;

  bool operator<(Solution const& solution) const {
    return score < solution.score;
  }
};
typedef std::vector<Solution> Solutions;

inline Tile tileFromChar(char c) {
	return (Tile)c;
}

inline char charFromTile(Tile t) {
	return (char)t;
}

inline bool rockType(Tile t) {
  return t == Tile::Rock || t == Tile::HigherOrderRock;
}

inline bool trampolineType(Tile t) {
  return (char)t >= (char)Tile::TrampolineA && (char)t <= (char)Tile::TrampolineI;
}

inline std::string stateToString(State s) {
  switch (s) {
    case State::InProgress: return "InProgress";
    case State::Win: return "Win";
    case State::Lose: return "Lose";
    case State::Aborted: return "Aborted";
    default: assert(!"Unknown state" ); return "";
  }
}

inline char commandChar(RobotCommand command) {
  switch (command) {
    case RobotCommand::Left: return 'L';
    case RobotCommand::Right: return 'R';
    case RobotCommand::Up: return 'U';
    case RobotCommand::Down: return 'D';
    case RobotCommand::Wait: return 'W';
    case RobotCommand::Abort: return 'A';
    case RobotCommand::Slice: return 'S';
    default: assert(!"Unknown command"); return ' ';
  }
}

inline RobotCommand charToCommand(char command) {
	switch (command) {
		case 'L': return RobotCommand::Left;
		case 'R': return RobotCommand::Right;
		case 'U': return RobotCommand::Up;
		case 'D': return RobotCommand::Down;
		case 'W': return RobotCommand::Wait;
		case 'A': return RobotCommand::Abort;
    case 'S': return RobotCommand::Slice;
		default: assert(!"Unknown command"); return RobotCommand::Wait;
	}
}

inline std::string commandName(RobotCommand command) {
  switch (command) {
    case RobotCommand::Left: return "Left";
    case RobotCommand::Right: return "Right";
    case RobotCommand::Up: return "Up";
    case RobotCommand::Down: return "Down";
    case RobotCommand::Wait: return "Wait";
    case RobotCommand::Abort: return "Abort";
    case RobotCommand::Slice: return "Slice";
    default: assert(!"Unknown command"); return "";
  }
}

inline std::string commandString(RobotCommands commands) {
  std::string str;
  for (auto c : commands)
    str.push_back(commandChar(c));
  return str;
}
