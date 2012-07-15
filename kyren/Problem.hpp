#pragma once

#include <vector>
#include <set>
#include <algorithm>
#include <iostream>
#include <memory>
#include <cstring>

#include "Types.hpp"
#include "Grid.hpp"

struct Problem {
  typedef Grid<Tile> TileGrid;

  static std::shared_ptr<Problem> read(std::istream& is);

  int indexOfTrampTarget(Tile c) const;
  Tile getTargetForTramp(Tile c) const;
  std::vector<Position> getTrampLocsForTarget(Tile c) const;

  int initialTileHistogram[128];
  std::vector<Position> liftLoc;
  std::vector<Position> trampLoc;
  std::vector<Position> targetLoc;

  std::vector<std::pair<Tile, Tile>> trampMapping;
  std::set<Position> rockBeardPositions;

  int width;
  int height;
  int numInitialLambdas;

  int robotX;
  int robotY;

  struct {
    int initWaterLevel;
    int waterproof;
    int floodingFreq;
  } water;
  struct {
    int growthrate;
    int initRazors;
  } beard;

  TileGrid tiles;
};
