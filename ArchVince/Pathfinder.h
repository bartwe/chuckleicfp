#ifndef PATHFINDER_H
#define PATHFINDER_H

#include <algorithm>
#include "Mine.h"
#include "NodeMap.h"

class Pathfinder
{
  public:
    struct updStep
    {
      int id;
      Tiles type;
    };

    Pathfinder(Mine tmine);

    std::string getSteps();

  private:

    struct scoreCard
    {
      std::string moves;
      int score;
    };

    struct searchState
    {
      int id;
      std::vector< int > lambdas;
      std::vector< updStep > steps;
      std::string commands;
      int moveCost, cost;
      int score;
    };

    std::string findPath(Point start, Point end);
    std::string findPath(int start, int end);
    int getLowest(std::vector< searchState > nodeList);
    int manhattan(int x1, int y1, int x2, int y2);
    int manhattan(Point p1, Point p2);
    int contains(std::vector< searchState > list, searchState key);
    int getTarget(searchState ss, int id);
    void clearStoredNodes();
    std::vector< updStep > stepUpdMap(searchState ss, updStep step);
    int pushOpen(searchState ss);
    Tiles getType(searchState ss, int id);
    bool vectorMatch(std::vector< int > v, std::vector< int > w);
    bool liftStatus(searchState ss);

    Mine mine;
    Point robot;
    Point lift;
    bool liftReachable;
    std::vector< NodeMap::MNode > nodes;
    std::vector< Point > lambdas;
    std::vector< Point > badLambdas;
    std::vector< searchState > openNodes;
    std::vector< searchState > closedNodes;
};

#endif // PATHFINDER_H
