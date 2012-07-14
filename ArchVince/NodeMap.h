#ifndef NODEMAP_H
#define NODEMAP_H

#include "Mine.h"

class NodeMap
{
  public:
    struct MNode
    {
      Tiles type;
      std::vector< int > connections;
      int x, y;
      std::vector< Commands > commands;
    };

    NodeMap(Mine mine);

    MNode get(int nodePos);

    Point robot;
    Point lift;
    bool liftReachable;
    std::vector< Point > lambdas;
    std::vector< MNode > nodes;
    Mine mine;
};

#endif // NODEMAP_H
