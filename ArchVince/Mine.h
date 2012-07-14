#ifndef MINE_H
#define MINE_H

#include <vector>
#include <string>
#include <iostream>

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

enum class Commands : char
{
  Left = 'L',
  Right = 'R',
  Up = 'U',
  Down = 'D',
  Wait = 'W',
  Abort = 'A'
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
    int rx, ry; //Robot x and y

    std::vector< Point > lambdas;
    Point lift;
    Point robot;

  private:
    std::vector< std::vector< Tiles > > mine;
    std::vector< std::vector< int > > ids;
};

#endif
