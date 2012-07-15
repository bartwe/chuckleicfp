#pragma once

namespace Best {
  void reserveSpace(int maxcommandlength);

  bool isImprovement(int score);
  void improveSolution(int newscore, const char* newsolution);

  char* getBest();
  int getBestScore();
}
