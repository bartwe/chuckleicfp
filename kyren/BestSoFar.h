

namespace Best {
void ReserveSpace(int maxcommandlength);
bool IsImprovement(int score);
void ImproveSolution(int newscore, const char* newsolution);
char* GetBest();

void GiveUp();
}
