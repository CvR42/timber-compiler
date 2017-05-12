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
#include "error.h"
#include "global.h"
#include <vnusctl.h>
#include "../mapper/engine.h"

static FILE *infile;
static FILE *outfile;
static tmstring infilename = tmstringNIL;
static tmstring outfilename = tmstringNIL;
static tmstring errfilename = tmstringNIL;

/* Table of debugging flags plus associated information.

   Table is ended by an entry with flagchar '\0'
 */
dbflag flagtab[] = {
    { 's', &givestat, "statistics" },
    { 'O', &enableOwnerComputes, "enable alternative Owner Computes scheme" },
    { '\0', (bool *)0, "" },
};

/* Print usage message to file 'f'. */
static void usage( FILE *f , const char *exec)
{
    fprintf( f, "%s\n", application_name );
	fprintf( f, "Usage: %s [<flags>]\n", exec);
	fprintf( f, "or   : %s -h\n", exec);
    fputs(
	"\n"
	"<flags> is one or more of the following\n"
	" -d<flags>\tSwitch on the given debugging flags.\n"
	" -e<errfile>\tWrite error messages to the given file instead of stderr.\n"
	" -i<infile>\tRead input from the given file instead of stdin.\n"
	" -o<outfile>\tWrite output to the given file instead of stdout.\n"
	" -l<level>\tProcess using level:\n"
	"\t\t2: hoist owners\n"
	" -w<level>\tWarning level:\n"
	"\t\t2: show basic warnings\n"
	"\t\t3: show info about rule applications\n"
	"\t\t4: show debug info\n"
	"\n"
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

static void scan_formal_string_arg
( int argc, char *argv[], int &argix, tmstring &s, const char opt )
{
    char buf[256];
    const char *arg = argv[argix];
    if( arg[2] != '\0' ){
        if( s != tmstringNIL ){
            rfre_tmstring( s );
        }
        s = new_tmstring( arg+2 );
    }
    else {
        if( argix+1>=argc ){
            sprintf(buf, "missing formalParameter for -%c", opt);
            error( buf );
        }
        else {
            if( s != tmstringNIL ){
                rfre_tmstring( s );
            }
            argix++;
            s = new_tmstring( argv[argix] );
        }
    }
}


/* Scan command line parameters and options. */
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
		case 'l':
		    if (arg[2] != '\0')
		    {
			trans_level = atoi(arg+2);
		    }
		    break;
		    
                case 'd':
                    setdbflags( &arg[2], flagtab, true );
                    break;
                    
                case 'h':
                    usage( stdout, argv[0] );
                    exit(0);
                    
                case 'e':
                    scan_formal_string_arg(argc, argv, argix, errfilename, arg[1]);
                    break;
                    
                case 'o':
                    scan_formal_string_arg(argc, argv, argix, outfilename, arg[1]);
                    break;
                    
                case 'i':
                    scan_formal_string_arg(argc, argv, argix, infilename, arg[1]);
                    break;
                    
		case 'w':
		{
		    int parse_level = -1;
		    if (arg[2] != '\0')
		    {
			parse_level = atoi(arg+2);
		    }
		    switch (parse_level)
		    {
			case 2:
			    warning_level = WARN_WARNING;
			    break;
			case 3:
			    warning_level = WARN_RULE;
			    break;
			case 4:
			    warning_level = WARN_DEBUG;
			    break;
			default:
			    fprintf( stderr, "Incorrect warning level: %s\n", arg );
			    usage( stderr, argv[0] );
			    exit( 1 );
			    break;
		    }
		    break;
		}
		
                default:
                    fprintf( stderr, "Bad option: -%c\n", op );
                    usage( stderr, argv[0] );
                    exit( 1 );
            }
        }
        else {
            fprintf( stderr, "Bad parameter: %s\n", arg );
            usage( stderr, argv[0] );
            exit( 1 );
        }
    }
}

int main( int argc, char *argv[] )
{
    vnusprog theProg;

    infile = stdin;
    outfile = stdout;
    scanargs( argc, argv );
    if( infilename != tmstringNIL ){
        infile = ckfopen( infilename, "r" );
    } else {
        infilename = new_tmstring( "" );
    }
    if( outfilename != tmstringNIL ){
        outfile = ckfopen( outfilename, "w" );
    }
    if( errfilename != tmstringNIL ){
        ckfreopen( errfilename, "w", stderr );
    }
    errcheck();
    {/*ALWAYS*/
        tm_lineno = 1;
        if( fscan_vnusprog( infile, &theProg ) ){
            fprintf( stderr, "%s: %s(%d): %s\n", application_name, infilename, tm_lineno, tm_errmsg );
            exit( 1 );
        }
    }
    errcheck();

    theProg = apply_transformation(theProg);

    {/*ALWAYS*/
        TMPRINTSTATE *st;
        int level;
        
        st = tm_setprint( outfile, 1, 75, 8, 0 );
        print_vnusprog( st, theProg );
        level = tm_endprint( st );
        if( level != 0 ){
            fprintf( stderr, "Internal botch: bracket level = %d\n", level );
        }
    }
    rfre_vnusprog( theProg );
    errcheck();
    fre_tmstring( infilename );
    fre_tmstring( outfilename );
    fre_tmstring( errfilename );
//    flush_tmadmin();
    flush_tmsymbol();
    if( get_balance_tmadmin() != 0 ){
        fprintf( stderr, "Tm object allocation not balanced\n" );
        givestat = true;
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

