#include "Searcher.hpp"

int main(int argc, char** argv) {
  Mine mine;
  mine.read(std::cin);

  Searcher searcher(mine);;
  auto result = searcher.bruteForce(24);
  mine.evaluateAndPrint(result.commands);
  std::cout << "solution: " << Mine::commandString(result.commands) << " length: " << result.commands.size() << " score: " << result.score << std::endl;

  return 0;
}
