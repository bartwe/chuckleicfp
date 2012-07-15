#include "Mine.h"

Mine::Mine(std::vector<std::string> rows)
{
  width = 0;
  height = 1;
  water = 0;
  flooding = 0;
  waterproof = 0;
  for(int i = 0; i < 9; i++)
  {
    targets[i] = -1;
    tramps[i] = {-1, -1};
    targetsPos[i] = {-1, -1};
  }
  while(height <= (int)rows.size() && rows[height-1] != "")
  {
    height++;
  }
  if(height <= (int)rows.size())
  {
    height--;
    for(int i = height; i < (int)rows.size(); i++)
    {
      if(rows[i].substr(0, 6).compare("Water ") == 0)
        water = atoi(rows[i].substr(6).c_str());
      else if(rows[i].substr(0, 9).compare("Flooding ") == 0)
        water = atoi(rows[i].substr(9).c_str());
      else if(rows[i].substr(0, 11).compare("Waterproof ") == 0)
        water = atoi(rows[i].substr(11).c_str());
      else if(rows[i].substr(0, 11).compare("Trampoline ") == 0)
      {
        if(rows[i].at(11) == 'A') targets[0] = atoi(rows[i].substr(22).c_str());
        else if(rows[i].at(11) == 'B') targets[1] = atoi(rows[i].substr(22).c_str());
        else if(rows[i].at(11) == 'C') targets[2] = atoi(rows[i].substr(22).c_str());
        else if(rows[i].at(11) == 'D') targets[3] = atoi(rows[i].substr(22).c_str());
        else if(rows[i].at(11) == 'E') targets[4] = atoi(rows[i].substr(22).c_str());
        else if(rows[i].at(11) == 'F') targets[5] = atoi(rows[i].substr(22).c_str());
        else if(rows[i].at(11) == 'G') targets[6] = atoi(rows[i].substr(22).c_str());
        else if(rows[i].at(11) == 'H') targets[7] = atoi(rows[i].substr(22).c_str());
        else if(rows[i].at(11) == 'I') targets[8] = atoi(rows[i].substr(22).c_str());
      }
    }
  }
  Tiles trampolines[9];
  trampolines[0] = Tiles::a;
  trampolines[1] = Tiles::b;
  trampolines[2] = Tiles::c;
  trampolines[3] = Tiles::d;
  trampolines[4] = Tiles::e;
  trampolines[5] = Tiles::f;
  trampolines[6] = Tiles::g;
  trampolines[7] = Tiles::h;
  trampolines[8] = Tiles::i;

  for(int i = 0; i < height; i++)
    width = std::max(width, (int)rows[i].length());

  {
    std::vector< Tiles > row;
    std::vector< int > idrow;
    for(int j = 0; j < width; j++)
    {
      idrow.push_back(-1);
      row.push_back(Tiles::wall);
    }
    mine.push_back(row);
    ids.push_back(idrow);
  }

  for(int i = 0; i < height; i++)
  {
    std::vector< Tiles > row;
    std::vector< int > idrow;
    idrow.push_back(-1);
    row.push_back(Tiles::wall);

    for(int j = 0; j < width; j++)
    {
      idrow.push_back(-1);
      if(j >= (int)rows[i].length())
      {
        row.push_back(Tiles::empty);
        continue;
      }
      switch(rows[i][j])
      {
        case 'R':
          row.push_back(Tiles::empty);
          robot = {j, i};
          break;
        case '#':
          row.push_back(Tiles::wall); break;
        case '*':
          row.push_back(Tiles::rock); break;
        case '\\':
          row.push_back(Tiles::lambda);
          lambdas.push_back({j, i});
          break;
        case 'L':
          row.push_back(Tiles::lift);
          lift = {j, i};
          break;
        case '.':
          row.push_back(Tiles::earth); break;
        case ' ':
          row.push_back(Tiles::empty); break;
        case 'A':
          tramps[0] = {j, i};
          row.push_back(Tiles::a); break;
        case 'B':
          tramps[1] = {j, i};
          row.push_back(trampolines[1]); break;
        case 'C':
          tramps[2] = {j, i};
          row.push_back(trampolines[2]); break;
        case 'D':
          tramps[3] = {j, i};
          row.push_back(trampolines[3]); break;
        case 'E':
          tramps[4] = {j, i};
          row.push_back(trampolines[4]); break;
        case 'F':
          tramps[5] = {j, i};
          row.push_back(trampolines[5]); break;
        case 'G':
          tramps[6] = {j, i};
          row.push_back(trampolines[6]); break;
        case 'H':
          tramps[7] = {j, i};
          row.push_back(trampolines[7]); break;
        case 'I':
          tramps[8] = {j, i};
          row.push_back(trampolines[8]); break;
        case '1':
          targetsPos[0] = {j, i};
          row.push_back(Tiles::t1); break;
        case '2':
          targetsPos[1] = {j, i};
          row.push_back(Tiles::t2); break;
        case '3':
          targetsPos[2] = {j, i};
          row.push_back(Tiles::t3); break;
        case '4':
          targetsPos[3] = {j, i};
          row.push_back(Tiles::t4); break;
        case '5':
          targetsPos[4] = {j, i};
          row.push_back(Tiles::t5); break;
        case '6':
          targetsPos[5] = {j, i};
          row.push_back(Tiles::t6); break;
        case '7':
          targetsPos[6] = {j, i};
          row.push_back(Tiles::t7); break;
        case '8':
          targetsPos[7] = {j, i};
          row.push_back(Tiles::t8); break;
        case '9':
          targetsPos[8] = {j, i};
          row.push_back(Tiles::t9); break;
        default:
          std::cout << "Invalid character found in map" << std::endl;
          break;
      }
    }
    idrow.push_back(-1);
    row.push_back(Tiles::wall);
    mine.push_back(row);
    ids.push_back(idrow);
  }

  {
    std::vector< Tiles > row;
    std::vector< int > idrow;
    for(int j = 0; j < width; j++)
    {
      idrow.push_back(-1);
      row.push_back(Tiles::wall);
    }
    mine.push_back(row);
    ids.push_back(idrow);
  }
}

Tiles Mine::at(int x, int y)
{
  return mine[y+1][x+1];
}

Tiles Mine::at(Point p)
{
  return mine[p.y+1][p.x+1];
}

void Mine::setID(int x, int y, int id)
{
  ids[y+1][x+1] = id;
}

void Mine::setID(Point p, int id)
{
  ids[p.y+1][p.x+1] = id;
}

int Mine::getID(int x, int y)
{
  return ids[y+1][x+1];
}

int Mine::getID(Point p)
{
  return ids[p.y+1][p.x+1];
}
