#include "Best.hpp"

#include <cstring>
#include <cstdlib>
#include <cstdio>

Best::Best(int maxcommandlength) {
  MAXLEN = maxcommandlength;
  BESTSCORE = -1000000000;

	dabest = (char*)malloc(MAXLEN+1);
	alternate = (char*)malloc(MAXLEN+1);
	memcpy(dabest, "A", 2);
}

bool Best::isImprovement(int score) {
	return score > BESTSCORE;
}

void Best::improveSolution(int score, const char* newsolution) {
	if (!isImprovement(score)) return;
	BESTSCORE = score;

	int cmdlen = strlen(newsolution);
	if (cmdlen > MAXLEN) cmdlen = MAXLEN;

	memcpy(alternate, newsolution, cmdlen);
	alternate[cmdlen] = 0;
	char* prev = dabest;

	__asm__ __volatile__ ("" ::: "memory");
	dabest = alternate;
	alternate = prev;
	__asm__ __volatile__ ("" ::: "memory");
}

char* Best::getBest() {
	return dabest;
}

int Best::getBestScore() {
  return BESTSCORE;
}
