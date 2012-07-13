#include "Searcher.hpp"

int main(int argc, char** argv) {
  Mine mine;
  mine.read(std::cin);

  Searcher searcher(mine);;
  auto pair = searcher.bruteForce(13);
  std::cout << pair.first << " " << pair.second << std::endl;

  return 0;
}
