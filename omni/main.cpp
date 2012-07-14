#include "main.h"
#include "timer.hpp"

int main(int argc, char** argv) {
  if (argc != 3)
    return -1;

  std::string executable = argv[1];
  std::string directory = argv[2];

  auto maps = getMaps(directory);
  
  Timer timer;
  for (auto map : maps) {
    tm.start();

    

    tm.stop();
    std::cout << map << " finished, time: " << tm.duration() << std::endl;
  }
}

std::vector<std::string> getMaps(std::string const& directory) {
  DIR *dp;
  struct dirent *dirp;
  if (!(dp = opendir(directory.c_str()))) {
    cout << "Error(" << errno << ") opening " << dir << endl;
    return std::vector<std::string>();
  }
 
  std::vector<std::string> files;
  while (dirp = readdir(dp)) {
    files.push_back(string(dirp->d_name));
  }
  closedir(dp);
  return files;
}
