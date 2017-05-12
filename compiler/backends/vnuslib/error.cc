/* File: error.cc
 *
 * Miscellaneous service routines.
 */

#include <tmc.h>
#include <ctype.h>
#include <assert.h>
#include <string.h>

#include "config.h"
#include "vnusbasetypes.h"
#include "tmadmin.h"
#include "vnusctl.h"

#if !HAVE_STRERROR
extern char *sys_errlist[];
extern int sys_nerr;
#endif

char errpos[ERRARGLEN] = "";
char errarg[ERRARGLEN] = "";

static bool goterr = false;

/* Given an origin 'org', fill 'errpos' with the origin info from 'org'. */
static void orig_errpos( const_origin org )
{
    if( org != originNIL ){
	sprintf( errpos, "%s:%d", org->file->name, org->line );
    }
}

/* General error printing routine: print error message 'msg' possibly preceded
 * by string in 'errpos', and possibly followed by string in 'errarg'.
 *
 * set a flag to indicate an error has occurred. If the function
 * errcheck() is called, it will cause an exit( 1 ) if 'goterr' is now
 * true.
 */
void error( const char *msg )
{
    if( errpos[0] != '\0' ) fprintf( stderr, "%s: ", errpos );
    fputs( msg, stderr );
    if( errarg[0] != '\0' ){
	if( strlen( msg )>40 ){
	    fputs( ":\n", stderr );
	    if( errpos[0] != '\0' ) fprintf( stderr, "%s: ", errpos );
	    fprintf( stderr, " %s", errarg);
	}
	else {
	    fprintf( stderr, ": %s", errarg);
	}
    }
    fputs( "\n", stderr );
    errarg[0] = '\0';
    errpos[0] = '\0';
    goterr = true;
}

/* Handle an internal error. */
void internal_error( const char *msg )
{
    char msg1[500];

    sprintf( msg1, "Internal error: %s", msg );
    error( msg1 );
    exit( 2 );
}

/* Given an origin 'org' and a message 'msg', fill errpos with the position,
 * and generate an internal error message 'msg'.
 */
void origin_internal_error( const_origin org, const char *msg )
{
    orig_errpos( org );
    internal_error( msg );
}

/* Given an origsymbol 's' and a message 'msg', fill errpos and errarg
 * with the position and name of the symbol respectively, and generate an
 * internal botch message 'msg'.
 */
void origsymbol_internal_error( const_origsymbol s, const char *msg )
{
    tmsymbol sym = s->sym;

    if( sym == tmsymbolNIL ){
	sprintf( errarg, "<NIL symbol>" );
    }
    else {
	sprintf( errarg, "'%s'", sym->name );
    }
    origin_internal_error( s->orig, msg );
}

/* Check if 'goterr' flag is set, and do exit(1) if it is. */
void errcheck( void )
{
   if( !goterr ) return;
   exit( 1 );
}

/* Given an origsymbol 's' and a message 'msg', fill errpos and errarg
 * with the position and name of the symbol respectively, and generate error
 * message 'msg'.
 */
void origin_error( const_origin s, const char *msg )
{
    orig_errpos( s );
    error( msg );
}

/* Given an origsymbol 's' and a message 'msg', fill errpos and errarg
 * with the position and name of the symbol respectively, and generate error
 * message 'msg'.
 */
void origsymbol_error( const_origsymbol s, const char *msg )
{
    tmsymbol sym = s->sym;

    if( sym == tmsymbolNIL ){
	sprintf( errarg, "<NIL symbol>" );
    }
    else {
	sprintf( errarg, "'%s'", sym->name );
    }
    origin_error( s->orig, msg );
}

/* System error handler. Note that there is supposed to be a
 * function 'strerror()' that simplifies this, but at least under
 * SunOS 4.1.4 it isn't there.
 */
void sys_error( int no )
{
#if HAVE_STRERROR
    error( strerror( no ) );
#else
    if( no>sys_nerr ){
	(void) sprintf( errarg, "%d", no );
	error( "unknown system error" );
    }
    else {
	error( sys_errlist[no] );
    }
#endif
}

