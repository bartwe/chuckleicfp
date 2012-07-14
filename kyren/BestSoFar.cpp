#include "BestSoFar.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

static char* dabest;
static char* alternate;
static int MAXLEN = 0;

static void ourhandler(int signum)
{
	printf("%s\n", Best::GetBest());
	exit(0);
}

namespace Best {
void ReserveSpace(int maxcommandlength)
{
	MAXLEN = maxcommandlength;
	dabest = (char*)malloc(MAXLEN+1);
	alternate = (char*)malloc(MAXLEN+1);
	memcpy(dabest, "A", 2);

	// Now register
	signal(SIGINT, ourhandler);
}
void ImproveSolution(char* newsolution)
{
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
char* GetBest()
{
	return dabest;
}

void GiveUp()
{
	ourhandler(SIGINT);
}

}
