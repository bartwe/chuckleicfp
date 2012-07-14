#ifndef PATHFINDER_H
#define PATHFINDER_H

#include <algorithm>
#include "Mine.h"
#include "NodeMap.h"

class Pathfinder
{
  public:
    Pathfinder(Mine tmine);

    std::string getSteps();
    bool liftStatus();

  private:
    struct PNode
    {
      Tiles type;
      std::vector< int > connections;
      std::vector< std::string > commands;
      std::string commandChain;
      int moveCost, cost;
      int x, y;
      int target;
    };

    std::string findPath(Point start, Point end);
    std::string findPath(int start, int end);
    int getLowest(std::vector< int > nodeList);
    int manhattan(int x1, int y1, int x2, int y2);
    PNode mToP(NodeMap::MNode m);
    bool contains(std::vector< int > list, int key);
    int getTarget(int node);
    void updTargets();

    Mine mine;
    Point robot;
    Point lift;
    bool liftReachable;
    int pushOpen(int node);
    std::vector< PNode > nodes;
    std::vector< Point > lambdas;
    std::vector< Point > badLambdas;
    std::vector< int > openNodes;
    std::vector< int > closedNodes;
};

#endif // PATHFINDER_H
