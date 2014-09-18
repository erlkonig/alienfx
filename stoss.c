/* stoss.c : tokenize a char* into char** (splat to splat-splat) */

static const char *Version[] = {
	"@(#) stoss.c Copyright (c) 1998 C. Alex. North-Keys",
	"$Grueppe: Talisman $",
	"$Anfang: 1998-12-25 09:21:34 GMT (Dec Fri) 914577694 $",
	"$Compiliert: "__DATE__" " __TIME__ " $",
	"$Source: /home/erlkonig/cvs/pod/src/Lib/stoss.c,v $",
	"$State: Exp $",
	"$Revision: 1.1.1.1 $",
	"$Date: 2004/06/05 17:28:04 $",
	"$Author: erlkonig $",
	(const char*)0
	};

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "stoss.h"

#ifdef TEST
#define DEBUG 
int main(int ac, char **av)
{
	char line[] = "  This is a   set    of \n \t\t tokens\n\n.  See?";
	char **tokens = Stoss((char*)0, &line[0]);
	unsigned int token_count = Sscount(tokens);
	int i;
	if(8 == token_count)
		puts("Saw expected 8 tokens, including '.' by itself as one\n");
	else {
		fprintf(stderr, "Incorrect token count of %d; aborting\n", token_count);
		return -1;
	}
	for(i = 0 ; tokens[i] ; ++i)
		printf("#%d: \"%s\"\n", i, tokens[i]);
	free(tokens), tokens = (char**)0;
	if(ac > 1)
	{
		tokens = Stoss((char*)0, av[1]);
		for(i = 0 ; tokens[i] ; ++i)
			printf("#%d: \"%s\"\n", i, tokens[i]);
		free(tokens), tokens = (char**)0;
	}		
	return 0;
}
#endif /* TEST */

char **Stoss(const char *splitters_input, const char *s) /*free the result*/
{
	char **vector = (char**)0; /* return null-terminated vector of strings */
	const char *splitters = splitters_input ? splitters_input : " \t\n";
	int smax = strlen(s);		/* strlen gives index of final nul */
	int bufmax = smax;
	char buf[bufmax + 1];
	int si = 0, bufi = 0;
	int insplitter = 1;
	int splitters_omitted = 0;
	int tokens = 0;
	int vsize = -1;
#ifdef DEBUG
	printf("splitters \"%s\", string \"%s\"\n", splitters, s);
#endif
	for(si = 0 ; si <= smax ; ++si)	/* should read the nul */
	{
		if( !s[si] || strchr(splitters, s[si])) /* s[si] is splitter or nul */
		{
			if(insplitter)
			{
				if( !s[si])
					buf[bufi] = s[si]; /* always override with nul */
				++splitters_omitted;
			} else {			/* wasn't looking at a splitter last time */
				buf[bufi++] = s[si]; /* copy over the single splitter */
				insplitter = 1;	/* but we are now */
			}
		} else {
			buf[bufi++] = s[si];
			if(insplitter)		/* we were just in a split... */
				++tokens;		/* so we must be starting another token */
			insplitter = 0;
		}
	}
#ifdef DEBUG
	printf("bufmax %d, buf \"%s\"\n", bufmax, &buf[0]);
#endif

	/* should now have buf filled with nice, 1-char split tokens */
	/* and we have a count of them. */
	vsize = (  ((tokens + 1) * sizeof(*vector))
			+ ((bufmax + 1) - splitters_omitted + 1));
#ifdef DEBUG
	printf("tokens %d, sizeof *vector %lu\n", tokens, sizeof(*vector));
	printf("bufmax %d, splitters_omitted %d\n", bufmax, splitters_omitted);
	printf("vector size: %d\n",  vsize);
#endif
	if(vector = (char**)malloc(vsize)) 
	{
		int tok, vufi = 0;			/* token index */
		char *vuf = (char*)&vector[tokens + 1];
		strcpy(vuf, &buf[0]);
		vector[tokens] = (char*)0;	/* null terminator */
#ifdef DEBUG
			printf("vector %p, vuf %p\n", vector, vuf);
#endif

		for(tok = 0 ; tok < tokens ; ++tok) /* starts at token at vuf[0] */
		{
#ifdef DEBUG
			printf("token[%d] '%c'", tok, vuf[vufi]);
#endif
			vector[tok] = &vuf[vufi++]; /* save address of token. */
			while( ! strchr(splitters, vuf[vufi]))
			{
#ifdef DEBUG
				printf(" '%c'", vuf[vufi]);
#endif
				++vufi;			/* search for start of next token */
			}
			/* on splitter */
#ifdef DEBUG
			printf(" (splitter '%c')\n", vuf[vufi]);
#endif
			vuf[vufi] = '\0';		/* replace splitter with nul */
			++vufi;				/* advance to token */
		}
		fflush(stdout);
	}
	else fputs(__FUNCTION__, stderr), perror(" - vector");
	return vector;
}

int Sscount(char **ss) /* return index of final NULL (char**) */
{
	int i = 0;
	while(ss[i])
		++i;
	return i;
}
