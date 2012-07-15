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
  robot
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

  private:
    std::vector< std::vector< Tiles > > mine;
    std::vector< std::vector< int > > ids;
};

#endif
