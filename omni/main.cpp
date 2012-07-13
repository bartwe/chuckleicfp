#include <iostream>
#include <vector>
#include <string>

enum class TileTypes {
  Empty,
  Robot,
  Wall,
  Rock,
  Lambda,
  ClosedLL,
  OpenLL,
  Earth
};

int main() {
  std::vector<std::string> mapStrings;
  std::vector<std::vector<TileTypes>> map;
  std::string tmp;

  while (std::getline(std::cin, tmp)) {
    mapStrings.push_back(tmp);
  }

  for (auto i : mapStrings) {
    for (auto j : i) {
      std::vector<TileTypes> row;
      switch (j) {
        case 'R':
          row.push_back(TileTypes::Robot);
          break;
        case '#':
          row.push_back(TileTypes::Wall);
          break;
        case '*':
          row.push_back(TileTypes::Rock);
          break;
        case '\\':
          row.push_back(TileTypes::Lambda);
          break;
        case 'L':
          row.push_back(TileTypes::ClosedLL);
          break;
        case 'O':
          row.push_back(TileTypes::OpenLL);
          break;
        case '.':
          row.push_back(TileTypes::Earth);
          break;
        case ' ':
          row.push_back(TileTypes::Empty);
          break;
        default:
          throw -1;
      }
      map.push_back(row);
    }
  }
  return 0;
}
