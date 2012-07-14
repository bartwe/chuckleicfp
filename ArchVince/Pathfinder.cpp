#include "Pathfinder.h"

Pathfinder::Pathfinder(Mine tmine) : mine(tmine)
{
  NodeMap nodeMap(tmine);
  mine = nodeMap.mine;
  robot = nodeMap.robot;
  lift = nodeMap.lift;
  lambdas = nodeMap.lambdas;
  liftReachable = nodeMap.liftReachable;
  for(int i = 0; i < (int)nodeMap.nodes.size(); i++)
    nodes.push_back(mToP(nodeMap.nodes[i]));
}

std::string Pathfinder::findPath(Point start, Point end)
{
  return findPath(mine.getID(start), mine.getID(end));
}

std::string Pathfinder::findPath(int start, int end)
{
  updTargets();
  openNodes.push_back(start);
  nodes[start].moveCost = 0;
  nodes[start].cost = manhattan(nodes[start].x, nodes[start].y, nodes[nodes[start].target].x, nodes[nodes[start].target].y);
  while(openNodes.size() > 0)
  {
    int nodeIndex = getLowest(openNodes);
    if(nodeIndex == nodes[nodeIndex].target)
    {
      if(nodeIndex == end) return nodes[nodeIndex].commandChain;
      int toRemove = -1;
      for(int i = 0; i < (int)lambdas.size(); i++)
      {
        if(mine.getID(lambdas[i]) == nodeIndex)
          toRemove = i;
      }
      lambdas.erase(lambdas.begin() + toRemove, lambdas.begin() + toRemove + 1);
      updTargets();
      openNodes.push_back(nodeIndex);
      nodes[nodeIndex].moveCost = 0;
      nodes[nodeIndex].cost = manhattan(nodes[nodeIndex].x, nodes[nodeIndex].y, nodes[nodes[nodeIndex].target].x, nodes[nodes[nodeIndex].target].y);
    }
    for(int i = 0; i < (int)nodes[nodeIndex].connections.size(); i++)
    {
      int conn = nodes[nodeIndex].connections[i];
      if((nodes[conn].moveCost == -1 || nodes[conn].moveCost > nodes[nodeIndex].moveCost + 1) && !contains(closedNodes, conn))
      {
        nodes[conn].moveCost = nodes[nodeIndex].moveCost + 1;
        nodes[conn].cost = nodes[conn].moveCost + manhattan(nodes[conn].x, nodes[conn].y, nodes[nodes[conn].target].x, nodes[nodes[conn].target].y);
        nodes[conn].commandChain = nodes[nodeIndex].commandChain + nodes[nodeIndex].commands[i];
        if(!contains(openNodes, conn)) pushOpen(conn);
      }
    }
    openNodes.erase(openNodes.begin(), openNodes.begin() + 1);
    closedNodes.push_back(nodeIndex);
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
  return openNodes[lowestIndex];
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

int Pathfinder::pushOpen(int node)
{
  /*int i = 0;
  while(i <= (int)openNodes.size() && nodes[openNodes[i]].cost < nodes[node].cost)
  i++;
  openNodes.insert(openNodes.begin() + i, node);
  return i;*/
  if(!contains(openNodes, node)) openNodes.push_back(node);
  return 0;
}

int Pathfinder::getTarget(int node)
{
  if(liftStatus())
  {
    if(liftReachable) return mine.getID(lift);
    else return -1;
  }
  int lowestIndex = 0;
  int lowestDist = manhattan(lambdas[0].x, lambdas[0].y, nodes[node].x, nodes[node].y);
  for(int i = 1; i < (int)lambdas.size(); i++)
  {
    int dist = manhattan(lambdas[i].x, lambdas[i].y, nodes[node].x, nodes[node].y);
    if(dist < lowestDist)
    {
      lowestDist = dist;
      lowestIndex = i;
    }
  }
  return mine.getID(lambdas[lowestIndex]);
}

void Pathfinder::updTargets()
{
  for(int i = 0; i < (int)nodes.size(); i++)
  {
    nodes[i].target = getTarget(i);
    nodes[i].cost = 0;
    nodes[i].moveCost = -1;
  }
  openNodes.erase(openNodes.begin(), openNodes.end());
  closedNodes.erase(closedNodes.begin(), closedNodes.end());
}
