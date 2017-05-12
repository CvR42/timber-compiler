/* File: error.h */

#ifndef _a_ERROR_H
#define _a_ERROR_H

#include <tmc.h>
#include "vnusbasetypes.h"
#include "tmadmin.h"

extern int errno;

#define ERRARGLEN 80
extern char errpos[];
extern char errarg[];


void orig_errpos (const origin org);
extern void error (const char *msg);
extern void origin_error (const origsymbol s, const char *msg);
extern void sys_error (int no);
extern void errcheck (void);
extern void internal_botch (const char *msg);
extern void origin_internal_botch (const origsymbol s, const char *msg);

#define badtag(t) tm_badtag(__FILE__,__LINE__,t)


/* structure to describe debugging flags */
typedef struct _cvr_db_flags {
    char flagchar;       /* char to switch the flag on */
    int *flagadr;        /* pointer to the flag */
    char *flagdescr;     /* description string */
} dbflag;

#define dbflagNIL (dbflag*) 0

extern dbflag* finddbflag (int c, dbflag *flagtab);
extern void setdbflags (const char *s, dbflag *flagtab, int val);
extern void helpdbflags (FILE *f, const dbflag *flagtab);

#endif
