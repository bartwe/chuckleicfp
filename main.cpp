#include <iostream>
#include <deque>
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
  std::deque<std::string> mapStrings;
  std::string tmp;
  while (std::getline(std::cin, tmp)) {
    mapStrings.push_back(tmp);
  }
  return 0;
}
