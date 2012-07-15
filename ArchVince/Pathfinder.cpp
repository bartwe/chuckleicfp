#include "Pathfinder.h"

Pathfinder::Pathfinder(Mine tmine) : mine(tmine)
{
  NodeMap nodeMap(tmine);
  mine = nodeMap.mine;
  robot = nodeMap.robot;
  lift = nodeMap.lift;
  lambdas = nodeMap.lambdas;
  liftReachable = nodeMap.liftReachable;
  nodes = nodeMap.nodes;
}

std::string Pathfinder::findPath(Point start, Point end)
{
  return findPath(mine.getID(start), mine.getID(end));
}

std::string Pathfinder::findPath(int start, int end)
{
  clearStoredNodes();
  searchState startState;
  startState.id = start;
  for(int l = 0; l < (int)lambdas.size(); l++)
    startState.lambdas.push_back(mine.getID(lambdas[l]));
  startState.moveCost = 0;
  int startTarget = getTarget(startState, start);
  startState.cost = manhattan(nodes[start].x, nodes[start].y, nodes[startTarget].x, nodes[startTarget].y);
  openNodes.push_back(startState);
  while(openNodes.size() > 0)
  {
    int bestIndex = getLowest(openNodes);
    searchState ss = openNodes[bestIndex];
    int target = getTarget(ss, ss.id);
    if(ss.id == target)
    {
      if(ss.id == end)
      {
        std::cout << "Score: " << 3 * ss.score - (int)ss.commands.size() << std::endl;
        return ss.commands;
      }
      int toRemove = -1;
      for(int i = 0; i < (int)ss.lambdas.size(); i++)
      {
        if(ss.lambdas[i] == ss.id)
          toRemove = i;
      }
      ss.lambdas.erase(ss.lambdas.begin() + toRemove, ss.lambdas.begin() + toRemove + 1);
      ss.score += 25;
      ss.moveCost = 0;
      int target = getTarget(ss, ss.id);
      ss.cost = manhattan(nodes[ss.id].x, nodes[ss.id].y, nodes[target].x, nodes[target].y);
      clearStoredNodes();
      openNodes.push_back(ss);
    }
    for(int i = 0; i < (int)nodes[ss.id].connections.size(); i++)
    {
      int conn = nodes[ss.id].connections[i];
      searchState connSS;
      connSS.id = conn;
      for(int s = 0; s < (int)ss.steps.size(); s++)
        connSS.steps.push_back(ss.steps[s]);
      for(int l = 0; l < (int)ss.lambdas.size(); l++)
        connSS.lambdas.push_back(ss.lambdas[l]);
      int pos = contains(openNodes, connSS);
      if(pos == -1)
      {
        if(contains(closedNodes, connSS) == -1)
        {
          connSS.commands = ss.commands + nodes[ss.id].commands[i];
          connSS.moveCost = ss.moveCost + 1;
          connSS.score = ss.score;
          int target = getTarget(ss, conn);
          connSS.cost = connSS.moveCost + manhattan(nodes[conn].x, nodes[conn].y, nodes[target].x, nodes[target].y);
          pushOpen(connSS);
        }
      }
      else
      {
        if(openNodes[pos].moveCost > ss.moveCost + 1)
        {
          openNodes[pos].moveCost = ss.moveCost + 1;
          int target = getTarget(ss, conn);
          openNodes[pos].cost = connSS.moveCost + manhattan(nodes[conn].x, nodes[conn].y, nodes[target].x, nodes[target].y);
          openNodes[pos].commands = ss.commands + nodes[ss.id].commands[i];
        }
      }
    }
    openNodes.erase(openNodes.begin() + bestIndex, openNodes.begin() + bestIndex + 1);
    closedNodes.push_back(ss);
  }
  return "A";
}

std::string Pathfinder::getSteps()
{
  return findPath(robot, lift);
}

bool Pathfinder::liftStatus(searchState ss)
{
  return ss.lambdas.size() == 0;
}

int Pathfinder::getLowest(std::vector< searchState > nodeList)
{
  int lowestCost = nodeList[0].cost;
  int lowestIndex = 0;
  for(int i = 1; i < (int)nodeList.size(); i++)
  {
    if(nodeList[i].cost < lowestCost)
    {
      lowestCost = nodeList[i].cost;
      lowestIndex = i;
    }
  }
  return lowestIndex;
}

int Pathfinder::manhattan(int x1, int y1, int x2, int y2)
{
  return std::abs(x2 - x1) + std::abs(y2 - y1);
}

int Pathfinder::manhattan(Point p1, Point p2)
{
  return std::abs(p1.x - p2.x) + std::abs(p1.y - p2.y);
}

int Pathfinder::contains(std::vector< searchState > list, searchState key)
{
  for(int i = 0; i < (int)list.size(); i++)
  {
    if(list[i].id == key.id && list[i].steps.size() == key.steps.size())
    {
      bool match = true;
      for(int j = 0; j < (int)list[i].steps.size(); j++)
      {
        bool found = false;
        for(int h = 0; h < (int)key.steps.size(); h++)
        {
          if(key.steps[h].id == list[i].steps[j].id && key.steps[h].type == list[i].steps[j].type)
          {
            found = true;
            h = (int)key.steps.size();
          }
        }
        if(!found) match = false;
      }
      if(match && vectorMatch(list[i].lambdas, key.lambdas)) return i;
    }
  }
  return -1;
}

bool Pathfinder::vectorMatch(std::vector< int > v, std::vector< int > w)
{
  for(int i = 0; i < (int)v.size(); i++)
  {
    bool found = false;
    for(int j = 0; j < (int)w.size(); j++)
    {
      if(w[j] == v[i])
      {
        found = true;
        j = (int)w.size();
      }
    }
    if(!found) return false;
  }
  return true;
}

int Pathfinder::pushOpen(searchState ss)
{
  if(contains(openNodes, ss) == -1) openNodes.push_back(ss);
  return 0;
}

int Pathfinder::getTarget(searchState ss, int id)
{
  if(liftStatus(ss))
  {
    if(liftReachable) return mine.getID(lift);
    else return -1;
  }
  int lowestIndex = 0;
  int lowestDist = manhattan(nodes[ss.lambdas[0]].x, nodes[ss.lambdas[0]].y, nodes[id].x, nodes[id].y);
  for(int i = 1; i < (int)ss.lambdas.size(); i++)
  {
    int dist = manhattan(nodes[ss.lambdas[i]].x, nodes[ss.lambdas[i]].y, nodes[id].x, nodes[id].y);
    if(dist < lowestDist)
    {
      lowestDist = dist;
      lowestIndex = i;
    }
  }
  return ss.lambdas[lowestIndex];
}

void Pathfinder::clearStoredNodes()
{
  openNodes.erase(openNodes.begin(), openNodes.end());
  closedNodes.erase(closedNodes.begin(), closedNodes.end());
}

std::vector< Pathfinder::updStep > Pathfinder::stepUpdMap(searchState ss, updStep step)
{
  bool found = false;
  for(int i = 0; i < (int)ss.steps.size(); i++)
  {
    if(ss.steps[i].id == step.id)
    {
      ss.steps[i].type = step.type;
      found = true;
      break;
    }
  }
  if(!found)ss.steps.push_back(step);

  std::vector< updStep > steps;
  for(int i = 0; i < (int)nodes.size(); i++)
  {
    if(getType(ss, i) == Tiles::rock)
    {
      int belowID = mine.getID(nodes[i].x, nodes[i].y + 1);
      if(belowID != -1)
      {
        if(getType(ss, belowID) == Tiles::empty)
        {
          steps.push_back({belowID, Tiles::rock});
          steps.push_back({i, Tiles::empty});
          continue;
        }
        int belowRightID = mine.getID(nodes[i].x + 1, nodes[i].y + 1);
        if(belowRightID != -1)
        {
          if(getType(ss, belowID) == Tiles::lambda && getType(ss, belowRightID) == Tiles::empty)
          {
            steps.push_back({belowRightID, Tiles::rock});
            steps.push_back({i, Tiles::empty});
            continue;
          }
          if(getType(ss, belowID) == Tiles::rock && getType(ss, belowRightID) == Tiles::empty)
          {
            steps.push_back({belowRightID, Tiles::rock});
            steps.push_back({i, Tiles::empty});
            continue;
          }
          if(getType(ss, belowID) == Tiles::rock && getType(ss, belowRightID) == Tiles::rock)
          {
            int belowLeftID = mine.getID(nodes[i].x - 1, nodes[i].y + 1);
            if(belowLeftID != -1 && getType(ss, belowLeftID) == Tiles::empty)
            {
              steps.push_back({belowLeftID, Tiles::rock});
              steps.push_back({i, Tiles::empty});
              continue;
            }
          }
        }
      }
    }
  }
  return steps;
}

Tiles Pathfinder::getType(searchState ss, int id)
{
  for(int i = 0; i < (int) ss.steps.size(); i++)
  {
    if(ss.steps[i].id == id)
      return ss.steps[i].type;
  }
  return nodes[id].type;
}
