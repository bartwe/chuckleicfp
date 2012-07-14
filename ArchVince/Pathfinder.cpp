#include "Pathfinder.h"

Pathfinder::Pathfinder(Mine tmine) : mine(tmine)
{
  NodeMap nodeMap(tmine);
  mine = nodeMap.mine;
  for(int i = 0; i < (int)nodeMap.nodes.size(); i++)
    nodes.push_back(mToP(nodeMap.nodes[i]));
}

std::string Pathfinder::findPath(Point start, Point end)
{
  return findPath(mine.getID(start), mine.getID(end));
}

std::string Pathfinder::findPath(int start, int end)
{
  openNodes.push_back(start);
  nodes[start].moveCost = 0;
  nodes[start].cost = manhattan(nodes[start].x, nodes[start].y, nodes[end].x, nodes[end].y);
  while(openNodes.size() > 0)
  {
    int nodeIndex = getLowest(openNodes);
    if(nodeIndex == end) return nodes[nodeIndex].commandChain;
    for(int i = 0; i < (int)nodes[nodeIndex].connections.size(); i++)
    {
      int conn = nodes[nodeIndex].connections[i];
      if((nodes[conn].moveCost == -1 || nodes[conn].moveCost > nodes[nodeIndex].moveCost + 1) && !contains(closedNodes, nodeIndex))
      {
        if(!contains(openNodes, conn)) openNodes.push_back(conn);
        nodes[conn].moveCost = nodes[nodeIndex].moveCost + 1;
        nodes[conn].cost = nodes[conn].moveCost + manhattan(nodes[conn].x, nodes[conn].y, nodes[end].x, nodes[end].y);
        nodes[conn].commandChain = nodes[nodeIndex].commandChain + (char)nodes[nodeIndex].commands[i];
        openNodes.erase(std::remove(openNodes.begin(), openNodes.end(), nodeIndex), openNodes.end());
        closedNodes.push_back(nodeIndex);
      }
    }
  }
  return "A";
}

std::string Pathfinder::getSteps()
{
  return findPath(robot, lift);
}

bool Pathfinder::liftStatus()
{
  return lambdas.size() == 0;
}

Pathfinder::PNode Pathfinder::mToP(NodeMap::MNode m)
{
  PNode p;
  p.commands = m.commands;
  p.connections = m.connections;
  p.x = m.x;
  p.y = m.y;
  p.commandChain = "";
  p.moveCost = -1;
  return p;
}

int Pathfinder::getLowest(std::vector< int > nodeList)
{
  int lowestCost = nodes[nodeList[0]].cost;
  int lowestIndex = 0;
  for(int i = 1; i < (int)nodeList.size(); i++)
  {
    if(nodes[nodeList[i]].cost < lowestCost)
    {
      lowestCost = nodes[nodeList[i]].cost;
      lowestIndex = i;
    }
  }
  return lowestIndex;
}

int Pathfinder::manhattan(int x1, int y1, int x2, int y2)
{
  return std::abs(x2 - x1) + std::abs(y2 - y1);
}

bool Pathfinder::contains(std::vector< int > list, int key)
{
  for(int i = 0; i < (int)list.size(); i++)
  {
    if(list[i] == key)
      return true;
  }
  return false;
}
