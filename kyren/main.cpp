#include "Searcher.hpp"

int main(int argc, char** argv) {
  Mine mine;
  mine.read(std::cin);

  Searcher searcher(mine);;
  auto result = searcher.bruteForce(12);
  mine.evaluateAndPrint(result.commands);

  return 0;
}
