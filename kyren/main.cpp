#include "Mine.hpp"

int main(int argc, char** argv) {
  Mine mine;
  mine.read(std::cin);

  // Run sample for contest1.map

  mine.evaluateAndPrint({
      RobotCommand::Down,
      RobotCommand::Left,
      RobotCommand::Left,
      RobotCommand::Left,
      RobotCommand::Down,
      RobotCommand::Down,
      RobotCommand::Right,
      RobotCommand::Right,
      RobotCommand::Right,
      RobotCommand::Left,
      RobotCommand::Up,
      RobotCommand::Left,
      RobotCommand::Left,
      RobotCommand::Down,
      RobotCommand::Left});

  std::cout << std::endl << "now rewinding!" << std::endl << std::endl;

  while (mine.popMove())
    mine.print();
  return 0;
}
