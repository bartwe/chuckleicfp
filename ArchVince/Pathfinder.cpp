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
  water = nodeMap.water;
  waterproof = nodeMap.waterproof;
  flooding = nodeMap.flooding;
  for(int i = 0; i < 9; i++)
  {
    targets[i] = mine.targets[i];
    if(mine.targetsPos[i].x != -1)
    {
      targetsIDs[i] = (mine.getID(mine.targetsPos[i]));
    }
    else
      targetsIDs[i] = (-1);

    if(mine.tramps[i].x != -1)
    {
      tramps[i] = (mine.getID(mine.tramps[i]));
    }
    else
      tramps[i] = (-1);
  }
}

std::string Pathfinder::findPath(Point start, Point end)
{
  return findPath(mine.getID(start), mine.getID(end));
}

std::string Pathfinder::findPath(int start, int end)
{
  searchState bestScorer;
  int bestScore = 0;
  clearStoredNodes();
  searchState startState;
  startState.id = start;
  for(int l = 0; l < (int)lambdas.size(); l++)
    startState.lambdas.push_back(mine.getID(lambdas[l]));
  startState.moveCost = 0;
  int startTarget = getTarget(startState, start);
  startState.cost = manhattan(nodes[start].x, nodes[start].y, nodes[startTarget].x, nodes[startTarget].y);
  startState.score = 0;
  openNodes.push_back(startState);
  while(openNodes.size() > 0)
  {
    int bestIndex = getLowest(openNodes);
    searchState ss = openNodes[bestIndex];
    if(ss.score - (int)ss.commands.size() > bestScore)
    {
      bestScore = ss.score - (int)ss.commands.size();
      bestScorer = ss;
    }
    if(isFlooded(ss))
    {
      openNodes.erase(openNodes.begin() + bestIndex, openNodes.begin() + bestIndex + 1);
      while((int)ss.similar.size() > 0)
      {
        openNodes.push_back(stateFromSimilar(ss, ss.similar.back()));
        ss.similar.pop_back();
      }
    }
    ss = processSimilar(ss, bestIndex);
    bestIndex = getLowest(openNodes);
    //std::cout << openNodes.size() << " " << ss.commands << std::endl;
    int target = getTarget(ss, ss.id);
    if(ss.id == target)
    {
      if(ss.id == end)
      {
        if(3 * ss.score  - (int)ss.commands.size() > bestScore)
        {
          std::cout << "Score: " << 3 * ss.score  - (int)ss.commands.size() << std::endl;
          return ss.commands;
        }
        std::cout << "Score: " << 2 * bestScorer.score  - (int)bestScorer.commands.size() << std::endl;
        return bestScorer.commands + "A";
      }
      int toRemove = -1;
      for(int i = 0; i < (int)ss.lambdas.size(); i++)
      {
        if(ss.lambdas[i] == ss.id)
          toRemove = i;
      }
      clearStoredNodes();
      ss.lambdas.erase(ss.lambdas.begin() + toRemove, ss.lambdas.begin() + toRemove + 1);
      ss.score += 25;
      ss.moveCost = 0;
      int target = getTarget(ss, ss.id);
      ss.cost = manhattan(nodes[ss.id].x, nodes[ss.id].y, nodes[target].x, nodes[target].y);
      //openNodes.erase(openNodes.begin() + bestIndex, openNodes.begin() + bestIndex + 1);
      openNodes.push_back(ss);
      continue;
    }
    for(int i = 0; i < (int)nodes[ss.id].connections.size(); i++)
    {
      int conn = nodes[ss.id].connections[i];
      if(isLegal(ss, conn))
      {
        searchState connSS;
        connSS.id = conn;
        for(int s = 0; s < (int)ss.steps.size(); s++)
          connSS.steps.push_back(ss.steps[s]);
        for(int l = 0; l < (int)ss.lambdas.size(); l++)
          connSS.lambdas.push_back(ss.lambdas[l]);
        if(getType(ss, conn) == Tiles::rock)
        {
          updStep step1, step2;
          step1.id = conn;
          step1.type = Tiles::empty;
          step2.type = Tiles::rock;
          if(nodes[conn].x < nodes[ss.id].x)
            step2.id = mine.getID(nodes[conn].x - 1, nodes[conn].y);
          else if(nodes[conn].x > nodes[ss.id].x)
            step2.id = mine.getID(nodes[conn].x + 1, nodes[conn].y);
          else
            std::cout << "WARNING: Illegal move with rock" << std::endl;
          connSS.steps = applyUpd(connSS.steps, connSS, step1);
          connSS.steps = applyUpd(connSS.steps, connSS, step2);
        }
        else if(getType(ss, conn) == Tiles::lambda || getType(ss, conn) == Tiles::earth)
        {
          updStep step1;
          step1.id = conn;
          step1.type = Tiles::empty;
          connSS.steps = applyUpd(connSS.steps, connSS, step1);
        }
        else if(isTramp(getType(ss, conn)))
        {
          connSS = jump(connSS);
        }
        connSS.steps = stepUpdMap(connSS);
        for(int j = 0; j < (int)connSS.steps.size(); j++)
        {
          if(getType(connSS, connSS.steps[j].id) == Tiles::rock && getType(ss, connSS.steps[j].id) != getType(connSS, connSS.steps[j].id))
          {
            while((int)ss.similar.size() > 0)
            {
              openNodes.push_back(stateFromSimilar(ss, ss.similar.back()));
              ss.similar.pop_back();
            }
          }
        }
        if((int)connSS.steps.size() > 0 && connSS.steps[0].id == -1)
          continue;

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
            connSS.similar.insert(connSS.similar.end(), ss.similar.begin(), ss.similar.end());
            openNodes.push_back(connSS);
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
            openNodes[pos].similar.insert(openNodes[pos].similar.end(), ss.similar.begin(), ss.similar.end());
          }
        }
      }
    }
    openNodes.erase(openNodes.begin() + bestIndex, openNodes.begin() + bestIndex + 1);
    closedNodes.push_back(ss);
  }
  std::cout << "Score: " << 2 * bestScorer.score  - (int)bestScorer.commands.size() << std::endl;
  return bestScorer.commands + "A";
}

bool Pathfinder::isFlooded(searchState ss)
{
  int waterLevel = mine.height - water;
  int submerged;
  int robotY = robot.y;
  for(int i = 0; i < (int)ss.commands.size(); i++)
  {
    if(ss.commands.at(i) == 'D') robotY++;
    else if (ss.commands.at(i) == 'U') robotY--;
    if(flooding != 0 && i != 0 && i % flooding == 0) waterLevel++;
    if(robotY > waterLevel) submerged++;
    else submerged = 0;
    if(submerged > waterproof) return true;
  }
  return false;
}

Pathfinder::searchState Pathfinder::jump(searchState ss)
{
  int num = -1;
  if(getType(ss, ss.id, false) == Tiles::a) num = 0;
  if(getType(ss, ss.id, false) == Tiles::b) num = 1;
  if(getType(ss, ss.id, false) == Tiles::c) num = 2;
  if(getType(ss, ss.id, false) == Tiles::d) num = 3;
  if(getType(ss, ss.id, false) == Tiles::e) num = 4;
  if(getType(ss, ss.id, false) == Tiles::f) num = 5;
  if(getType(ss, ss.id, false) == Tiles::g) num = 6;
  if(getType(ss, ss.id, false) == Tiles::h) num = 7;
  if(getType(ss, ss.id, false) == Tiles::i) num = 8;
  if(num != -1)
  {
    ss.steps = applyUpd(ss.steps, ss, {ss.id, Tiles::empty});
    ss.id = targetsIDs[targets[num]];
    ss.steps = applyUpd(ss.steps, ss, {ss.id, Tiles::empty});
    for(int i = 0; i < 9; i++)
    {
      if(i != num && targets[i] == targets[num])
      {
        ss.steps = applyUpd(ss.steps, ss, {tramps[i], Tiles::empty});
      }
    }
  }
  return ss;
}

Pathfinder::searchState Pathfinder::stateFromSimilar(searchState ss, similarState sim)
{
  searchState toRet;
  toRet.commands = sim.replaceCommand + ss.commands.substr(sim.replaceLength);
  toRet.cost = ss.cost + sim.addedCost;
  toRet.id = ss.id;
  toRet.lambdas = ss.lambdas;
  toRet.moveCost = ss.moveCost + sim.addedCost;
  toRet.score = ss.score;
  sim.addedSteps.insert(sim.addedSteps.end(), ss.steps.begin(), ss.steps.end());
  toRet.steps = sim.addedSteps;
  return toRet;
}

Pathfinder::searchState Pathfinder::processSimilar(searchState ss, int index)
{
  std::vector< int > similar = getSimilar(ss, index);
  std::vector< similarState > similars = ss.similar;
  for(int i = 0; i < (int)similar.size(); i++)
  {
    similarState toAdd;
    toAdd.addedCost = openNodes[similar[i]].cost - ss.cost;
    toAdd.addedSteps = fixMismatch(ss, openNodes[similar[i]]);
    toAdd.replaceCommand = openNodes[similar[i]].commands;
    toAdd.replaceLength = (int)ss.commands.size();
    similars.push_back(toAdd);
    for(int j = 0; j < (int)openNodes[similar[i]].similar.size(); j++)
    {
      similarState childAdd;
      childAdd.addedCost = toAdd.addedCost + openNodes[similar[i]].similar[j].addedCost;
      childAdd.addedSteps = openNodes[similar[i]].similar[j].addedSteps;
      childAdd.addedSteps.insert(childAdd.addedSteps.end(), toAdd.addedSteps.begin(), toAdd.addedSteps.end());
      childAdd.replaceCommand = openNodes[similar[i]].similar[j].replaceCommand + toAdd.replaceCommand.substr(openNodes[similar[i]].similar[j].replaceLength);
      childAdd.replaceLength = toAdd.replaceLength;
      similars.push_back(childAdd);
    }
  }
  ss.similar = similars;
  for(int i = 0; i < (int)similar.size(); i++)
  {
    openNodes.erase(openNodes.begin() + similar[i] - i, openNodes.begin() + similar[i] - i + 1);
  }
  return ss;
}

std::vector< int > Pathfinder::getSimilar(searchState ss, int index)
{
  std::vector< int > similar;
  for(int i = 0; i < (int)openNodes.size(); i++)
  {
    if(ss.id == openNodes[i].id && vectorMatch(openNodes[i].lambdas, ss.lambdas) && i != index)
    {
      bool match = true;
      std::vector< updStep > steps = fixMismatch(ss, openNodes[i]);
      for(int j = 0; j < (int)steps.size(); j++)
      {
        if(steps[j].type == Tiles::rock)
        {
          match = false;
          break;
        }
      }
      if(match)
      {
        steps = fixMismatch(openNodes[i], ss);
        for(int j = 0; j < (int)steps.size(); j++)
        {
          if(steps[j].type == Tiles::rock)
          {
            match = false;
            break;
          }
        }
      }
      if(match) similar.push_back(i);
    }
  }
  return similar;
}

std::vector< Pathfinder::updStep > Pathfinder::fixMismatch(searchState s, searchState m)
{
  std::vector< updStep > steps;
  for(int i = 0; i < (int)s.steps.size(); i++)
  {
    if(getType(s, s.steps[i].id) != getType(m, s.steps[i].id))
      steps.push_back({s.steps[i].id, getType(m, s.steps[i].id)});
  }
  for(int i = 0; i < (int)m.steps.size(); i++)
  {
    if(getType(s, m.steps[i].id) != getType(m, m.steps[i].id))
    {
      bool found = false;
      for(int j = 0; j < (int)steps.size(); j++)
      {
        if(m.steps[i].id == steps[j].id)
        {
          found = true;
          break;
        }
      }
      if(!found) steps.push_back({m.steps[i].id, getType(m, m.steps[i].id)});
    }
  }

  return steps;
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
          if(key.steps[h].id == list[i].steps[j].id && (key.steps[h].type == list[i].steps[j].type || key.steps[h].type != Tiles::rock))
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
  bool check[9];
  for(int i = 0; i < 9; i++) check[i] = false;
  return getTarget(ss, id, check);
}

int Pathfinder::getTarget(searchState ss, int id, bool checkTargets[9])
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

  bool lambda = true;
  for(int i = 0; i < 9; i++)
  {
    if(tramps[i] != -1)
    {
      if(!checkTargets[targets[i]] && isTramp(getType(ss, tramps[i])))
      {
        checkTargets[targets[i]] = true;
        int dist = manhattan(nodes[tramps[i]].x, nodes[tramps[i]].y, nodes[id].x, nodes[id].y);
        int target = getTarget(ss, targetsIDs[targets[i]], checkTargets);
        dist += manhattan(nodes[targetsIDs[targets[i]]].x, nodes[targetsIDs[targets[i]]].y, nodes[target].x, nodes[target].y);
        checkTargets[targets[i]] = false;
        if(dist < lowestDist)
        {
          lowestDist = dist;
          lambda = false;
          lowestIndex = i;
        }
      }
    }
  }
  if(lambda) return ss.lambdas[lowestIndex];
  return tramps[lowestIndex];
}

bool Pathfinder::isTramp(Tiles type)
{
  return (type == Tiles::a || type == Tiles::b || type == Tiles::c || type == Tiles::d || type == Tiles::e || type == Tiles::f || type == Tiles::g || type == Tiles::h || type == Tiles::i);
}

void Pathfinder::clearStoredNodes()
{
  openNodes.erase(openNodes.begin(), openNodes.end());
  closedNodes.erase(closedNodes.begin(), closedNodes.end());
}

std::vector< Pathfinder::updStep > Pathfinder::applyUpd(std::vector< updStep > steps, searchState ss, updStep step1)
{
  ss.steps = steps;
  if(step1.type == Tiles::rock && step1.id == mine.getID(nodes[ss.id].x, nodes[ss.id].y - 1))
  {
    std::vector< Pathfinder::updStep > toRet;
    toRet.push_back({-1, Tiles::robot});
    return toRet;
  }
  bool found = false;
  for(int i = 0; i < (int)steps.size(); i++)
  {
    if(steps[i].id == step1.id)
    {
      steps[i].type = step1.type;
      found = true;
      break;
    }
  }
  if(!found)steps.push_back(step1);
  return steps;
}

std::vector< Pathfinder::updStep > Pathfinder::stepUpdMap(searchState ss)
{
  std::vector< updStep > steps = ss.steps;
  for(int i = 0; i < (int)nodes.size(); i++)
  {
    if(getType(ss, i) == Tiles::rock)
    {
      int belowID = mine.getID(nodes[i].x, nodes[i].y + 1);
      if(belowID != -1)
      {
        if(getType(ss, belowID) == Tiles::empty)
        {
          steps = applyUpd(steps, ss, {belowID, Tiles::rock});
          steps = applyUpd(steps, ss, {i, Tiles::empty});
          continue;
        }
        int belowRightID = mine.getID(nodes[i].x + 1, nodes[i].y + 1);
        if(belowRightID != -1)
        {
          if(getType(ss, belowID) == Tiles::lambda && getType(ss, belowRightID) == Tiles::empty)
          {
            steps = applyUpd(steps, ss, {belowRightID, Tiles::rock});
            steps = applyUpd(steps, ss, {i, Tiles::empty});
            continue;
          }
          if(getType(ss, belowID) == Tiles::rock && getType(ss, belowRightID) == Tiles::empty)
          {
            steps = applyUpd(steps, ss, {belowRightID, Tiles::rock});
            steps = applyUpd(steps, ss, {i, Tiles::empty});
            continue;
          }
          if(getType(ss, belowID) == Tiles::rock && getType(ss, belowRightID) == Tiles::rock)
          {
            int belowLeftID = mine.getID(nodes[i].x - 1, nodes[i].y + 1);
            if(belowLeftID != -1 && getType(ss, belowLeftID) == Tiles::empty)
            {
              steps = applyUpd(steps, ss, {belowLeftID, Tiles::rock});
              steps = applyUpd(steps, ss, {i, Tiles::empty});
              continue;
            }
          }
        }
      }
    }
  }
  return steps;
}

Tiles Pathfinder::getType(searchState ss, int id, bool r)
{
  if(r && id == ss.id) return Tiles::robot;
  for(int i = 0; i < (int) ss.steps.size(); i++)
  {
    if(ss.steps[i].id == id)
      return ss.steps[i].type;
  }
  return nodes[id].type;
}

bool Pathfinder::isLegal(searchState ss, int cid)
{
  Tiles type = getType(ss, cid);
  switch(type)
  {
    case Tiles::empty:
      return true;
    case Tiles::lambda:
      return true;
    case Tiles::earth:
      return true;
    case Tiles::wall:
      std::cout << "WARNING: Connection to wall encountered" << std::endl;
      return false;
    case Tiles::lift:
      return liftStatus(ss);
    case Tiles::rock:
      if(nodes[cid].x > nodes[ss.id].x)
        return getType(ss, mine.getID(nodes[cid].x + 1, nodes[cid].y)) == Tiles::empty;
      if(nodes[cid].x < nodes[ss.id].x)
        return getType(ss, mine.getID(nodes[cid].x - 1, nodes[cid].y)) == Tiles::empty;
      return false;
    case Tiles::a:
      return true;
    case Tiles::b:
      return true;
    case Tiles::c:
      return true;
    case Tiles::d:
      return true;
    case Tiles::e:
      return true;
    case Tiles::f:
      return true;
    case Tiles::g:
      return true;
    case Tiles::h:
      return true;
    case Tiles::i:
      return true;
    case Tiles::t1:
      return false;
    case Tiles::t2:
      return false;
    case Tiles::t3:
      return false;
    case Tiles::t4:
      return false;
    case Tiles::t5:
      return false;
    case Tiles::t6:
      return false;
    case Tiles::t7:
      return false;
    case Tiles::t8:
      return false;
    case Tiles::t9:
      return false;
    default:
      return false;
  }
}
