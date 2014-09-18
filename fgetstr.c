/* fgetstr.c : get from FILE* a string not containing TERMINATORS */

static const char *Version[] = {
	"@(#) fgetstr.c Copyright (c) 1992 C. Alex. North-Keys",
	"$Grueppe: Talismangrueppe $",
	"$Anfang: Fri Sep 25 07:08:26 GMT 1992 $",
	"$Compiliert: "__DATE__" " __TIME__ " $",
	"$Source: /home/erlkonig/cvs/pod/src/Lib/fgetstr.c,v $",
	"$Revision: 1.1.1.1 $",
	"$Date: 2004/06/05 17:28:04 $",
	"$Author: erlkonig $",
	(char*)0
	};

/* note - if your compiler can`t allocate arrays on the stack, TOO BAD */

#include "fgetstr.h"

#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>

#ifdef TEST
#define DEBUG
#endif

#ifdef DEBUG
#undef DEBUG
#define DEBUG(b) {b}
#else
#define DEBUG(b)  
#endif

/*------- debugging ``main'' -------*/
#ifdef TEST
int main (int argc, char **argv)
{
	char *text = (char*)0;
	char *ends = "\n";
	
	if(argc > 1) ends = argv[1];
	
	while(text = Fgetstr(stdin, ends))
	{
		puts(text);
		free(text);
	}
	
	return 0;
}
#endif

/*	return specifications -
 *
 *	terminators include : ends, \0, and EOF
 *
 *	root	EOF?	text?	ended? 	stat	returned value
 *			-		-		-		...	
 *	1		-		-		1		return	""
 *			-		1		-		...	
 *	2		-		1		1		return	"text"
 *	3		1		-		-		return	-null-		EOF-*accepted*
 *	4		1		-		1		return	""			EOF-postponed
 *	5		1		1		-		return	"text"		EOF-postponed/fake-end
 *	6		1		1		1		return	"text"		EOF-postponed/true-end
 *
 *	on ENOMEM, return -null-
 *
 */
 
static char *Fgetstr_R(FILE *ifp, const char *ends, unsigned int offset)
{
	char *s = (char*)0;						/* the crucial string to return */
	unsigned int bufmax = offset;			/* as large as so far */
	unsigned int bufidx = 0;				/* index within buffer */
	char buffer[bufmax + 1];				/* on-stack allocation required */
	int ended = 0;							/* end character seen ? */
	int	eof = 0;							/* e-o-f seen ? */
	
	DEBUG(fprintf(stderr, "(%d", offset););
		
	while(bufidx <= bufmax)		/* pre-recurse - attempt to fill buffer */
	{
		int c = getc(ifp);
		
		if( (ended = ( !c || (ends && strchr(ends,c)))) || (eof = (EOF==c)) )  
			break;
		
		buffer[bufidx++] = (char)c;
	}
	
	/* note - the buffer *must* at least have room for the terminal \0 */

	if(ended || (eof && offset))  					/* root 1,2,4,6 5 */
	{
		unsigned int offset_max = offset + bufidx;
		DEBUG(fprintf(stderr, " malloc %d", offset_max + 1););
		if(s = (char*)malloc((offset_max + 1) * sizeof(char)))
			s[offset_max] = '\0';
		else
			s = (char*)0, perror("Fgetstr_R - malloc");
	}
	else
	{
		if(eof && !offset)	/* && !ended */		/* root 3 */
			s = (char*)0;
		else
			s = Fgetstr_R(ifp, ends, offset + bufidx);	/* recurse */
	}
	
	/* post-recurse */

	if(s)
		strncpy(&s[offset], &buffer[0], bufidx);  /* cnv. idx to count */

	DEBUG(fprintf(stderr, ")", offset););
	return s;
}

char *Fgetstr (FILE *ifp, const char *ends)
{
	register char *s = (char*)0;
	DEBUG(fprintf(stderr, "Fgetstr "););
	s = Fgetstr_R(ifp, ends, 0);
	DEBUG(fprintf(stderr, ".\n"););
	return s;
}
