#include "BestSoFar.hpp"

#include <cstring>
#include <cstdlib>
#include <cstdio>

static int BESTSCORE = -1000000000;
static char* dabest;
static char* alternate;
static int MAXLEN = 0;

namespace Best {

void reserveSpace(int maxcommandlength) {
	// Only init just once
	if ( MAXLEN != 0 ) return;

	MAXLEN = maxcommandlength;
	dabest = (char*)malloc(MAXLEN+1);
	alternate = (char*)malloc(MAXLEN+1);
	memcpy(dabest, "A", 2);
}

bool isImprovement(int score) {
	return score > BESTSCORE;
}

void improveSolution(int score, const char* newsolution) {
	if (!isImprovement(score)) return;
	BESTSCORE = score;

	int cmdlen = strlen(newsolution);
	if ( cmdlen > MAXLEN ) cmdlen = MAXLEN;

	memcpy(alternate, newsolution, cmdlen);
	alternate[cmdlen] = 0;
	char* prev = dabest;

	__asm__ __volatile__ ("" ::: "memory");
	dabest = alternate;
	alternate = prev;
	__asm__ __volatile__ ("" ::: "memory");
}

char* getBest() {
	return dabest;
}

int getBestScore() {
  return BESTSCORE;
}

}
