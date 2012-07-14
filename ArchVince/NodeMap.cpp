#include "NodeMap.h"

NodeMap::NodeMap(Mine tmine) : mine(tmine)
{
  liftReachable = false;
  robot = mine.robot;
  lift = mine.lift;
  std::vector< Point > expand;
  expand.push_back({mine.rx, mine.ry});
  while((int)expand.size() > 0)
  {
    MNode tNode;
    Point tPoint = expand.back();
    tNode.x = tPoint.x;
    tNode.y = tPoint.y;
    //if(mine.getID(tPoint) == -1) std::cout << expand.size() << " " << tPoint.x << " " << tPoint.y << std::endl;
    expand.pop_back();
    if(mine.getID(tPoint) == -1)
    {
      tNode.type = mine.at(tPoint);
      if(tNode.type == Tiles::lift)
      {
        liftReachable = true;
      }
      if(tNode.type == Tiles::lambda) lambdas.push_back({tNode.x, tNode.y});
      if(mine.at(tPoint.x - 1, tPoint.y) != Tiles::wall)
      {
        if(mine.getID(tPoint.x - 1, tPoint.y) != -1)
        {
          tNode.connections.push_back(mine.getID(tPoint.x - 1, tPoint.y));
          nodes[mine.getID(tPoint.x - 1, tPoint.y)].connections.push_back(nodes.size());
          nodes[mine.getID(tPoint.x - 1, tPoint.y)].commands.push_back(Commands::Left);
        }
        else
        {
          expand.push_back({tPoint.x - 1, tPoint.y});
        }
      }
      if(mine.at(tPoint.x + 1, tPoint.y) != Tiles::wall)
      {
        if(mine.getID(tPoint.x + 1, tPoint.y) != -1)
        {
          tNode.connections.push_back(mine.getID(tPoint.x + 1, tPoint.y));
          nodes[mine.getID(tPoint.x + 1, tPoint.y)].connections.push_back(nodes.size());
          nodes[mine.getID(tPoint.x + 1, tPoint.y)].commands.push_back(Commands::Right);
        }
        else
        {
          expand.push_back({tPoint.x + 1, tPoint.y});
        }
      }
      if(mine.at(tPoint.x, tPoint.y - 1) != Tiles::wall)
      {
        if(mine.getID(tPoint.x, tPoint.y - 1) != -1)
        {
          tNode.connections.push_back(mine.getID(tPoint.x, tPoint.y - 1));
          nodes[mine.getID(tPoint.x, tPoint.y - 1)].connections.push_back(nodes.size());
          nodes[mine.getID(tPoint.x, tPoint.y - 1)].commands.push_back(Commands::Up);
        }
        else
        {
          expand.push_back({tPoint.x, tPoint.y - 1});
        }
      }
      if(mine.at(tPoint.x, tPoint.y + 1) != Tiles::wall)
      {
        if(mine.getID(tPoint.x, tPoint.y + 1) != -1)
        {
          tNode.connections.push_back(mine.getID(tPoint.x, tPoint.y + 1));
          nodes[mine.getID(tPoint.x, tPoint.y + 1)].connections.push_back(nodes.size());
          nodes[mine.getID(tPoint.x, tPoint.y + 1)].commands.push_back(Commands::Down);
        }
        else
        {
          expand.push_back({tPoint.x, tPoint.y + 1});
        }
      }
      mine.setID(tPoint, (int)nodes.size());
      nodes.push_back(tNode);
    }
  }
  std::cout << "NodeMap built" << std::endl;
  std::cout << "Node Count: " << nodes.size() << std::endl;
}