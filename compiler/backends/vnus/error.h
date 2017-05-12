/* File: error.h */
#ifndef __VNUS_ERROR_H__
#define __VNUS_ERROR_H__


/* structure to describe debugging flags */
typedef struct _cvr_db_flags {
    char flagchar;		/* char to switch the flag on */
    bool *flagadr;		/* pointer to the flag */
    const char *flagdescr;	/* description string */
} dbflag;

#define dbflagNIL (dbflag *)0

extern void setdbflags( const char *s, dbflag *flagtab, bool val );
extern void helpdbflags( FILE *f, const dbflag *flagtab );

#endif
