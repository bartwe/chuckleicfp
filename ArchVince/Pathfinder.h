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

    struct similarState
    {
      int replaceLength;
      std::string replaceCommand;
      int addedCost;
      std::vector< updStep > addedSteps;
    };

    struct searchState
    {
      int id;
      std::vector< int > lambdas;
      std::vector< updStep > steps;
      std::vector< similarState > similar;
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
    std::vector< updStep > stepUpdMap(searchState ss);
    int pushOpen(searchState ss);
    Tiles getType(searchState ss, int id);
    bool vectorMatch(std::vector< int > v, std::vector< int > w);
    bool liftStatus(searchState ss);
    bool isLegal(searchState ss, int cid);
    std::vector< updStep > applyUpd(std::vector< updStep > steps, searchState ss, updStep step1);
    std::vector< int > getSimilar(searchState ss, int index);
    searchState processSimilar(searchState ss, int index);
    std::vector< updStep > fixMismatch(searchState s, searchState o);
    searchState stateFromSimilar(searchState ss, similarState sim);

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
