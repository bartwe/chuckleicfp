#pragma once

#include <cstdint>
#include <cassert>
#include <string>

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

enum class State : uint8_t {
  InProgress,
  Win,
  Lose,
  Aborted
};

inline Tile tileFromChar(char c) {
	return (Tile)c;
}

inline char charFromTile(Tile t) {
	return (char)t;
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
