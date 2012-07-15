#pragma once

// Utility class to (in an atomic / thread safe way) update a best solution and score.
class Best {
public:
  static int const DefaultMaxCommand = 1024 * 1024 * 4;
  Best(int maxCommandLength = DefaultMaxCommand);

  bool isImprovement(int score);
  void improveSolution(int newscore, const char* newsolution);

  char* getBest();
  int getBestScore();

private:
  int MAXLEN;
  int BESTSCORE;
  char* dabest;
  char* alternate;
};
