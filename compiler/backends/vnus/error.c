/* file: error.c
 *
 * error handling
 */

#include <stdio.h>

/* the tm support library */
#include <tmc.h>

#include "error.h"

/* Given a character 'c' and a flag table 'flagtab',
   return a pointer to the entry of character 'c', or return
   dbflagNIL if not found.
 */
static dbflag *finddbflag( int c, dbflag *flagtab )
{
    dbflag *p = flagtab;

    while( p->flagchar != '\0' ){
	if( p->flagchar == c ){
	    return p;
	}
	p++;
    }
    return dbflagNIL;
}

/* Give help information on debugging flags */
void helpdbflags( FILE *f, const dbflag *flagtab )
{

    fputs( "Debugging flags:\n", f );
    const dbflag *p = flagtab;
    while( p->flagchar != '\0' ){
	fprintf( f, " %c - %s.\n", p->flagchar, p->flagdescr );
	p++;
    }
    fputs( " . - all flags.\n", f );
}

/* Given a string 's' containing debug flags, a flag table 'flagtab' and
   a value 'val', look up all flags given in 's', and set the associated
   value to 'val'. The flag character '.' means that all flags must
   be set.
 */
void setdbflags( const char *s, dbflag *flagtab, bool val )
{

    while( *s != '\0' ){
	dbflag *e;
	int c = *s++;

	if( c == '.' ){
	    e = flagtab;
	    while( e->flagchar != '\0' ){
		*e->flagadr = val;
		e++;
	    }
	}
	else {
	    e = finddbflag( c, flagtab );
	    if( e == dbflagNIL ){
		fprintf( stderr, "Unknown debug flag: '%c'\n", c );
		exit( 1 );
	    }
	    *e->flagadr = val;
	}
    }
}

void tm_fatal( const char *file, const int line, const char *s )
{
    if( file[0] == '\0' && line == 0 ){
        fprintf( stderr, "*** fatal error: %s\n", s );
    }
    else {
        fprintf( stderr, "%s:%d: *** fatal error: %s\n", file, line, s );
    }
    exit( 1 );
}
