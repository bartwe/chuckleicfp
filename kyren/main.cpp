#include "Mine.hpp"

int main(int argc, char** argv) {
  Mine mine;
  mine.read(std::cin);

  // Run sample for contest1.map

  mine.evaluateAndPrint({Down, Left, Left, Left, Down, Down, Right, Right, Right, Left, Up, Left, Left, Down, Left});
  return 0;
}
