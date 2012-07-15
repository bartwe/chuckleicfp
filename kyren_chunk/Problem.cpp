#include "Problem.hpp"

std::shared_ptr<Problem> Problem::read(std::istream& is) {
  auto problem = std::make_shared<Problem>();

  std::vector<std::vector<Tile>> readContent;
  int maxRow = 0;
  std::string line;
  while (std::getline(is, line)) {
    if (line.size() == 0) break;

    std::vector<Tile> row;
    row.resize(line.size());
    memcpy(&row[0], &line[0], line.size());

    maxRow = std::max<int>(maxRow, row.size());

    readContent.push_back(std::move(row));
  }

  problem->water.initWaterLevel = 0;
  problem->water.floodingFreq = 0;
  problem->water.waterproof = 10;

  problem->beard.growthrate = 25;
  problem->beard.initRazors = 0;

  while (std::getline(is, line)) {
    char* cline = (char*)line.c_str();
    char* eocmd = strchr(cline, ' ');
    if (!eocmd) continue;

    const char* command = cline;
    *eocmd = 0;
    const char* parameter = eocmd + 1;

    if (strcmp(command, "Water") == 0) {
      problem->water.initWaterLevel = atoi(parameter);
    } else if (strcmp(command, "Flooding") == 0) {
      problem->water.floodingFreq = atoi(parameter);
    } else if (strcmp(command, "Waterproof") == 0) {
      problem->water.waterproof = atoi(parameter);
    } else if (strcmp(command, "Growth") == 0) {
      problem->beard.growthrate = atoi(parameter);
    } else if (strcmp(command, "Razors") == 0) {
      problem->beard.initRazors = atoi(parameter);
    } else if (strcmp(command, "Trampoline") == 0) {
      problem->trampMapping.push_back({tileFromChar(parameter[0]), tileFromChar(parameter[strlen(parameter)-1])});
    }
  }

  problem->width = maxRow;
  problem->height = readContent.size();

  problem->tiles.reset(problem->width, problem->height, Tile::Empty);
  // Turn upside down so (0, 0) is bottom left
  std::reverse(readContent.begin(), readContent.end());

  problem->numInitialLambdas = 0;

  memset(problem->initialTileHistogram, 0, sizeof problem->initialTileHistogram);

  for (int i = 0; i < readContent.size(); ++i) {
    auto const& row = readContent[i];
    for (int j = 0; j < row.size(); ++j) {
      auto c = row[j];
      if (c == Tile::Robot) {
        problem->robotX = j;
        problem->robotY = i;
      } else if (c == Tile::ClosedLift) {
        problem->liftLoc.push_back({j, i});
      } else if (c == Tile::Lambda || c == Tile::HigherOrderRock) {
	      problem->numInitialLambdas++;
      } else if (c >= Tile::TrampolineA && c <= Tile::TrampolineI) {
        int index = problem->indexOfTrampTarget(c);
        if ((int)c >= problem->trampLoc.size()) {
          problem->trampLoc.resize(index+1);
        }
        problem->trampLoc[index] = {j, i};
      } else if (c >= Tile::Target1 && c <= Tile::Target9) {
        int index = problem->indexOfTrampTarget(c);
        if ((int)c >= problem->targetLoc.size()) {
          problem->targetLoc.resize(index+1);
        }
        problem->targetLoc[index] = {j, i};
      }

      problem->tiles.at(j, i) = c;

      if (rockType(c) || c == Tile::Beard)
        problem->rockBeardPositions.insert({j, i});

      problem->initialTileHistogram[charFromTile(c)]++;
    }
  }

  assert(problem->getTileCount(Tile::Robot) == 1);
  assert(problem->getTileCount(Tile::ClosedLift) == 1);
  assert(problem->getTileCount(Tile::OpenLift) == 0);
  assert(problem->getTileCount(Tile::Lambda) == problem->numInitialLambdas);

  return problem;
}

int Problem::getTileCount(Tile c) const {
  return initialTileHistogram[charFromTile(c)];
}

int Problem::indexOfTrampTarget(Tile c) const {
  if (c >= Tile::TrampolineA && c <= Tile::TrampolineI)
    return (int)c - (int)Tile::TrampolineA;
  else if (c >= Tile::Target1 && c <= Tile::Target9)
    return (int)c - (int)Tile::Target1;
  else
    return -1; //error
}

Tile Problem::getTargetForTramp(Tile c) const {
  for (auto i : trampMapping) {
    if (i.first == c) {
      return i.second;
    }
  }
  return Tile::Empty;
}

std::vector<Position> Problem::getTrampLocsForTarget(Tile c) const {
  std::vector<Position> res;
  for (auto i : trampMapping) {
    if (i.second == c) {
      res.push_back(trampLoc[indexOfTrampTarget(i.first)]);
    }
  }
  return res;
}

