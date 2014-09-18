/* stoss.h : tokenize a char* into char** (splat to splat-splat) */

/* @(#) stoss.h Copyright (c) 1998 C. Alex. North-Keys */
/* $Grueppe: Talisman $ */
/* $Anfang: 1998-12-25 09:21:34 GMT (Dec Fri) 914577694 $ */
/* $Source: /home/erlkonig/cvs/pod/src/Lib/stoss.h,v $ */
/* $State: Exp $ */
/* $Revision: 1.1.1.1 $ */
/* $Date: 2004/06/05 17:28:04 $ */
/* $Author: erlkonig $ */

#ifndef ___stoss_h
#define ___stoss_h ___stoss_h

#ifdef __cplusplus
extern "C" {
#endif

extern char **Stoss(const char *splitters, const char *s); /*free the result*/
extern int    Sscount(char **ss); /* give index of final NULL (char**) */

#ifdef __cplusplus
};
#endif

#endif /* ___stoss_h - herunten gibts nichts */
