#include <fstream>

#include "Mine.h"
#include "NodeMap.h"
#include "Pathfinder.h"

using namespace std;

int main()
{
  string input = "";

  do
  {
    string filename = "";
    cin >> input;
    filename = input;
    if(filename.substr(0, 1).compare("`") == 0) filename = "C:\\Users\\Mike\\ICFP\\maps\\" + filename.substr(1);
    ifstream mineFile (filename);
    if (mineFile.is_open())
    {
      vector<string> mapList;
      string line = "";
      while (mineFile.good())
      {
        getline (mineFile,line);
        cout << line << endl;
        mapList.push_back(line);
      }
      mineFile.close();
      mapList.pop_back();

      Mine mine (mapList);
      Pathfinder pathfinder(mine);
      std::cout << pathfinder.getSteps() << std::endl;
    }
    else cout << "Unable to open file" << endl;
  }
  while(input.compare(".") != 0);
  return 0;
}
