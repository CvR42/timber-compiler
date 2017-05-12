/* file: main.c
 *
 * main(), initialization and termination.
 */

/* standard UNIX libaries */
#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>

/* the tm support library */
#include <tmc.h>

#include "defs.h"
#include "tmadmin.h"
#include <vnusctl.h>
#include "error.h"
#include "global.h"
#include "parser.h"
#include "lex.h"
#include "symbol_table.h"
#include "symbol_support.h"
#include "check.h"
#include "generate.h"
#include "rewrite.h"
#include "pragma.h"
#include "service.h"
#include "type.h"

static FILE *infile;
static FILE *outfile;
static tmstring infilename = tmstringNIL;
static tmstring outfilename = tmstringNIL;
static tmstring errfilename = tmstringNIL;
static tmstring_list includefiles = tmstring_listNIL;

static bool parseonly = false;
static bool dontcheck = false;
static bool writeTm = false;
static bool readTm = false;
static bool my_yydebug = false;
static bool dump_gc_stats = false;

/* Table of debugging flags plus associated information.

   Table is ended by an entry with flagchar '\0'
 */
dbflag flagtab[] = {
    { 'G', &dump_gc_stats, "Let the generated program print GC statistics" },
    { 'S', &trace_scope_admin, "trace scope administration" },
    { 'a', &trace_analysis, "trace program analysis" },
    { 'c', &dontcheck, "Do not check Vnus before code generation" },
    { 'd', &dumptree, "Dump parse tree" },
    { 'f', &dontflattenselection, "Don't rewrite selections to flat selections" },
    { 'g', &parseonly, "Do not generate code (for syntax checking)" },
    { 'r', &trace_struct_register, "trace structure registration and lookup" },
    { 's', &givestat, "statistics" },
    { 't', &generateTimingCode, "Generate timing code" },
    { 'x', &lextr, "lexical analysis tracing" },
    { 'y', &my_yydebug, "Yacc action tracing" },
    { '\0', (bool *)0, "" },
};

/* Print usage message to file 'f'. */
static void usage( FILE *f )
{
    fprintf( f, "Timber Vnus Backend\n" );
    fputs(
	"Usage: vnus [<flags>] [<infile>]\n"
	"or   : vnus -h\n"
	"\n"
	"<flags> is one or more of the following\n"
	" -a<proc>\tUse the given processor array.\n"
	" -d<flags>\tSwitch on the given debugging flags.\n"
	" -e<errfile>\tWrite error messages to the given file instead of stderr.\n"
	" -l\t\tGenerate calls for 64-bit long class.\n"
	" -o<outfile>\tWrite output to the given file instead of stdout.\n"
	" -p\t\tGenerate parallel instead of sequential code.\n"
	" -r\t\tParse input as Tm datastructures instead of Vnus code.\n"
	" -t\t\tWrite Tm datastructures instead of C++ code.\n"
	" -I<file>\tAdd a `#include <file>' line to the generated C++ code\n"
	"\n"
	"<proc> is a list of processor dimensions, e.g. '-p3,5,2'\n"
	"\n"
	,
	f
    );
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

/* Scan command line parameters and options. */
static void scanargs( int argc, char *argv[] )
{
    includefiles = new_tmstring_list();
    for( int argix=1; argix<argc; argix++ ){
        const char *arg = argv[argix];

        if( arg[0] == '-' ){
	    char op = arg[1];

	    switch (op){
		case 'd':
		    setdbflags( &arg[2], flagtab, 1 );
		    break;

		case 'h':
		    usage( stdout );
		    exit(0);

		case 'e':
		    if( arg[2] != '\0' ){
			if( errfilename != tmstringNIL ){
			    rfre_tmstring( errfilename );
			}
			errfilename = new_tmstring( arg+2 );
		    }
		    else {
			if( argix+1>=argc ){
			    error( "missing formalParameter for -e" );
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

		case 'r':
		    readTm = true;
		    break;

	        case 't':
	            writeTm = true;
		    break;

	        case 'l':
	            longAsClass = true;
                    break;

		case 'p':
		    program_type = PROG_DISTRIBUTED;
		    break;
		    
		case 'a':
		    if( arg[2] != '\0' ){
			scan_number_of_processors( arg+2 );
		    }
		    else {
			if( argix+1>=argc ){
			    error( "-a flag is not followed by number of processors" );
			}
			else {
			    scan_number_of_processors( argv[argix+1] );
			    argix++;
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
			    error( "missing filename for -o" );
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

		case 'I':
		{
		    tmstring inc;

		    if( arg[2] != '\0' ){
			inc = new_tmstring( arg+2 );
		    }
		    else {
			if( argix+1>=argc ){
			    error( "missing path for -I" );
			    inc = tmstringNIL;
			}
			else {
			    argix++;
			    inc = new_tmstring( argv[argix] );
			}
		    }
		    includefiles = append_tmstring_list( includefiles, inc );
		    break;
		}

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
    yydebug = my_yydebug?1:0;
}

int main( int argc, char *argv[] )
{
    vnusprog theprog;

    processor_array = tmuint_listNIL;
    numberOfProcessors = 1;
    infile = stdin;
    outfile = stdout;
    infilename = new_tmstring( "" );
    init_lex();
    init_symbol_table();
    init_symbol_support();
    scanargs( argc, argv );
    if( outfilename != tmstringNIL ){
	outfile = ckfopen( outfilename, "w" );
    }
    if( errfilename != tmstringNIL ){
        ckfreopen( errfilename, "w", stderr );
    }
    errcheck();
    if( readTm ){
	tm_lineno = 1;
	if( fscan_vnusprog( infile, &theprog ) ){
	    fprintf( stderr, "vnus: %s(%d): %s\n", infilename, tm_lineno, tm_errmsg );
	    exit( 1 );
	}
    }
    else {
	theprog = parse( infile, infilename );
    }
    errcheck();
    if( dumptree ){
	TMPRINTSTATE *st = tm_setprint( stderr, 1, 75, 8, 0 );
	print_vnusprog( st, theprog );
	int level = tm_endprint( st );
	if( level != 0 ){
	    fprintf( stderr, "Internal botch: bracket level = %d\n", level );
	}
    }
    theprog = constant_fold_vnusprog( theprog );
    errcheck();
    derive_processor_array( theprog );
    if( processor_array == tmuint_listNIL ){
        processor_array = new_tmuint_list();
        numberOfProcessors = 1;
    }
    derive_processors_pragma( theprog );
    if( !dontcheck ){
	check_vnusprog( theprog );
    }
    errcheck();
    if( !parseonly ){
	if( writeTm ){
	    theprog = constant_fold_vnusprog( theprog );
	    TMPRINTSTATE *st = tm_setprint( outfile, 1, 75, 8, 0 );
	    print_vnusprog( st, theprog );
	    int level = tm_endprint( st );
	    if( level != 0 ){
		fprintf( stderr, "Internal botch: bracket level = %d\n", level );
	    }
	}
	else {
	    theprog = rewrite_vnusprog( theprog );
	    if( !dontcheck ){
		check_vnusprog( theprog );
	    }
	    errcheck();
	    theprog = constant_fold_vnusprog( theprog );
	    errcheck();
	    generate_vnusprog( outfile, includefiles, theprog, dump_gc_stats );
	}
    }
    rfre_vnusprog( theprog );
    errcheck();
    end_lex();
    end_symbol_table();
    end_symbol_support();
    rfre_tmuint_list( processor_array );
    rfre_tmstring( infilename );
    rfre_tmstring( outfilename );
    rfre_tmstring( errfilename );
    if( includefiles != tmstring_listNIL ){
	rfre_tmstring_list( includefiles );
	includefiles = tmstring_listNIL;
    }
    flush_tmsymbol();
    clear_function_types();
    if( get_balance_tmadmin() != 0 ){
        fprintf( stderr, "Tm object allocation not balanced\n" );
        givestat = 1;
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
