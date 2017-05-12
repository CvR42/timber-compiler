/* File: prettyprint.c
 *
 * Generate Vnus code from the final parse tree.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <tmc.h>
#include <assert.h>

#include "defs.h"
#include "tmadmin.h"
#include "global.h"
#include "error.h"
#include "prettyprint.h"
#include "symbol_table.h"
#include "constant.h"
#include "collect.h"
#include "typederive.h"
#include "typename.h"
#include "service.h"
#include "pragma.h"
#include "dump.h"
#include "markadmin.h"
#include "vnus_types.h"

static bool generated_get_buffer = false;
static bool generated_get_length = false;
static int gc_serial = 10000;

static Pragma_list the_global_pragmas = Pragma_listNIL;

#define GCMAGIC 0x7251fe55L

/* Forward declaration of mutually recursive functions. */
static void pp_statements( FILE *f, const_TypeEntry_list typelist, const_Entry_list symtab, unsigned int indent, const_statement_list smts );
static void pp_statement_list( FILE *f, const_TypeEntry_list typelist, const_Entry_list symtab, unsigned int indent, const_statement_list smts );
static void pp_Block( FILE *f, const_TypeEntry_list typelist, const_Entry_list symtab, unsigned int indent, const_Block smts );
static void pp_expression( FILE *f, const_TypeEntry_list typelist, const_Entry_list symtab, const_expression x );
static void pp_expression_list( FILE *f, const_TypeEntry_list typelist, const_Entry_list symtab, const_expression_list xl );
static void pp_actualParameters(
 FILE *f,
 const_TypeEntry_list typelist,
 const_Entry_list symtab,
 const_expression_list this_xl,
 const_expression_list xl
);
static void pp_type(
 FILE *f,
 const_TypeEntry_list typelist,
 const_Entry_list symtab,
 const_Pragma_list pragmas,
 const_type t
);
static void pp_type_list( FILE *f, const_TypeEntry_list typelist, const_Entry_list symtab, const_type_list l );
static void pp_vtable_function_type( FILE *f, const_TypeEntry_list typelist, const_Entry_list symtab, const type_list thisparms, const_Signature s, const_type rettype );
static void pp_PragmaExpression( FILE *f, const_PragmaExpression x );
static bool pp_opt_Pragma_list( FILE *f, unsigned int indent, modflags modifiers, bool show_modifiers, const_Pragma_list pl );

/* Given a file handle 'f' and a indent 'indent', generate this
 * indent.
 */
static void pp_indent( FILE *f, unsigned int indent )
{
    for( unsigned int n=0; n<indent; n++ ){
	fputc( ' ', f );
    }
}

/* Given a file handle 'f', an indent 'indent' and a tmstring 's',
 * generate a line consisting of 's' indented to the given indent.
 */
static void pp_indent_line( FILE *f, unsigned int indent, const char *s )
{
    pp_indent( f, indent );
    fprintf( f, "%s\n", s );
}

static void pp_tmsymbol( FILE *f, const tmsymbol s )
{
    fputs( s->name, f );
}

// Given a string representing a fully qualified type name, print a mutated
// version that is acceptable as a variable name to Vnus.
//
// For the moment the name mangling of the Java Native Interface (JNI)
// is used.
static void pp_censored_typename( FILE *f, const char *s )
{
    tmstring cs = jni_mangle_string( s );

    fputs( cs, f );
    rfre_tmstring( cs );
}

static void pp_censored_typename( FILE *f, const tmsymbol s )
{
    pp_censored_typename( f, s->name );
}

static void pp_censored_typename( FILE *f, const origsymbol s )
{
    pp_censored_typename( f, s->sym );
}

static void pp_origsymbol( FILE *f, const origsymbol s )
{
    pp_tmsymbol( f, s->sym );
}

/* A table of strings for the basetypes. */
static const char *basetype_strings[] =
{
    "boolean",	/* BT_BOOLEAN */
    "string",	/* BT_STRING */
    "complex",	/* BT_COMPLEX */
    "byte",	/* BT_BYTE */
    "short",	/* BT_SHORT */
    "char",	/* BT_CHAR */
    "int",	/* BT_INTEGER */
    "long",	/* BT_LONG */
    "float",	/* BT_FLOAT */
    "double"	/* BT_DOUBLE */
};

/* Given a file handle 'f' and a type 't', generate a type expression
 * for that type.
 */
static void pp_basetype( FILE *f, const BASETYPE t )
{
     fputs( basetype_strings[(int) t], f );
}

static void pp_Sizes( FILE *f, const_expression rankexpr )
{
    unsigned int rank;

    if( !extract_array_rank( originNIL, rankexpr, &rank ) ){
	rank = 0;
    }
    fputs( "[", f );
    for( unsigned int ix=0; ix<rank; ix++ ){
	if( ix != 0 ){
	    fputs( ", ", f );
	}
	fputs( "dontcare", f );
    }
    fputs( "]", f );
}

static void pp_Sizes( FILE *f, const_tmuint_list sizes )
{
    fputs( "[", f );
    for( unsigned int ix=0; ix<sizes->sz; ix++ ){
	if( ix != 0 ){
	    fputs( ", ", f );
	}
	fprintf( f, "%lu", (unsigned long) sizes->arr[ix] );
    }
    fputs( "]", f );
}

/* Given a file handle 'f' and a field 'fld', print the field. */
static void pp_unnamed_field(
 FILE *f,
 const_TypeEntry_list typelist,
 const_Entry_list symtab,
 unsigned int n,
 const_type t
)
{
    fprintf( f, "field%u:", n );
    pp_type( f, typelist, symtab, Pragma_listNIL, t );
}

/* Given a file handle 'f' and a field list 'l', print the field list. */
static void pp_unnamed_field_list(
 FILE *f,
 const_TypeEntry_list typelist,
 const_Entry_list symtab,
 const_type_list l
)
{
    fputs( "[ ", f );
    for( unsigned int ix=0; ix<l->sz; ix++ ){
	if( ix != 0 ){
	    fputs( ", ", f );
	}
	pp_unnamed_field( f, typelist, symtab, ix, l->arr[ix] );
    }
    fputs( " ]", f );
}

/* Given a file handle 'f', a type 't' and an expression 'power',
 * and a field list 'l', create a list of fake record fields by printing the
 * field 't' 'power' times.
 */
static void pp_repeated_fake_field(
 FILE *f,
 const_TypeEntry_list typelist,
 const_Entry_list symtab,
 const_type t,
 const_expression power
)
{
    unsigned int sz;

    fputs( "[ ", f );
    if( extract_vector_power( originNIL, power, &sz ) ){
	for( unsigned int ix=0; ix<sz; ix++ ){
	    if( ix != 0 ){
		fputs( ", ", f );
	    }
	    pp_unnamed_field( f, typelist, symtab, ix, t );
	}
    }
    fputs( " ]", f );
}

/* Given a file handle 'f' and a field 'fld', print the field. */
static void pp_field(
 FILE *f,
 const_TypeEntry_list typelist,
 const_Entry_list symtab,
 const tmsymbol nm
)
{
    pp_tmsymbol( f, nm );
    fputs( ": ", f );
    Entry e = lookup_Entry( symtab, nm );
    if( e->tag != TAGFieldEntry ){
	internal_error( "'%s' is not a field name", nm->name );
    }
    pp_type( f, typelist, symtab, Pragma_listNIL, to_const_FieldEntry(e)->t );
}

static void pp_ObjectType( FILE *f, const_TypeEntry_list typelist, tmsymbol nm )
{
    const_TypeEntry te = lookup_TypeEntry( typelist, nm );

    switch( te->tag ){
	case TAGClassEntry:
	    fputs( "pointer record ", f );
	    pp_tmsymbol( f, to_const_ClassEntry(te)->recordname );
	    break;

	case TAGInterfaceEntry:
	    fputs( "pointer record __any_interface_type", f );
	    break;

	case TAGPackageEntry:
	case TAGForwardObjectEntry:
	    internal_error( "ObjectType '%s' is not a class or interface??", nm->name );
	    break;

    }
}

static void pp_ObjectType( FILE *f, const_TypeEntry_list typelist, const_origsymbol nm )
{
    pp_ObjectType( f, typelist, nm->sym );
}

/* Given a file handle 'f' and a type 't', generate a type expression
 * for that type.
 */
static void pp_type(
 FILE *f,
 const_TypeEntry_list typelist,
 const_Entry_list symtab,
 const_Pragma_list pragmas,
 const_type t
)
{
    switch( t->tag ){
	case TAGPrimitiveType:
	    pp_basetype( f, to_const_PrimitiveType(t)->base );
	    break;

	case TAGPragmaType:
	{
	    const_Pragma_list my_pl = to_const_PragmaType(t)->pragmas;
	    Pragma_list pl;

	    if( pp_opt_Pragma_list( f, 0, 0, false, my_pl ) ){
		fputc( ' ', f );
	    }
	    if( pragmas == Pragma_listNIL ){
		pl = rdup_Pragma_list( my_pl );
	    }
	    else {
		pl = concat_Pragma_list(
		    rdup_Pragma_list( pragmas ),
		    rdup_Pragma_list( my_pl )
		);
	    }
	    pp_type( f, typelist, symtab, pl, to_const_PragmaType(t)->t );
	    rfre_Pragma_list( pl );
	    break;
	}

        case TAGArrayType:
	{
	    fputs( "pointer shape ", f );
	    pp_Sizes( f, to_const_ArrayType(t)->rank );
	    fputs( " ", f );
#if 0
	    const char *dist = get_string_pragma(
		the_global_pragmas,
		pragmas,
		"dist"
	    );
	    if( dist != NULL ){
		fputs( dist, f );
		fputs( " ", f );
	    }
#endif
	    pp_type( f, typelist, symtab, Pragma_listNIL, to_const_ArrayType(t)->elmtype );
	    break;
	}

        case TAGPrimArrayType:
	    fputs( "array ", f );
	    pp_type( f, typelist, symtab, Pragma_listNIL, to_const_PrimArrayType(t)->elmtype );
	    break;

	case TAGVectorType:
	    fputs( "record ", f );
	    pp_repeated_fake_field( f, typelist, symtab, to_const_VectorType(t)->elmtype, to_const_VectorType(t)->power );
	    break;

	case TAGTupleType:
	    fputs( "record ", f );
	    pp_unnamed_field_list( f, typelist, symtab, to_const_TupleType(t)->fields );
	    break;

	case TAGVoidType:
	    /* This is wrong, but why not. */
	    fputs( "void", f );
	    break;

	case TAGNullType:
	    /* This is wrong, but why not. */
	    fputs( "pointer anything", f );
	    break;

	case TAGFunctionType:
	{
	    const_FunctionType fn = to_const_FunctionType(t);

	    if( fn->rettype->tag == TAGVoidType ){
		fputs( "procedure [", f );
		pp_type_list( f, typelist, symtab, fn->formals );
		fputs( "] ", f );
	    }
	    else {
		fputs( "function [", f );
		pp_type_list( f, typelist, symtab, fn->formals );
		fputs( "] ", f );
		pp_type( f, typelist, symtab, Pragma_listNIL, fn->rettype );
	    }
	    break;
	}

	case TAGExceptionVariableType:
	    fputs( "pointer record []", f );
	    break;

	case TAGGCRefLinkType:
	    fputs( "record __gc_reflink", f );
	    break;

	case TAGGCTopRefLinkType:
	    fputs( "pointer record __gc_reflink", f );
	    break;

	case TAGGenericObjectType:
	    internal_error( "generic object reference should have been rewritten" );
	    break;

	case TAGObjectType:
	{
	    const_origsymbol nm = to_const_ObjectType(t)->name;

	    pp_ObjectType( f, typelist, nm );
	    break;
	}

	case TAGTypeType:
	    internal_error( "type 'type' should have been rewritten" );
	    break;

	case TAGTypeOf:
	    internal_error( "typeof should have been rewritten" );
	    break;

	case TAGTypeOfIf:
	    internal_error( "typeofif should have been rewritten" );
	    break;

    }
}

/* Given a list of types, print a formal parameter list from these types. */
static void pp_type_list(
 FILE *f,
 const_TypeEntry_list typelist,
 const_Entry_list symtab,
 const_type_list l
)
{
    if( l->sz == 0 ){
	return;
    }
    for( unsigned int ix=0; ix<l->sz; ix++ ){
	if( ix != 0 ){
	    fputs( ", ", f );
	}
	pp_type( f, typelist, symtab, Pragma_listNIL, l->arr[ix] );
    }
}

/* Given a formal parameter 'arg', generate a formal parameter declaration. */
static void pp_FormalParameter( FILE *f, const_FormalParameter arg )
{
    pp_origsymbol( f, arg->name );
}

static void pp_FormalParameters( FILE *f, const_FormalParameter_list this_parms, const_FormalParameter_list argv )
{
    bool first = true;

    fputs( "[", f );
    for( unsigned int tix=0; tix<this_parms->sz; tix++ ){
	if( first ){
	    first = false;
	}
	else {
	    fputs( ", ", f );
	}
	pp_FormalParameter( f, this_parms->arr[tix] );
    }
    for( unsigned int ix=0; ix<argv->sz; ix++ ){
	if( first ){
	    first = false;
	}
	else {
	    fputs( ", ", f );
	}
	pp_FormalParameter( f, argv->arr[ix] );
    }
    fputs( "]", f );
}

static void pp_vtable_function_type(
 FILE *f,
 const_TypeEntry_list typelist,
 const_Entry_list symtab,
 const type_list thisparms,
 const_Signature s,
 const_type rettype
)
{
    if( thisparms == NULL ){
	internal_error( "null" );
    }
    // If a function is in the vtable, it should have a 'this' parameter..
    assert( thisparms->sz != 0 );
    if( rettype->tag == TAGVoidType ){
	fputs( "procedure [", f );
	pp_type_list( f, typelist, symtab, thisparms );
	if( thisparms->sz != 0 && s->parameters->sz != 0 ){
	    fputs( ", ", f );
	}
	pp_type_list( f, typelist, symtab, s->parameters );
	fputs( "]", f );
    }
    else {
	fputs( "function [", f );
	pp_type_list( f, typelist, symtab, thisparms );
	if( thisparms->sz != 0 && s->parameters->sz != 0 ){
	    fputs( ", ", f );
	}
	pp_type_list( f, typelist, symtab, s->parameters );
	fputs( "] ", f );
	pp_type( f, typelist, symtab, Pragma_listNIL, rettype );
    }
}

/* A table of strings for the unary operators.  */
static const char *unop_strings[] =
{
    " not ",	/* UNOP_NOT */
    " not ",	/* UNOP_INVERT */
    "+",	/* UNOP_PLUS */
    "~"		/* UNOP_NEGATE */
};

/* Given an operand and an operand, generate a unary operator expression.  */
static void pp_unary_operator( FILE *f, UNOP tor )
{
    fprintf( f, "%s", unop_strings[(int) tor] );
}

/* A table of strings for the binary operators. */
static const char *binop_strings[] =
{
    "and",	/* BINOP_AND */
    "or",	/* BINOP_OR */
    "mod",	/* BINOP_MOD */
    "+",	/* BINOP_PLUS */
    "-",	/* BINOP_MINUS */
    "*",	/* BINOP_TIMES */
    "/",	/* BINOP_DIVIDE */
    "=",	/* BINOP_EQUAL */
    "<>",	/* BINOP_NOTEQUAL */
    "<",	/* BINOP_LESS */
    "<=",	/* BINOP_LESSEQUAL */
    ">",	/* BINOP_GREATER */
    ">=",	/* BINOP_GREATEREQUAL */
    "xor",	/* BINOP_XOR */
    "<<",	/* BINOP_SHIFTLEFT */
    ">>",	/* BINOP_SHIFTRIGHT */
    ">>>"	/* BINOP_USHIFTRIGHT */
};

/* Given a binary operator, generate its Vnus representation.  */
static void pp_binary_operator( FILE *f, BINOP tor )
{
    fputs( binop_strings[(int) tor], f );
}

/* Given a shortcircuit operator, generate its Vnus representation.  */
static void pp_shortcircuit_operator( FILE *f, SHORTOP tor )
{
    const char *str = "??";

    switch( tor ){
	case SHORTOP_AND:	str = "shortand";	break;
	case SHORTOP_OR:	str = "shortor";	break;
    }
    fputs( str, f );
}

static void pp_MethodInvocation(
 FILE *f,
 const_TypeEntry_list typelist,
 const_Entry_list symtab,
 const char *callname,
 const_MethodInvocation call
)
{
    fputs( callname, f );
    fputc( ' ', f );
    pp_origsymbol( f, call->name );
    fputs( " ", f );
    pp_actualParameters( f, typelist, symtab, call->thisparameters, call->parameters );
}

/* Given a method invocation expression, generate a function
 * call expression for it.
 */
static void pp_functioncall( FILE *f, const_TypeEntry_list typelist, const_Entry_list symtab, const_MethodInvocationExpression call )
{
    pp_MethodInvocation( f, typelist, symtab, "functioncall", call->invocation );
}
 
// Given a file handle 'f', a type list, a symbol table, and an expression
// that evaluates to the class instance that contains the vtable, generate
// a cast to the type of the vtable of the class instance.
static void pp_virtual_call(
 FILE *f,
 const_TypeEntry_list typelist,
 const_Entry_list symtab,
 const char *whatcall,
 const_MethodInvocation call
)
{
    const_expression vtab_source = call->vtab;
    origsymbol proc = call->name;
    const_expression_list thisparms = call->thisparameters;
    const_expression_list parms = call->parameters;

    type t = derive_type_expression( symtab, vtab_source );
    if( t != typeNIL ){
	if( t->tag != TAGObjectType ){
	    tmstring s = typename_type( t );
	    sprintf( errarg, "actual type: %s", s );
	    internal_error( "only objects have a vtable" );
	}
	else {
	    TypeEntry te = lookup_TypeEntry( typelist, to_ObjectType(t)->name );
	    switch( te->tag ){
		case TAGInterfaceEntry:
		    fputs( whatcall, f );
		    fputs( " *field *field *field *", f );
		    pp_expression( f, typelist, symtab, vtab_source );
		    fputs( " __ivtable ", f );
		    pp_censored_typename( f, te->name );
		    fputs( " __ivte_", f );
		    pp_origsymbol( f, proc );
		    fputs( " ", f );
		    pp_actualParameters( f, typelist, symtab, thisparms, parms );
		    break;

		case TAGPackageEntry:
		case TAGForwardObjectEntry:
		    internal_error( "only a class or interface has a vtable" );
		    break;

		case TAGClassEntry:
		    fputs( whatcall, f );
		    fputs( " *field *cast pointer record __t_", f );
		    pp_tmsymbol( f, to_ClassEntry( te )->vtabname );
		    fputs( " field *", f );
		    pp_expression( f, typelist, symtab, vtab_source );
		    fputs( " __vtable __vte_", f );
		    pp_origsymbol( f, proc );
		    fputs( " ", f );
		    pp_actualParameters( f, typelist, symtab, thisparms, parms );
		    break;

	    }

	}
	rfre_type( t );
    }
    fputc( ' ', f );
}

/* Given a method invocation expression, generate a function
 * call expression for it.
 */
static void pp_virtualfunctioncall( FILE *f, const_TypeEntry_list typelist, const_Entry_list symtab, const_MethodInvocationExpression call )
{
    pp_virtual_call(
	f,
	typelist,
	symtab,
	"functioncall",
	call->invocation
    );
}
 
/* Given a file handle 'f' and a Cardinality 'card', print it. */
static void pp_Cardinality( FILE *f, const_TypeEntry_list typelist, const_Entry_list symtab, const_Cardinality card )
{
   pp_origsymbol( f, card->name );
   fputs( "=", f );
   if( card->lowerbound != expressionNIL ){
       pp_expression( f, typelist, symtab, card->lowerbound );
   }
   else {
       fputs( "0", f );
   }
   fputs( ":", f );
   pp_expression( f, typelist, symtab, card->upperbound );
   if( card->stride != expressionNIL ){
       fputs( ":", f );
       pp_expression( f, typelist, symtab, card->stride );
   }
}

/* Given a file handle 'f' and a list of Cardinalities 'cl', print it. */
static void pp_Cardinalities( FILE *f, const_TypeEntry_list typelist, const_Entry_list symtab, const_Cardinality_list cl )
{
    fputs( "[", f );
    for( unsigned int ix=0; ix<cl->sz; ix++ ){
	if( ix != 0 ){
	    fputs( ", ", f );
	}
	pp_Cardinality( f, typelist, symtab, cl->arr[ix] );
    }
    fputs( "]", f );
}

static void pp_subscripts( FILE *f, const_TypeEntry_list typelist, const_Entry_list symtab, const_expression_list sels )
{
    fputs( "[", f );
    for( unsigned int ix=0; ix<sels->sz; ix++ ){
	if( ix != 0 ){
	    fputs( ", ", f );
	}
	pp_expression( f, typelist, symtab, sels->arr[ix] );
    }
    fputs( "]", f );
}

// Given an expression that is used to subscript an array, generate
// something that Vnus is happy with.
static void pp_subscript( FILE *f, const_TypeEntry_list typelist, const_Entry_list symtab, const_expression subscript )
{
    if( subscript->tag == TAGVectorExpression ){
	pp_subscripts( f, typelist, symtab, to_const_VectorExpression(subscript)->fields );
    }
    else if( subscript->tag == TAGVariableNameExpression ){
	// Probably because the vector expression has been broken out.
	// Grin and bear it.
	const_VariableNameExpression nx = to_const_VariableNameExpression(subscript);
	type t = derive_type_expression( symtab, nx );
	unsigned int sz;
	if( t->tag == TAGTupleType ){
	    sz = to_TupleType(t)->fields->sz;
	}
	else if( t->tag == TAGVectorType ){
	    if( !extract_vector_power( nx->name->org, to_VectorType(t)->power, &sz ) ){
		sz = 0;
	    }
	}
	else {
	    internal_error( "Subscript variable is not a tuple" );
	}
	rfre_type( t );
	fputc( '[', f );
	for( unsigned n = 0; n<sz; n++ ){
	    if( n != 0 ){
		fputc( ',', f );
	    }
	    fputs( "field ", f );
	    pp_origsymbol( f, nx->name );
	    fprintf( f, " %u", n );
	}
	fputc( ']', f );
    }
    else {
	dump_expression( stderr, subscript, "Subscript expression" );
	internal_error( "Non-trivial subscript expression should have been rewritten" );
    }
}

static void pp_string_constant( FILE *f, const_tmstring s )
{
    fputc( '"', f );
    for( const char *p = (const char *) s; *p != '\0'; p++ ){
	if( p[0] == '"' || (p[0] == '\\' && p[1] == 'u') ){
	    fputc( '\\', f );
	}
	fputc( p[0], f );
    }
    fputc( '"', f );
}

static void pp_PragmaExpressions( FILE *f, const_PragmaExpression_list l )
{
    fputs( "(", f );
    for( unsigned int ix=0; ix<l->sz; ix++ ){
	if( ix != 0 ){
	    fputs( " ", f );
	}
	pp_PragmaExpression( f, l->arr[ix] );
    }
    fputs( ")", f );
}

static void pp_PragmaExpression( FILE *f, const_PragmaExpression x )
{
    switch( x->tag ){
	case TAGNumberPragmaExpression:
	{
	    tmstring s = vnus_double_to_string( to_const_NumberPragmaExpression(x)->v );
	    fputs( s, f );
	    rfre_tmstring( s );
	    break;
	}

	case TAGStringPragmaExpression:
	    pp_string_constant( f, to_const_StringPragmaExpression(x)->s );
	    break;

	case TAGBooleanPragmaExpression:
	    fprintf( f, "%s", to_const_BooleanPragmaExpression(x)->b ? "true" : "false" );
	    break;

	case TAGNamePragmaExpression:
	    pp_origsymbol( f, to_const_NamePragmaExpression(x)->name );
	    break;

	case TAGExternalNamePragmaExpression:
	    fputc( '@', f );
	    pp_origsymbol( f, to_const_ExternalNamePragmaExpression(x)->name );
	    break;

	case TAGListPragmaExpression:
	    pp_PragmaExpressions( f, to_const_ListPragmaExpression(x)->l );
	    break;
    }
}

static void pp_Pragma( FILE *f, const_Pragma pr )
{
    switch( pr->tag ){
        case TAGFlagPragma:
	    pp_origsymbol( f, pr->name );
            break;

	case TAGValuePragma:
	    pp_origsymbol( f, pr->name );
	    fputs( "=", f );
	    pp_PragmaExpression( f, to_const_ValuePragma(pr)->x );
	    break;

    }
}

static void pp_Pragma_list( FILE *f, const_Pragma_list pl )
{
    fputs( "[", f );
    for( unsigned int ix=0; ix<pl->sz; ix++ ){
        if( ix!=0 ){
	    fputs( ",", f );
        }
        pp_Pragma( f, pl->arr[ix] );
    }
    fputs( "]", f );
}

static bool pp_opt_Pragma_list( FILE *f, unsigned int indent, modflags modifiers, bool show_modifiers, const_Pragma_list pl )
{
    bool did_indent = false;

    if( !show_modifiers ){
	 modifiers = 0;
    }
    if( has_any_flag( modifiers, ACC_FINAL|ACC_DEDUCED_FINAL ) ){
	pp_indent( f, indent );
	fputs( "final ", f );
	did_indent = true;
    }
    if( modifiers & ACC_VOLATILE ){
	pp_indent( f, indent );
	fputs( "volatile ", f );
	did_indent = true;
    }
    if( pl == Pragma_listNIL ){
	return did_indent;
    }
    Pragma_list pragmas = rdup_Pragma_list( pl );
    if( show_modifiers ){
	unsigned int ix;

	if( search_pragma_list( pragmas, "local", &ix ) ){
	    pragmas = delete_Pragma_list( pragmas, ix );
	    fputs( "local ", f );
	    did_indent = true;
	}
	if( search_pragma_list( pragmas, "unchecked", &ix ) ){
	    pragmas = delete_Pragma_list( pragmas, ix );
	    fputs( "unchecked ", f );
	    did_indent = true;
	}
	if( search_pragma_list( pragmas, "readonly", &ix ) ){
	    pragmas = delete_Pragma_list( pragmas, ix );
	    fputs( "readonly ", f );
	    did_indent = true;
	}
    }
    if( pragmas->sz != 0 ){
	pp_indent( f, indent );
	fputs( "pragma ", f );
	pp_Pragma_list( f, pragmas );
	did_indent = true;
    }
    rfre_Pragma_list( pragmas );
    return did_indent;
}

static void pp_char_constant( FILE *f, vnus_char tm_s )
{
    if( tm_s<0x80 && tm_s>=' ' ){
	// Innocent character, just print it.
	fprintf( f, "'%c'", (char) tm_s );
    }
    else {
	// It's always safe to print a char like that, although
	// Vnus may mess it up at its side.
	fprintf( f, "cast char %ld", (long int) tm_s );
    }
}

static void pp_internalize_expression( FILE *f, const_TypeEntry_list typelist, const_Entry_list symtab, tmsymbol fnname, const_expression x )
{
    fputs( "functioncall ", f );
    pp_tmsymbol( f, fnname );
    fputs( " [", f );
    pp_expression( f, typelist, symtab, x );
    fputc( ']', f );
}

// Given a file handle 'f' and a type 't', prettyprint the default
// value for this type to 'f'.
static void pp_defaultvalue( FILE *f, const_TypeEntry_list typelist, const_Entry_list symtab, const_type t )
{
    switch( t->tag ){
	case TAGPragmaType:
	    pp_defaultvalue( f, typelist, symtab, to_const_PragmaType(t)->t );
	    break;

	case TAGTupleType:
	{
	    type_list fields = to_const_TupleType(t)->fields;

	    fputs( "record [", f );
	    for( unsigned int ix=0; ix<fields->sz; ix++ ){
		if( ix != 0 ){
		    fputc( ',', f );
		}
		pp_defaultvalue( f, typelist, symtab, fields->arr[ix] );
	    }
	    fputc( ']', f );
	    break;
	}

	case TAGVectorType:
	{
	    unsigned int power;

	    if( extract_vector_power( originNIL, to_const_VectorType(t)->power, &power ) ){
		fputs( "record [", f );
		for( unsigned int ix=0; ix<power; ix++ ){
		    if( ix != 0 ){
			fputc( ',', f );
		    }
		    pp_defaultvalue( f, typelist, symtab, to_const_VectorType(t)->elmtype );
		}
		fputc( ']', f );
	    }
	    break;
	}

	case TAGPrimitiveType:
	    switch( to_const_PrimitiveType(t)->base ){
		case BT_BOOLEAN:
		    fputs( "false", f );
		    break;

		case BT_STRING:
		    fputs( "\"\"", f );
		    break;

		case BT_COMPLEX:
		    fputs( "complex 0d 0d", f );
		    break;

		case BT_BYTE:
		    fputs( "0b", f );
		    break;

		case BT_SHORT:
		    fputs( "0s", f );
		    break;

		case BT_CHAR:
		    fputs( "'\\0'", f );
		    break;

		case BT_INT:
		    fputs( "0", f );
		    break;

		case BT_LONG:
		    fputs( "0l", f );
		    break;

		case BT_FLOAT:
		    fputs( "0.0", f );
		    break;

		case BT_DOUBLE:
		    fputs( "0.0d", f );
		    break;

	    }
	    break;

	case TAGTypeType:
	case TAGTypeOf:
	case TAGTypeOfIf:
	case TAGExceptionVariableType:
	case TAGGCRefLinkType:
	case TAGGCTopRefLinkType:
	case TAGArrayType:
	case TAGPrimArrayType:
	case TAGObjectType:
	case TAGGenericObjectType:
	case TAGNullType:
	case TAGFunctionType:
	case TAGVoidType:
	    fputs( "cast ", f );
	    pp_type( f, typelist, symtab, Pragma_listNIL, t );
	    fputs( " null", f );
	    break;
    }
}

/* Given a file handle 'f' and an expression 'x', generate C code
 * to evaluate the expression.
 */
static void pp_expression( FILE *f, const_TypeEntry_list typelist, const_Entry_list symtab, const_expression x )
{
    if( x == expressionNIL ){
	fputs( "null", f );
	return;
    }
    switch( x->tag )
    {
	case TAGNullExpression:
	    fputs( "null", f );
	    break;

	case TAGByteExpression:
	    fprintf( f, "%ldb", (long) to_const_ByteExpression(x)->v );
	    break;

	case TAGShortExpression:
	    fprintf( f, "%lds", (long) to_const_ShortExpression(x)->v );
	    break;

	case TAGIntExpression:
	    fprintf( f, "%ld", (long) to_const_IntExpression(x)->v );
	    break;

	case TAGLongExpression:
	    fprint_vnus_long( f, to_const_LongExpression(x)->v );
	    fputs( "l", f );
	    break;

	case TAGFloatExpression:
	{
	    // TODO: print in a more readable format when possible
	    vnus_int n = vnus_floatbits_to_int( to_const_FloatExpression(x)->v );
	    fprintf( f, "%ldt", (long) n );
	    break;
	}

	case TAGDoubleExpression:
	{
	    // TODO: print in a more readable format when possible
	    vnus_double v = to_const_DoubleExpression(x)->v;
	    char buf[100];
	    bool ok = true;

	    if( sprintf( buf, "%.9f", v ) == 0 ){
	        ok = false;
	    }
	    if( strcmp( buf, "inf" ) == 0 ){
	        ok = false;
	    }
	    if( strcmp( buf, "-inf" ) == 0 ){
	        ok = false;
	    }
	    if( ok ){
	        double d1 = strtod( buf, NULL );
		if( d1 != v || d1 == -v ){
		    ok = false;
		}
	    }
	    if( ok ){
	        fputs( buf, f );
		fputc( 'd', f );
	    }
	    else {
		vnus_long n = vnus_doublebits_to_long( v );

		fprint_vnus_long( f, n );
		fputc( 'u', f );
	    }
	    break;
	}

	case TAGCharExpression:
	    pp_char_constant( f, to_const_CharExpression(x)->c );
	    break;

	case TAGDefaultValueExpression:
	    pp_defaultvalue( f, typelist, symtab, to_const_DefaultValueExpression(x)->t );
	    break;

	case TAGClassIdExpression:
	{
	    // TODO: implement this correctly
	    origsymbol nm = to_const_ClassIdExpression(x)->cl;
	    TypeEntry te = lookup_TypeEntry( typelist, nm );
	    if( !is_ConcreteTypeEntry( te ) ){
		origsymbol_internal_error( nm, "Not a concrete type" );
		break;
	    }
	    fprintf( f, "%d", to_ConcreteTypeEntry(te)->classno );
	    break;
	}

	case TAGGetBufExpression:
	    fputs( "getbuf *", f );
	    pp_expression( f, typelist, symtab, to_const_GetBufExpression(x)->array );
	    break;

	case TAGSizeofExpression:
	    fputs( "sizeof ", f );
	    pp_type( f, typelist, symtab, Pragma_listNIL, to_const_SizeofExpression(x)->t );
	    break;

	case TAGInternalizeExpression:
	    pp_internalize_expression(
		f,
		typelist,
		symtab,
		to_const_InternalizeExpression(x)->fnname,
		to_const_InternalizeExpression(x)->x
	    );
	    break;

	case TAGCastExpression:
	    fputs( "cast ", f );
	    pp_type( f, typelist, symtab, Pragma_listNIL, to_const_CastExpression(x)->t );
	    fputs( " ", f );
	    pp_expression( f, typelist, symtab, to_const_CastExpression(x)->x );
	    break;

	case TAGForcedCastExpression:
	    fputs( "cast ", f );
	    pp_type( f, typelist, symtab, Pragma_listNIL, to_const_ForcedCastExpression(x)->t );
	    fputs( " ", f );
	    pp_expression( f, typelist, symtab, to_const_ForcedCastExpression(x)->x );
	    break;

	case TAGGetSizeExpression:
	    fputs( "getsize *", f );
	    pp_expression( f, typelist, symtab, to_const_GetSizeExpression(x)->array );
	    fputs( " ", f );
	    pp_expression( f, typelist, symtab, to_const_GetSizeExpression(x)->dim );
	    break;

	case TAGGetLengthExpression:
	    fputs( "getlength *", f );
	    pp_expression( f, typelist, symtab, to_const_GetLengthExpression(x)->array );
	    break;

	case TAGStringExpression:
	    pp_string_constant( f, to_const_StringExpression(x)->s );
	    break;

	case TAGBooleanExpression:
	    fprintf( f, "%s", to_const_BooleanExpression(x)->b ? "true" : "false" );
	    break;

	case TAGComplexExpression:
	    fputs( "complex ", f );
	    pp_expression( f, typelist, symtab, to_const_ComplexExpression(x)->re );
	    fputs( " ", f );
	    pp_expression( f, typelist, symtab, to_const_ComplexExpression(x)->im );
	    break;

	case TAGVectorExpression:
	    fputs( "record [", f );
	    pp_expression_list( f, typelist, symtab, to_const_VectorExpression(x)->fields );
	    fputs( "]", f );
	    break;

	case TAGFieldInvocationExpression:
	case TAGSuperInvocationExpression:
	case TAGOuterSuperInvocationExpression:
	case TAGTypeInvocationExpression:
	    internal_error( "invocation should have been rewritten" );
	    break;

	case TAGTypeExpression:
	    internal_error( "type expression should have been rewritten" );
	    break;

	case TAGMethodInvocationExpression:
	{
	    const_MethodInvocationExpression call = to_const_MethodInvocationExpression(x);

	    if( call->invocation->vtab == NULL ){
		pp_functioncall( f, typelist, symtab, call );
	    }
	    else {
		pp_virtualfunctioncall( f, typelist, symtab, call );
	    }
	    break;
	}

	case TAGVectorSubscriptExpression:
	    fputs( "field ", f );
	    pp_expression( f, typelist, symtab, to_const_VectorSubscriptExpression(x)->vector );
	    fprintf( f, " %u", to_const_VectorSubscriptExpression(x)->subscript );
	    break;

	case TAGSubscriptExpression:
	{
	    const_expression arr = to_const_SubscriptExpression(x)->array;
	    type t = derive_type_expression( symtab, arr );
	    if( t->tag == TAGPrimArrayType ){
		fputs( "(", f );
	    }
	    else {
		fputs( "(*", f );
	    }
	    rfre_type( t );
	    pp_expression( f, typelist, symtab, arr );
	    fputs( ", ", f );
	    pp_subscript( f, typelist, symtab, to_const_SubscriptExpression(x)->subscripts );
	    fputs( ")", f );
	    break;
	}

	case TAGUnopExpression:
	    fputs( "( ", f );
	    pp_unary_operator( f, to_const_UnopExpression(x)->optor );
	    fputs( ", ", f );
	    pp_expression( f, typelist, symtab, to_const_UnopExpression(x)->operand );
	    fputs( " )", f );
	    break;

	case TAGBinopExpression:
	    fputs( "( ", f );
	    pp_expression( f, typelist, symtab, to_const_BinopExpression(x)->left );
	    fputs( ", ", f );
	    pp_binary_operator( f, to_const_BinopExpression(x)->optor );
	    fputs( ", ", f );
	    pp_expression( f, typelist, symtab, to_const_BinopExpression(x)->right );
	    fputs( " )", f );
	    break;

	case TAGShortopExpression:
	    fputs( "( ", f );
	    pp_expression( f, typelist, symtab, to_const_ShortopExpression(x)->left );
	    fputs( ", ", f );
	    pp_shortcircuit_operator( f, to_const_ShortopExpression(x)->optor );
	    fputs( ", ", f );
	    pp_expression( f, typelist, symtab, to_const_ShortopExpression(x)->right );
	    fputs( " )", f );
	    break;

	case TAGNewClassExpression:
	    internal_error( "Class constructor invocation should have been rewritten" );
	    break;

	case TAGNewRecordExpression:
	{
	    const_type t = to_const_NewRecordExpression(x)->cl;
	    if( t->tag != TAGObjectType ){
		internal_error( "record type is not a class" );
		break;
	    }
	    TypeEntry te = lookup_TypeEntry( typelist, to_const_ObjectType(t)->name );
	    if( te->tag != TAGClassEntry ){
		internal_error( "record type is not a class" );
		break;
	    }
	    tmsymbol recnm = to_ClassEntry(te)->recordname;
	    fprintf( f, "functioncall __init_%s [new record %s]", recnm->name, recnm->name );
	    break;
	}

	case TAGInstanceOfExpression:
	    internal_error( "InstanceOf expression should have been rewritten" );
	    break;

	case TAGTypeInstanceOfExpression:
	    internal_error( "TypeInstanceOf expression should have been rewritten" );
	    break;

	case TAGInterfaceInstanceOfExpression:
	{
	    // Generate code to test wether a given class is an instance
	    // of the given type. That is easy, we simple check wether
	    // the corresponding entry in the ivtable is non-null
	    fputs( "(cast pointer record [] field *field *", f );
	    pp_expression( f, typelist, symtab, to_const_InterfaceInstanceOfExpression( x )->x );
	    fputs( " __ivtable ", f );
	    pp_censored_typename( f, to_const_InterfaceInstanceOfExpression( x )->t );
	    fputs( ",<>,cast pointer record [] null)", f );
	    break;
	}

	case TAGClassInstanceOfExpression:
	{
	    const_ClassInstanceOfExpression iof = to_const_ClassInstanceOfExpression( x );
	    TypeEntry te = lookup_TypeEntry( typelist, iof->t );
	    int classno = 0;
	    int max_subclass = 0;

	    if( te == TypeEntryNIL ){
		break;
	    }
	    if( is_ConcreteTypeEntry( te ) ){
		classno = to_ConcreteTypeEntry( te )->classno;
		max_subclass = to_ConcreteTypeEntry( te )->last_subclassno;
	    }
	    else {
		origsymbol_internal_error( iof->t, "Not a concrete type" );
	    }
	    if( te->usecount == 0 ){
		fputs( "false", f );
	    }
	    else {
		if( classno == max_subclass ){
		    fprintf( f, "(%u,", classno );
		    pp_binary_operator( f, BINOP_EQUAL );
		    fputs( ", field * field * field *", f );
		    pp_expression( f, typelist, symtab, iof->x );
		    fputs( " __vtable __introspection classno )", f );
		}
		else {
		    fprintf( f, "((%u,", classno );
		    pp_binary_operator( f, BINOP_LESSEQUAL );
		    fputs( ", field * field * field *", f );
		    pp_expression( f, typelist, symtab, iof->x );
		    fputs( " __vtable __introspection classno ), ", f );
		    pp_shortcircuit_operator( f, SHORTOP_AND );
		    fputs( ",( field * field * field *", f );
		    pp_expression( f, typelist, symtab, iof->x );
		    fputs( " __vtable __introspection last_subclassno, ", f );
		    pp_binary_operator( f, BINOP_LESSEQUAL );
		    fprintf( f, ", %u))", max_subclass );
		}
	    }
	    break;
	}

	case TAGPostIncrementExpression:
	case TAGPostDecrementExpression:
	case TAGPreIncrementExpression:
	case TAGPreDecrementExpression:
	    internal_error( "increment or decrement expression should have been rewritten" );
	    break;

	case TAGAssignOpExpression:
	    internal_error( "assignment expression should have been rewritten" );
	    break;

	case TAGAnnotationExpression:
	    fputs( "expressionpragma ", f );
	    pp_Pragma_list( f, to_const_AnnotationExpression(x)->pragmas );
	    fputs( " ", f );
	    pp_expression( f, typelist, symtab, to_const_AnnotationExpression(x)->x );
	    break;

	case TAGBracketExpression:
	    pp_expression( f, typelist, symtab, to_const_BracketExpression(x)->x );
	    break;

	case TAGIfExpression:
	    fputs( "if ", f );
	    pp_expression( f, typelist, symtab, to_const_IfExpression(x)->cond );
	    fputs( " ", f );
	    pp_expression( f, typelist, symtab, to_const_IfExpression(x)->thenval );
	    fputs( " ", f );
	    pp_expression( f, typelist, symtab, to_const_IfExpression(x)->elseval );
	    break;

	case TAGVariableNameExpression:
	    pp_origsymbol( f, to_const_VariableNameExpression(x)->name );
	    break;

	case TAGArrayInitExpression:
	{
	    const_ArrayInitExpression init = to_const_ArrayInitExpression(x);

	    fputs( "shape ", f );
	    if( init->t == typeNIL ){
		internal_error( "array init should have a type" );
	    }
	    if( init->t->tag != TAGArrayType ){
		tmstring s = typename_type( init->t );

		sprintf( errarg, "actual type: %s", s );
		internal_error( "array init should have array type" );
	    }
	    const_ArrayType at = to_const_ArrayType( init->t );

	    expression_list elms = init->elements;
	    const_expression rankexpr = at->rank;
	    unsigned int rank;

	    if( !extract_array_rank( originNIL, rankexpr, &rank ) ){
		rank = 0;
	    }
	    tmuint_list shape = setroom_tmuint_list( new_tmuint_list(), rank );
	    for( unsigned int ix=0; ix<rank; ix++ ){
		shape = append_tmuint_list( shape, 0 );
	    }
	    shape = calculate_array_shape( originNIL, elms, shape, 0, rank );

	    pp_Sizes( f, shape );
	    rfre_tmuint_list( shape );
	    fputs( " ", f );
	    pp_type( f, typelist, symtab, Pragma_listNIL, at->elmtype  );
	    fputs( " [", f );
	    pp_expression_list( f, typelist, symtab, init->elements );
	    fputs( "]", f );
	    break;
	}

	case TAGClassExpression:
	    internal_error( "'class' expression should have been rewritten" );
	    break;

	case TAGTypeFieldExpression:
	    internal_error( "type field should have been rewritten" );
	    break;

	case TAGSuperFieldExpression:
	case TAGOuterSuperFieldExpression:
	    internal_error( "'super' field should have been rewritten" );
	    break;

	case TAGFieldExpression:
	{
	    type t = derive_type_expression( symtab, to_const_FieldExpression(x)->rec );

	    fputs( "field *", f );
	    pp_expression( f, typelist, symtab, to_const_FieldExpression(x)->rec );
	    fputs( " ", f );
	    pp_origsymbol( f, to_const_FieldExpression(x)->field );
	    rfre_type( t );
	    break;
	}

	case TAGNewInitArrayExpression:
	    internal_error( "new array with initialization should have been rewritten" );
	    break;

	case TAGOuterThisExpression:
	    internal_error( "outer this expression should have been rewritten" );
	    break;

	case TAGNewArrayExpression:
	{
	    const_NewArrayExpression narr = to_const_NewArrayExpression(x);
	    optexpression init = narr->initval;
	    bool nulled = false;

	    if( init->tag == TAGOptExpr ){
		if(
		    to_OptExpr(init)->x->tag == TAGDefaultValueExpression &&
		    narr->t->tag == TAGPrimitiveType
		){
		    nulled = true;
		    fputs( "nullednew", f );
		}
		else {
		    fputs( "fillednew", f );
		}
	    }
	    else {
		// TODO: once Vnus allows uninitialized shapes, generate
		// code for that case.
		fputs( "nullednew", f );
		nulled = true;
	    }
	    ActualDim dim = to_const_NewArrayExpression(x)->sizes->arr[0];
	    (void) pp_opt_Pragma_list( f, 1, 0, false, dim->pragmas );
	    fputs( " shape ", f );
	    if( to_const_NewArrayExpression(x)->sizes->sz != 1 ){
		internal_error( "nested new array expression should have been rewritten" );
	    }
	    pp_subscript( f, typelist, symtab, dim->vector );
	    fputs( " ", f );
	    pp_type( f, typelist, symtab, Pragma_listNIL, narr->t );
	    if( !nulled && init->tag == TAGOptExpr ){
		fputs( " ", f );
		pp_expression( f, typelist, symtab, to_OptExpr(init)->x );
	    }
	    if( pref_nogarbagecollection ){
		fputs( " null", f );
	    }
	    else {
		tmsymbol markfn = get_arraymarker_name( narr->t );
		if( markfn == tmsymbolNIL ){
		    fputs( " null", f );
		}
		else {
		    fputs( " ", f );
		    pp_tmsymbol( f, markfn );
		}
	    }
	    break;
	}

	case TAGNotNullAssertExpression:
	{
	    const_NotNullAssertExpression ax = to_const_NotNullAssertExpression(x);
	    fputs( "notnullassert ", f );
	    pp_expression( f, typelist, symtab, ax->x );
	    break;
	}

	case TAGWhereExpression:
	{
	    const_WhereExpression wx = to_const_WhereExpression(x);
	    fputs( "where ", f );
	    if( wx->scope != tmsymbolNIL ){
		pp_tmsymbol( f, wx->scope );
		fputs( " ", f );
	    }
	    pp_expression( f, typelist, symtab, wx->x );
	    break;
	}

    }
}

/* Given a file handle and a list of expressions, generate an expression
 * list.
 */
static void pp_expression_list( FILE *f, const_TypeEntry_list typelist, const_Entry_list symtab, const_expression_list xl )
{
    bool first = true;
    for( unsigned int ix=0; ix<xl->sz; ix++ ){
	if( first ){
	    first = false;
	}
	else {
	    fputs( ", ", f );
	}
	pp_expression( f, typelist, symtab, xl->arr[ix] );
    }
}

/* Given a file handle and a list of expressions, generate an expression
 * list.
 */
static void pp_actualParameters(
 FILE *f,
 const_TypeEntry_list typelist,
 const_Entry_list symtab,
 const_expression_list this_xl,
 const_expression_list xl
)
{
    bool first = true;
    fputs( "[", f );
    if( this_xl != expression_listNIL ){
	for( unsigned int ix=0; ix<this_xl->sz; ix++ ){
	    if( first ){
		first = false;
	    }
	    else {
		fputs( ", ", f );
	    }
	    pp_expression( f, typelist, symtab, this_xl->arr[ix] );
	}
    }
    for( unsigned int ix=0; ix<xl->sz; ix++ ){
	if( first ){
	    first = false;
	}
	else {
	    fputs( ", ", f );
	}
	pp_expression( f, typelist, symtab, xl->arr[ix] );
    }
    fputs( "]", f );
}

static void pp_assignment( FILE *f, const_TypeEntry_list typelist, const_Entry_list symtab, unsigned int indent, const_AssignStatement act )
{
    pp_indent( f, indent );
    if( act->op == ASSIGN ){
	fputs( "assign ", f );
	pp_expression( f, typelist, symtab, act->lhs );
	fputs( " ", f );
	pp_expression( f, typelist, symtab, act->rhs );
    }
    else {
	fputs( "assignop ", f );
	pp_expression( f, typelist, symtab, act->lhs );
	fputs( " ", f );
	pp_binary_operator( f, assignop_to_binop( act->op ) );
	fputs( " ", f );
	pp_expression( f, typelist, symtab, act->rhs );
    }
    fputs( "\n", f );
}

static void pp_SwitchCase( FILE *f, const_TypeEntry_list typelist, const_Entry_list symtab, unsigned int indent, const_SwitchCase cs )
{
    pp_indent_line( f, indent, "(" );
    pp_indent( f, indent+INDENTSTEP );
    switch( cs->tag ){
	case TAGSwitchCaseValue:
	{
	    vnus_long v = evaluate_integral_constant( to_const_SwitchCaseValue(cs)->cond );

	    fprintf( f, "%ld,\n", (long) v );
	    break;
	}

	case TAGSwitchCaseDefault:
	    fputs( "default,\n", f );
	    break;

    }
    pp_statements( f, typelist, symtab, indent+INDENTSTEP, cs->body );
    pp_indent_line( f, indent, ")" );
}

static void pp_SwitchCase_list( FILE *f, const_TypeEntry_list typelist, const_Entry_list symtab, unsigned int indent, const_SwitchCase_list l )
{
    for( unsigned int ix=0; ix<l->sz; ix++ ){
	if( ix!=0 ){
	    pp_indent_line( f, indent, "," );
	}
	pp_SwitchCase( f, typelist, symtab, indent, l->arr[ix] );
    }
}

static void pp_WaitCase( FILE *f, const_TypeEntry_list typelist, const_Entry_list symtab, unsigned int indent, const_WaitCase cs )
{
    pp_indent( f, indent );
    switch( cs->tag ){
	case TAGWaitCaseValue:
	{
	    fputs( "value ", f );
	    pp_expression( f, typelist, symtab, to_const_WaitCaseValue(cs)->cond );
	    fputc( '\n', f );
	    break;
	}

	case TAGWaitCaseTimeout:
	    fputs( "timeout ", f );
	    pp_expression( f, typelist, symtab, to_const_WaitCaseTimeout(cs)->time );
	    fputc( '\n', f );
	    break;

    }
    pp_statements( f, typelist, symtab, indent+INDENTSTEP, cs->body );
}

static void pp_WaitCase_list( FILE *f, const_TypeEntry_list typelist, const_Entry_list symtab, unsigned int indent, const_WaitCase_list l )
{
    for( unsigned int ix=0; ix<l->sz; ix++ ){
	if( ix!=0 ){
	    pp_indent_line( f, indent, "," );
	}
	pp_WaitCase( f, typelist, symtab, indent, l->arr[ix] );
    }
}

// Given a file handle, some context, and a wait statement, print
// that statement.
static void pp_wait_statement(
 FILE *f,
 const_TypeEntry_list typelist,
 const_Entry_list symtab,
 unsigned int indent,
 const_WaitStatement smt
)
{
    pp_indent_line( f, indent, "wait [" );
    pp_WaitCase_list( f, typelist, symtab, indent, smt->cases );
    pp_indent_line( f, indent, "]" );
}

static void pp_statement(
    FILE *f,
    const_TypeEntry_list typelist,
    const_Entry_list symtab,
    unsigned int indent,
    const_statement smt,
    bool *first
)
{
    if( is_declaration( smt ) ){
	return;
    }
    if( *first ){
	*first = false;
    }
    else {
	pp_indent_line( f, indent, "," );
    }
    if( smt->labels != origsymbol_listNIL ){
	origsymbol_list labels = smt->labels;
	for( unsigned int ix=0; ix<labels->sz; ix++ ){
	    pp_indent( f, indent );
	    pp_origsymbol( f, labels->arr[ix] );
	    if( ix+1>=labels->sz ){
		fputs( ":\n", f );
	    }
	    else {
		fputs( ": empty,\n", f );
	    }
	}
    }
    if( smt->pragmas != Pragma_listNIL && smt->pragmas->sz != 0 ){
        (void) pp_opt_Pragma_list( f, indent, 0, false, smt->pragmas );
        fputs( "\n", f );
    }
    switch( smt->tag ){
        case TAGIfStatement:
	    pp_indent( f, indent );
	    fputs( "if ", f );
	    pp_expression( f, typelist, symtab, to_const_IfStatement(smt)->cond );
	    fputs( "\n", f );
	    pp_Block( f, typelist, symtab, indent+INDENTSTEP, to_const_IfStatement(smt)->thenbody );
	    pp_Block( f, typelist, symtab, indent+INDENTSTEP, to_const_IfStatement(smt)->elsebody );
	    break;

	case TAGBlockStatement:
	    pp_Block( f, typelist, symtab, indent, to_const_BlockStatement(smt)->body );
	    break;

        case TAGDoWhileStatement:
	    assert( to_const_DoWhileStatement(smt)->update == NULL );
	    pp_indent( f, indent );
	    fputs( "dowhile ", f );
	    pp_expression( f, typelist, symtab, to_const_DoWhileStatement(smt)->cond );
	    fputs( "\n", f );
	    pp_Block( f, typelist, symtab, indent+INDENTSTEP, to_const_DoWhileStatement(smt)->body );
	    break;

        case TAGWhileStatement:
	    assert( to_const_WhileStatement(smt)->update == NULL );
	    pp_indent( f, indent );
	    fputs( "while ", f );
	    pp_expression( f, typelist, symtab, to_const_WhileStatement(smt)->cond );
	    fputs( "\n", f );
	    pp_Block( f, typelist, symtab, indent+INDENTSTEP, to_const_WhileStatement(smt)->body );
	    break;

	case TAGReturnStatement:
	    pp_indent_line( f, indent, "return" );
	    break;

	case TAGValueReturnStatement:
	    pp_indent( f, indent );
	    fputs( "return ", f );
	    pp_expression( f, typelist, symtab, to_const_ValueReturnStatement(smt)->v );
	    fputs( "\n", f );
	    break;

	case TAGGotoStatement:
	    pp_indent( f, indent );
	    fputs( "goto ", f );
	    pp_origsymbol( f, to_const_GotoStatement(smt)->target );
	    fputs( "\n", f );
	    break;

        case TAGForStatement:
	    pp_indent( f, indent );
	    fputs( "for ", f );
	    pp_Cardinalities( f, typelist, symtab, to_const_ForStatement(smt)->cards );
	    fputs( "\n", f );
	    pp_Block( f, typelist, symtab, indent+INDENTSTEP, to_const_ForStatement(smt)->body );
	    break;

	case TAGSwitchStatement:
	    pp_indent( f, indent );
	    fputs( "switch ", f );
	    pp_expression( f, typelist, symtab, to_const_SwitchStatement(smt)->cond );
	    fputs( " [\n ", f );
	    pp_SwitchCase_list( f, typelist, symtab, indent, to_const_SwitchStatement(smt)->cases );
	    pp_indent_line( f, indent, "]" );
	    break;

	case TAGEachStatement:
	    pp_indent_line( f, indent, "each [" );
	    pp_statement_list( f, typelist, symtab, indent+INDENTSTEP, to_const_EachStatement(smt)->statements );
	    pp_indent_line( f, indent, "]" );
	    break;

        case TAGForeachStatement:
	    pp_indent( f, indent );
	    fputs( "foreach ", f );
	    pp_Cardinalities( f, typelist, symtab, to_const_ForeachStatement(smt)->cards );
	    fputs( "\n", f );
	    pp_Block( f, typelist, symtab, indent+INDENTSTEP, to_const_ForeachStatement(smt)->body );
	    break;

	case TAGEmptyStatement:
	    pp_indent_line( f, indent, "empty" );
	    break;

        case TAGAssignStatement:
	    pp_assignment( f, typelist, symtab, indent, to_const_AssignStatement(smt) );
	    break;

        case TAGPrintStatement:
            pp_indent( f, indent );
	    fputs( "print ", f );
	    pp_actualParameters( f, typelist, symtab, expression_listNIL, to_const_PrintStatement(smt)->argv );
	    fputs( "\n", f );
	    break;

        case TAGPrintLineStatement:
            pp_indent( f, indent );
	    fputs( "println ", f );
	    pp_actualParameters( f, typelist, symtab, expression_listNIL, to_const_PrintLineStatement(smt)->argv );
	    fputs( "\n", f );
	    break;

        case TAGMethodInvocationStatement:
	{
	    const_MethodInvocationStatement call = to_const_MethodInvocationStatement(smt);
	    pp_indent( f, indent );
	    if( call->invocation->vtab == NULL ){
		pp_MethodInvocation( f, typelist, symtab, "procedurecall", call->invocation );
	    }
	    else {
		pp_virtual_call(
		    f,
		    typelist,
		    symtab,
		    "procedurecall",
		    call->invocation
		);
	    }
	    fputs( "\n", f );
	    break;
	}

	case TAGDeleteStatement:
	    pp_indent( f, indent );
	    fputs( "delete ", f );
	    pp_expression( f, typelist, symtab, to_const_DeleteStatement(smt)->adr );
	    fputs( "\n", f );
	    break;

	case TAGThisConstructorInvocationStatement:
	case TAGSuperConstructorInvocationStatement:
	    internal_error( "explicit constructor invocation should have been rewritten" );
	    break;

        case TAGFieldInvocationStatement:
	    internal_error( "field invocation statement should have been rewritten" );
	    break;

        case TAGTypeInvocationStatement:
	    internal_error( "type invocation statement should have been rewritten" );
	    break;

        case TAGOuterSuperInvocationStatement:
        case TAGSuperInvocationStatement:
        case TAGOuterSuperConstructorInvocationStatement:
	    internal_error( "super invocation statement should have been rewritten" );
	    break;

	case TAGClassicForStatement:
	    internal_error( "classic for statement should have been rewritten" );
	    break;

	case TAGBreakStatement:
	case TAGContinueStatement:
	    internal_error( "break or continue statement should have been rewritten" );
	    break;

	case TAGThrowStatement:
	    pp_indent( f, indent );
	    fputs( "throw ", f );
	    pp_expression( f, typelist, symtab, to_const_ThrowStatement(smt)->x );
	    fputs( "\n", f );
	    break;

	case TAGInternalThrowStatement:
	    pp_indent( f, indent );
	    fputs( "throw ", f );
	    pp_expression( f, typelist, symtab, to_const_InternalThrowStatement(smt)->x );
	    fputs( "\n", f );
	    break;

	case TAGConstructorInvocationStatement:
	    internal_error( "'ConstructorInvocationStatement' should have been rewritten" );
	    break;

	case TAGTryStatement:
	    internal_error( "'TryStatement' should have been rewritten" );
	    break;

	case TAGWaitStatement:
	    pp_wait_statement( f, typelist, symtab, indent, to_const_WaitStatement( smt ) );
	    break;

	case TAGSimpleTryStatement:
	    pp_indent( f, indent );
	    fputs( "catch ", f );
	    pp_FormalParameter( f, to_const_SimpleTryStatement(smt)->catchvar );
	    fputs( "\n", f );
	    pp_Block( f, typelist, symtab, indent+INDENTSTEP, to_const_SimpleTryStatement(smt)->body );
	    pp_Block( f, typelist, symtab, indent+INDENTSTEP, to_const_SimpleTryStatement(smt)->catches );
	    break;

	case TAGSynchronizedStatement:
	    // TODO: do this right !!!
	    pp_Block( f, typelist, symtab, indent, to_const_SynchronizedStatement(smt)->body );
	    break;

	case TAGExpressionStatement:
	    pp_indent( f, indent );
	    fputs( "expression ", f );
	    pp_expression( f, typelist, symtab, to_const_ExpressionStatement(smt)->x );
	    fputs( "\n", f );
	    break;

	case TAGGCBuildRefChainLinkStatement:
	{
	    const_GCBuildRefChainLinkStatement cs = to_const_GCBuildRefChainLinkStatement(smt);
	    tmsymbol_list candidates = cs->candidates;
	    tmsymbol obj = add_tmsymbol( "java.lang.Object" );

	    pp_indent( f, indent );
	    fputs( "assign field ", f );
	    pp_tmsymbol( f, cs->linkname );
	    fputs( " next ", f );
	    tmsymbol oldlinkname = cs->oldlinkname;
	    if( oldlinkname == tmsymbolNIL ){
		// There is no previous refchain link in this context, so
		// the end of the link is in __gc_reflink_chain.
		fputs( "__gc_reflink_chain", f );
	    }
	    else {
		// There is a previous refchain link in this context.
		// Link to it directly, so that we don't have to update
		// __gc_reflink_chain just because we want to build a new
		// refchain link.
		fputc( '&', f );
		pp_tmsymbol( f, oldlinkname );
	    }
	    fputs( ",\n", f );
	    pp_indent( f, indent );
	    fputs( "assign field ", f );
	    pp_tmsymbol( f, cs->linkname );
	    fprintf( f, " nelm %u,\n", candidates==NULL?0:candidates->sz );
	    pp_indent( f, indent );
	    fputs( "assign field ", f );
	    pp_tmsymbol( f, cs->linkname );
	    fputs( " elm cast array pointer ", f );
	    pp_ObjectType( f, typelist, obj );
	    if( candidates != NULL && candidates->sz != 0 ){
		fputs( " array pointer ", f );
		pp_ObjectType( f, typelist, obj );
		fputs( " [", f );
		bool firstelm = true;
		for( unsigned int iy=0; iy<candidates->sz; iy++ ){
		    if( !firstelm ){
			fputs( ", ", f );
		    }
		    else {
			firstelm = false;
		    }
		    fputs( "cast pointer ", f );
		    pp_ObjectType( f, typelist, obj );
		    fputs( " &", f );
		    pp_tmsymbol( f, candidates->arr[iy] );
		}
		fputs( "]", f );
	    }
	    else {
		fputs( " null", f );
	    }
	    fputs( ",\n", f );
	    pp_indent( f, indent );
	    fputs( "assign field ", f );
	    pp_tmsymbol( f, cs->linkname );
	    fprintf( f, " serial %d,\n", gc_serial++ );
	    pp_indent( f, indent );
	    fputs( "assign field ", f );
	    pp_tmsymbol( f, cs->linkname );
	    fprintf( f, " magic %ld\n", GCMAGIC );
	    break;
	}

	case TAGGCSetRefChainLinkStatement:
	{
	    const_GCSetRefChainLinkStatement cs = to_const_GCSetRefChainLinkStatement(smt);
	    tmsymbol linkname = cs->linkname;
	    if( linkname == tmsymbolNIL ){
		pp_indent_line( f, indent, "empty" );
	    }
	    else {
		// __gc_reflink_chain = &<link>
		pp_indent( f, indent );
		fputs( "assign __gc_reflink_chain &", f );
		pp_tmsymbol( f, linkname );
		fputs( "\n", f );
	    }
	    break;
	}

	case TAGGCSetTopRefChainLinkStatement:
	{
	    const_GCSetTopRefChainLinkStatement cs = to_const_GCSetTopRefChainLinkStatement(smt);
	    tmsymbol linkname = cs->linkname;
	    if( linkname == tmsymbolNIL ){
		pp_indent_line( f, indent, "empty" );
	    }
	    else {
		// __gc_reflink_chain = <link>
		pp_indent( f, indent );
		fputs( "assign __gc_reflink_chain ", f );
		pp_tmsymbol( f, linkname );
		fputs( "\n", f );
	    }
	    break;
	}

	case TAGStaticInitializer:
	case TAGInstanceInitializer:
	case TAGFunctionDeclaration:
	case TAGAbstractFunctionDeclaration:
	case TAGNativeFunctionDeclaration:
	case TAGConstructorDeclaration:
	case TAGFieldDeclaration:
	case TAGInterfaceDeclaration:
	case TAGClassDeclaration:
	    pp_indent_line( f, indent, "empty" );
	    break;
    }
}

static void pp_statement_list( FILE *f, const_TypeEntry_list typelist, const_Entry_list symtab, unsigned int indent, const_statement_list smts )
{
    if( smts == statement_listNIL ){
	return;
    }
    bool first = true;

    for( unsigned int ix=0; ix<smts->sz; ix++ ){
	pp_statement( f, typelist, symtab, indent, smts->arr[ix], &first );
    }
}

static void pp_statements( FILE *f, const_TypeEntry_list typelist, const_Entry_list symtab, unsigned int indent, const_statement_list smts )
{
    pp_indent( f, indent );
    fputs( "statements [\n", f );
    pp_statement_list( f, typelist, symtab, indent+INDENTSTEP, smts );
    pp_indent( f, indent );
    fputs( "]\n", f );
}

static void pp_Block( FILE *f, const_TypeEntry_list typelist, const_Entry_list symtab, unsigned int indent, const_Block blk )
{
    pp_indent( f, indent );
    fputs( "statements ", f );
    if( blk->scope != tmsymbolNIL ){
	pp_tmsymbol( f, blk->scope );
	fputc( ' ', f );
    }
    if( pp_opt_Pragma_list( f, 0, 0, false, blk->pragmas ) ){
	fputc( ' ', f );
    }
    fputs( "[\n", f );
    pp_statement_list( f, typelist, symtab, indent+INDENTSTEP, blk->statements );
    pp_indent( f, indent );
    fputs( "]\n", f );
}

static void pp_function(
 FILE *f, 
 const_TypeEntry_list typelist,
 const_Entry_list symtab,
 unsigned int indent,
 const tmsymbol name,
 const_FormalParameter_list this_parms,
 const_FormalParameter_list parms,
 const type rettype,
 const_Pragma_list pragmas,
 const_Block body
)
{
    pp_indent( f, indent );
    if( rettype->tag == TAGVoidType ){
	fputs( "procedure ", f );
    }
    else {
	fputs( "function ", f );
    }
    pp_tmsymbol( f, name );
    fputs( " ", f );
    pp_FormalParameters( f, this_parms, parms );
    if( rettype->tag != TAGVoidType ){
	fputs( " ", f );
	pp_type( f, typelist, symtab, Pragma_listNIL, rettype );
    }
    (void) pp_opt_Pragma_list( f, 1, 0, false, pragmas );
    fputs( "\n", f );
    pp_Block( f, typelist, symtab, indent, body );
}

static void pp_optexpression(
 FILE *f,
 const_TypeEntry_list typelist,
 const_Entry_list symtab,
 const_optexpression x
)
{
    switch( x->tag ){
	case TAGOptExprNone:
	    break;

	case TAGOptExpr:
	    fputs( " ", f );
	    pp_expression( f, typelist, symtab, to_const_OptExpr(x)->x );
    }
}

static void pp_declaration(
 FILE *f,
 const_TypeEntry_list typelist,
 const_Entry_list symtab,
 unsigned int indent,
 const_Entry decl,
 bool *first
)
{
    const tmsymbol name = decl->name;
    const Pragma_list pragmas = decl->pragmas;

    if( is_VariableEntry(decl) && !to_const_VariableEntry(decl)->isused ){
	return;
    }
    if(
	is_MethodEntry(decl) &&
	to_const_MethodEntry(decl)->directusecount == 0 &&
	!to_const_MethodEntry(decl)->isvirtuallyused
    ){
#if 0
	pp_indent( f, indent );
	fprintf( f, "// '%s' not used, skipped.\n", name->name );
#endif
	return;
    }
    // Skip the remainder of this function for declaration types that
    // we don't want to handle at all.
    if( decl->tag == TAGAbstractFunctionEntry ){
	return;
    }
    if( *first == false ){
	pp_indent_line( f, INDENTSTEP, "," );
    }
    *first = false;
    switch( decl->tag ){
	case TAGFunctionEntry:
	    pp_function(
		f,
		typelist,
		symtab,
		indent,
		name,
		to_const_FunctionEntry(decl)->thisparameters,
		to_const_FunctionEntry(decl)->parameters,
		to_const_FunctionEntry(decl)->t,
		pragmas,
		to_const_FunctionEntry(decl)->body
	    );
	    break;

        case TAGCardinalityVariableEntry:
	{
	    pp_indent( f, indent );
	    fputs( "cardinalityvariable ", f );
	    pp_tmsymbol( f, name );
	    (void) pp_opt_Pragma_list( f, 1, decl->flags, true, pragmas );
	    break;
	}

        case TAGGlobalVariableEntry:
	{
	    const_GlobalVariableEntry dv = to_const_GlobalVariableEntry(decl);
	    varflags modifiers = dv->flags;

	    if( dv->init->tag == TAGOptExprNone ){
		modifiers &= ~(ACC_FINAL|ACC_DEDUCED_FINAL);
	    }
	    pp_indent( f, indent );
	    fputs( "globalvariable ", f );
	    pp_tmsymbol( f, name );
	    fputs( " ", f );
	    pp_type( f, typelist, symtab, Pragma_listNIL, to_const_GlobalVariableEntry(decl)->t );
	    pp_optexpression( f, typelist, symtab, to_const_GlobalVariableEntry(decl)->init );
	    (void) pp_opt_Pragma_list( f, 1, modifiers, true, pragmas );
	    break;
	}

        case TAGLocalVariableEntry:
	{
	    const_LocalVariableEntry dv = to_const_LocalVariableEntry(decl);
	    varflags modifiers = dv->flags;

	    if( dv->init->tag == TAGOptExprNone ){
		modifiers &= ~(ACC_FINAL|ACC_DEDUCED_FINAL);
	    }
	    pp_indent( f, indent );
	    fputs( "localvariable ", f );
	    pp_tmsymbol( f, name );
	    fputs( " ", f );
	    pp_tmsymbol( f, dv->scope );
	    fputs( " ", f );
	    pp_type( f, typelist, symtab, Pragma_listNIL, dv->t );
	    pp_optexpression( f, typelist, symtab, dv->init );
	    (void) pp_opt_Pragma_list( f, 1, modifiers, true, pragmas );
	    break;
	}

        case TAGGCLinkEntry:
	    pp_indent( f, indent );
	    fputs( "localvariable ", f );
	    pp_tmsymbol( f, name );
	    fputs( " ", f );
	    pp_tmsymbol( f, to_const_GCLinkEntry(decl)->scope );
	    fputs( " record __gc_reflink", f );
	    (void) pp_opt_Pragma_list( f, 1, decl->flags, true, pragmas );
	    break;

        case TAGGCTopLinkEntry:
	    pp_indent( f, indent );
	    fputs( "localvariable ", f );
	    pp_tmsymbol( f, name );
	    fputs( " ", f );
	    pp_tmsymbol( f, to_const_GCTopLinkEntry(decl)->scope );
	    fputs( " pointer record __gc_reflink __gc_reflink_chain", f );
	    (void) pp_opt_Pragma_list( f, 1, decl->flags, true, pragmas );
	    break;

        case TAGFormalParameterEntry:
	    pp_indent( f, indent );
	    fputs( "formalvariable ", f );
	    pp_tmsymbol( f, name );
	    fputs( " ", f );
	    pp_tmsymbol( f, to_const_FormalParameterEntry(decl)->scope );
	    fputs( " ", f );
	    pp_type( f, typelist, symtab, Pragma_listNIL, to_const_FormalParameterEntry(decl)->t );
	    (void) pp_opt_Pragma_list( f, 1, decl->flags, true, pragmas );
	    break;

        case TAGExternalFunctionEntry:
	{
	    const_ExternalFunctionEntry fn = to_const_ExternalFunctionEntry(decl);

	    pp_indent( f, indent );
	    if( fn->t->tag == TAGVoidType ){
		fputs( "externalprocedure ", f );
	    }
	    else {
		fputs( "externalfunction ", f );
	    }
	    pp_tmsymbol( f, name );
	    fputs( " ", f );
	    pp_FormalParameters( f, fn->thisparameters, fn->parameters );
	    if( fn->t->tag != TAGVoidType ){
		fputs( " ", f );
		pp_type( f, typelist, symtab, Pragma_listNIL, fn->t );
	    }
	    (void) pp_opt_Pragma_list( f, 1, decl->flags, true, pragmas );
	    break;
	}

	case TAGAbstractFunctionEntry:
	{
	    const_AbstractFunctionEntry fn = to_const_AbstractFunctionEntry(decl);

	    pp_indent( f, indent );
	    if( fn->t->tag == TAGVoidType ){
		fputs( "externalprocedure ", f );
	    }
	    else {
		fputs( "externalfunction ", f );
	    }
	    pp_tmsymbol( f, name );
	    fputs( " ", f );
	    pp_FormalParameters( f, fn->thisparameters, fn->parameters );
	    if( fn->t->tag != TAGVoidType ){
		fputs( " ", f );
		pp_type( f, typelist, symtab, Pragma_listNIL, fn->t );
	    }
	    (void) pp_opt_Pragma_list( f, 1, decl->flags, true, pragmas );
	    break;
	}

	case TAGForwardFieldEntry:
	{
	    origsymbol_internal_error(
		to_const_ForwardFieldEntry( decl )->realname,
		"Forward field should have been resolved"
	    );
	    break;
	}

	case TAGForwardFunctionEntry:
	{
	    tmstring s = typename_Signature( to_const_ForwardFunctionEntry( decl )->realname );

	    internal_error(
		"prettyprint: forward method '%s' should have been resolved",
		s
	    );
	    break;
	}

	case TAGFieldEntry:
	    internal_error( "shouldn't try to print a class or interface" );
	    break;

    }
    fputs( "\n", f );
}


/* Given a file handle 'f', a type entry 'e' and a pointer to a boolean
 * 'first', generate a new initialized variable declaration that contains
 * the introspection info for that type.
 */
static tmsymbol pp_introspection_TypeEntry(
 FILE *f,
 const_TypeEntry_list typelist,
 const_ConcreteTypeEntry e,
 bool *first
)
{
    tmsymbol obj = add_tmsymbol( "java.lang.Object" );

    if( e->tag == TAGPackageEntry || e->usecount == 0 ){
	return tmsymbolNIL;
    }
    if( *first == false ){
	pp_indent_line( f, INDENTSTEP, "," );
    }
    *first = false;
    tmsymbol nm = e->introname;
    pp_indent( f, INDENTSTEP );
    fputs( "globalvariable ", f );
    pp_tmsymbol( f, nm );
    fputs( " record __introspection_type record [\"", f );
    pp_origsymbol( f, e->name );
    fprintf(
	f,
	"\", %u, %u, ",
	e->classno,
	e->last_subclassno
    );
    switch( e->tag ){
	case TAGClassEntry:
	{
	    tmsymbol ivtabname = to_const_ClassEntry(e)->ivtabname;
	    tmsymbol vtabname = to_const_ClassEntry(e)->vtabname;
	    tmsymbol markmethodname = to_const_ClassEntry(e)->markMethod;
	    tmsymbol packmethodname = to_const_ClassEntry(e)->packMethod;
	    tmsymbol unpackmethodname = to_const_ClassEntry(e)->unpackMethod;

	    fputs( "cast pointer record [] &", f );
	    pp_tmsymbol( f, vtabname );
	    fputs( ", cast pointer record [] &", f );
	    pp_tmsymbol( f, ivtabname );
	    fprintf( f, ", %ld", (long int) e->flags );

	    // Mark method
	    fputs( ",cast pointer procedure [", f );
	    pp_ObjectType( f, typelist, obj );
	    fputs( "] ", f );
	    if( pref_nogarbagecollection ){
		fputs( "null", f );
	    }
	    else {
		fputs( "&", f );
		pp_tmsymbol( f, markmethodname );
	    }

	    // Finalizer method.
	    // TODO: put in something real when it's available.
	    fputs( ",cast pointer procedure [", f );
	    pp_ObjectType( f, typelist, obj );
	    fputs( "] null", f );

	    // Packing method
	    fputs( ",cast pointer procedure [", f );
	    pp_ObjectType( f, typelist, obj );
	    fputs( ",", f );
	    pp_ObjectType( f, typelist, obj );
	    fputs( ",", f );
	    pp_ObjectType( f, typelist, obj );
	    fputs( "] ", f );
	    if( packmethodname == tmsymbolNIL ){
		fputs( "null", f );
	    }
	    else {
		fputs( "&", f );
		pp_tmsymbol( f, packmethodname );
	    }

	    // Unpacking method
	    fputs( ",cast pointer function [", f );
	    pp_ObjectType( f, typelist, obj );
	    fputs( ",", f );
	    pp_ObjectType( f, typelist, obj );
	    fputs( "] ", f );
	    pp_ObjectType( f, typelist, obj );
	    fputs( " ", f );
	    if( unpackmethodname == tmsymbolNIL ){
		fputs( "null", f );
	    }
	    else {
		fputs( "&", f );
		pp_tmsymbol( f, unpackmethodname );
	    }
	    break;
	}

	case TAGInterfaceEntry:
	    fputs( "cast pointer record [] null", f );
	    fputs( ", cast pointer record [] null", f );
	    fprintf( f, ", %ld", (long int) e->flags );

	    // Mark method
	    fputs( ",cast pointer procedure [", f );
	    pp_ObjectType( f, typelist, obj );
	    fputs( "] null", f );

	    // Finalizer method
	    fputs( ",cast pointer procedure [", f );
	    pp_ObjectType( f, typelist, obj );
	    fputs( "] null", f );

	    // Packing method
	    fputs( ",cast pointer procedure [", f );
	    pp_ObjectType( f, typelist, obj );
	    fputs( ",", f );
	    pp_ObjectType( f, typelist, obj );
	    fputs( ",", f );
	    pp_ObjectType( f, typelist, obj );
	    fputs( "] null", f );

	    // Unpacking method
	    fputs( ",cast pointer function [", f );
	    pp_ObjectType( f, typelist, obj );
	    fputs( ",", f );
	    pp_ObjectType( f, typelist, obj );
	    fputs( "] ", f );
	    pp_ObjectType( f, typelist, obj );
	    fputs( " null", f );
	    break;

	case TAGForwardObjectEntry:
	case TAGPackageEntry:
	    internal_error( "An introspection entry is only for classes and interfaces" );
#if 0
	    fputs( "cast pointer record [] null, cast pointer record [] null, ", f );
	    fprintf( f, "%ld,cast pointer procedure [", (long int) e->flags );
	    pp_ObjectType( f, typelist, obj );
	    fputs( "] null", f );
	    fputs( ",cast pointer procedure [", f );
	    pp_ObjectType( f, typelist, obj );
	    fputs( "] null", f );
	    fputs( ",cast pointer procedure [", f );
	    pp_ObjectType( f, typelist, obj );
	    fputs( "] null", f );
	    fputs( ",cast pointer procedure [", f );
	    pp_ObjectType( f, typelist, obj );
	    fputs( "] null", f );
#endif
	    break;
    }
    fputs( "]\n", f );
    return nm;
}

/* Given a file handle 'f', a list of types 'typelist', and a list of
 * (variable and field) declarations 'decls', generate initialized
 * data structures to describe the properties of all types and fields.
 */
static void pp_introspection_tables(
 FILE *f,
 const_TypeEntry_list typelist,
 const_Entry_list decls,
 bool *first
)
{
    tmsymbol_list nms = new_tmsymbol_list();
    tmsymbol obj = add_tmsymbol( "java.lang.Object" );

    (void) decls;
    if( *first == false ){
	pp_indent_line( f, INDENTSTEP, "," );
    }
    *first = false;
    pp_indent( f, INDENTSTEP );
    fputs( "record __introspection_type [nm:string, classno:int, last_subclassno:int,vtable:pointer record [],ivtable:pointer record [],flags:int,gcmarking:pointer procedure [", f );
    pp_ObjectType( f, typelist, obj );
    fputs( "]", f );
    fputs( ",finalizer:pointer procedure [", f );
    pp_ObjectType( f, typelist, obj );
    fputs( "]", f );
    fputs( ",ser_write:pointer procedure [", f );
    pp_ObjectType( f, typelist, obj );
    fputs( ",", f );
    pp_ObjectType( f, typelist, obj );
    fputs( ",", f );
    pp_ObjectType( f, typelist, obj );
    fputs( "],ser_read:pointer function [", f );
    pp_ObjectType( f, typelist, obj );
    fputs( ",", f );
    pp_ObjectType( f, typelist, obj );
    fputs( "] ", f );
    pp_ObjectType( f, typelist, obj );
    fputs( "]\n", f );
    for( unsigned int iy=0; iy<typelist->sz; iy++ ){
	const_TypeEntry e = typelist->arr[iy];
	if( is_ConcreteTypeEntry( e ) ){
	    tmsymbol nm = pp_introspection_TypeEntry(
		f,
		typelist,
		to_const_ConcreteTypeEntry( e ),
		first
	    );
	    if( nm != tmsymbolNIL ){
		nms = append_tmsymbol_list( nms, nm );
	    }
	}
    }
    if( *first == false ){
	pp_indent_line( f, INDENTSTEP, "," );
    }
    *first = false;
    pp_indent( f, INDENTSTEP );
    fprintf(
	f,
	"globalvariable __spar_rte_typelist"
	" array %d pointer record __introspection_type"
	" array pointer record __introspection_type [\n",
	nms->sz
    );
    for( unsigned int ix=0; ix<nms->sz;  ix++ ){
	pp_indent( f, 2*INDENTSTEP );
	fputs( "&", f );
	pp_tmsymbol( f, nms->arr[ix] );
	if( ix+1<nms->sz ){
	    fputs( ",", f );
	}
	fputs( "\n", f );
    }
    pp_indent_line( f, INDENTSTEP, "]," );
    pp_indent( f, INDENTSTEP );
    fprintf( f, "globalvariable __spar_rte_typelist_length int %d\n", nms->sz );
    rfre_tmsymbol_list( nms );
}

// Construct the record declaration of interface 'inf'
static void pp_interface_vtable_record(
 FILE *f,
 const_TypeEntry_list typelist,
 const_Entry_list symtab,
 const_InterfaceEntry ie
)
{
    pp_indent( f, INDENTSTEP );
    fputs( "// The record type for the vtable of interface type ", f );
    pp_origsymbol( f, ie->name );
    fputc( '\n', f );
    pp_indent( f, INDENTSTEP );
    fprintf( f, "record __t_%s [\n", ie->vtabname->name );
    VtableEntry_list vtable = construct_Interface_vtable(
	typelist,
	symtab,
	ie->name->sym
    );
    for( unsigned int ix=0; ix<vtable->sz; ix++ ){
	VtableEntry vte = vtable->arr[ix];

	pp_indent( f, 2*INDENTSTEP );
	fputs( "__ivte_", f );
	pp_tmsymbol( f, vte->virtualname );
	fputs( ": pointer ", f );
	pp_vtable_function_type( f, typelist, symtab, vte->thisparms, vte->t, vte->rettype );
	if( ix+1<vtable->sz ){
	    fputs( ",", f );
	}
	fputs( "\n", f );
    }
    rfre_VtableEntry_list( vtable );
    pp_indent_line( f, INDENTSTEP, "]" );
}

// Given a file handle 'f', a list of types 'typelist', and a list of
// interface types 'iftl', generate a type declaration for the record
// representing the interface vtable.
static void pp_ivtable_type(
 FILE *f,
 const_TypeEntry_list typelist,
 const_Entry_list symtab,
 bool *first
)
{
    if( *first == false ){
	pp_indent_line( f, INDENTSTEP, "," );
    }
    *first = false;
    pp_indent_line( f, INDENTSTEP, "// The representation of all interface types" );
    pp_indent_line( f, INDENTSTEP, "record __any_interface_type [" );
    pp_indent( f, 2*INDENTSTEP );
    fputs( "__vtable: pointer record [ __introspection: pointer record __introspection_type ],\n", f );
    pp_indent_line( f, 2*INDENTSTEP, "__ivtable: pointer record __ivtable_type" );
    pp_indent_line( f, INDENTSTEP, "]" );
    for( unsigned int tix=0; tix<typelist->sz; tix++ ){
	const_TypeEntry te = typelist->arr[tix];

	if( te->tag == TAGInterfaceEntry && te->usecount>0 ){
	    const_InterfaceEntry ite = to_const_InterfaceEntry( te );

	    pp_indent_line( f, INDENTSTEP, "," );
	    pp_interface_vtable_record( f, typelist, symtab, ite );
	}
    }
    pp_indent_line( f, INDENTSTEP, "," );
    bool first_elm = true;
    pp_indent_line( f, INDENTSTEP, "record __ivtable_type [" );
    for( unsigned int ix=0; ix<typelist->sz; ix++ ){
	const_TypeEntry te = typelist->arr[ix];

	if( te->tag == TAGInterfaceEntry && te->usecount>0 ){
	    const_InterfaceEntry ite = to_const_InterfaceEntry( te );

	    if( first_elm ){
		first_elm = false;
	    }
	    else {
		pp_indent_line( f, 2*INDENTSTEP, "," );
	    }
	    pp_indent( f, 2*INDENTSTEP );
	    pp_censored_typename( f, ite->name );
	    fputs( ": pointer record __t_", f );
	    pp_tmsymbol( f, ite->vtabname );
	    fputc( '\n', f );
	}
    }
    pp_indent_line( f, INDENTSTEP, "]" );
}


// Given a class vtable 'class_vtab' and a signature 's', search for
// the method with signature 's' in the vtable.
static bool search_Vtable( const_VtableEntry_list class_vtab, const_Signature s, unsigned int *pos )
{
    for( unsigned int ix=0; ix<class_vtab->sz; ix++ ){
	const_Signature cs = class_vtab->arr[ix]->t;
	if( isequal_Signature( s, cs ) ){
	    *pos = ix;
	    return true;
	}
    }
    return false;
}

static tmsymbol pp_interface_vtable(
 FILE *f,
 const_TypeEntry_list typelist,
 const_Entry_list symtab,
 tmsymbol classnm,
 const_VtableEntry_list class_vtab,
 tmsymbol inf,
 bool *first
)
{
    tmsymbol nm = gen_tmsymbol( "ivtab" );

    const_TypeEntry e = lookup_TypeEntry( typelist, inf );
    if( e == TypeEntryNIL || e->usecount == 0 || e->tag != TAGInterfaceEntry ){
	return tmsymbolNIL;
    }
    const_InterfaceEntry ie = to_const_InterfaceEntry( e );
    VtableEntry_list vtable = construct_Interface_vtable(
	typelist,
	symtab,
	inf
    );
    if( *first == false ){
	pp_indent_line( f, INDENTSTEP, "," );
    }
    *first = false;


    // Construct an initialized variable that forms the actual vtable
    // for this interface.
    pp_indent( f, INDENTSTEP );
    fputs( "globalvariable ", f );
    pp_tmsymbol( f, nm );
    fprintf( f, " record __t_%s record [\n", ie->vtabname->name );
    for( unsigned int ix=0; ix<vtable->sz; ix++ ){
	VtableEntry vte = vtable->arr[ix];
	unsigned int pos;

	pp_indent( f, 2*INDENTSTEP );
	if( search_Vtable( class_vtab, vte->t, &pos ) ){
	    fputs( "cast pointer ", f );
	    pp_vtable_function_type( f, typelist, symtab, vte->thisparms, vte->t, vte->rettype );
	    fputs( " ", f );
	    tmsymbol realnm = class_vtab->arr[pos]->realname;
	    if( realnm == tmsymbolNIL ){
		fputs( "NULL", f );
	    }
	    else {
		fputs( "&", f );
		pp_tmsymbol( f, realnm );
	    }
	}
	else {
	    tmstring s = typename_Signature( vte->t );
	    internal_error( "Class '%s' should implement method %s", classnm->name,  s );
	}
	if( ix+1<vtable->sz ){
	    fputs( ",", f );
	}
	fputs( "\n", f );
    }
    pp_indent_line( f, INDENTSTEP, "]" );
    rfre_VtableEntry_list( vtable );
    return nm;
}

/* Given a file handle 'f' and a field 'nm', generate code to mark the given
 * field in the mark phase of garbage collection.
 */
static void pp_field_marking(
 FILE *f,
 const_TypeEntry_list typelist,
 const_Entry_list symtab,
 unsigned int indent,
 tmsymbol thisnm,
 const tmsymbol nm
)
{
    const_Entry e = lookup_Entry( symtab, nm );
    if( e->tag != TAGFieldEntry ){
	internal_error( "'%s' is not a field name", nm->name );
    }
    const_type t = to_const_FieldEntry(e)->t;
    if( is_collectable_type( t ) ){
	pp_indent( f, indent );
	fputs( "procedurecall Java_spar_compiler_GC_pushRef [cast ", f );
	pp_ObjectType( f, typelist, add_tmsymbol( "java.lang.Object" ) );
	fputs( " field *", f );
	pp_tmsymbol( f, thisnm );
	fputs( " ", f );
	pp_tmsymbol( f, nm );
	fputs( "],\n", f );
    }
}

static const char *name_primitive_packer( BASETYPE bt )
{
    const char *res = "???";

    switch( bt ){
	case BT_BOOLEAN:	res = "ser_writeBoolean";	break;
	case BT_STRING:		res = "ser_writeString";	break;
	case BT_COMPLEX:	res = "ser_writeComplex";	break;
	case BT_BYTE:		res = "ser_writeByte";		break;
	case BT_SHORT:		res = "ser_writeShort";		break;
	case BT_CHAR:		res = "ser_writeChar";		break;
	case BT_INT:		res = "ser_writeInt";		break;
	case BT_LONG:		res = "ser_writeLong";		break;
	case BT_FLOAT:		res = "ser_writeFloat";		break;
	case BT_DOUBLE:		res = "ser_writeDouble";	break;
    }
    return res;
}

/* Given a file handle 'f' and a field 'nm', generate code to write the given
 * field in the write phase of serialization.
 */
static void pp_field_packing(
 FILE *f,
 const_TypeEntry_list typelist,
 const_Entry_list symtab,
 unsigned int indent,
 tmsymbol writer,
 tmsymbol handlebase,
 const_expression x,
 const_type t
)
{
    tmsymbol obj = add_tmsymbol( "java.lang.Object" );

    switch( t->tag ){
	case TAGPrimitiveType:
	{
	    const char *fnname = name_primitive_packer( to_const_PrimitiveType(t)->base );
	    pp_indent( f, indent );
	    fprintf( f, "procedurecall %s [", fnname );
	    pp_tmsymbol( f, writer );
	    fputc( ',', f );
	    pp_tmsymbol( f, handlebase );
	    fputs( ",", f );
	    pp_expression( f, typelist, symtab, x );
	    fputs( "],\n", f );
	    break;
	}

	case TAGObjectType:
	    pp_indent( f, indent );
	    fputs( "procedurecall ser_writeObject [", f );
	    pp_tmsymbol( f, writer );
	    fputc( ',', f );
	    pp_tmsymbol( f, handlebase );
	    fputs( ",cast ", f );
	    pp_ObjectType( f, typelist, obj );
	    fputc( ' ', f );
	    pp_expression( f, typelist, symtab, x );
	    fputs( "],\n", f );
	    break;

	case TAGArrayType:
	    pp_indent( f, indent );
	    fputs( "procedurecall ser_writeArray [", f );
	    pp_tmsymbol( f, writer );
	    fputc( ',', f );
	    pp_tmsymbol( f, handlebase );
	    fputs( ",cast ", f );
	    pp_ObjectType( f, typelist, obj );
	    fputc( ' ', f );
	    pp_expression( f, typelist, symtab, x );
	    fputs( "],\n", f );
	    break;

	case TAGPragmaType:
	    pp_field_packing( f, typelist, symtab, indent, writer, handlebase, x, to_const_PragmaType(t)->t );
	    break;

	case TAGTupleType:
	{
	    const_type_list fields = to_const_TupleType(t)->fields;

	    if( fields == type_listNIL ){
		break;
	    }
	    for( unsigned int ix=0; ix<fields->sz; ix++ ){
		expression x1 = new_VectorSubscriptExpression(
		    rdup_expression( x ),
		    ix
		);
		pp_field_packing( f, typelist, symtab, indent, writer, handlebase, x1, fields->arr[ix] );
		rfre_expression( x1 );
	    }
	    break;
	}

	case TAGVectorType:
	{
	    const_expression power = to_const_VectorType(t)->power;
	    const_type elmtype = to_const_VectorType(t)->elmtype;
	    unsigned int sz;

	    if( !extract_vector_power( originNIL, power, &sz ) ){
		break;
	    }
	    if( elmtype == typeNIL ){
		break;
	    }

	    for( unsigned int ix=0; ix<sz; ix++ ){
		expression x1 = new_VectorSubscriptExpression(
		    rdup_expression( x ),
		    ix
		);
		pp_field_packing( f, typelist, symtab, indent, writer, handlebase, x1, elmtype );
		rfre_expression( x1 );
	    }
	    break;
	}

	case TAGTypeType:
	case TAGNullType:
	case TAGFunctionType:
	case TAGTypeOf:
	case TAGTypeOfIf:
	case TAGVoidType:
	case TAGPrimArrayType:
	case TAGGenericObjectType:
	case TAGExceptionVariableType:
	case TAGGCRefLinkType:
	case TAGGCTopRefLinkType:
	    tmstring s = typename_type( t );
	    internal_error( "Illegal type for serialization '%s'", s );
	    break;
    }
}

/* Given a file handle 'f' and a field 'nm', generate code to write the given
 * field in the write phase of serialization.
 */
static void pp_field_packing(
 FILE *f,
 const_TypeEntry_list typelist,
 const_Entry_list symtab,
 unsigned int indent,
 tmsymbol writer,
 tmsymbol handlebase,
 tmsymbol data,
 const tmsymbol nm
)
{
    const_Entry e = lookup_Entry( symtab, nm );
    if( e->tag != TAGFieldEntry ){
	internal_error( "'%s' is not a field name", nm->name );
    }
    if( has_any_flag( e->flags, ACC_STATIC|ACC_TRANSIENT ) ){
	return;
    }
    const_type t = to_const_FieldEntry(e)->t;
    expression x = new_FieldExpression(
	new_VariableNameExpression( new_origsymbol( data, originNIL ), 0 ),
	new_origsymbol( nm, originNIL )
    );
    pp_field_packing( f, typelist, symtab, indent, writer, handlebase, x, t );
    rfre_expression( x );
}

/* Given a file handle 'f', a type entry 'e' and a pointer to a boolean
 * 'first', generate a new initialized variable declaration that contains
 * the vtable and ivtable for that type.
 */
static void pp_vtable_ClassEntry(
 FILE *f,
 ClassEntry e,
 const_TypeEntry_list typelist,
 const_Entry_list symtab,
 bool *first
)
{
    unsigned int ix;

    if( e->usecount == 0 ){
	return;
    }
    VtableEntry_list vtable = construct_Class_vtable( e, typelist, symtab );
    if( *first == false ){
	pp_indent_line( f, INDENTSTEP, "," );
    }
    *first = false;

    // Generate a record definition for the type itself.
    pp_indent( f, INDENTSTEP );
    fputs( "record ", f );
    pp_tmsymbol( f, e->recordname );
    fputs( " [\n", f );
    pp_indent( f, 2*INDENTSTEP );
    fputs( "__vtable: pointer record [ __introspection: pointer record __introspection_type ],\n", f );
    pp_indent( f, 2*INDENTSTEP );
    fputs( "__ivtable: pointer record __ivtable_type\n", f );
    if( e->outerthis == tmsymbolNIL ){
	// We're not an inner class, but since a subclass might be,
	// create a dummy field for the outer this field.
	pp_indent_line( f, 2*INDENTSTEP, "," );
	pp_indent( f, 2*INDENTSTEP );
	fputs( "__dummy_outerthis: ", f );
	pp_ObjectType( f, typelist, add_tmsymbol( "java.lang.Object" ) );
	fputc( '\n', f );
    }
    else {
	pp_indent_line( f, 2*INDENTSTEP, "," );
	pp_indent( f, 2*INDENTSTEP );
	pp_tmsymbol( f, e->outerthis );
	fputs( ": ", f );
	pp_ObjectType( f, typelist, e->enclosingtype );
	fputc( '\n', f );
    }
    tmsymbol_list fl = lookup_object_all_fields(
	typelist,
	e->name->sym,
	NOT_STATIC
    );
    for( ix=0; ix<fl->sz; ix++ ){
	pp_indent_line( f, 2*INDENTSTEP, "," );
	pp_indent( f, 2*INDENTSTEP );
	pp_field( f, typelist, symtab, fl->arr[ix] );
	fputs( "\n", f );
    }
    pp_indent_line( f, INDENTSTEP, "]," );

    if( !pref_nogarbagecollection ){
	// Generate a procedure definition for the marking method of this class.
	tmsymbol mark_thisnm = gen_tmsymbol( "markthis" );
	tmsymbol mark_scopenm = gen_tmsymbol( "markscope" );
	pp_indent( f, INDENTSTEP );
	fputs( "formalvariable ", f );
	pp_tmsymbol( f, mark_thisnm );
	fputs( " ", f );
	pp_tmsymbol( f, mark_scopenm );
	fputs( " pointer record ", f );
	pp_tmsymbol( f, e->recordname );
	fputs( ",\n", f );
	pp_indent( f, INDENTSTEP );
	fputs( "procedure ", f );
	pp_tmsymbol( f, e->markMethod );
	fputs( " [", f );
	pp_tmsymbol( f, mark_thisnm );
	fputs( "] statements ", f );
	pp_tmsymbol( f, mark_scopenm );
	fputs( " [\n", f );
	for( ix=0; ix<fl->sz; ix++ ){
	    pp_field_marking( f, typelist, symtab, 2*INDENTSTEP, mark_thisnm, fl->arr[ix] );
	}
	pp_indent_line( f, INDENTSTEP, "]," );
    }
    if( pref_generate_serialization && e->packMethod != tmsymbolNIL ){
	tmsymbol obj = add_tmsymbol( "java.lang.Object" );
	// Generate a procedure definition for the packing and unpacking
	// methods of this class.
	tmsymbol pack_writer = gen_tmsymbol( "writer" );
	tmsymbol pack_handlebase = gen_tmsymbol( "handlebase" );
	tmsymbol pack_data = gen_tmsymbol( "data" );
	tmsymbol pack_scopenm = gen_tmsymbol( "ser_write_scope" );

	pp_indent( f, INDENTSTEP );
	fputs( "formalvariable ", f );
	pp_tmsymbol( f, pack_writer );
	fputs( " ", f );
	pp_tmsymbol( f, pack_scopenm );
	fputs( " ", f );
	pp_ObjectType( f, typelist, obj );
	fputs( ",\n", f );

	pp_indent( f, INDENTSTEP );
	fputs( "formalvariable ", f );
	pp_tmsymbol( f, pack_handlebase );
	fputs( " ", f );
	pp_tmsymbol( f, pack_scopenm );
	fputs( " ", f );
	pp_ObjectType( f, typelist, obj );
	fputs( ",\n", f );

	pp_indent( f, INDENTSTEP );
	fputs( "formalvariable ", f );
	pp_tmsymbol( f, pack_data );
	fputs( " ", f );
	pp_tmsymbol( f, pack_scopenm );
	fputs( " pointer record ", f );
	pp_tmsymbol( f, e->recordname );
	fputs( ",\n", f );

	pp_indent( f, INDENTSTEP );
	fputs( "procedure ", f );
	pp_tmsymbol( f, e->packMethod );
	fputs( " [", f );
	pp_tmsymbol( f, pack_writer );
	fputs( ", ", f );
	pp_tmsymbol( f, pack_handlebase );
	fputs( ", ", f );
	pp_tmsymbol( f, pack_data );
	fputs( "] statements ", f );
	pp_tmsymbol( f, pack_scopenm );
	fputs( " [\n", f );
	for( ix=0; ix<fl->sz; ix++ ){
	    pp_field_packing( f, typelist, symtab, 2*INDENTSTEP, pack_writer, pack_handlebase, pack_data, fl->arr[ix] );
	}
	pp_indent_line( f, INDENTSTEP, "]," );

	tmsymbol unpack_reader = gen_tmsymbol( "reader" );
	tmsymbol unpack_handlebase = gen_tmsymbol( "handlebase" );
	tmsymbol unpack_scopenm = gen_tmsymbol( "scope" );

	pp_indent( f, INDENTSTEP );
	fputs( "formalvariable ", f );
	pp_tmsymbol( f, unpack_reader );
	fputs( " ", f );
	pp_tmsymbol( f, unpack_scopenm );
	fputs( " ", f );
	pp_ObjectType( f, typelist, obj );
	fputs( ",\n", f );

	pp_indent( f, INDENTSTEP );
	fputs( "formalvariable ", f );
	pp_tmsymbol( f, unpack_handlebase );
	fputs( " ", f );
	pp_tmsymbol( f, unpack_scopenm );
	fputs( " ", f );
	pp_ObjectType( f, typelist, obj );
	fputs( ",\n", f );

	pp_indent( f, INDENTSTEP );
	fputs( "function ", f );
	pp_tmsymbol( f, e->unpackMethod );
	fputs( " [", f );
	pp_tmsymbol( f, unpack_reader );
	fputs( ", ", f );
	pp_tmsymbol( f, unpack_handlebase );
	fputs( "] ", f );
	fputs( " pointer record ", f );
	pp_tmsymbol( f, e->recordname );
	fputs( " statements ", f );
	pp_tmsymbol( f, unpack_scopenm );
	fputs( " [\n", f );
#if 0
	for( ix=0; ix<fl->sz; ix++ ){
	    pp_field_unpacking( f, typelist, symtab, 2*INDENTSTEP, unpack_thisnm, fl->arr[ix] );
	}
#endif
	pp_indent( f, 2*INDENTSTEP );
	fputs( "return cast pointer record ", f );
	pp_tmsymbol( f, e->recordname );
	fputs( " null\n", f );
	pp_indent_line( f, INDENTSTEP, "]," );
    }
    rfre_tmsymbol_list( fl );

    // Generate a record definition for the vtable of this class
    pp_indent( f, INDENTSTEP );
    fputs( "// The record type for the vtable of class type ", f );
    pp_origsymbol( f, e->name );
    fputc( '\n', f );
    pp_indent( f, INDENTSTEP );
    fprintf( f, "record __t_%s [\n", e->vtabname->name );
    pp_indent_line( f, 2*INDENTSTEP, "__introspection: pointer record __introspection_type" );
    if( vtable->sz != 0 ){
	pp_indent_line( f, 2*INDENTSTEP, "," );
    }
    for( ix=0; ix<vtable->sz; ix++ ){
	const_VtableEntry vte = vtable->arr[ix];

	pp_indent( f, 2*INDENTSTEP );
	fputs( "__vte_", f );
	pp_tmsymbol( f, vte->virtualname );
	fputs( ": pointer ", f );
	pp_vtable_function_type( f, typelist, symtab, vte->thisparms, vte->t, vte->rettype );
	if( ix+1<vtable->sz ){
	    fputs( ",", f );
	}
	fputs( "\n", f );
    }
    pp_indent_line( f, INDENTSTEP, "]," );

    // Generate a global variable declaration that is initialized with
    // the vtable for this class
    pp_indent( f, INDENTSTEP );
    fputs( "globalvariable ", f );
    pp_tmsymbol( f, e->vtabname );
    fprintf( f, " record __t_%s record [\n", e->vtabname->name );
    pp_indent( f, 2*INDENTSTEP );
    fputs( "&", f );
    pp_tmsymbol( f, e->introname );
    fputs( "\n", f );
    if( vtable->sz != 0 ){
	pp_indent_line( f, 2*INDENTSTEP, "," );
    }
    for( ix=0; ix<vtable->sz; ix++ ){
	VtableEntry vte = vtable->arr[ix];

	pp_indent( f, 2*INDENTSTEP );
	fputs( "cast pointer ", f );
	pp_vtable_function_type( f, typelist, symtab, vte->thisparms, vte->t, vte->rettype );
	fputs( " ", f );
	if( vte->realname == tmsymbolNIL ){
	    fputs( "NULL", f );
	}
	else {
	    fputs( "&", f );
	    pp_tmsymbol( f, vte->realname );
	}
	if( ix+1<vtable->sz ){
	    fputs( ",", f );
	}
	fputs( "\n", f );
    }
    pp_indent_line( f, INDENTSTEP, "]" );
    tmsymbol_list all_interfaces = collect_interfaces( typelist, origsymbol_listNIL, new_tmsymbol_list(), e );
    tmsymbol_list all_interface_names = new_tmsymbol_list();
    for( ix=0; ix<all_interfaces->sz; ix++ ){
	tmsymbol inm = pp_interface_vtable(
	    f,
	    typelist,
	    symtab,
	    e->name->sym,
	    vtable,
	    all_interfaces->arr[ix],
	    first
	);
	all_interface_names = append_tmsymbol_list( all_interface_names, inm );
    }
    pp_indent_line( f, INDENTSTEP, "," );
    pp_indent( f, INDENTSTEP );
    fputs( "globalvariable ", f );
    pp_tmsymbol( f, e->ivtabname );
    fprintf( f, " record __ivtable_type record [\n" );
    bool ivfirst = true;
    for( ix=0; ix<typelist->sz; ix++ ){
	TypeEntry te = typelist->arr[ix];
	unsigned int pos;

	if( te->tag == TAGInterfaceEntry && te->usecount>0 ){
	    if( ivfirst ){
		ivfirst = false;
	    }
	    else {
		pp_indent_line( f, 2*INDENTSTEP, "," );
	    }
	    pp_indent( f, 2*INDENTSTEP );
	    if( search_tmsymbol_list( all_interfaces, te->name->sym, &pos ) ){
		fprintf( f, "&%s\n", all_interface_names->arr[pos]->name );
	    }
	    else {
		fputs( "cast pointer record __t_", f );
		pp_tmsymbol( f, to_InterfaceEntry( te )->vtabname );
		fputs( " null\n", f );
	    }
	}
    }
    rfre_tmsymbol_list( all_interfaces );
    rfre_tmsymbol_list( all_interface_names );
    pp_indent_line( f, INDENTSTEP, "]," );
    tmsymbol thisnm = gen_tmsymbol( "this" );
    tmsymbol scopenm = gen_tmsymbol( "scope" );
    pp_indent( f, INDENTSTEP );
    fputs( "formalvariable ", f );
    pp_tmsymbol( f, thisnm );
    fputs( " ", f );
    pp_tmsymbol( f, scopenm );
    fputs( " pointer record ", f );
    pp_tmsymbol( f, e->recordname );
    fputs( ",\n", f );
    pp_indent( f, INDENTSTEP );
    fputs( "function __init_", f );
    pp_tmsymbol( f, e->recordname );
    fputs( " [", f );
    pp_tmsymbol( f, thisnm );
    fputs( "] pointer record ", f );
    pp_tmsymbol( f, e->recordname );
    fputs( " statements ", f );
    pp_tmsymbol( f, scopenm );
    fputs( " [\n", f );
    pp_indent( f, 2*INDENTSTEP );
    fputs( "assign field *", f );
    pp_tmsymbol( f, thisnm );
    fputs( " __vtable cast pointer record [ __introspection: pointer record __introspection_type ] &", f );
    pp_tmsymbol( f, e->vtabname );
    fputs( ",\n", f );
    pp_indent( f, 2*INDENTSTEP );
    fputs( "assign field *", f );
    pp_tmsymbol( f, thisnm );
    fputs( " __ivtable &", f );
    pp_tmsymbol( f, e->ivtabname );
    fputs( ",\n", f );
    pp_indent( f, 2*INDENTSTEP );
    fputs( "return ", f );
    pp_tmsymbol( f, thisnm );
    fputs( "\n", f );
    pp_indent( f, INDENTSTEP );
    fputs( "]\n", f );
    rfre_VtableEntry_list( vtable );
}

/* Given a file handle 'f', a type entry 'e' and a pointer to a boolean
 * 'first', generate a new initialized variable declaration that contains
 * the vtable for that type.
 */
static void pp_vtable_TypeEntry(
 FILE *f,
 TypeEntry e,
 const_TypeEntry_list typelist,
 const_Entry_list symtab,
 bool *first
)
{
    switch( e->tag ){
	case TAGForwardObjectEntry:
	case TAGPackageEntry:
	    break;

	case TAGInterfaceEntry:
	    break;

	case TAGClassEntry:
	    pp_vtable_ClassEntry( f, to_ClassEntry( e ), typelist, symtab, first );
	    break;
    }
}

/* Given a file handle 'f', a list of types 'typelist', and a list of
 * (variable and field) declarations 'decls', generate initialized
 * data structures to describe the virtual table of each type.
 */
static void pp_virtuality_tables(
 FILE *f,
 const_TypeEntry_list typelist,
 const_Entry_list symtab,
 bool *first
)
{
    for( unsigned int ix=0; ix<typelist->sz; ix++ ){
	pp_vtable_TypeEntry( f, typelist->arr[ix], typelist, symtab, first );
    }
}

static void pp_serialization_write_declaration( FILE *f, const_TypeEntry_list typelist, const char *nm, bool objtype )
{
    tmsymbol obj = add_tmsymbol( "java.lang.Object" );
    tmsymbol writer = gen_tmsymbol( "writer" );
    tmsymbol handlebase = gen_tmsymbol( "handlebase" );
    tmsymbol data = gen_tmsymbol( "data" );

    pp_indent_line( f, INDENTSTEP, "," );
    pp_indent( f, INDENTSTEP );
    fputs( "formalvariable ", f );
    pp_tmsymbol( f, writer );
    fprintf( f, " ser_write%s ", nm );
    pp_ObjectType( f, typelist, obj );
    fputs( ",\n", f );

    pp_indent( f, INDENTSTEP );
    fputs( "formalvariable ", f );
    pp_tmsymbol( f, handlebase );
    fprintf( f, " ser_write%s ", nm );
    pp_ObjectType( f, typelist, obj );
    fputs( ",\n", f );

    pp_indent( f, INDENTSTEP );
    fputs( "formalvariable ", f );
    pp_tmsymbol( f, data );
    fprintf( f, " ser_write%s ", nm );
    if( objtype ){
	pp_ObjectType( f, typelist, obj );
    }
    else {
	fputs( nm, f );
    }
    fputs( ",\n", f );

    pp_indent( f, INDENTSTEP );
    fprintf( f, "externalprocedure ser_write%s[", nm );
    pp_tmsymbol( f, writer );
    fputs( ",", f );
    pp_tmsymbol( f, handlebase );
    fputs( ",", f );
    pp_tmsymbol( f, data );
    fputs( "]\n", f );
}

static void pp_serialization_read_declaration( FILE *f, const_TypeEntry_list typelist, const char *nm, const char *retnm )
{
    tmsymbol obj = add_tmsymbol( "java.lang.Object" );
    tmsymbol handlebase = gen_tmsymbol( "handlebase" );
    tmsymbol reader = gen_tmsymbol( "reader" );

    pp_indent_line( f, INDENTSTEP, "," );
    pp_indent( f, INDENTSTEP );
    fputs( "formalvariable ", f );
    pp_tmsymbol( f, reader );
    fprintf( f, " ser_read%s ", nm );
    pp_ObjectType( f, typelist, obj );
    fputs( ",\n", f );

    pp_indent( f, INDENTSTEP );
    fputs( "formalvariable ", f );
    pp_tmsymbol( f, handlebase );
    fprintf( f, " ser_read%s ", nm );
    pp_ObjectType( f, typelist, obj );
    fputs( ",\n", f );

    pp_indent( f, INDENTSTEP );
    fprintf( f, "externalfunction ser_read%s[", nm );
    pp_tmsymbol( f, reader );
    fputs( ",", f );
    pp_tmsymbol( f, handlebase );
    fputs( "] ", f );
    if( retnm == NULL ){
	pp_ObjectType( f, typelist, obj );
    }
    else {
	fprintf( f, "%s", retnm );
    }
    fputs( "\n", f );
}

static void pp_serialization_declarations( FILE *f, const_TypeEntry_list typelist )
{
    pp_serialization_write_declaration( f, typelist, "Boolean", false );
    pp_serialization_write_declaration( f, typelist, "Char", false );
    pp_serialization_write_declaration( f, typelist, "Byte", false );
    pp_serialization_write_declaration( f, typelist, "Short", false );
    pp_serialization_write_declaration( f, typelist, "Int", false );
    pp_serialization_write_declaration( f, typelist, "Long", false );
    pp_serialization_write_declaration( f, typelist, "Float", false );
    pp_serialization_write_declaration( f, typelist, "Double", false );
    pp_serialization_write_declaration( f, typelist, "Complex", false );
    pp_serialization_write_declaration( f, typelist, "String", false );
    pp_serialization_write_declaration( f, typelist, "Object", true );
    pp_serialization_write_declaration( f, typelist, "Array", true );

    pp_serialization_read_declaration( f, typelist, "Boolean", "boolean" );
    pp_serialization_read_declaration( f, typelist, "Char", "char" );
    pp_serialization_read_declaration( f, typelist, "Byte", "byte" );
    pp_serialization_read_declaration( f, typelist, "Short", "short" );
    pp_serialization_read_declaration( f, typelist, "Int", "int" );
    pp_serialization_read_declaration( f, typelist, "Long", "long" );
    pp_serialization_read_declaration( f, typelist, "Float", "float" );
    pp_serialization_read_declaration( f, typelist, "Double", "double" );
    pp_serialization_read_declaration( f, typelist, "Complex", "complex" );
    pp_serialization_read_declaration( f, typelist, "String", "string" );
    pp_serialization_read_declaration( f, typelist, "Object", NULL );
    pp_serialization_read_declaration( f, typelist, "Array", NULL );
}

static void pp_boundsviolator_declaration(
 FILE *f,
 tmsymbol boundsviolator
)
{
    pp_indent_line( f, INDENTSTEP, "," );
    pp_indent_line( f, INDENTSTEP, "procedure VnusEventArrayBoundViolated [] statements [" );
    pp_indent( f, INDENTSTEP*2 );
    fputs( "procedurecall ", f );
    pp_tmsymbol( f, boundsviolator );
    fputs( " []\n", f );
    pp_indent_line( f, INDENTSTEP, "]" );
}

// Generate a declaration for the out-of-memory assumed function
static void pp_outofmemory_declaration(
 FILE *f,
 tmsymbol outofmemory
)
{
    pp_indent_line( f, INDENTSTEP, "," );
    pp_indent_line( f, INDENTSTEP, "procedure VnusEventOutOfMemory [] statements [" );
    pp_indent( f, INDENTSTEP*2 );
    fputs( "procedurecall ", f );
    pp_tmsymbol( f, outofmemory );
    fputs( " []\n", f );
    pp_indent_line( f, INDENTSTEP, "]" );
}

// Generate a declaration for the garbage collector assumed function
static void pp_garbagecollector_declaration(
 FILE *f,
 tmsymbol collector
)
{
    pp_indent_line( f, INDENTSTEP, "," );
    pp_indent_line( f, INDENTSTEP, "function VnusEventGarbageCollect [] int statements [" );
    pp_indent( f, INDENTSTEP*2 );
    fputs( "return functioncall ", f );
    pp_tmsymbol( f, collector );
    fputs( " []\n", f );
    pp_indent_line( f, INDENTSTEP, "]" );
}

// Generate a declaration for the null-pointer event assumed function
static void pp_nullpointer_declaration(
 FILE *f,
 tmsymbol nullpointer
)
{
    pp_indent_line( f, INDENTSTEP, "," );
    pp_indent_line( f, INDENTSTEP, "procedure VnusEventNullPointer [] statements [" );
    pp_indent( f, INDENTSTEP*2 );
    fputs( "procedurecall ", f );
    pp_tmsymbol( f, nullpointer );
    fputs( " []\n", f );
    pp_indent_line( f, INDENTSTEP, "]" );
}

static void pp_arraymarker_declaration(
 FILE *f,
 const_TypeEntry_list typelist,
 const_Entry_list symtab,
 const_arraymarker marker
)
{
    if( marker->fnname == tmsymbolNIL ){
	return;
    }
    tmsymbol parm = gen_tmsymbol( "arr" );
    tmsymbol buf = gen_tmsymbol( "buf" );
    tmsymbol len = gen_tmsymbol( "len" );
    tmsymbol ix = gen_tmsymbol( "ix" );
    tmsymbol scopenm = gen_tmsymbol( "scope" );

    pp_indent_line( f, INDENTSTEP, "," );

    if( !generated_get_buffer ){
	tmsymbol parm1 = gen_tmsymbol( "arr" );

	// Generate declaration of the formal variable.
	pp_indent( f, INDENTSTEP );
	fputs( "formalvariable ", f );
	pp_tmsymbol( f, parm1 );
	fputs( " vnus_get_shape_buffer ", f );
	pp_ObjectType( f, typelist, add_tmsymbol( "java.lang.Object" ) );
	fputs( " final,\n", f );

	pp_indent( f, INDENTSTEP );
	fputs( "externalfunction vnus_get_shape_buffer [", f );
	pp_tmsymbol( f, parm1 );
	fputs( "] array ", f );
	pp_ObjectType( f, typelist, add_tmsymbol( "java.lang.Object" ) );
	fputs( ",\n", f );
	generated_get_buffer = true;
    }
    if( !generated_get_length ){
	tmsymbol parm1 = gen_tmsymbol( "arr" );

	// Generate declaration of the formal variable.
	pp_indent( f, INDENTSTEP );
	fputs( "formalvariable ", f );
	pp_tmsymbol( f, parm1 );
	fputs( " vnus_get_shape_length ", f );
	pp_ObjectType( f, typelist, add_tmsymbol( "java.lang.Object" ) );
	fputs( " final,\n", f );

	pp_indent( f, INDENTSTEP );
	fputs( "externalfunction vnus_get_shape_length [", f );
	pp_tmsymbol( f, parm1 );
	fputs( "] int,\n", f );
	generated_get_length = true;
    }

    // Generate declaration of the formal variable.
    pp_indent( f, INDENTSTEP );
    fputs( "formalvariable ", f );
    pp_tmsymbol( f, parm );
    fputs( " ", f );
    pp_tmsymbol( f, scopenm );
    fputs( " ", f );
    pp_ObjectType( f, typelist, add_tmsymbol( "java.lang.Object" ) );
    fputs( " final,\n", f );

    // Generate declaration of the local variable 'buf'.
    pp_indent( f, INDENTSTEP );
    fputs( "localvariable ", f );
    pp_tmsymbol( f, buf );
    fputs( " ", f );
    pp_tmsymbol( f, scopenm );
    fputs( " array ", f );
    pp_type( f, typelist, symtab, Pragma_listNIL, marker->t );
    fputs( " cast array ", f );
    pp_type( f, typelist, symtab, Pragma_listNIL, marker->t );
    fputs( " functioncall vnus_get_shape_buffer [", f );
    pp_tmsymbol( f, parm );
    fputs( " ] final,\n", f );

    // Generate declaration of the local variable 'len'.
    pp_indent( f, INDENTSTEP );
    fputs( "localvariable ", f );
    pp_tmsymbol( f, len );
    fputs( " ", f );
    pp_tmsymbol( f, scopenm );
    fputs( " int ", f );
    fputs( "functioncall vnus_get_shape_length [", f );
    pp_tmsymbol( f, parm );
    fputs( "] final,\n", f );

    // Generate declaration of the local variable 'ix'.
    pp_indent( f, INDENTSTEP );
    fputs( "cardinalityvariable ", f );
    pp_tmsymbol( f, ix );
    fputs( ",\n", f );

    // Generate procedure declaration
    pp_indent( f, INDENTSTEP );
    fputs( "procedure ", f );
    pp_tmsymbol( f, marker->fnname );
    fputs( " [", f );
    pp_tmsymbol( f, parm );
    fputs( "] statements ", f );
    pp_tmsymbol( f, scopenm );
    fputs( " [\n", f );
    pp_indent( f, 2*INDENTSTEP );
    fputs( "foreach [", f );
    pp_tmsymbol( f, ix );
    fputs( "=0:", f );
    pp_tmsymbol( f, len );
    fputs( "] statements [\n", f );
    pp_indent( f, 3*INDENTSTEP );
    // TODO: generate correct code for tuple types.
    fputs( "procedurecall Java_spar_compiler_GC_pushRef [cast ", f );
    pp_ObjectType( f, typelist, add_tmsymbol( "java.lang.Object" ) );
    fputs( " (", f );
    pp_tmsymbol( f, buf );
    fputs( ",[", f );
    pp_tmsymbol( f, ix );
    fputs( "])]\n", f );
    pp_indent_line( f, 2*INDENTSTEP, "]" );
    pp_indent_line( f, INDENTSTEP, "]" );
}

static void pp_declarations(
 FILE *f,
 const_TypeEntry_list typelist,
 const_Entry_list decls,
 tmsymbol boundsviolator,
 tmsymbol outofmemory,
 tmsymbol nullpointer,
 tmsymbol garbagecollector

)
{
    bool first;

    fputs( "declarations [\n", f );

    // Generate the declaration of the record __gc_reflink.
    pp_indent( f, INDENTSTEP );
    fputs( "record __gc_reflink [next:pointer record __gc_reflink,nelm:int,elm:array pointer ", f );
    pp_ObjectType( f, typelist, add_tmsymbol( "java.lang.Object" ) );
    fputs( ",serial:int,magic:int],\n", f );

    pp_indent_line( f, INDENTSTEP, "globalvariable __gc_globallink record __gc_reflink," );

    pp_indent_line( f, INDENTSTEP, "globalvariable __gc_reflink_chain pointer record __gc_reflink," );

    pp_indent_line( f, INDENTSTEP, "formalvariable __main_x __main_catchblock pointer record []" );
    first = false;
    for( unsigned int ix=0; ix<decls->sz; ix++ ){
	Entry d = decls->arr[ix];

	if( d->tag != TAGFieldEntry ){
	    pp_declaration( f, typelist, decls, INDENTSTEP, d, &first );
	}
    }
    pp_introspection_tables( f, typelist, decls, &first );
    pp_ivtable_type( f, typelist, decls, &first );
    pp_virtuality_tables( f, typelist, decls, &first );
    pp_serialization_declarations( f, typelist );
    pp_boundsviolator_declaration( f, boundsviolator );
    pp_outofmemory_declaration( f, outofmemory );
    pp_nullpointer_declaration( f, nullpointer );
    pp_garbagecollector_declaration( f, garbagecollector );
    const_arraymarker_list arraymarkers = get_arraymarkers();
    for( unsigned int iy = 0; iy<arraymarkers->sz; iy++ ){
	pp_arraymarker_declaration( f, typelist, decls, arraymarkers->arr[iy] );
    }
    fputs( "]\n", f );
}

/* Given a file handle, an indent and the name of an init function,
 * generate an invocation to this function.
 */
static void pp_init( FILE *f, unsigned int indent, tmsymbol init )
{
    pp_indent( f, indent );
    fprintf( f, "procedurecall %s [],\n", init->name );
}

/* Given a file handle, an indent and a list of init function names,
 * generate invocations to all these functions.
 */
static void pp_init_list( FILE *f, unsigned int indent, tmsymbol_list inits )
{
    for( unsigned int ix=0; ix<inits->sz; ix++ ){
	pp_init( f, indent, inits->arr[ix] );
    }
}

/* Given a file handle, an indent, and the name of the main function to invoke,
 * generate code to invoke it.
 */
static void pp_main( FILE *f, unsigned int indent, tmsymbol mn )
{
    pp_indent( f, indent );
    fprintf( f, "procedurecall %s []\n", mn->name );
}

/* Given a file handle, an indent, and the name of the default exception
 * handling function to invoke, * generate code to invoke it.
 */
static void pp_exceptor( FILE *f, SparProgram theprog, unsigned int indent, tmsymbol nm )
{
    pp_indent( f, indent );
    fprintf( f, "procedurecall %s [cast ", nm->name );
    type t = new_ObjectType( add_origsymbol( "java.lang.Exception" ) );
    pp_type(
	f,
	theprog->typelist,
	theprog->symtab,
	Pragma_listNIL,
	t
    );
    fputs( " __main_x]\n", f );
    rfre_type( t );
}

// Generate Vnus code to build the a link in the chain of active reference sets
// for the global variables.
static void pp_GC_global_reflink( FILE *f, unsigned int indent, const_TypeEntry_list typelist, const_Entry_list symtab )
{
    tmsymbol_list candidates = get_global_GC_links( symtab );

    if( candidates->sz != 0 ){
	tmsymbol obj = add_tmsymbol( "java.lang.Object" );

	pp_indent_line( f, indent, "assign field __gc_globallink next cast pointer record __gc_reflink null," );
	pp_indent( f, indent );
	fprintf( f, "assign field __gc_globallink nelm %u,\n", candidates->sz );
	pp_indent( f, indent );
	fputs( "assign field __gc_globallink elm cast array pointer ", f );
	pp_ObjectType( f, typelist, obj );
	fputs( " array pointer ", f );
	pp_ObjectType( f, typelist, obj );
	fputs( " [", f );
	bool first = true;
	for( unsigned int iy=0; iy<candidates->sz; iy++ ){
	    if( !first ){
		fputs( ", ", f );
	    }
	    else {
		first = false;
	    }
	    fputs( "cast pointer ", f );
	    pp_ObjectType( f, typelist, obj );
	    fputs( " &", f );
	    pp_tmsymbol( f, candidates->arr[iy] );
	}
	fputs( "],\n", f );
	pp_indent( f, indent );
	fprintf( f, "assign field __gc_globallink serial %d,\n", gc_serial++ );
	pp_indent( f, indent );
	fprintf( f, "assign field __gc_globallink magic %ld,\n", GCMAGIC );
	pp_indent_line( f, indent, "assign __gc_reflink_chain &__gc_globallink," );
    }
    rfre_tmsymbol_list( candidates );
}

void pp_SparProgram( FILE *f, SparProgram theprog )
{
    fputs( "// Generated by the Spar front-end\n", f );
    fputs( "\n", f );
    fputs( "program\n", f );
    fputs( "\n", f );
    (void) pp_opt_Pragma_list( f, 0, 0, false, theprog->pragmas );
    fputs( "\n\n", f );
    the_global_pragmas = theprog->pragmas;
    pp_declarations(
	f,
	theprog->typelist,
	theprog->symtab,
	theprog->boundsviolator,
	theprog->outofmemory,
	theprog->nullpointer,
	theprog->garbagecollector
    );
    fputs( "\nstatements [\n", f );
    if( !pref_nogarbagecollection ){
	pp_GC_global_reflink( f, INDENTSTEP, theprog->typelist, theprog->symtab );
    }
    pp_indent_line( f, INDENTSTEP, "catch __main_x statements __main_catchblock [" );
    pp_init_list( f, INDENTSTEP, theprog->inits );
    pp_main( f, 2*INDENTSTEP, theprog->main );
    pp_indent_line( f, INDENTSTEP, "]" );
    pp_indent_line( f, INDENTSTEP, "statements [" );
    pp_exceptor( f, theprog, 2*INDENTSTEP, theprog->exceptor );
    pp_indent_line( f, INDENTSTEP, "]" );
    fputs( "]\n", f );
}
