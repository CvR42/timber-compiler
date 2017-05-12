/* file: main.c
 *
 * main(), initialization and termination.
 */

/* standard UNIX libaries */
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

/* the tm support library */
#include <tmc.h>

#include "defs.h"
#include "tmadmin.h"
#include "error.h"
#include "global.h"
#include "parser.h"
#include "lex.h"
#include "check.h"
#include "mangle.h"
#include "fwdregtype.h"
#include "regtype.h"
#include "rewrite.h"
#include "lower.h"
#include "prettyprint.h"
#include "version.h"
#include "service.h"
#include "config.h"
#include "constfold.h"
#include "doimports.h"
#include "markused.h"
#include "addgc.h"
#include "markadmin.h"

static FILE *infile;
static FILE *outfile;
static tmstring infilename = tmstringNIL;
static tmstring outfilename = tmstringNIL;
static tmstring errfilename = tmstringNIL;

static int dontcheck = FALSE;
static bool restrict_nostring = false;
static bool restrict_nocards = false;
static bool restrict_nocomplex = false;
static bool restrict_nodelete = false;
static bool restrict_noeach = false;
static bool restrict_noinline = false;
static bool restrict_nopragma = false;
static bool restrict_noprint = false;
static bool restrict_javaarray = false;
static bool restrict_java = false;
static int dumploweredtree = FALSE;

/* Table of debugging flags plus associated information.

   Table is ended by an entry with flagchar '\0'
 */
static dbflag flagtab[] = {
    { 'D', &dumpmangledtree, "dump mangled parse tree" },
    { 'G', &trace_gcadmin, "trace the generation of GC administration" },
    { 'I', &trace_inlining, "trace inlining" },
    { 'L', &trace_innerclasses, "trace transformations of inner, local and anonymous classes" },
    { 'S', &trace_symtab, "trace symbol table construction" },
    { 'a', &trace_analysis, "trace program analysis" },
    { 'b', &trace_bindings, "trace the binding of shorthand types" },
    { 'c', &dontcheck, "do not check the parse tree before code generation" },
    { 'd', &dumptree, "dump the parse tree" },
    { 'f', &trace_files, "trace file imports" },
    { 'g', &pref_no_codegeneration, "do not generate code" },
    { 'i', &trace_imports, "trace source file searching for imports" },
    { 'l', &dumploweredtree, "dump parse tree after lowering phase" },
    { 'm', &showmangling, "show translations done by name mangling" },
    { 'o', &showmethodbinding, "show the binding of signatures to methods" },
    { 'p', &trace_generics, "trace the handling of parameterized types" },
    { 's', &givestat, "show allocation statistics" },
    { 't', &trace_typelist, "trace the construction of the type list" },
    { 'u', &trace_usage, "show usage administration maintainance" },
    { 'v', &trace_vtable, "trace construction of the vtables" },
    { 'x', &lextr, "trace lexical analysis" },
    { 'y', &yydebug, "trace yacc operations" },
    { '\0', 0, "" },
};

struct warning {
    const char *name;
    int *flag;
    const char *descr;
};

static struct warning warnings[] = {
    { "ignoredvalue", &warn_ignoredvalue, "warn about values returned from methods that are ignored" },
    { "semicolon", &warn_semicolon, "warn about ';' after a class or method declaration" },
    { 0, 0, 0 }
};

/* Given a warning string 'str', a table of warning flags 'table', and
 * a pointer to an index 'posp', return false if the string can not be found
 * in the table, or return true if the string can be found, and set '*posp'
 * to the position of the string in the table.
 */
static bool find_warn_flag( const char *flag, struct warning *table, unsigned int *posp )
{
    unsigned int ix = 0;

    while( table[ix].name != 0 ){
	if( strcmp( table[ix].name, flag ) == 0 ){
	    *posp = ix;
	    return true;
	}
	ix++;
    }
    return false;
}

/* Given a file handle 'f' and a table of warning flags 'table', print
 * an overview of the flags in the table to the given file.
 */
static void helpwarnflags( FILE *f, struct warning *table )
{
    unsigned int ix;
    unsigned int pos;
    size_t maxlen = strlen( "all" );

    fputs( "\nWarning flags:\n", f );
    ix = 0;
    while( table[ix].name != 0 ){
	size_t len = strlen( table[ix].name );

	if( len>maxlen ){
	    maxlen = len;
	}
	ix++;
    }
    ix = 0;
    while( table[ix].name != 0 ){
	pos = strlen( table[ix].name );
	fputc( ' ', f );
	fputs( table[ix].name, f );
	while( pos<maxlen ){
	    fputc( ' ', f );
	    pos++;
	}
	fputc( ' ', f );
	fputs( table[ix].descr, f );
	fputc( '\n', f );
	ix++;
    }
    pos = strlen( "all" );
    fputs( " all", f );
    while( pos<maxlen ){
	fputc( ' ', f );
	pos++;
    }
    fputs( " set all warning flags\n", f );
    fputs( "A flag can be reset by prefixing it with 'no'.\n", f );
}

/* Given a string that should contain a warning flag, and a table of
 * flags, try to find the string in the table, and set that flag.
 *
 * If the special flag "all" is given, *all* flags in the table are
 * set. If the flag is not found, but it contains the prefix 'no', and
 * the remainder matches a flag, that flag is reset.
 */
static void set_warn_flag( const char *flag, struct warning *table )
{
    unsigned int pos;

    if( strcmp( flag, "all" ) == 0 ){
	unsigned int ix = 0;

	while( table[ix].name != 0 ){
	    *(table[ix].flag) = 1;
	    ix++;
	}
    }
    if( find_warn_flag( flag, table, &pos ) ){
	*(table[pos].flag) = 1;
    }
    else if(
	flag[0] == 'n' &&
	flag[1] == 'o' &&
	find_warn_flag( flag+2, table, &pos )
    ){
	*(table[pos].flag) = 0;
    }
}

struct longoption {
    const char *name;
    bool *ptr;
};

static struct longoption long_options[] = {
    { "help", 0 },
    { "java", &restrict_java },
    { "verify", &verify_mode },
    { "java-array", &restrict_javaarray },
    { "noboundscheck", &pref_no_boundscheck },
    { "nocards", &restrict_nocards },
    { "nocomplex", &restrict_nocomplex },
    { "nodelete", &restrict_nodelete },
    { "noeach", &restrict_noeach },
    { "nogc", &pref_nogarbagecollection },
    { "noinline", &restrict_noinline },
    { "noinlining", &pref_no_inlining },
    { "nonotnullassert", &pref_nonotnullassert },
    { "nopragma", &restrict_nopragma },
    { "noprint", &restrict_noprint },
    { "nostring", &restrict_nostring },
    { "strict-analysis", &pref_strictanalysis },
    { "version", 0 },
    { 0, 0 }
};

/* Print version number to file 'f'. */
static void show_version( FILE *f )
{
    fprintf( f, "Spar frontend version: %s\n", spar_version );
}

/* Print usage message to file 'f'. */
static void show_usage( FILE *f )
{
    show_version( f );
    fputs(
"\n"
"Usage: sparfront [-d<debugflags>] [-o <outfile>] <infile>\n"
"or   : sparfront -h\n"
"\n"
"Flags:\n"
"-d <debugflags>: Switch on given debug flags (see below).\n"
"-e <outfile>:    Write errors to given file (default: standard error).\n"
"-h, --help:      Show this help text.\n"
"-o <outfile>:    Write output to given file (default: standard out).\n"
"-P <path>:       Prepend the given path to the search path.\n"
"--verify:        Verify mode.\n"
"--version:       Show the version number of the program.\n"
"-W <warnflag>:   Switch on given warning flag (see below).\n"
"--nocards:       Do not allow cardinality lists.\n"
"--nocomplex:     Do not treat 'complex' as a keyword (so no complex type).\n"
"--nodelete:      Do not treat 'delete' as a keyword.\n"
"--noeach:        Do not treat 'each' and 'foreach' as keywords.\n"
"--nogc:          Do generate garbage collection code.\n"
"--strict-analysis:Keep analysis strictly conformant with Java.\n"
"--noinline:      Do not allow 'inline' keyword.\n"
"--noinlining:    Do not automatically inline any methods.\n"
"--nopragma:      Do not allow pragmas.\n"
"--noboundscheck: Do not generate code for bounds checking.\n"
"--noprint:       Do not treat '__print' and '__println' as keywords.\n"
"--java-array:    Only allow Java array declarations.\n"
"--java:          Restrict input to the Java subset (equivalent to --nocards\n"
"                 --nocomplex --nodelete --noeach --noinline --nopragma\n"
"                 --noprint --java-array).\n"
        ,
	f
    );
    helpwarnflags( f, warnings );
    fputs( "\n", f );
    helpdbflags( f, flagtab );
}

/* Construct the search path for Spar sources.
 *
 * Before anything else, the sparpath is populated with the values of
 * DEVEL_SPARLIB_PATH, INSTALL_SPARLIB_PATH and the environment variable
 * KAFFELIB_PATH. DEVEL_SPARLIB_PATH is only valid in the development
 * environment. INSTALL_SPARLIB_PATH points to the directory where the
 * sparlib stuff would be installed (but might not be during development).
 * KAFFELIB_PATH points to the path where the kaffe library is installed.
 *
 * We do not have any control over the last one; we can only hope that
 * the user has set that environment variable right, although at configuration
 * time this is checked.
 *
 * After all this standard stuff, all paths in SPARPATH are appended.
 * These are supposed to point to user libraries.
 */
static void construct_sparpath()
{
    sparpath = new_tmstring_list();

#if defined( DEVEL_SPARLIB_PATH )
    sparpath = append_tmstring_list(
	sparpath,
	new_tmstring( DEVEL_SPARLIB_PATH )
    );
#endif
#if defined( INSTALL_SPARLIB_PATH )
    sparpath = append_tmstring_list(
	sparpath,
	new_tmstring( INSTALL_SPARLIB_PATH )
    );
#endif
    const char *kaffepath = getenv( "KAFFEROOT" );
    if( kaffepath == NULL ){
	fprintf( stderr, "You must set environment variable KAFFEROOT\n" );
	exit( 2 );
    }
    if( kaffepath[0] == '\0' ){
	fprintf( stderr, "an empty KAFFEROOT environment variable is not acceptable\n" );
	exit( 2 );
    }
    tmstring long_kaffepath = printf_tmstring( "%s/libraries/javalib", kaffepath );
    sparpath = append_tmstring_list( sparpath, long_kaffepath );
    const char *pathstring = getenv( "SPARPATH" );

    if( pathstring != NULL ){
	tmstring buf = rdup_tmstring( pathstring );
	char *p = (char *) buf;

	while( p[0] != '\0' ){
	    char *cp = strchr( p, ':' );
	    char *nextp;

	    if( cp != NULL ){
		/* a ':' was found. */
		nextp = cp+1;
	    }
	    else {
		cp = p+strlen( p );
		nextp = cp;
	    }
	    if( cp>p && cp[-1] == PATHSEP ){
		/* Zap path separator at the end of the path. */
		cp[-1] = '\0';
	    }
	    cp[0] = '\0';
	    sparpath = append_tmstring_list( sparpath, new_tmstring( p ) );
	    p = nextp;
	}
	rfre_tmstring( buf );
    }
}

// We cannot find SANITYCHECK_CLASS. Something must be very rotten, so
// complain.
static void complain_bad_sparpath( FILE *f )
{
    fprintf(
	f,
	"The class '" SANITYCHECK_CLASS "' cannot be found. Something must be wrong.\n"
    );
    fprintf(
	f,
	"I have searched the following directories:\n" 
    );
    if( sparpath == NULL ){
	fputs( "(sparpath is null.)\n", f );
    }
    else {
	for( unsigned int ix=0; ix<sparpath->sz; ix++ ){
	    fputs( sparpath->arr[ix], f );
	    fputc( '\n', f );
	}
    }
}

/* Same as fopen, but give error message if file can't be opened */
static FILE *ckfopen( const char *nm, const char *acc )
{
    FILE *hnd = fopen( nm, acc );

    if( NULL == hnd ){
	(void) strcpy( errarg, nm );
	sys_error( errno );
	exit( 2 );
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

// Given an option 'opt' and the next commandline argument if any,
// parse this option.
// Return true iff the next commandline argument is used by this
// option, and should be skipped by the loop traversing the arguments.
static bool parse_long_option( const char *opt )
{
    unsigned int ix=0;

    if( strcmp( opt, "help" ) == 0 ){
	show_usage( stdout );
	exit( 0 );
    }
    if( strcmp( opt, "version" ) == 0 ){
	show_version( stdout );
	exit( 0 );
    }
    bool found = false;
    while( long_options[ix].name != NULL ){
	if( strcmp( opt, long_options[ix].name ) == 0 ){
	    *(long_options[ix].ptr) = true;
	    found = true;
	    break;
	}
	ix++;
    }
    if( !found ){
	fprintf( stderr, "Unknown option --%s\n", opt );
	show_usage( stderr );
	exit( 1 );
    }
    return false;
}

// Given an option 'opt' and the next commandline argument if any,
// parse this option.
// Return true iff the next commandline argument is used by this
// option, and should be skipped by the loop traversing the arguments.
static bool parse_option( const char *opt, const char *nextarg )
{
    bool skipit = false;
    tmstring_list user_path = new_tmstring_list();

    switch( opt[0] ){
	case '\0':
	    internal_error( "empty option should not happen" );
	    break;

	case '-':
	    skipit = parse_long_option( opt+1 );
	    break;

	case 2:
	    show_version( stdout );
	    exit(0);

	case 'W':
	    if( opt[1] == '\0' ){
		set_warn_flag( nextarg, warnings );
		skipit = true;
	    }
	    else {
		set_warn_flag( opt+1, warnings );
	    }
	    break;

	case 'd':
	    if( opt[1] == '\0' ){
		setdbflags( nextarg, flagtab, TRUE );
		skipit = true;
	    }
	    else {
		setdbflags( opt+1, flagtab, TRUE );
	    }
	    break;

	case 'e':
	    if( errfilename != tmstringNIL ){
		rfre_tmstring( errfilename );
	    }
	    if( opt[1] == '\0' ){
		errfilename = new_tmstring( nextarg );
		skipit = true;
	    }
	    else {
		errfilename = new_tmstring( opt+1 );
	    }
	    break;

	case '?':
	case 'h':
	    show_usage( stdout );
	    exit( 0 );

	case 'o':
	    if( outfilename != tmstringNIL ){
		rfre_tmstring( outfilename );
	    }
	    if( opt[1] == '\0' ){
		outfilename = new_tmstring( nextarg );
		skipit = true;
	    }
	    else {
		outfilename = new_tmstring( opt+1 );
	    }
	    break;

	case 'P':
	{
	    tmstring path = tmstringNIL;

	    if( opt[1] == '\0' ){
		path = new_tmstring( nextarg );
		skipit = true;
	    }
	    else {
		path = new_tmstring( opt+1 );
	    }
	    if( path != tmstringNIL ){
		user_path = append_tmstring_list( user_path, path );
	    }
	    break;
	}

	case 'Z':
	{
	    tmstring path = tmstringNIL;
	    int codesize;

	    if( opt[1] == '\0' ){
		codesize = atoi( nextarg );
		skipit = true;
		if( codesize<=0 ){
		    fprintf( stderr, "Bad code size value '%s'\n", nextarg );
		    exit( 1 );
		}
	    }
	    else {
		codesize = atoi( opt+1 );
		if( codesize<=0 ){
		    fprintf( stderr, "Bad code size value '%s'\n", opt+1 );
		    exit( 1 );
		}
	    }
	    inlinable_codesize = codesize;
	    if( path != tmstringNIL ){
		user_path = append_tmstring_list( user_path, path );
	    }
	    break;
	}


	default:
	    fprintf( stderr, "Unknown option '-%c'\n", opt[0] );
	    show_usage( stderr );
	    exit( 1 );
    }
    // Prefix the user paths before the standard sparpath.
    sparpath = concat_tmstring_list( user_path, sparpath );
    return skipit;
}

/* Scan arguments and options. */
static void scanargs( int argc, char *argv[] )
{
    bool got_infile = false;

    for( int ix=1; ix<argc; ix++ ){
	const char *arg = argv[ix];

	if( arg[0] == '-' && arg[1] != '\0' ){
	    const char *nextarg = (const char *) NULL;

	    if( (ix+1)<argc ){
		nextarg = argv[ix+1];
	    }
	    if( parse_option( arg+1, nextarg ) ){
		ix++;
	    }
	}
	else {
	    if( got_infile ){
		fprintf( stderr, "At most one input file can be given\n" );
		show_usage( stderr );
		exit( 1 );
	    }
	    else {
		rfre_tmstring( infilename );
		infilename = new_tmstring( arg );
		infile = ckfopen( infilename, "r" );
		got_infile = true;
	    }
	}
    }
    if( restrict_nostring ){
	spar_features &= ~FEAT_STRING;
    }
    if( restrict_nocomplex ){
	spar_features &= ~FEAT_COMPLEX;
    }
    if( restrict_nopragma ){
	spar_features &= ~FEAT_PRAGMA;
    }
    if( restrict_nodelete ){
	spar_features &= ~FEAT_DELETE;
    }
    if( restrict_noeach ){
	spar_features &= ~FEAT_PARALLEL;
    }
    if( restrict_noinline ){
	spar_features &= ~FEAT_INLINE;
    }
    if( restrict_noprint ){
	spar_features &= ~FEAT_PRINT;
    }
    if( restrict_nocards ){
	spar_features &= ~FEAT_CARDS;
    }
    if( restrict_javaarray ){
	spar_features &= ~FEAT_ARRAY;
    }
    if( restrict_java ){
	spar_features = FEAT_JAVA;
    }
    java_features = FEAT_JAVA;
}

// Given a list of declarations that form the body of the main class
// return true iff there is an acceptable method 'main'.
// If there is a main( String args ) method, set has_long_main to
// true, else set it to false.
// one, this will be invoked. If there is no other method 'main',
// or if all other methods 'main' have more than one parameter, complain.
static bool has_main( const_statement_list decls, bool *has_long_main )
{
    tmsymbol main = add_tmsymbol( "main" );
    bool res = false;

    *has_long_main = false;
    for( unsigned int ix=0; ix<decls->sz; ix++ ){
	const_statement s = decls->arr[ix];

	if( s->tag == TAGFunctionDeclaration ){
	    const_FunctionDeclaration fn = to_const_FunctionDeclaration( s );

	    if( fn->name->sym == main ){
		if( fn->parameters->sz == 0 ){
		    // We know the answer: there *is* a 'main()' method.
		    res = true;
		}
		if( fn->parameters->sz == 1 ){
		    FormalParameter fp = fn->parameters->arr[0];
		    const_type t = fp->t;

		    if( t->tag == TAGArrayType ){
			if( is_String_type( to_const_ArrayType( t )->elmtype ) ){
			    *has_long_main = true;
			    res = true;
			}
		    }
		}
	    }
	}
    }
    return res;
}

// Given a type declaration 'decl' and the path of the file
// it was found in, return true iff it is the main class of
// this file, because it has the same name as its file.
static bool is_main_type( const_TypeDeclaration decl, const char *path )
{
    tmstring buf = new_tmstring( path );

    char *p = strrchr( buf, PATHSEP );
    if( p == NULL ){
	p = buf;
    }
    else {
	p++;
    }
    char *xp = strrchr( p, '.' );
    if( xp == NULL ){
	rfre_tmstring( buf );
	return false;
    }
    *xp = '\0';
    /* TODO: take into account mangling of difficult characters. */
    /* See also the related functions in doimports.c */
    bool res = strcmp( p, decl->name->sym->name ) == 0;
    rfre_tmstring( buf );
    return res;
}

// Given a SparProgramUnit 'unit', return the name of the (first)
// public class or interface of this unit.
//
// This class presumably has the main method.
static TypeDeclaration search_main_type( const SparProgramUnit unit )
{
    statement_list decls = unit->decls;

    for( unsigned int ix=0; ix<decls->sz; ix++ ){
	statement decl = decls->arr[ix];

	if( is_TypeDeclaration( decl ) ){
	    TypeDeclaration tdecl = to_TypeDeclaration( decl );

	    if( is_main_type( tdecl, unit->path ) ){
		return to_TypeDeclaration( decl );
	    }
	}
    }
    return TypeDeclarationNIL;
}

// Construct a wrapper method to build the correct argument vector
// and invoke 'main( String argv[] )' with it. If there is a 'main()'
// method, this wrapper method is never invoked, so it won't be compiled,
// and the incorrect invocation of 'main( String argv[] )' is not
// exposed.
static MethodDeclaration build_main_wrapper()
{
    origsymbol methodnm = gen_origsymbol( "main" );
    statement_list statements = new_statement_list();
    MethodInvocationExpression argv_build = new_MethodInvocationExpression(
	new_MethodInvocation(
	    expressionNIL,		// vtable
	    add_origsymbol( "spar.compiler.Argv.buildArgv" ),
	    new_expression_list(),	// This parameters
	    new_expression_list(),	// Parameters
	    0
	)
    );
    statement invocation = new_MethodInvocationStatement(
	gen_origin(),
	Pragma_listNIL,
	origsymbol_listNIL,	// Labels
	new_MethodInvocation(
	    expressionNIL,		// vtable
	    add_origsymbol( "main" ),
	    new_expression_list(),		// This parameter
	    append_expression_list(
		new_expression_list(),
		argv_build
	    ),
	    0
	)
    );
    Block fnbody = new_Block( tmsymbolNIL, Pragma_listNIL, statements );

    statements = append_statement_list(
	statements,
	invocation
    );
    type_list throws = new_type_list();
    // This wrapper function has the clause 'throws java.lang.Throwable',
    // since we don't want complaints about invoking the real main
    // if it has a throws clause.
    throws = append_type_list(
	throws,
	new_ObjectType( add_origsymbol( "java.lang.Throwable" ) )
    );
    return new_FunctionDeclaration(
	gen_origin(),
	Pragma_listNIL,
	origsymbol_listNIL,		// Labels
	ACC_STATIC|ACC_FINAL,
	false,				// used?
	methodnm,
	new_FormalParameter_list(),	// Formal parameters
	throws,				// Throws clause
	new_VoidType(),
	fnbody
    );
}

static SparProgram wrap_topunit( SparProgramUnit topunit, const char *fnm )
{
    tmsymbol mn = tmsymbolNIL;
    SparProgram theprog;
    tmsymbol packagename = tmsymbolNIL;
    TypeDeclaration main_type = search_main_type( topunit );

    TypeBinding_list typebindings = new_TypeBinding_list();
    if( main_type == TypeDeclarationNIL ){
	if( verify_mode ){
	    pref_no_codegeneration = TRUE;
	}
	else {
	    error( "File '%s' does not contain a class with the same name", fnm );
	    rfre_TypeBinding_list( typebindings );
	    return SparProgramNIL;
	}
    }
    else {
	bool has_long_main;

	if( has_main( main_type->body, &has_long_main ) ){
	    if( has_long_main ){
		MethodDeclaration wrapper = build_main_wrapper();
		main_type->body = append_statement_list( main_type->body, wrapper );
		mn = wrapper->name->sym;
	    }
	    else {
		mn = add_tmsymbol( "main" );
	    }
	    mn = qualify_tmsymbol( main_type->name->sym, mn );
	    if( topunit->packagename != origsymbolNIL ){
		packagename = topunit->packagename->sym;
		mn = qualify_tmsymbol( packagename, mn );
	    }
	    if( topunit->packagename != origsymbolNIL ){
		typebindings = append_TypeBinding_list(
		    typebindings,
		    new_TypeBinding( packagename, packagename )
		);
	    }
	}
	else {
	    if( verify_mode ){
		// There is no main, but no worries, we're in verify mode.
		pref_no_codegeneration = TRUE;
	    }
	    else {
		origin_error(
		    main_type->name->org,
		    "Class does not contain a 'main(String v[])' or 'main()' method"
		);
		rfre_TypeBinding_list( typebindings );
		return SparProgramNIL;
	    }
	}
    }
    theprog = new_SparProgram(
	rdup_Pragma_list( topunit->pragmas ),
	append_SparProgramUnit_list( new_SparProgramUnit_list(), topunit ),
	new_tmsymbol_list(),		/* inits */
	new_GenericMapping_list(),
	mn,				/* The name of the main program. */
	add_tmsymbol( EXCEPTION_HANDLER_NAME ),
	tmsymbolNIL,
	tmsymbolNIL,
	tmsymbolNIL,
	tmsymbolNIL,
	new_TypeEntry_list(),
	new_Entry_list(),
	typebindings
    );
    fwdregtype_SparProgramUnit( &theprog, topunit );
    regtype_SparProgramUnit( &theprog, topunit, fnm );
    return theprog;
}

int main( int argc, char *argv[] )
{
    SparProgramUnit topunit;
    SparProgram theprog;

    construct_sparpath();
    infile = stdin;
    outfile = stdout;
    infilename = new_tmstring( "" );
    init_lex();
    scanargs( argc, argv );
    if( outfilename != tmstringNIL ){
	outfile = ckfopen( outfilename, "w" );
    }
    if( errfilename != tmstringNIL ){
        ckfreopen( errfilename, "w", stderr );
    }
    if( !java_lang_visible() ){
	complain_bad_sparpath( stderr );
	exit( 2 );
    }
    topunit = parse( infile, infilename, has_java_suffix( infilename ) );
    errcheck();
    topunit = lower_SparProgramUnit( topunit );
    if( dumploweredtree ){
	TMPRINTSTATE *st = tm_setprint( stderr, 1, 75, 8, 0 );
	print_SparProgramUnit( st, topunit );
	int level = tm_endprint( st );
	if( level>0 ){
	    fprintf( stderr, "Internal botch: bracket level is %d\n", level );
	}
	(void) fflush( stderr );
    }
    theprog = wrap_topunit( topunit, infilename );
    if( dumptree ){
	TMPRINTSTATE *st = tm_setprint( stderr, 1, 75, 8, 0 );
	print_SparProgram( st, theprog );
	int level = tm_endprint( st );
	if( level>0 ){
	    fprintf( stderr, "Internal botch: bracket level is %d\n", level );
	}
	(void) fflush( stderr );
    }
    errcheck();
    theprog = mangle_SparProgram( theprog, topunit, verify_mode );
    errcheck();
    if( dumpmangledtree ){
	TMPRINTSTATE *st = tm_setprint( stderr, 1, 75, 8, 0 );
	print_SparProgram( st, theprog );
	int level = tm_endprint( st );
	if( level>0 ){
	    fprintf( stderr, "Internal botch: bracket level is %d\n", level );
	}
    }
    theprog = constant_fold_SparProgram( theprog );
    if( !dontcheck ){
	check_SparProgram( theprog );
    }
    errcheck();
    theprog = rewrite_SparProgram( theprog );
    if( !dontcheck ){
	check_SparProgram( theprog );
    }
    errcheck();
    if( pref_no_boundscheck ){
	Pragma p = new_ValuePragma(
	    add_origsymbol( "boundscheck" ),
	    new_BooleanPragmaExpression( false )
	);
	theprog->pragmas = insert_Pragma_list( theprog->pragmas, 0, p );
    }
    if( !pref_no_codegeneration ){
	markused_SparProgram( theprog );
	if( !pref_nogarbagecollection ){
	    addgc_SparProgram( theprog );
	}
	pp_SparProgram( outfile, theprog );
    }
    rfre_SparProgram( theprog );
    end_lex();
    fre_tmstring( infilename );
    fre_tmstring( outfilename );
    fre_tmstring( errfilename );
    rfre_tmstring_list( sparpath );
    clear_arraymarkers();
    flush_tmsymbol();
    if( get_balance_tmadmin() != 0 || get_balance_tmstring() != 0 ){
	fprintf( stderr, "Tm object allocation not balanced\n" );
	givestat = true;
    }
    if( givestat ){
	stat_lex( stderr );
	stat_tmadmin( stderr );
	stat_tmstring( stderr );
	report_lognew( stderr );
    }
    flush_lognew();
    exit( 0 );
    /* NOTREACHED */
    return 0;
}
