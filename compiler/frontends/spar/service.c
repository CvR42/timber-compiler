/* File: service.c
 *
 * Miscellaneous service routines.
 */

#include <tmc.h>
#include <assert.h>
#include <stdarg.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "defs.h"
#include "tmadmin.h"
#include "global.h"
#include "service.h"
#include "error.h"
#include "typederive.h"
#include "analyze.h"
#include "isconstant.h"
#include "typename.h"
#include "constfold.h"
#include "dump.h"

// Hack to write around a small header file problem
#if !defined( S_IFREG ) && defined(__S_IFREG)
#define S_IFREG __S_IFREG
#endif

bool search_tmsymbol_list( const_tmsymbol_list l, const tmsymbol s, unsigned int *ixp )
{
    for( unsigned int ix=0; ix<l->sz; ix++ ){
	if( l->arr[ix] == s ){
	    *ixp = ix;
	    return true;
	}
    }
    return false;
}

bool member_tmsymbol_list( const_tmsymbol_list l, const tmsymbol s )
{
    for( unsigned int ix=0; ix<l->sz; ix++ ){
	if( l->arr[ix] == s ){
	    return true;
	}
    }
    return false;
}

bool member_origsymbol_list( const_origsymbol_list l, const_origsymbol s )
{
    for( unsigned int ix=0; ix<l->sz; ix++ ){
	if( l->arr[ix]->sym == s->sym ){
	    return true;
	}
    }
    return false;
}

tmsymbol_list add_tmsymbol_list( tmsymbol_list l, tmsymbol s )
{
    if( !member_tmsymbol_list( l, s ) ){
	l = append_tmsymbol_list( l, s );
    }
    return l;
}

tmsymbol_list add_tmsymbol_list( tmsymbol_list l, const_tmsymbol_list le )
{
    if( le != tmsymbol_listNIL ){
	for( unsigned int ix=0; ix<le->sz; ix++ ){
	    l = add_tmsymbol_list( l, le->arr[ix] );
	}
    }
    return l;
}

origsymbol_list add_origsymbol_list( origsymbol_list l, const_origsymbol s )
{
    if( !member_origsymbol_list( l, s ) ){
	l = append_origsymbol_list( l, rdup_origsymbol( s ) );
    }
    return l;
}

origin gen_origin()
{
    static int gen_counter = 0;

    gen_counter++;
    // Put a breakpoint on this line, with the stop condition on
    // the desired generated symbol.
#if 0
    if( gen_counter == 12 ){
	yup();
    }
#endif
    return new_origin( add_tmsymbol( "(generated)" ), gen_counter );
}

origsymbol gen_origsymbol( const char *prefix )
{
    origsymbol res = new_origsymbol(
	gen_tmsymbol( prefix ),
	gen_origin()
    );
    return res;
}

// Given a tmsymbol, return an origsymbol with a synthetic origin.
origsymbol add_origsymbol( const tmsymbol s )
{
    if( s == tmsymbolNIL ){
	return origsymbolNIL;
    }
    return new_origsymbol( s, gen_origin() );
}

// Given a string, return an origsymbol representing a symbol with
// that string as name.
origsymbol add_origsymbol( const char *str )
{
    return add_origsymbol( add_tmsymbol( str ) );
}

BINOP assignop_to_binop( const ASSIGN_OP op )
{
    BINOP res = BINOP_PLUS;

    switch( op ){
	case ASSIGN_PLUS:		res = BINOP_PLUS;		break;
	case ASSIGN_MINUS:		res = BINOP_MINUS;		break;
	case ASSIGN_TIMES:		res = BINOP_TIMES;		break;
	case ASSIGN_DIVIDE:		res = BINOP_DIVIDE;		break;
	case ASSIGN_MOD:		res = BINOP_MOD;		break;
	case ASSIGN_XOR:		res = BINOP_XOR;		break;
	case ASSIGN_AND:		res = BINOP_AND;		break;
	case ASSIGN_OR:			res = BINOP_OR;			break;
	case ASSIGN_SHIFTLEFT:		res = BINOP_SHIFTLEFT;		break;
	case ASSIGN_SHIFTRIGHT:		res = BINOP_SHIFTRIGHT;		break;
	case ASSIGN_USHIFTRIGHT:	res = BINOP_USHIFTRIGHT;	break;
	case ASSIGN:
	    internal_error( "Plain assignment does not have a corresponding binary operator" );
	    break;
    }
    return res;
}

/* Given an assignment expression, convert it to an assignment statement. */
AssignStatement transmog_AssignmentExpression( AssignOpExpression x, const_origin org )
{
    AssignStatement res;
    if( x->op != ASSIGN && maybe_has_sideeffect( x->right, NO_SE_EXCEPTION|NO_SE_NEW|NO_SE_CLASSINIT ) ){
	res = new_AssignStatement(
	    rdup_origin( org ),
	    Pragma_listNIL,
	    origsymbol_listNIL,	// Labels
	    ASSIGN,
	    x->left,
	    new_BinopExpression(
		rdup_expression( x->left ),
		assignop_to_binop( x->op ),
		x->right
	    )
	);
    }
    else {
	res = new_AssignStatement(
	    rdup_origin( org ),
	    Pragma_listNIL,
	    origsymbol_listNIL,	// Labels
	    x->op,
	    x->left,
	    x->right
	);
    }
    fre_expression( x );
    return res;
}

/* Given a printf format string 'fmt' and a list of argumemts 'args', return
 * the size of the resulting string.
 *
 * Only a subset of the general printf formatting is accepted.
 */
static size_t printf_len( const char *fmt, va_list args )
{
    const char *p = fmt;
    size_t sz = 0;

    while( *p != '\0' ){
	char c = *p++;

	if( c != '%' ){
	    sz++;
	    continue;
	}
	c = *p++;
	switch( c ){
	    case '%':
		sz += 1;
		break;

	    case 'c':
	    {
		// A char is promoted to an int when passed through '...'.
		const char arg = va_arg( args, int );

		(void) arg;
		sz += 1;
		break;
	    }

	    case 'd':
	    {
		int arg = va_arg( args, int );

		if( arg<0 ){
		    arg = -arg;
		    sz++;
		}
		if( arg == 0 ){
		    sz += 1;
		}
		while( arg>0 ){
		    sz++;
		    arg /= 10;
		}
		break;
	    }

	    case 'l':
	    {
		c = *p++;

		switch( c ){
		    case 'l':
		    {
			c = *p++;

			switch( c ){
			    case 'd':
			    {
				long long int arg = va_arg( args, long long int );

				if( arg<0 ){
				    arg = -arg;
				    sz++;
				}
				if( arg == 0 ){
				    sz += 1;
				}
				while( arg>0 ){
				    sz++;
				    arg /= 10;
				}
				break;
			    }

			    default:
				sprintf( errarg, "format string: '%s'", fmt );
				internal_error( "cannot determine length of formatted string" );
				break;
			}
			break;
		    }

		    case 'd':
		    {
			long int arg = va_arg( args, long int );

			if( arg<0 ){
			    arg = -arg;
			    sz++;
			}
			if( arg == 0 ){
			    sz += 1;
			}
			while( arg>0 ){
			    sz++;
			    arg /= 10;
			}
			break;
		    }

		    default:
			sprintf( errarg, "format string: '%s'", fmt );
			internal_error( "cannot determine length of formatted string" );
			break;
		}
		break;
	    }

	    case 'u':
	    {
		unsigned int arg = va_arg( args, unsigned int );

		if( arg == 0 ){
		    sz += 1;
		}
		while( arg>0 ){
		    sz++;
		    arg /= 10;
		}
		break;
	    }

	    case 'x':
	    {
		unsigned int arg = va_arg( args, unsigned int );

		if( arg == 0 ){
		    sz += 1;
		}
		while( arg>0 ){
		    sz++;
		    arg /= 16;
		}
		break;
	    }

	    case 's':
	    {
		const char *arg = va_arg( args, char * );

		sz += strlen( arg );
		break;
	    }

	    default:
		sprintf( errarg, "format string: '%s'", fmt );
		internal_error( "cannot determine length of formatted string" );
		break;
	}
    }
    return sz;
}

tmstring printf_tmstring( const char *fmt, ... )
{
    va_list args;

    va_start( args, fmt );
    size_t sz = 1+printf_len( fmt, args );
    va_end( args );
    tmstring res = create_tmstring( sz );
    va_start( args, fmt );
    (void) vsprintf( (char *) res, fmt, args );
    va_end( args );
    return res;
}

tmstring qualify_tmstring( const char *prestr, const char *suffstr )
{
    if( prestr[0] == '\0' ){
	return new_tmstring( suffstr );
    }
    return printf_tmstring( "%s.%s", prestr, suffstr );
}

tmsymbol qualify_tmsymbol( const char *prestr, tmsymbol suff )
{
    if( suff == tmsymbolNIL ){
	return add_tmsymbol( prestr );
    }
    tmstring buf = qualify_tmstring( prestr, suff->name );
    tmsymbol res = add_tmsymbol( buf );

    rfre_tmstring( buf );
    return res;
}

tmsymbol qualify_tmsymbol( tmsymbol pre, tmsymbol suff )
{
    if( pre == tmsymbolNIL ){
	return suff;
    }
    if( suff == tmsymbolNIL ){
	return pre;
    }
    return qualify_tmsymbol( pre->name, suff );
}

/* Given a qualified name, return the short name. */
tmsymbol shortname( tmsymbol s )
{
    if( s == tmsymbolNIL ){
	return tmsymbolNIL;
    }
    const char *p = strrchr( s->name, '.' );
    if( p == NULL ){
	return s;
    }
    else {
	return add_tmsymbol( p+1 );
    }
}

// Given a qualified name, return the first part of that name.
static tmsymbol firstpart_tmsymbol( tmsymbol s )
{
    tmstring str = new_tmstring( s->name );
    char *p = strchr( str, '.' );
    if( p == NULL ){
	rfre_tmstring( str );
	return s;
    }
    p[0] = '\0';
    tmsymbol res = add_tmsymbol( str );
    rfre_tmstring( str );
    return res;
}

// Given a qualified name, return the first part of that name.
origsymbol firstpart_origsymbol( const_origsymbol s )
{
    origsymbol res = rdup_origsymbol( s );

    res->sym = firstpart_tmsymbol( res->sym );
    return res;
}

/* Given a character 'c' and a flag table 'flagtab',
 * return a pointer to the entry of character 'c', or return
 * dbflagNIL if not found.
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
    const dbflag *p = flagtab;

    fputs( "Debugging flags:\n", f );
    while( p->flagchar != '\0' ){
	fprintf( f, " %c - %s.\n", p->flagchar, p->flagdescr );
	p++;
    }
    fputs( " . - all flags.\n", f );
}

/* Given a string 's' containing debug flags, a flag table 'flagtab' and
 * a value 'val', look up all flags given in 's', and set the associated
 * value to 'val'. The flag character '.' means that all flags must
 * be set.
 */
void setdbflags( const char *s, dbflag *flagtab, int val )
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

/* Given the name of the variable and the origin, construct a
 * VariableNameExpression for this one. The origin is 'owned' by the
 * returned expression.
 */
origsymbol create_variable( const char *str, origin org )
{
    return new_origsymbol( add_tmsymbol( str ), org );
}

bool is_qualified_tmsymbol( tmsymbol nm )
{
    return nm != tmsymbolNIL && strchr( nm->name, '.' ) != NULL;
}

bool is_qualified_origsymbol( const_origsymbol nm )
{
    return is_qualified_tmsymbol( nm->sym );
}

/* Given a qualified name 'pre' and a qualified name 's', return true iff 'pre'
 * the sequence of simple names in 'pre' is a prefix of the sequence of
 * simple names in 's'.
 */
static bool is_qualified_prefix( const char *pre, const char *s )
{
    size_t sz = strlen( pre );

    if( sz == 0 ){
	// An empty name is always a prefix.
	return true;
    }
    if( strncmp( pre, s, sz ) == 0 && s[sz] == '.' ){
	return true;
    }
    return false;
}

/* Given a qualified name 'pre' and a qualified name 's', return true iff 'pre'
 * the sequence of simple names in 'pre' is a prefix of the sequence of
 * simple names in 's'.
 */
bool is_qualified_prefix( tmsymbol pre, tmsymbol s )
{
    return is_qualified_prefix( pre->name, s->name );
}

/* Given an expression 'x' and a desired basetype 't', return the expression,
 * but possibly cast to convert it to the desired type.
 */
expression force_basetype( const_Entry_list symtab, expression x, const BASETYPE t )
{
    const type actt = derive_type_expression( symtab, x );
    expression ans;

    if( actt->tag == TAGPrimitiveType && to_PrimitiveType(actt)->base == t ){
	ans = x;
    }
    else {
	ans = constant_fold_expression( new_CastExpression( new_PrimitiveType( t ), x ) );
    }
    rfre_type( actt );
    return ans;
}

/* Given an expression list 'l' and a desired basetype 't', return the
 * expression list, but if necessary expressions are cast to convert them
 * to the desired type.
 */
expression_list force_basetype( const_Entry_list symtab, const expression_list l, const BASETYPE t )
{
    for( unsigned int ix=0; ix<l->sz; ix++ ){
	l->arr[ix] = force_basetype( symtab, l->arr[ix], t );
    }
    return l;
}

// Given an actual dim expression 'x', force its vector to be a
// vector of int expressions.
static ActualDim force_int_vector( const_Entry_list symtab, ActualDim x )
{
    if( x->vector->tag == TAGVectorExpression ){
	// The vector is in fact a vector expression. This one
	// is easy to do.
	to_VectorExpression(x->vector)->fields = force_basetype( symtab, to_VectorExpression(x->vector)->fields, BT_INT );
	return x;
    }
    const type xt = derive_type_expression( symtab, x->vector );
    if( is_tuple_type( xt ) ){
	unsigned int sz;

	if( !extract_tuple_length( originNIL, xt, &sz ) ){
	    sz = 0;
	}
	// This is what we want it to be: [int^sz]
	type tt = new_VectorType(
	    new_PrimitiveType( BT_INT ),
	    new_IntExpression( (int) sz )
	);
	x->vector = constant_fold_expression( force_type( symtab, x->vector, tt ) );
	rfre_type( tt );
    }
    rfre_type( xt );
    return x;
}

/* Given an ActualDim list 'l', return the
 * ActualDim list, but if necessary vectors in each ActualDim are cast to
 * convert them to vectors of int expressions.
 */
ActualDim_list force_int_vectors( const_Entry_list symtab, const ActualDim_list l )
{
    for( unsigned int ix=0; ix<l->sz; ix++ ){
	l->arr[ix] = force_int_vector( symtab, l->arr[ix] );
    }
    return l;
}

/* Given an expression 'x' and a desired type 't', return a new
 * expression that may contain a cast to convert to the desired type.
 */
expression force_type( const_Entry_list symtab, expression x, const_type t )
{
    const type actt = derive_type_expression( symtab, x );
    expression ans;

    assert( t != typeNIL );
    if( actt == typeNIL ){
	return x;
    }
    if( is_equivalent_type( t, actt ) ){
	ans = x;
    }
    else {
	ans = constant_fold_expression( new_CastExpression( rdup_type( t ), x ) );
    }
    rfre_type( actt );
    return ans;
}

/* Given an expression 'x' and a desired type 't', return a new
 * expression that may contain a cast to convert to the desired type.
 */
optexpression force_type( const_Entry_list symtab, optexpression x, const_type t )
{
    switch( x->tag ){
	case TAGOptExprNone:
	    break;

	case TAGOptExpr:
	    to_OptExpr(x)->x = force_type( symtab, to_OptExpr(x)->x, t );
    }
    return x;
}

/* Given a list of expressions representing actual parameters, and given
 * a list of formal parameters, rewrite the actual parameters to the
 * final form for output.
 */
expression_list force_type(
 const_Entry_list symtab,
 const_origsymbol fn,
 expression_list l,
 const_type_list fl
)
{
    if( l->sz != fl->sz ){
	origsymbol_internal_error(
	    fn,
	    "formals and actuals differ in length (%u vs. %u)",
	    fl->sz,
	    l->sz
	);
    }
    for( unsigned int ix=0; ix<l->sz; ix++ ){
	l->arr[ix] = force_type( symtab, l->arr[ix], fl->arr[ix] );
    }
    return l;
}

/* Given a qualified symbol, break it into two new symbols at the
 * last '.'.
 */
void break_qualified_tmsymbol( tmsymbol nm, tmsymbol *first, tmsymbol *last )
{
    tmstring buf = rdup_tmstring( nm->name );
    char *p = strrchr( buf, '.' );

    if( p == NULL ){
	rfre_tmstring( buf );
	*first = add_tmsymbol( "" );
	*last = nm;
	return;
    }
    *p = '\0';
    *first = add_tmsymbol( buf );
    *last = add_tmsymbol( p+1 );
    rfre_tmstring( buf );
}

/* Given a qualified symbol, break it into two new symbols at the
 * last '.'.
 */
void break_qualified_name( const_origsymbol nm, origsymbol *first, origsymbol *last )
{
    tmsymbol my_first;
    tmsymbol my_last;

    break_qualified_tmsymbol( nm->sym, &my_first, &my_last );
    *first = new_origsymbol( my_first, rdup_origin( nm->org ) );
    *last = new_origsymbol( my_last, rdup_origin( nm->org ) );
}

/* Given the path to a file, return true iff that file exists. */
bool file_exists( const char *fnm )
{
    struct stat statbuf;

    if( fnm == NULL ){
	return false;
    }
    if( stat( fnm, &statbuf ) != 0 ){
	/* Can't find the file. */
	return false;
    }
    if( statbuf.st_mode & S_IFREG ){
        /* It is an ordinary file. */
	return true;
    }
    return false;
}

/* Given a path, possibly containing a terminating path separator,
 * and a file name, return a string containing the full path to the file.
 */
tmstring fullpath_file( const char *path, const char *file )
{
    tmstring buf;
    size_t pathlen = strlen( path );

    buf = create_tmstring( pathlen+strlen( file )+2 );
    if( pathlen>0 && path[pathlen-1] == PATHSEP ){
	sprintf( buf, "%s%s", path, file );
    }
    else {
	sprintf( buf, "%s%c%s", path, PATHSEP, file );
    }
    return buf;
}

/* Given a description 'desc' and a list of symbols 'l', check
 * that the given list of symbols does not contain duplicate entries.
 */
bool check_double_symbols( const char *desc, const_origsymbol_list l )
{
    bool ok = true;

    for( unsigned int ixa=0; ixa<l->sz; ixa++ ){
	origsymbol sa = l->arr[ixa];

	for( unsigned int ixb=ixa+1; ixb<l->sz; ixb++ ){
	    origsymbol sb = l->arr[ixb];

	    if( sa->sym == sb->sym ){
		sprintf(
		    errarg,
		    "%s '%s' at %s:%d and %s:%d",
		    desc,
		    sa->sym->name,
		    sa->org->file->name,
		    sa->org->line,
		    sb->org->file->name,
		    sb->org->line
		);
		error( "double declaration" );
		ok = FALSE;
	    }
	}
    }
    return ok;
}

/* Given a list of SparProgramUnits 'l' and a type symbol 't', if none of
 * the units defines 't', return FALSE. If one of these units defines the
 * given type, return TRUE, and assign the index of the defining unit to
 * '*pos'.
 */
bool search_type_SparProgramUnit_list(
 const_SparProgramUnit_list l,
 tmsymbol t,
 unsigned int *pos
)
{
    for( unsigned int ix = 0; ix<l->sz; ix++ ){
	const_SparProgramUnit unit = l->arr[ix];

	if(
	    unit != SparProgramUnitNIL &&
	    member_tmsymbol_list( unit->definitions, t )
	){
	    *pos = ix;
	    return true;
	}
    }
    return false;
}
 
bool is_Object_type( const_type t )
{
    if( t == typeNIL ){
	return false;
    }
    if( t->tag != TAGObjectType ){
	return false;
    }
    const char *tnm = to_const_ObjectType( t )->name->sym->name;
    return (
	(strcmp( tnm, "Object" ) == 0) ||
	(strcmp( tnm, "java.lang.Object" ) == 0)
    );
}

bool is_String_type( const_type t )
{
    if( t == typeNIL ){
	return false;
    }
    if( t->tag != TAGObjectType ){
	return false;
    }
    const_ObjectType ot = to_const_ObjectType( t );
    if( ot->name == origsymbolNIL ){
	return false;
    }
    const char *tnm = ot->name->sym->name;
    return (
	(strcmp( tnm, "String" ) == 0) ||
	(strcmp( tnm, "java.lang.String" ) == 0)
    );
}

bool is_vnusstring_type( const_type t )
{
    if( t == typeNIL || t->tag != TAGPrimitiveType ){
	return false;
    }
    return to_const_PrimitiveType( t )->base == BT_STRING;
}

bool is_boolean_type( const_type t )
{
    if( t == typeNIL || t->tag != TAGPrimitiveType ){
	return false;
    }
    return to_const_PrimitiveType( t )->base == BT_BOOLEAN;
}

type make_String_type()
{
    return new_PrimitiveType( BT_STRING );
}

/* Given a string, return a JNI-mangled string.
 *
 * For the moment, only the '_' rule is implemented, and not the
 * unicode encoding rule.
 */
tmstring jni_mangle_string( const char *s )
{
    size_t sz = strlen( s )+10;
    tmstring buf = create_tmstring( sz );

    /* TODO: implement unicode encoding */
    const char *p = s;
    unsigned int ix = 0;
    while( *p != '\0' ){
	char c = *p++;

	buf[ix++] = c;
	if( c == '_' ){
	    buf[ix++] = '1';
	}
	if( ix+1>=sz ){
	    buf = realloc_tmstring( buf, sz+10 );
	    sz += 10;
	}
    }
    buf[ix] = 0;
    char *b = buf;
    while( *b != '\0' ){
	if( *b == '.' ){
	    *b = '_';
	}
	b++;
    }
    return buf;
}

// Given the type of initialization (static or dynamic) and the name
// of the class we're talking about, generate a name for the constructor
// or function that contains the initialization code for the class.
static tmsymbol gen_role_tmsymbol( const char *typ, const char *role )
{
    tmstring str = printf_tmstring( "%s_%s", typ, role  );
    tmsymbol res = gen_tmsymbol( str );
    rfre_tmstring( str );
    return res;
}

tmsymbol gen_role_tmsymbol( tmsymbol typ, const char *role )
{
    return gen_role_tmsymbol( typ->name, role );
}

tmsymbol gen_role_tmsymbol( origsymbol typ, const char *role )
{
    return gen_role_tmsymbol( typ->sym->name, role );
}

// Given a type 't', return an OptExpr that contains the default
// init for that type.
OptExpr build_DefaultInit( const type t )
{
    return new_OptExpr( new_DefaultValueExpression( rdup_type( t ) ) );
}

// Given an origin, an expression 'x' that should contain the power expression
// of a vector, and a pointer to an unsigned int 'v', extract the value of
// this power expression.
//
// Complain if the expression is negative, not an int, or is not constant.
// Iff everything is ok, assign the value of 'x' to '*v'.
bool extract_vector_power( const_origin org, const_expression x, unsigned int *v )
{
    bool ok = true;

    if( x == expressionNIL ){
	return false;
    }
    if( x->tag != TAGIntExpression ){
	if( is_LiteralExpression(x) ){
	    origin_error( org, "The length of a vector should be of type int" );
	}
	else {
	    origin_error( org, "Cannot determine the length of the vector" );
	}
	ok = false;
    }
    else {
	vnus_int sz = to_const_IntExpression( x )->v;
	if( sz<0 ){
	    origin_error( org, "A vector cannot have negative length" );
	    ok = false;
	}
	else if( sz>= 10000 ){
	    sprintf( errarg, "%ld", (long) sz );
	    origin_error( org, "Unreasonably large vector length" );
	    ok = false;
	}
	else {
	    *v = (unsigned int) sz;
	}
    }
    return ok;
}

// Given an origin and a type 't' that should be a tuple or vector, determine
// the length of this tuple.
bool extract_tuple_length( const_origin org, const_type t, unsigned int *v )
{
    bool ok = true;
    if( t->tag == TAGTupleType ){
	const_TupleType tt = to_const_TupleType( t );
	*v = tt->fields->sz;

    }
    else if( t->tag == TAGVectorType ){
	const_VectorType vt = to_const_VectorType( t );
	ok = extract_vector_power( org, vt->power, v );
    }
    else {
	internal_error( "not a tuple" );
    }
    return ok;
}

// Given an origin, an expression 'x' that should contain the power expression
// of a vector, and a pointer to an unsigned int 'v', extract the value of
// this power expression.
//
// Complain if the expression is negative, not an int, or is not constant.
// Iff everything is ok, assign the value of 'x' to '*v'.
bool extract_array_rank( const_origin org, const_expression x, unsigned int *v )
{
    bool ok = true;

    if( x->tag != TAGIntExpression ){
	if( is_LiteralExpression(x) ){
	    origin_error( org, "The rank of an array should be of type int" );
	}
	else {
	    origin_error( org, "Cannot determine the rank of the array" );
	}
	ok = false;
    }
    else {
	vnus_int sz = to_const_IntExpression(x)->v;
	if( sz<0 ){
	    origin_error( org, "An array cannot have negative rank" );
	    ok = false;
	}
	else if( sz>= 10000 ){
	    sprintf( errarg, "%ld", (long) sz );
	    origin_error( org, "Unreasonably large array rank" );
	    ok = false;
	}
	else {
	    *v = (unsigned int) sz;
	}
    }
    return ok;
}

/* Given a tmsymbol fqname, return a unique new tmsymbol that resembles
 * this symbol.
 */
tmsymbol gen_fqname( tmsymbol fqname )
{
    tmstring buf = printf_tmstring( "%s_", fqname->name );
    char *p = buf;
    tmsymbol res;

    while( *p != '\0' ){
	if( *p == '.' ){
	    *p = '_';
	}
	p++;
    }
    res = gen_tmsymbol( buf );
    rfre_tmstring( buf );
    return res;
}

type clean_type( type t )
{
    switch( t->tag ){
	case TAGPragmaType:
	    return clean_type( to_PragmaType(t)->t );

	default:
	    break;
    }
    return t;
}

// Given a list of formal parameters, make sure that there are no
// duplicate names. Return true iff all is well.
bool check_duplicate_FormalParameters( const_FormalParameter_list l )
{
    bool ok = true;

    if( l == FormalParameter_listNIL ){
	return ok;
    }
    for( unsigned int ix=0; ix<l->sz; ix++ ){
	FormalParameter px = l->arr[ix];
	for( unsigned int iy=ix+1; iy<l->sz; iy++ ){
	    FormalParameter py = l->arr[iy];
	    if( px->name->sym == py->name->sym ){
		origsymbol_error( py->name, "duplicate formal parameter" );
		// Give the second parameter an other name, to
		// avoid problems later on.
		py->name->sym = gen_tmsymbol( "dummy" );
		ok = false;
	    }
	}
    }
    return ok;
}

/* Given a list of type bindings and a new type binding, add this
 * type binding to the list.
 */
TypeBinding_list register_TypeBinding(
 TypeBinding_list tl,
 tmsymbol file,
 TypeBinding b
)
{
    if( trace_bindings ){
	fprintf(
	    stderr,
	    "In '%s': registered type binding '%s' ==> '%s'\n",
	    file==NULL?"(no file)":file->name,
	    b->shortname->name,
	    b->longname->name
	);
	(void) fflush( stderr );
    }
    return append_TypeBinding_list( tl, b );
}

// Given a symbol 's' and a list of name translations 'sl', return
// the translation of name 's', or tmsymbolNIL if it is not in 'sl'.
tmsymbol translate_name( const_NameTranslation_list sl, tmsymbol s )
{
    if( s == tmsymbolNIL || sl == NULL ){
	return tmsymbolNIL;
    }
    for( unsigned int ix=0; ix<sl->sz; ix++ ){
	NameTranslation tr = sl->arr[ix];

	if( tr->from == s ){
	    return tr->to;
	}
    }
    return tmsymbolNIL;
}

/* Given a list of expressions representing an array initialization,
 * a list of upper bounds, the dimension to examine, and the rank (number
 * of dimensions), update the upper bound for this dimension.
 */
tmuint_list calculate_array_shape(
    origin org,
    expression_list elms,
    tmuint_list shape,
    unsigned int dim,
    unsigned int rank
)
{
    assert( dim<shape->sz );
    if( shape->arr[dim]<elms->sz ){
	shape->arr[dim] = elms->sz;
    }
    if( dim+1<rank ){
	/* This is a multidim array, so there should be nested
	 * array init expressions to fill it.
	 */
	for( unsigned int ix=0; ix<elms->sz; ix++ ){
	    expression x = elms->arr[ix];

	    if( x->tag != TAGArrayInitExpression ){
		origin_error( org, "Not enough nesting to initialize a %u-dimensional array", rank );
		shape->arr[dim] = 0;
		return shape;
	    }
	    else {
		ArrayInitExpression init = to_ArrayInitExpression( x );

		shape = calculate_array_shape(
		    org,
		    init->elements,
		    shape,
		    dim+1,
		    rank
		);
	    }
	}
    }
    return shape;
}

/* Given a list if uints, return an expression list for it.  */
ActualDim build_ActualDim( const_tmuint_list l )
{
    expression_list res = setroom_expression_list( new_expression_list(), l->sz );
    for( unsigned int ix=0; ix<l->sz; ix++ ){
	res = append_expression_list( res, new_IntExpression( (vnus_int) l->arr[ix] ) );
    }
    return new_ActualDim( new_VectorExpression( res ), new_Pragma_list() );
}

// Given a type 't', return true iff it is a tuple type. That is,
// either TupleType or VectorType.
bool is_tuple_type( const_type t )
{
    if( t == typeNIL ){
	return false;
    }
    if( t->tag == TAGPragmaType ){
	return is_tuple_type( to_const_PragmaType(t)->t );
    }
    return t != typeNIL && (t->tag == TAGTupleType || t->tag == TAGVectorType);
}

bool is_void_type( const_type t )
{
    if( t->tag == TAGPragmaType ){
	return is_void_type( to_const_PragmaType(t)->t );
    }
    return t->tag == TAGVoidType;
}

// Given a statement, return true iff it has a label.
bool is_labeled_statement( const_statement smt )
{
    return smt != statementNIL && smt->labels != NULL && smt->labels->sz != 0;
}

static bool is_boolean_constant( const_expression x, bool val )
{
    if( x->tag == TAGAnnotationExpression ){
	return is_boolean_constant( to_const_AnnotationExpression(x)->x, val );
    }
    if( x->tag != TAGBooleanExpression ){
	return false;
    }
    return to_const_BooleanExpression( x )->b == val;
}

bool is_true_constant( const_expression x )
{
    return is_boolean_constant( x, true );
}

bool is_false_constant( const_expression x )
{
    return is_boolean_constant( x, false );
}

// A routine that is used to set a breakpoint on.
void yup()
{
    fprintf( stderr, "Yup\n" );
}

static bool is_trivial_inline_declaration( const_declaration d )
{
    if( d->tag != TAGFieldDeclaration ){
	return false;
    }
    const_FieldDeclaration vd = to_const_FieldDeclaration( d );
    return maybe_has_sideeffect( vd->init, NO_SE_NEW );
}

// Given a statement list, return true iff the statement list only
// contains declarations without side-effects, and a single real
// statement at the end.
static bool is_trivial_inline_statement_list( statement_list sl )
{
    for( unsigned int ix=0; ix<sl->sz-1; ix++ ){
	if( !is_declaration( sl->arr[ix] ) ){
	    return false;
	}
	if( !is_trivial_inline_declaration( to_declaration( sl->arr[ix] ) ) ){
	    return false;
	}
    }
    return true;
}

// Given a block 'blk' and the name of the variable that is assigned
// the return value, return the expression that variable is assigned
// to if the block is trivial.
expression extract_trivial_inline_expression( Block blk, tmsymbol retvar )
{
    statement_list sl = blk->statements;

    if( sl->sz == 0 || !is_trivial_inline_statement_list( sl ) ){
	return expressionNIL;
    }
    statement last = sl->arr[sl->sz-1];
    // Now match '<retvar> = <x = <constant>>;', and return 'x'.
    if( last->tag != TAGAssignStatement ){
	return expressionNIL;
    }
    AssignStatement alast = to_AssignStatement(last);
    expression lhs = alast->lhs;
    if( lhs->tag != TAGVariableNameExpression ){
	return expressionNIL;
    }
    VariableNameExpression vlhs = to_VariableNameExpression( lhs );
    if( vlhs->name->sym != retvar ){
	return expressionNIL;
    }
    // TODO: see if there are other expressions that could be returned.
    if( !is_constant( alast->rhs ) ){
	return expressionNIL;
    }
    return rdup_expression( alast->rhs );
}

// Given a type 't', return true iff it is either a reference type or the
// null type.
static bool is_nullref_type( const_type t )
{
    return t == typeNIL || is_ReferenceType( t ) || t->tag == TAGNullType;
}

// Given the types of the then and else branch of a ?: operator, make
// sure that they are compatible with eachother.
// As an addition, we also allow ?: on __string;
// The restrictions enforced here are from JLS2 15.25
// combinations between __string and String should have been
// rewritten.
bool check_IfExpression_types( const_Entry_list symtab, const_origin org, const_type t_then, const_type t_else )
{
    bool ok = true;
    if(
	(is_numeric_type( symtab, t_then ) && (is_numeric_type( symtab, t_else ))) ||
	(is_boolean_type( symtab, t_then ) && (is_boolean_type( symtab, t_else ))) ||
	(is_nullref_type( t_then ) && (is_nullref_type( t_else ))) ||
	(is_vnusstring_type( t_then ) && (is_vnusstring_type( t_else ))) ||
	(is_vnusstring_type( t_then ) && (is_nullref_type( t_else ))) ||
	(is_nullref_type( t_then ) && (is_vnusstring_type( t_else )))

    ){
	// the expression is correct
    }
    else {
	const tmstring thennm = typename_type( t_then );
	const tmstring elsenm = typename_type( t_else );

	sprintf( errarg, "types are %s and %s", thennm, elsenm );
	rfre_tmstring( thennm );
	rfre_tmstring( elsenm );
	origin_error( org, "incompatible types in ?: expression" );
	ok = false;
    }
    return ok;
}

static bool are_equivalent_origsymbols( const_origsymbol a, const_origsymbol b )
{
    if( a == b ){
	return true;
    }
    if( a == origsymbolNIL || b == origsymbolNIL ){
	return false;
    }
    return a->sym == b->sym;
}

static bool are_equivalent_expressions( const_expression_list la, const_expression_list lb )
{
    if( la == lb ){
	return true;
    }
    if( la == expression_listNIL || lb == expression_listNIL ){
	return false;
    }
    if( la->sz != lb->sz ){
	return false;
    }
    for( unsigned int ix=0; ix<la->sz; ix++ ){
	if( !are_equivalent_expressions( la->arr[ix], lb->arr[ix] ) ){
	    return false;
	}
    }
    return true;
}

// Given two expressions 'a' and 'b', return true iff they are equivalent.
// That is, iff they always evaluate to the same value.
bool are_equivalent_expressions( const_expression a, const_expression b )
{
    bool res = false;

    if( a == b ){
	return true;
    }
    if( a == expressionNIL || b == expressionNIL ){
	return false;
    }
    if( a->tag == TAGNotNullAssertExpression ){
	return are_equivalent_expressions( to_const_NotNullAssertExpression(a)->x, b );
    }
    if( b->tag == TAGNotNullAssertExpression ){
	return are_equivalent_expressions( a, to_const_NotNullAssertExpression(b)->x );
    }
    if( a->tag == TAGBracketExpression ){
	return are_equivalent_expressions( to_const_BracketExpression(a)->x, b );
    }
    if( b->tag == TAGBracketExpression ){
	return are_equivalent_expressions( a, to_const_BracketExpression(b)->x );
    }
    if( a->tag == TAGAnnotationExpression ){
	return are_equivalent_expressions( to_const_AnnotationExpression(a)->x, b );
    }
    if( b->tag == TAGAnnotationExpression ){
	return are_equivalent_expressions( a, to_const_AnnotationExpression(b)->x );
    }
    if( a->tag != b->tag ){
	return false;
    }
    switch( a->tag )
    {
	case TAGByteExpression:
	    res = to_const_ByteExpression(a)->v == to_const_ByteExpression(b)->v;
	    break;

	case TAGShortExpression:
	    res = to_const_ShortExpression(a)->v == to_const_ShortExpression(b)->v;
	    break;

	case TAGIntExpression:
	    res = to_const_IntExpression(a)->v == to_const_IntExpression(b)->v;
	    break;

	case TAGLongExpression:
	    res = to_const_LongExpression(a)->v == to_const_LongExpression(b)->v;
	    break;

	case TAGFloatExpression:
	    res = to_const_FloatExpression(a)->v == to_const_FloatExpression(b)->v;
	    break;

	case TAGDoubleExpression:
	    res = to_const_DoubleExpression(a)->v == to_const_DoubleExpression(b)->v;
	    break;

	case TAGCharExpression:
	    res = to_const_CharExpression(a)->c == to_const_CharExpression(b)->c;
	    break;

	case TAGBooleanExpression:
	    res = to_const_BooleanExpression(a)->b == to_const_BooleanExpression(b)->b;
	    break;

	case TAGStringExpression:
	    res = isequal_tmstring( to_const_StringExpression(a)->s, to_const_StringExpression(b)->s );
	    break;

	case TAGNullExpression:
	    res = true;
	    break;

	case TAGSizeofExpression:
	    res = isequal_type( to_const_SizeofExpression(a)->t, to_const_SizeofExpression(b)->t );
	    break;

	case TAGSubscriptExpression:
	{
	    const_SubscriptExpression sa = to_const_SubscriptExpression(a);
	    const_SubscriptExpression sb = to_const_SubscriptExpression(b);

	    res = are_equivalent_expressions( sa->array, sb->array ) &&
		are_equivalent_expressions( sa->subscripts, sb->subscripts );
	    break;
	}


	case TAGVariableNameExpression:
	{
	    const_VariableNameExpression va = to_const_VariableNameExpression(a);
	    const_VariableNameExpression vb = to_const_VariableNameExpression(b);

	    res = are_equivalent_origsymbols( va->name, vb->name );
	    break;
	}

	case TAGFieldExpression:
	{
	    const_FieldExpression fa = to_const_FieldExpression(a);
	    const_FieldExpression fb = to_const_FieldExpression(b);

	    res = are_equivalent_expressions( fa->rec, fb->rec ) &&
		are_equivalent_origsymbols( fa->field, fb->field );
	    break;
	}

	case TAGBinopExpression:
	{
	    const_BinopExpression va = to_const_BinopExpression(a);
	    const_BinopExpression vb = to_const_BinopExpression(b);

	    res = are_equivalent_expressions( va->left, vb->left ) &&
		are_equivalent_expressions( va->right, vb->right );
	    break;
	}

	case TAGVectorSubscriptExpression:
	{
	    const_VectorSubscriptExpression va = to_const_VectorSubscriptExpression(a);
	    const_VectorSubscriptExpression vb = to_const_VectorSubscriptExpression(b);

	    res = are_equivalent_expressions( va->vector, vb->vector ) &&
		va->subscript == vb->subscript;
	    break;
	}

	case TAGVectorExpression:
	    res = are_equivalent_expressions(
		to_const_VectorExpression(a)->fields,
		to_const_VectorExpression(b)->fields
	    );
	    break;

	case TAGTypeFieldExpression:
	case TAGArrayInitExpression:
	case TAGAssignOpExpression:
	case TAGCastExpression:
	case TAGClassExpression:
	case TAGClassIdExpression:
	case TAGClassInstanceOfExpression:
	case TAGComplexExpression:
	case TAGDefaultValueExpression:
	case TAGFieldInvocationExpression:
	case TAGForcedCastExpression:
	case TAGGetBufExpression:
	case TAGGetLengthExpression:
	case TAGGetSizeExpression:
	case TAGIfExpression:
	case TAGInstanceOfExpression:
	case TAGInterfaceInstanceOfExpression:
	case TAGInternalizeExpression:
	case TAGMethodInvocationExpression:
	case TAGNewArrayExpression:
	case TAGNewClassExpression:
	case TAGNewInitArrayExpression:
	case TAGNewRecordExpression:
	case TAGOuterSuperFieldExpression:
	case TAGOuterSuperInvocationExpression:
	case TAGOuterThisExpression:
	case TAGPostDecrementExpression:
	case TAGPostIncrementExpression:
	case TAGPreDecrementExpression:
	case TAGPreIncrementExpression:
	case TAGShortopExpression:
	case TAGSuperFieldExpression:
	case TAGSuperInvocationExpression:
	case TAGTypeExpression:
	case TAGTypeInstanceOfExpression:
	case TAGTypeInvocationExpression:
	case TAGUnopExpression:
	case TAGWhereExpression:
	    // TODO: have a better look
	    res = isequal_expression( a, b );
	    break;

	// Should have been handled elsewhere.
	case TAGAnnotationExpression:
	case TAGBracketExpression:
	case TAGNotNullAssertExpression:
	    assert( false );
	    break;


    }
    return res;
}

// Given an expression list 'xl' and an expression 'x', return true
// if an equivalent expression of 'x' occurs in 'xl'.
bool occurs_expression_list( const_expression_list xl, const_expression x )
{
    for( unsigned int ix=0; ix<xl->sz; ix++ ){
	if( are_equivalent_expressions( xl->arr[ix], x ) ){
	    return true;
	}
    }
    return false;
}
