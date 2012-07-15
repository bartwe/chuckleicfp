#include "Mine.h"

Mine::Mine(std::vector<std::string> rows)
{
  width = 0;
  height = rows.size();
  for(int i = 0; i < (int)rows.size(); i++)
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
