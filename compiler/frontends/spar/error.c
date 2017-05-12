/* file: error.c
 *
 * Error handling.
 */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include <tmc.h>

#include "defs.h"
#include "config.h"
#include "tmadmin.h"
#include "error.h"
#include "service.h"

char errpos[ERRARGLEN] = "";
char errarg[ERRARGLEN] = "";

static bool goterr = false;

bool is_compilation_failure()
{
    return goterr;
}

/* Given an origin 'org', fill 'errpos' with the origin info from 'org'. */
static void orig_errpos( const_origin org )
{
    if( org != originNIL ){
	sprintf( errpos, "%s:%d", org->file->name, org->line );
    }
}

/* Central handler of all error printing routines. */
void verror( const char *msg, va_list args )
{
    if( errpos[0] != '\0' ){
	fprintf( stderr, "%s: ", errpos );
    }
    (void) vfprintf( stderr, msg, args );
    if( errarg[0] != '\0' ){
	fprintf( stderr, ": %s", errarg);
    }
    fputs( "\n", stderr );
    errarg[0] = '\0';
    errpos[0] = '\0';
    goterr = true;
}

/* Central handler of all warning printing routines. */
static void vwarning( const char *msg, va_list args )
{
    if( errpos[0] != '\0' ) fprintf( stderr, "%s: ", errpos );
    fputs( "warning: ", stderr );
    (void) vfprintf( stderr, msg, args );
    if( errarg[0] != '\0' ) fprintf( stderr, ": %s", errarg);
    fputs( "\n", stderr );
    errarg[0] = '\0';
    errpos[0] = '\0';
}

/* General error printing routine: print error message 'msg' possibly preceded
 * by the string in 'errpos', and possibly followed by the string in 'errarg'.
 *
 * Set a flag to indicate an error has occurred. If the function
 * errcheck() is called, it will cause an exit( 1 ) if 'goterr' is now
 * TRUE.
 */
void error( const char *msg, ... )
{
    va_list args;

    va_start( args, msg );
    verror( msg, args );
    va_end( args );
}

/* Given an origsymbol 's' and a message 'msg', fill errpos and errarg
 * with the position and name of the symbol respectively, and generate error
 * message 'msg'.
 */
void origsymbol_error( const_origsymbol s, const char *msg, ... )
{
    tmsymbol sym;
    va_list args;

    orig_errpos( s->org );
    sym = s->sym;
    if( sym == tmsymbolNIL ){
        sprintf( errarg, "<NIL symbol>" );
    }
    else {
        sprintf( errarg, "'%s'", sym->name );
    }
    va_start( args, msg );
    verror( msg, args );
    va_end( args );
}

/* Given an origsymbol 's' and a message 'msg', fill errpos and errarg
 * with the position and name of the symbol respectively, and generate warning
 * message 'msg'.
 */
void origsymbol_warning( const_origsymbol s, const char *msg, ... )
{
    tmsymbol sym;
    va_list args;

    orig_errpos( s->org );
    sym = s->sym;
    if( sym == tmsymbolNIL ){
        sprintf( errarg, "<NIL symbol>" );
    }
    else {
        sprintf( errarg, "'%s'", sym->name );
    }
    va_start( args, msg );
    vwarning( msg, args );
    va_end( args );
}

/* Given an origin 'org' and a message 'msg', fill errpos 
 * with the position, and generate error
 * message 'msg', duly formatted.
 */
void origin_error( const_origin org, const char *msg, ... )
{
    va_list args;

    orig_errpos( org );
    va_start( args, msg );
    verror( msg, args );
    va_end( args );
}

/* System error handler. Note that there is supposed to be a
 * function 'strerror()' that simplifies this, but at least under
 * SunOS 4.1.4 it isn't there.
 */
void sys_error( int no )
{
#ifdef HAVE_STRERROR
    error( strerror( no ) );
#else
    extern char *sys_errlist[];
    extern int sys_nerr;

    if( no>sys_nerr ){
	(void) sprintf( errarg, "%d", no );
	error( "unknown system error" );
    }
    else {
	error( sys_errlist[no] );
    }
#endif
}

/* Handle an internal botch. */
void internal_error( const char *msg, ... )
{
    va_list args;
    tmstring buf = printf_tmstring( "Internal error: %s", msg );

    va_start( args, msg );
    verror( buf, args );
    va_end( args );
    rfre_tmstring( buf );
    exit( 2 );
}

/* Given an origsymbol 's' and a message 'msg', fill errpos and errarg
 * with the position and name of the symbol respectively, and generate an
 * internal botch message 'msg'.
 */
void origsymbol_internal_error( const_origsymbol s, const char *msg, ... )
{
    tmsymbol sym = s->sym;
    tmstring buf = printf_tmstring( "Internal error: %s", msg );
    va_list args;

    orig_errpos( s->org );
    if( sym == tmsymbolNIL ){
        sprintf( errarg, "<NIL symbol>" );
    }
    else {
        sprintf( errarg, "'%s'", sym->name );
    }
    va_start( args, msg );
    verror( buf, args );
    va_end( args );
    rfre_tmstring( buf );
    exit( 2 );
}

/* Given an origin 'org' and a message 'msg', fill errpos and errarg
 * with the position and name of the symbol respectively, and generate an
 * internal botch message 'msg'.
 */
void origin_internal_error( const_origin org, const char *msg, ... )
{
    tmstring buf = printf_tmstring( "Internal error: %s", msg );
    va_list args;

    orig_errpos( org );
    va_start( args, msg );
    verror( buf, args );
    va_end( args );
    rfre_tmstring( buf );
    exit( 2 );
}

/* General warning printing routine: print warning message 'msg' possibly
 * preceded by string in 'errpos', and possibly followed by string in
 * 'errarg'.
 */
void warning( const char *msg, ... )
{
    va_list args;

    va_start( args, msg );
    vwarning( msg, args );
    va_end( args );
}

/* Given a group of modifiers that are deemed to be bad, complain
 * about them.
 */
void complain_modifiers( const_origin org, const modflags flags, const char *where )
{
    if( flags & ACC_PUBLIC ){
	 origin_error( org, "'public' modifier not allowed for a %s", where );
    }
    if( flags & ACC_PRIVATE ){
	 origin_error( org, "'private' modifier not allowed for a %s", where );
    }
    if( flags & ACC_PROTECTED ){
	 origin_error( org, "'protected' modifier not allowed for a %s", where );
    }
    if( flags & ACC_STATIC ){
	 origin_error( org, "'static' modifier not allowed for a %s", where );
    }
    if( flags & ACC_FINAL ){
	 origin_error( org, "'final' modifier not allowed for a %s", where );
    }
    if( flags & ACC_SYNCHRONIZED ){
	 origin_error( org, "'synchronized' modifier not allowed for a %s", where );
    }
    if( flags & ACC_VOLATILE ){
	 origin_error( org, "'volatile' modifier not allowed for a %s", where );
    }
    if( flags & ACC_TRANSIENT ){
	 origin_error( org, "'transient' modifier not allowed for a %s", where );
    }
    if( flags & ACC_NATIVE ){
	 origin_error( org, "'native' modifier not allowed for a %s", where );
    }
    if( flags &  ACC_INTERFACE  ){
	 origin_error( org, "'interface' modifier not allowed for a %s", where );
    }
    if( flags & ACC_ABSTRACT ){
	 origin_error( org, "'abstract' modifier not allowed for a %s", where );
    }
    if( flags & ACC_STRICTFP ){
	 origin_error( org, "'strictfp' declaration not allowed for a %s", where );
    }
    if( flags & ACC_INLINE ){
	 origin_error( org, "'inline' declaration not allowed for a %s", where );
    }
}

/* Given an origin 'org' and a message 'msg', fill errpos 
 * with the position, and generate warning
 * message 'msg', duly formatted.
 */
void origin_warning( const_origin org, const char *msg, ... )
{
    va_list args;

    orig_errpos( org );
    va_start( args, msg );
    vwarning( msg, args );
    va_end( args );
}

/* Check if 'goterr' flag is set, and do exit(1) if it is. */
void errcheck( void )
{
   if( !goterr ) return;
   exit( 1 );
}
