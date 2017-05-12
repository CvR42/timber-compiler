/* File: dump.c
 *
 * Write a language construct to a given file for debugging purposes.
 */

#include <stdio.h>
#include <stdarg.h>
#include <tmc.h>

#include "defs.h"
#include "tmadmin.h"
#include "dump.h"

#define LINEWIDTH 75
#define TABSIZE 8
#define INDENT 0

void dump_Block( FILE *f, const_Block blk, const char *msg, ... )
{
    va_list args;

    va_start( args, msg );
    (void) vfprintf( f, msg, args );
    va_end( args );
    fputs( "\n", f );
    TMPRINTSTATE *st = tm_setprint( f, 1, LINEWIDTH, TABSIZE, INDENT );
    print_Block( st, blk );
    int level = tm_endprint( st );
    if( level>0 ){
	fprintf( f, "Internal botch: bracket level is %d\n", level );
    }
}

void dump_Entry( FILE *f, const_Entry blk, const char *msg, ... )
{
    va_list args;

    va_start( args, msg );
    (void) vfprintf( f, msg, args );
    va_end( args );
    fputs( "\n", f );
    TMPRINTSTATE *st = tm_setprint( f, 1, LINEWIDTH, TABSIZE, INDENT );
    print_Entry( st, blk );
    int level = tm_endprint( st );
    if( level>0 ){
	fprintf( f, "Internal botch: bracket level is %d\n", level );
    }
}

void dump_statement( FILE *f, const_statement smt, const char *msg, ... )
{
    va_list args;

    va_start( args, msg );
    (void) vfprintf( f, msg, args );
    va_end( args );
    fputs( "\n", f );
    TMPRINTSTATE *st = tm_setprint( f, 1, LINEWIDTH, TABSIZE, INDENT );
    print_statement( st, smt );
    int level = tm_endprint( st );
    if( level>0 ){
	fprintf( f, "Internal botch: bracket level is %d\n", level );
    }
}

void dump_expression( FILE *f, const_expression smt, const char *msg, ... )
{
    va_list args;

    va_start( args, msg );
    (void) vfprintf( f, msg, args );
    va_end( args );
    fputs( "\n", f );
    TMPRINTSTATE *st = tm_setprint( f, 1, LINEWIDTH, TABSIZE, INDENT );
    print_expression( st, smt );
    int level = tm_endprint( st );
    if( level>0 ){
	fprintf( f, "Internal botch: bracket level is %d\n", level );
    }
}

void dump_TypeEntry_list( FILE *f, const_TypeEntry_list tl, const char *msg, ... )
{
    va_list args;

    va_start( args, msg );
    (void) vfprintf( f, msg, args );
    va_end( args );
    fputs( "\n", f );
    TMPRINTSTATE *st = tm_setprint( f, 1, LINEWIDTH, TABSIZE, INDENT );
    print_TypeEntry_list( st, tl );
    int level = tm_endprint( st );
    if( level>0 ){
	fprintf( f, "Internal botch: bracket level is %d\n", level );
    }
}

void dump_TypeBinding_list( FILE *f, const_TypeBinding_list tl, const char *msg, ... )
{
    va_list args;

    va_start( args, msg );
    (void) vfprintf( f, msg, args );
    va_end( args );
    fputs( "\n", f );
    TMPRINTSTATE *st = tm_setprint( f, 1, LINEWIDTH, TABSIZE, INDENT );
    print_TypeBinding_list( st, tl );
    int level = tm_endprint( st );
    if( level>0 ){
	fprintf( f, "Internal botch: bracket level is %d\n", level );
    }
}
