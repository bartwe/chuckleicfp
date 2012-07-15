#pragma once

#include <cstdint>

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

inline Tile tileFromChar(char c)
{
	return (Tile)c;
}

inline char charFromTile(Tile t)
{
	return (char)t;
}

