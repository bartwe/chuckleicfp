#ifndef MINE_H
#define MINE_H

#include <vector>
#include <string>
#include <iostream>
#include <stdlib.h>

enum class Tiles : char
{
  empty,
  earth,
  rock,
  wall,
  lift,
  lambda,
  robot,
  a,
  b,
  c,
  d,
  e,
  f,
  g,
  h,
  i,
  t1,
  t2,
  t3,
  t4,
  t5,
  t6,
  t7,
  t8,
  t9
};

struct Point
{
  int x, y;
};

class Mine
{
  public:
    Mine(std::vector< std::string > rows);

    Tiles at(int x, int y);
    Tiles at(Point p);

    void setID(int x, int y, int id);
    void setID(Point p, int id);

    int getID(int x, int y);
    int getID(Point p);

    int width, height;

    std::vector< Point > lambdas;
    Point lift;
    Point robot;
    int water, flooding, waterproof;
    int targets[9];
    Point targetsPos[9];
    Point tramps[9];

  private:
    std::vector< std::vector< Tiles > > mine;
    std::vector< std::vector< int > > ids;
};

#endif
