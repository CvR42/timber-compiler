/* file: main.c
 *
 * main(), initialization and termination.
 */

/* standard UNIX libaries */
#include <stdio.h>

/* the tm support library */
#include <tmc.h>

#include "defs.h"
#include "vnus_types.h"
#include "tmadmin.h"
#include "error.h"
#include "global.h"
#include "parser.h"
#include "lex.h"
#include "symbol_table.h"
#include "check.h"
#include "mangle.h"
#include "extract.h"
#include "rewrite.h"
#include "prettyprint.h"

static FILE *infile;
static FILE *outfile;
static tmstring infilename = tmstringNIL;
static tmstring outfilename = tmstringNIL;
static tmstring errfilename = tmstringNIL;

static int parseonly = FALSE;
static int dontcheck = FALSE;

/* Table of debugging flags plus associated information.

   Table is ended by an entry with flagchar '\0'
 */
dbflag flagtab[] = {
    { 'c', &dontcheck, "Do not check V-nus before code generation" },
    { 'D', &dumpmangledtree, "Dump mangled parse tree" },
    { 'd', &dumptree, "Dump parse tree" },
    { 'g', &parseonly, "Do not generate code (for syntax checking)" },
    { 'm', &showmangling, "Show translations done by name mangling" },
    { 'S', &tracesymtab, "trace symbol table construction" },
    { 's', &givestat, "show allocation statistics" },
    { 'x', &lextr, "Lexical analysis tracing" },
    { 'y', &yydebug, "Yacc action tracing" },
    { '\0', (int *)0, "" },
};

/* Print usage message to file 'f'. */
static void usage( FILE *f )
{
    fprintf( f, "ParTool sugar Vnus Frontend\n" );
    fprintf( f, "Usage: svnus [-d<debugflags>] [-o <outfile>] <infile>\n" );
    fprintf( f, "or   : svnus -h\n" );
    helpdbflags( f, flagtab );
}

/* Same as fopen, but give error message if file can't be opened */
static FILE *ckfopen( const char *nm, const char *acc )
{
    FILE *hnd;

    hnd = fopen( nm, acc );
    if( NULL == hnd ){
	(void) strcpy( errarg, nm );
	sys_error( errno );
	exit( 1 );
    }
    return hnd;
}

/* Similar to freopen, but give error message if file can't be opened.
 * Therefore, file handle need not be returned.
 */
static void ckfreopen( const char *nm, const char *acc, FILE *f )
{
    if( freopen( nm, acc, f ) == NULL ){
	(void) strcpy( errarg, nm );
	sys_error( errno );
	exit( 1 );
    }
}

/* Scan arguments and options. */
static void scanargs( int argc, char *argv[] )
{
    int argix;
    char op;
    const char *arg;

    for( argix=1; argix<argc; argix++ ){
	arg = argv[argix];
	if( arg[0] == '-' ){
	    op = arg[1];
	    switch (op){
		case 'd':
		case 'D':
		    setdbflags( &arg[2], flagtab, TRUE );
		    break;

		case 'h':
		    usage( stdout );
		    exit(0);

		case 'e':
		case 'E':
		    if( arg[2] != '\0' ){
			if( errfilename != tmstringNIL ){
			    rfre_tmstring( errfilename );
			}
			errfilename = new_tmstring( arg+2 );
		    }
		    else {
			if( argix+1>=argc ){
			    error( "missing argument for -e" );
			}
			else {
			    if( errfilename != tmstringNIL ){
				rfre_tmstring( errfilename );
			    }
			    argix++;
			    errfilename = new_tmstring( argv[argix] );
			}
		    }
		    break;

		case 'o':
		    if( arg[2] != '\0' ){
			if( outfilename != tmstringNIL ){
			    rfre_tmstring( outfilename );
			}
			outfilename = new_tmstring( arg+2 );
		    }
		    else {
			if( argix+1>=argc ){
			    error( "missing argument for -o" );
			}
			else {
			    if( outfilename != tmstringNIL ){
				rfre_tmstring( outfilename );
			    }
			    argix++;
			    outfilename = new_tmstring( argv[argix] );
			}
		    }
		    break;

		default:
		    fprintf( stderr, "Bad option: -%c\n", op );
		    usage( stderr );
		    exit( 1 );
	    }
	}
	else {
	    fre_tmstring( infilename );
	    infilename = new_tmstring( argv[argix] );
	    infile = ckfopen( infilename, "r" );
	}
    }
}

int main( int argc, char *argv[] )
{
    vnusprog theprog;

    infile = stdin;
    outfile = stdout;
    infilename = new_tmstring( "" );
    init_lex();
    init_symbol_table();
    scanargs( argc, argv );
    if( outfilename != tmstringNIL ){
	outfile = ckfopen( outfilename, "w" );
    }
    if( errfilename != tmstringNIL ){
        ckfreopen( errfilename, "w", stderr );
    }
    theprog = parse( infile, infilename );
    errcheck();
    if( dumptree ){
	TMPRINTSTATE *st;
	int level;

	st = tm_setprint( stderr, 1, 75, 8, 0 );
	print_vnusprog( st, theprog );
	level = tm_endprint( st );
	if( level>0 ){
	    fprintf( stderr, "Internal botch: bracket level is %d\n", level );
	}
	fflush( stderr );
    }
    set_symbol_table( theprog->vnusdecls );
    theprog = mangle_vnusprog( theprog );
    if( dumpmangledtree ){
	TMPRINTSTATE *st;
	int level;

	st = tm_setprint( stderr, 1, 75, 8, 0 );
	print_vnusprog( st, theprog );
	level = tm_endprint( st );
	if( level>0 ){
	    fprintf( stderr, "Internal botch: bracket level is %d\n", level );
	}
    }
    theprog = extract_vnusprog( theprog );
    if( !dontcheck ){
	check_vnusprog( theprog );
    }
    errcheck();
    theprog = rewrite_vnusprog( theprog );
    if( !dontcheck ){
	check_vnusprog( theprog );
    }
    errcheck();
    if( !parseonly ){
	pp_vnusprog( outfile, theprog );
    }
    rfre_vnusprog( theprog );
    end_lex();
    end_symbol_table();
    fre_tmstring( infilename );
    fre_tmstring( outfilename );
    fre_tmstring( errfilename );
    flush_tmsymbol();
    if( get_balance_tmadmin() != 0 ){
	fprintf( stderr, "Tm object allocation not balanced\n" );
	givestat = TRUE;
    }
    if( givestat ){
	stat_tmadmin( stderr );
	stat_tmstring( stderr );
	report_lognew( stderr );
    }
    flush_lognew();
    exit( 0 );
    return 0;
}
