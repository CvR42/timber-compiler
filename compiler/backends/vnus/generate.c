/* File: generate.c
 *
 * Code generation routines.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <tmc.h>
#include <ctype.h>
#include <float.h>
#include <math.h>

#include "defs.h"
#include "tmadmin.h"
#include <vnusctl.h>
#include "global.h"
#include "error.h"
#include "generate.h"
#include "symbol_table.h"
#include "symbol_support.h"
#include "type.h"
#include "service.h"
#include "pragma.h"

/* Service function prefix. */
#define SERVICE_PREFIX_STRING "vnus_service_"
#define PARALLEL_FUNCTION_NAME (SERVICE_PREFIX_STRING "a_thread")
#define PARALLEL_FUNCTION_WRAPPER_NAME (SERVICE_PREFIX_STRING "a_thread_wrapper")

static tmsymbol me_name;
static Pragma_list global_pragmas;

/* Forward declaration of mutually recursive functions. */
static void generate_statements(
 FILE *f,
 const_declaration_list decls,
 const unsigned int indent,
 const_statement_list smts
);
static void generate_location(
 FILE *f,
 const_declaration_list decls,
 const Pragma_list pragmas,
 const_location loc
);
static void generate_expression(
 FILE *f,
 const_declaration_list decls,
 const_Pragma_list pragmas,
 const_expression x
);
static void generate_expression_list(
 FILE *f,
 const_declaration_list decls,
 const_Pragma_list pragmas,
 const_expression_list xl
);
static void generate_bracket_block(
 FILE *f,
 const_declaration_list decls,
 const unsigned int indent,
 const_block blk
);
static void generate_block(
 FILE *f,
 const_declaration_list decls,
 const unsigned int indent,
 const_block blk
);

#define TABSTEP 8

/* Given an expression, return a new expression where all pragmas
 * have been skipped.
 */
static const_expression skip_expression_wrappers( const_expression x )
{
    if( x->tag == TAGExprWrapper ){
	return skip_expression_wrappers( to_const_ExprWrapper(x)->x );
    }
    return x;
}

/* Given an location, return a new location where all pragmas
 * have been skipped.
 */
static const_location skip_location_wrappers( const_location x )
{
    if( x->tag == TAGLocWrapper ){
	return skip_location_wrappers( to_const_LocWrapper(x)->l );
    }
    return x;
}

/* Given a file handle 'f' and a indent 'indent', generate this
 * indent.
 */
static void generate_indent( FILE *f, unsigned int indent )
{
    while( indent>=TABSTEP ){
	fputc( '\t', f );
	indent -= TABSTEP;
    }
    for( unsigned int n=0; n<indent; n++ ){
	fputc( ' ', f );
    }
}

/* Given a file handle 'f', an indent 'indent' and a tmstring 's',
 * generate a line consisting of 's' indented to the given indent.
 */
static void generate_indent_line( FILE *f, unsigned int indent, const char *s )
{
    generate_indent( f, indent );
    fprintf( f, "%s\n", s );
}

static void generate_tmsymbol( FILE *f, const tmsymbol s )
{
    fputs( s->name, f );
}

static void generate_origsymbol( FILE *f, const_origsymbol s )
{
    generate_tmsymbol( f, s->sym );
}

/* Given a size 'frm', generate a dimension expression for
 * use in a shape constructor.
 */
static void generate_dimension(
 FILE *f,
 const_declaration_list decls,
 const_size frm
)
{
    switch( frm->tag ){
	case TAGSizeExpression:
	    generate_expression( f, decls, Pragma_listNIL, to_const_SizeExpression(frm)->x );
	    break;

	case TAGSizeDontcare:
	    fputs( "0", f );
	    break;

    }
}

/* Given a list of dimensions, a mark function expression, and possibly
 * a fill expression, generate a parameter list for an invocation of the
 * creation function.
 */
static void generate_shape_creation_parms(
 FILE *f,
 const_declaration_list decls,
 const_size_list fl,
 const_origsymbol markfn,
 const_expression fill
)
{
    fputs( "( (VnusBase::markfntype) ", f );
    if( markfn == origsymbolNIL ){
	fputs( "NULL", f );
    }
    else {
	fputs( "&", f );
	generate_origsymbol( f, markfn );
    }
    if( fill != expressionNIL ){
	fputs( ", ", f );
	generate_expression( f, decls, Pragma_listNIL, fill );
    }
    for( unsigned int ix=0; ix<fl->sz; ix++ ){
        fputs( ", ", f );
        generate_dimension( f, decls, fl->arr[ix] );
    }
    fputs( " )", f );
}

/* Given a block size 'bsz' and a size list 'fl', generate a dimension
 * list for use in a shape constructor.
 */
void generate_dimensionlist(
 FILE *f,
 const_declaration_list decls,
 const_expression init,
 const unsigned int nelms,
 const_size_list fl,
 const_origsymbol markfn
)
{
    fputs( "( (VnusBase::markfntype) ", f );
    if( markfn == origsymbolNIL ){
	fputs( "NULL", f );
    }
    else {
	fputs( "&", f );
	generate_origsymbol( f, markfn );
    }
    if( init != expressionNIL ){
	fputs( ", ", f );
        generate_expression( f, decls, Pragma_listNIL, init );
        fprintf( f, ", %u", nelms );
    }
    for( unsigned int ix=0; ix<fl->sz; ix++ ){
        fputs( ", ", f );
        generate_dimension( f, decls, fl->arr[ix] );
    }
    fputs( " )", f );
}

/* Given an optional expression, generate an initialization when necessary. */
static void generate_initalization( FILE *f, const_declaration_list decls, const_optexpression x )
{
    switch( x->tag ){
	case TAGOptExprNone:
	    break;

	case TAGOptExpr:
	    fputs( " = ", f );
	    generate_expression( f, decls, Pragma_listNIL, to_const_OptExpr(x)->x );
	    break;
    }
}

/* Given a file handle 'f', an indent 'indent', a variable name 'nm' and
 * a type 't', generate a type declaration for that variable.
 */
static void generate_forward_variable_declaration(
 FILE *f,
 const_declaration_list decls,
 const unsigned int indent,
 const_origsymbol nm,
 int flags,
 const_type t
)
{
    generate_indent( f, indent );
    fputs( "extern ", f );
    tmstring decl = declaration_string( decls, t, flags, nm->sym->name, false );
    fputs( decl, f );
    rfre_tmstring( decl );
    fputs( ";\n", f );
}

static void generate_variable_initializer(
 FILE *f,
 const_declaration_list decls,
 const_type t,
 const_optexpression init
)
{
    switch( t->tag ){
	case TAGTypeBase:
	case TAGTypeRecord:
	case TAGTypeNamedRecord:
	case TAGTypePointer:
	case TAGTypeUnsizedArray:
	case TAGTypeArray:
	case TAGTypeNeutralPointer:
	    generate_initalization( f, decls, init );
	    break;

	case TAGTypePragmas:
	    generate_variable_initializer( f, decls, to_const_TypePragmas(t)->t, init );
	    break;

	case TAGTypeMap:
	    generate_variable_initializer( f, decls, to_const_TypeMap(t)->t, init );
	    break;

	case TAGTypeShape:
	{
	    expression x = expressionNIL;
	    unsigned int nelms = 0;
	    if( init != optexpressionNIL && init->tag == TAGOptExpr ){
		expression sx = to_const_OptExpr( init )->x;
		if( sx->tag != TAGExprShape ){
		    internal_error( "A shape can only be initialized with a shape expression" );
		}
		else {
		    x = to_ExprShape(sx)->arr;
		    nelms = to_ExprShape(sx)->nelms;
		}
	    }
	    // If this assertion fails, we need a gc marker function
	    // to pass to generate_dimensionlist()
	    assert( to_const_TypeShape(t)->elmtype->tag == TAGTypeBase );
	    generate_dimensionlist(
		f,
		decls,
		x,
		nelms,
		to_const_TypeShape(t)->sizes,
		origsymbolNIL
	    );
	    break;
	}

	case TAGTypeProcedure:
	case TAGTypeFunction:
	    internal_error( "Function or procedure variables not allowed" );
	    break;
    }
}

/* Given a file handle 'f', an indent 'indent', a variable name 'nm' and
 * a type 't', generate a type declaration for that variable.
 */
static void generate_variable_declaration(
 FILE *f,
 const_declaration_list decls,
 const unsigned int indent,
 const_origsymbol nm,
 const_type t,
 const int flags,
 const_optexpression init
)
{
    tmstring decl = declaration_string( decls, t, flags, nm->sym->name, false );

    generate_indent( f, indent );
    fputs( decl, f );
    rfre_tmstring( decl );
    generate_variable_initializer( f, decls, t, init );
    fputs( ";\n", f );
}

/* Given a file handle 'f', a variable name 'nm' and
 * a type 't', generate a type declaration for that external variable.
 */
static void generate_external_variable_declaration(
 FILE *f,
 const_declaration_list decls,
 const_origsymbol nm,
 const int flags,
 const_type t
)
{
    tmstring decl = declaration_string( decls, t, flags, nm->sym->name, false );

    fputs( "// extern ", f );
    fputs( decl, f );
    rfre_tmstring( decl );
    fputs( ";\n", f );
}

/* Given a formal parameter 'arg', generate a formal parameter declaration. */
static void generate_formalparameter(
 FILE *f,
 const_declaration_list decls,
 const_origsymbol arg
)
{
    const_type argtype = lookup_argtype( decls, arg );
    bool isconst = is_readonly( decls, arg, global_pragmas );
    tmstring typenm;

    bool isref = (isconst && argtype->tag == TAGTypeShape && is_location( decls, arg, global_pragmas ));
    if( isconst ){
	fputs( "const ", f );
    }
    typenm = type_string( decls, argtype );
    fprintf( f, "%s ", typenm );
    rfre_tmstring( typenm );
    if( isref ){
	fputs( "&", f );
    }
    fputs( arg->sym->name, f );
}

/* Given a list of formalParameters 'al', generate a formal parameter list. */
static void generate_formalparameter_list(
 FILE *f,
 const_declaration_list decls,
 const_origsymbol_list xl
)
{
    bool said_something = false;

    fputs( "( ", f );
    for( unsigned int ix=0; ix<xl->sz; ix++ ){
	if( said_something ){
	    fputs( ", ", f );
	}
	generate_formalparameter( f, decls, xl->arr[ix] );
	said_something = true;
    }
    if( !said_something ){
        fputs( "void", f );
    }
    fputs( " )", f );
}

/* Given a file handle 'f', an indent 'indent' and a
 * local identifier 'local', generate a local declaration
 * for this variable.
 */
static void generate_local_declaration(
 FILE *f,
 const_declaration_list decls,
 const unsigned int indent,
 const_origsymbol local
)
{
    const_declaration d = lookup_declaration( decls, local );

    switch( d->tag ){
	case TAGDeclCardinalityVariable:
	    /* We do not generate declarations for cardinality variables. */
	    break;

        case TAGDeclLocalVariable:
	    generate_variable_declaration(
		f,
		decls,
		indent,
		local,
		to_const_DeclLocalVariable(d)->t,
		d->flags,
		to_const_DeclLocalVariable(d)->init
	    );
	    break;

        case TAGDeclFormalVariable:
	    /* Nothing, was declared as formal parameter. */
	    break;

        case TAGDeclFunction:
        case TAGDeclRecord:
        case TAGDeclProcedure:
        case TAGDeclGlobalVariable:
        case TAGDeclExternalFunction:
        case TAGDeclExternalProcedure:
        case TAGDeclExternalVariable:
	    origsymbol_internal_error( local, "This declaration should be global" );
	    break;

    }
}

/* Given a file handle 'f', an indent 'indent' and a
 * list of local identifiers 'locals', generate local declarations
 * for these variables.
 */
static void generate_local_declarations(
    FILE *f,
    const_declaration_list decls,
    const unsigned int indent,
    const_origsymbol_list locals
)
{
    for( unsigned int ix=0; ix<locals->sz; ix++ ){
	generate_local_declaration( f, decls, indent, locals->arr[ix] );
    }
}

/* A table of strings for the binary operators. */
static const char *binop_strings[] =
{
    "&",	/* BINOP_AND */
    "/",	/* BINOP_DIVIDE */
    "==",	/* BINOP_EQUAL */
    ">",	/* BINOP_GREATER */
    ">=",	/* BINOP_GREATEREQUAL */
    "<",	/* BINOP_LESS */
    "<=",	/* BINOP_LESSEQUAL */
    "-",	/* BINOP_MINUS */
    "%",	/* BINOP_MOD */
    "!=",	/* BINOP_NOTEQUAL */
    "|",	/* BINOP_OR */
    "+",	/* BINOP_PLUS */
    "<<",	/* BINOP_SHIFTLEFT */
    ">>",	/* BINOP_SHIFTRIGHT */
    "&&",	/* BINOP_SHORTAND */
    "||",	/* BINOP_SHORTOR */
    "*",	/* BINOP_TIMES */
    ">>",	/* BINOP_USHIFTRIGHT */
    "^"		/* BINOP_XOR */
};

/* Given two operands and an operator, generate a binary operator
 * expression.
 */
static void generate_binary_operation(
 FILE *f,
 const_declaration_list decls,
 const_type ta,
 const_expression randa,
 const BINOP tor,
 const_expression randb
)
{
    if( ta == typeNIL ){
	fputs( "0", f );
	return;
    }
    if( ta->tag == TAGTypePragmas ){
	generate_binary_operation( f, decls, to_const_TypePragmas(ta)->t, randa, tor, randb );
	return;
    }
    if( ta->tag == TAGTypeMap ){
	generate_binary_operation( f, decls, to_const_TypeMap(ta)->t, randa, tor, randb );
	return;
    }
    if( tor == BINOP_USHIFTRIGHT ){
	tmstring tnm;

	assert( ta->tag == TAGTypeBase );
	tnm = type_string( decls, ta );
	fprintf( f, "((%s) (((Unsigned%s)", tnm, tnm );
	generate_expression( f, decls, Pragma_listNIL, randa );
	fputs( ")>>", f );
	generate_expression( f, decls, Pragma_listNIL, randb );
	fputs( "))", f );
	rfre_tmstring( tnm );
    }
    else if( ta->tag == TAGTypeBase && to_const_TypeBase(ta)->base == BT_STRING ){
	fputs( "(strcmp(", f );
	generate_expression( f, decls, Pragma_listNIL, randa );
	fputs( ",", f );
	generate_expression( f, decls, Pragma_listNIL, randb );
	fputs( ")", f );
	fprintf( f, "%s", binop_strings[(int) tor] );
	fputs( "0)", f );
    }
    else if(
	tor == BINOP_MOD &&
	ta->tag == TAGTypeBase &&
	(
	    to_const_TypeBase(ta)->base == BT_FLOAT ||
	    to_const_TypeBase(ta)->base == BT_DOUBLE
	)
    ){
	fputs( "fmod(", f );
	generate_expression( f, decls, Pragma_listNIL, randa );
	fputs( ",", f );
	generate_expression( f, decls, Pragma_listNIL, randb );
	fputs( ")", f );
    }
    else {
	fprintf( f, "(" );
	generate_expression( f, decls, Pragma_listNIL, randa );
	fprintf( f, "%s", binop_strings[(int) tor] );
	generate_expression( f, decls, Pragma_listNIL, randb );
	fputs( ")", f );
    }
}

/* Given two operands and an operator, generate a binary operator
 * expression.
 */
static void generate_binary_operation(
 FILE *f,
 const_declaration_list decls,
 const_expression randa,
 const BINOP tor,
 const_expression randb
)
{
    const type ta = derive_type_expression( decls, originNIL, randa );

    generate_binary_operation( f, decls, ta, randa, tor, randb );
    rfre_type( ta );
}

// Given an operator and a list of operands, generate code to evaluate
// the reduction.
// For the moment we transform this into a linked list of binary operations
// since generating the proper binary operation is quite complex.
static void generate_reduction_operation(
 FILE *f,
 const_declaration_list decls,
 const BINOP optor,
 const_expression_list operands
)
{
    if( operands->sz<1 ){
	internal_error( "A reduction should have elements" );
    }
    // Start with the expression at the root of the tree
    expression x = rdup_expression( operands->arr[0] );
    // And now keep wrapping x in a binop with the new expression to
    // the right.
    for( unsigned int ix=1; ix<operands->sz; ix++ ){
	x = new_ExprBinop( x, optor, rdup_expression( operands->arr[ix] ) );
    }
    generate_expression( f, decls, Pragma_listNIL, x );
    rfre_expression( x );
}

/* A table of strings for the unary operators.
 * The operator  tmstring for UNOP_PLUS is empty since the GNU Complex
 * class does not have a unary plus operator, and this is the
 * easiest way to handle it.
 */
static const char *boolean_unop_strings[] =
{
    "!",	/* UNOP_NOT */
    "",		/* UNOP_PLUS */
    "-"		/* UNOP_NEGATE */
};

/* A table of strings for the unary operators.
 * The operator  tmstring for UNOP_PLUS is empty since the GNU Complex
 * class does not have a unary plus operator, and this is the
 * easiest way to handle it.
 */
static const char *unop_strings[] =
{
    "~",	/* UNOP_NOT */
    "",		/* UNOP_PLUS */
    "-"		/* UNOP_NEGATE */
};

/* Given an operand and an operand, generate a unary operator expression.  */
static void generate_unary_operation(
 FILE *f,
 const_declaration_list decls,
 const UNOP tor,
 const_expression operand
)
{
    type t = derive_type_expression( decls, originNIL, operand );

    if( t == NULL ){
	fputs( "0", f );
	return;
    }
    if( t->tag == TAGTypeBase && to_const_TypeBase(t)->base == BT_BOOLEAN ){
	fprintf( f, "(%s", boolean_unop_strings[(int) tor] );
    }
    else {
	fprintf( f, "(%s", unop_strings[(int) tor] );
    }
    generate_expression( f, decls, Pragma_listNIL, operand );
    fprintf( f, ")" );
    rfre_type( t );
}

/* Given a list of expressions 'xl', generate a parameter list.  */
static void generate_actualparameter_list( FILE *f, const_declaration_list decls, const_expression_list xl )
{
    if( xl->sz == 0 ){
	fputs( "()", f );
    }
    else {
	fputs( "( ", f );
	for( unsigned int ix=0; ix<xl->sz; ix++ ){
	    if( ix!=0 ){
		fputs( ", ", f );
	    }
	    generate_expression( f, decls, Pragma_listNIL, xl->arr[ix] );
	}
	fputs( " )", f );
    }
}

/* Given a list of expressions 'xl', generate a index list to be used
 * as a subscript.
 */
static void generate_index_list( FILE *f, const_declaration_list decls, const_expression_list xl )
{
    if( xl->sz == 0 ){
	fprintf( f, "()" );
	return;
    }
    fprintf( f, "( " );
    for( unsigned int ix=0; ix<xl->sz; ix++ ){
	if( ix!=0 ){
	    fprintf( f, ", " );
	}
	generate_expression( f, decls, Pragma_listNIL, xl->arr[ix] );
    }
    fprintf( f, " )" );
}

// Generate a C expression to access the given shape or array with the
// given list of subscripts.
static void generate_selection(
 FILE *f,
 const_declaration_list decls,
 const_Pragma_list pragmas, 
 const_expression selected,
 const_expression_list selectors
)
{
    const type t = derive_type_expression( decls, originNIL, selected );

    generate_expression( f, decls, Pragma_listNIL, selected );
    if( t->tag == TAGTypeUnsizedArray || t->tag == TAGTypeArray ){
	if( selectors->sz == 1 ){
	    fputs( "[", f );
	    generate_expression( f, decls, Pragma_listNIL, selectors->arr[0] );
	    fputs( "]", f );
	}
	else {
	    internal_error( "An array element should have exactly one subscript" );
	}
    }
    else {
	bool checked;

	if(
	    get_boolean_pragma(
		global_pragmas,
		pragmas,
		"boundscheck",
		&checked
	    )
	) {
	    if( checked ){
		fputs( ".AccessChecked", f );
	    }
	    else {
		fputs( ".AccessNotChecked", f );
	    }
	}
	generate_index_list( f, decls, selectors );
    }
    rfre_type( t );
}

// Generate a C expression to access the given shape or array with the
// given index expression.
static void generate_flat_selection(
 FILE *f,
 const_declaration_list decls,
 const_expression selected,
 const_expression ix
)
{
    generate_expression( f, decls, Pragma_listNIL, selected );
    fputs( "[", f );
    generate_expression( f, decls, Pragma_listNIL, ix );
    fputs( "]", f );
}

/* Given a location, generate an expression for it.  */
static void generate_location(
 FILE *f,
 const_declaration_list decls,
 const Pragma_list pragmas,
 const_location loc
)
{
    switch( loc->tag ){
        case TAGLocName:
	    fprintf( f, "%s", to_const_LocName(loc)->name->sym->name );
	    break;

	case TAGLocSelection:
	    generate_selection(
		f,
		decls,
		pragmas, 
		to_const_LocSelection(loc)->shape,
		to_const_LocSelection(loc)->indices
	    );
	    break;

	case TAGLocFlatSelection:
	    generate_flat_selection(
		f,
		decls,
		to_const_LocFlatSelection(loc)->shape,
		to_const_LocFlatSelection(loc)->index
	    );
	    break;

	case TAGLocDeref:
	    fputs( "(*", f );
	    generate_expression( f, decls, Pragma_listNIL, to_const_LocDeref(loc)->ref );
            fputs( ")", f );
	    break;

	case TAGLocWhere:
	    internal_error( "where location should have been rewritten" );
	    break;

	case TAGLocNotNullAssert:
	    fputs( "VnusNotNullAssert(", f );
	    generate_location( f, decls, Pragma_listNIL, to_const_LocNotNullAssert(loc)->l );
	    fputs( ")", f );
	    break;

	case TAGLocField:
	{
	    int pos;
	    bool brac = false;
	    const_expression rec = skip_expression_wrappers( to_const_LocField(loc)->rec );
	    const type rectype = derive_type_expression( decls, to_const_LocField(loc)->field->orig, rec );
	    bool arrow = false;

	    if( rec->tag == TAGExprDeref ){
		arrow = true;
		rec = skip_expression_wrappers( to_const_ExprDeref(rec)->ref );
	    }

	    brac = (rec->tag != TAGExprName && rec->tag != TAGExprFunctionCall);
	    if( brac ){
		fputs( "(", f );
	    }
	    generate_expression( f, decls, Pragma_listNIL, rec );
	    if( brac ){
		fputs( ")", f );
	    }
	    pos = find_field( decls, rectype, to_const_LocField(loc)->field );
	    if( pos<0 ){
		pos = 0;
	    }
	    rfre_type( rectype );
	    if( arrow ){
		fputs( "->", f );
	    }
	    else {
		fputs( ".", f );
	    }
	    fprintf( f, "field%u", pos );
	    break;
	}

	case TAGLocFieldNumber:
	{
	    const_expression rec = skip_expression_wrappers( to_const_LocFieldNumber(loc)->rec );
	    bool arrow = false;

	    if( rec->tag == TAGExprDeref ){
		arrow = true;
		rec = skip_expression_wrappers( to_const_ExprDeref(rec)->ref );
	    }

	    bool brac = (rec->tag != TAGExprName && rec->tag != TAGExprFunctionCall);
	    if( brac ){
		fputs( "(", f );
	    }
	    generate_expression( f, decls, Pragma_listNIL, rec );
	    if( brac ){
		fputs( ")", f );
	    }
	    int pos = to_const_LocFieldNumber(loc)->field;
	    if( pos<0 ){
		pos = 0;
	    }
	    if( arrow ){
		fputs( "->", f );
	    }
	    else {
		fputs( ".", f );
	    }
	    fprintf( f, "field%u", pos );
	    break;
	}

	case TAGLocWrapper:
	    generate_location(
		f,
		decls,
		to_const_LocWrapper(loc)->pragmas,
		to_const_LocWrapper(loc)->l
	    );
	    break;
    }
}

// Given a file handle 'f', a declaration list 'decls' and a type 't',
// generate the proper expression after a 'new' to allocate an instance.
static void generate_new_dimensionlist( FILE *f, const_declaration_list decls, const_type t )
{
    switch( t->tag ){
	case TAGTypeBase:
	case TAGTypeNamedRecord:
	case TAGTypeRecord:
	case TAGTypePointer:
	case TAGTypeUnsizedArray:
	case TAGTypeArray:
	case TAGTypeNeutralPointer:
	    break;

	case TAGTypePragmas:
	    generate_new_dimensionlist( f, decls, to_const_TypePragmas(t)->t );
	    break;

	case TAGTypeMap:
	    generate_new_dimensionlist( f, decls, to_const_TypeMap(t)->t );
	    break;

	case TAGTypeShape:
	case TAGTypeProcedure:
	case TAGTypeFunction:
	    internal_error( "Function or procedure variables not allowed" );
	    break;
    }
}

static void generate_filled_new( FILE *f, const_declaration_list decls, const_type t, const_expression init, origsymbol gcmarker  )
{
    if( t->tag == TAGTypePragmas ){
	generate_filled_new( f, decls, to_const_TypePragmas(t)->t, init, gcmarker );
	return;
    }
    if( t->tag == TAGTypeMap ){
	generate_filled_new( f, decls, to_const_TypeMap(t)->t, init, gcmarker );
	return;
    }
    assert( t->tag == TAGTypeShape );

    tmstring tnm = type_string( decls, t );
    fprintf( f, "%s::create_filled_shape", tnm );
    generate_shape_creation_parms(
	f,
	decls,
	to_const_TypeShape(t)->sizes,
	gcmarker,
	init
    );
    rfre_tmstring( tnm );
}

// Generate code to allocate and null an array.
// The current code assumes that the nulling is done in the 'new',
// so it doesn't generate specific code for it.
static void generate_nulled_new( FILE *f, const_declaration_list decls, const_type t, origsymbol gcmarker )
{
    if( t->tag == TAGTypePragmas ){
	generate_nulled_new( f, decls, to_const_TypePragmas(t)->t, gcmarker );
	return;
    }
    if( t->tag == TAGTypeMap ){
	generate_nulled_new( f, decls, to_const_TypeMap(t)->t, gcmarker );
	return;
    }
    assert( t->tag == TAGTypeShape );

    tmstring tnm = type_string( decls, t );
    fprintf( f, "%s::create_nulled_shape", tnm );
    generate_shape_creation_parms(
	f,
	decls,
	to_const_TypeShape(t)->sizes,
	gcmarker,
	expressionNIL
    );
    rfre_tmstring( tnm );
}

static void generate_new_record( FILE *f, const_declaration_list decls, const_type t, const_expression_list fields  )
{
    if( t->tag == TAGTypePragmas ){
	generate_new_record( f, decls, to_const_TypePragmas(t)->t, fields );
	return;
    }
    if( t->tag == TAGTypeMap ){
	generate_new_record( f, decls, to_const_TypeMap(t)->t, fields );
	return;
    }
    assert( t->tag == TAGTypeRecord || t->tag == TAGTypeNamedRecord );

    tmstring tnm = type_string( decls, t );
    fprintf( f, "(new %s( ", tnm );
    generate_expression_list( f, decls, Pragma_listNIL, fields );
    fputs( " ) )", f );
    rfre_tmstring( tnm );
}

/* Given a file handle 'f' and an expression 'x', generate C code
 * to evaluate the expression.
 */
static void generate_expression(
 FILE *f,
 const_declaration_list decls,
 const_Pragma_list pragmas,
 const_expression x
)
{
    switch( x->tag ){
	case TAGExprNull:
	    fputs( "NULL", f );
	    break;

	case TAGExprByte:
	    fprintf( f, "((VnusByte) %d)", (int) to_const_ExprByte(x)->v );
	    break;

	case TAGExprShort:
	    fprintf( f, "((VnusShort) %d)", (int) to_const_ExprShort(x)->v );
	    break;

	case TAGExprInt:
	{
	    vnus_int v = to_const_ExprInt(x)->v;

	    if( v+1 == -VNUS_INT_MAX ){
		fputs( "(-2147483647L - 1L)", f );
	    }
	    else if( v<0 ){
		fprintf( f, "(%ldL)", (long) v );
	    }
	    else {
		fprintf( f, "%ldL", (long) v );
	    }
	    break;
	}

	case TAGExprLong:
	{
	    vnus_long v = to_const_ExprLong(x)->v;

	    if( longAsClass ){
		fprintf( f, "signed_longlong(%lldL,%lluLU)",(v>>32),(v&0xFFFFFFFF) );
	    }
	    else {
		if( v<0 ){
		    fprintf( f, "(%lldLL)", v );
		}
		else {
		    fprintf( f, "%lldLL", v );
		}
	    }
	    break;
	}

	case TAGExprFloat:
	{
	    vnus_float n = to_const_ExprFloat(x)->v;
	    char buf[100];
	    bool ok = true;

	    if( n == -n  || fabs( n )<=FLT_MIN ){
		ok = false;
	    }
	    // First test if a normally formated floating point
	    // constant does the right thing. If a strtod on the
	    // result of the sprintf gives the right result, we
	    // assume the compiler will also interpret it the right
	    // way.
	    if( ok ){
		if( sprintf( buf, "%.3g", n ) == 0 ){
		    ok = false;
		}
		if( ok ){
		    double f1 = strtod( buf, NULL );
		    if( (float) f1 != n ){
			ok = false;
		    }
		}
		// There is no simple way to generate the correct strings
		// for infinities. (1.0/0.0) causes warnings in the GNU
		// compiler, and the expressions in the numeric_limits template
		// look nice, but their include file <limits> is not available
		// in recent GNU compilers.
		if( strcmp( buf, "inf" ) == 0 ){
		    ok = false;
		}
		else if( strcmp( buf, "-inf" ) == 0 ){
		    ok = false;
		}
	    }

	    if( ok ){
		if( buf[0] == '-' ){
		    fputc( '(', f );
		}
		fputs( buf, f );
		if( buf[0] == '-' ){
		    fputc( ')', f );
		}
	    }
	    else {
		vnus_int ni = vnus_float_bits_to_int( n );

		if( ni == 0 ){
		    // Don't use VnusIntBitsToFloat for this important
		    // constant, since it may be costly to do so.
		    fputs( "0.0", f );
		}
		else {
		    if( ni+1 == -VNUS_INT_MAX ){
			fputs( "VnusIntBitsToFloat(-2147483647L - 1L)", f );
		    }
		    else {
			fprintf( f, "VnusIntBitsToFloat(%ldL)", (long int) ni );
		    }
		}
	    }
	    break;
	}

	case TAGExprDouble:
	{
	    vnus_double n = to_const_ExprDouble(x)->v;
	    char buf[100];
	    bool ok = true;

	    if( n == -n ){
		ok = false;
	    }
	    if( fabs( n )<=DBL_MIN ){
		ok = false;
	    }
	    
	    // First test if a normally formated floating point
	    // constant does the right thing. If a strtod on the
	    // result of the sprintf gives the right result, we
	    // assume the compiler will also interpret it the right
	    // way.
	    if( ok ){
		if( sprintf( buf, "%.9g", n ) == 0 ){
		    ok = false;
		}
		if( ok ){
		    double d1 = strtod( buf, NULL );

		    if( d1 != n || n == -n ){
			ok = false;
		    }
		}
		// There is no simple way to generate the correct strings
		// for infinities. (1.0/0.0) causes warnings in the GNU
		// compiler, and the expressions in the numeric_limits template
		// look nice, but their include file <limits> is not available
		// in recent GNU compilers.
		if( strcmp( buf, "inf" ) == 0 ){
		    ok = false;
		}
		else if( strcmp( buf, "-inf" ) == 0 ){
		    ok = false;
		}
	    }

	    if( ok ){
		if( buf[0] == '-' ){
		    fputc( '(', f );
		}
		fputs( buf, f );
		if( buf[0] == '-' ){
		    fputc( ')', f );
		}
	    }
	    else {
		vnus_long ni = vnus_double_bits_to_long( n );
		if( ni == 0 ){
		    // Don't use VnusLongBitsToDouble for this important
		    // constant, since it may be costly to do so.
		    fputs( "0.0", f );
		}
		else {
		    if( ni+1 == -VNUS_LONG_MAX ){
			fputs( "VnusLongBitsToDouble(-9223372036854775807LL - 1L)", f );
		    }
		    else {
			fprintf( f, "VnusLongBitsToDouble(%lldLL)", ni );
		    }
		}
	    }
	    break;
	}

	case TAGExprChar:
	{
	    vnus_char c = to_const_ExprChar(x)->c;
	    if( isprint( c ) ){
		fprintf( f, "'%c'", c );
	    }
	    else {
		fprintf( f, "'\\%03d'", (int) c );
	    }
	    break;
	}

	case TAGExprString:
	    fprintf( f, "\"%s\"", to_const_ExprString(x)->s );
	    break;

	case TAGExprBoolean:
	    fprintf( f, "%s", to_const_ExprBoolean(x)->b ? "true" : "false" );
	    break;

	case TAGExprComplex:
	    fputs( "VnusComplex( ", f );
	    generate_expression( f, decls, Pragma_listNIL, to_const_ExprComplex(x)->re );
	    fputs( ", ", f );
	    generate_expression( f, decls, Pragma_listNIL, to_const_ExprComplex(x)->im );
	    fputs( " )", f );
	    break;

	case TAGExprShape:
	{
	    type t = to_const_ExprShape(x)->elmtype;

	    tmstring nm = type_string( decls, t );
	    unsigned int rank =  to_const_ExprShape(x)->sizes->sz;
	    if( rank<4 ){
		fprintf( f, "ShapeD%u<%s>", rank, nm );
	    }
	    else {
		fprintf( f, "ShapeDn<%s,%u>", nm, rank );
	    }
	    // If this assertion fails, we need a gc marker function
	    // to pass to generate_dimensionlist()
	    assert( t->tag == TAGTypeBase );
	    generate_dimensionlist(
		f,
		decls,
		to_const_ExprShape(x)->arr,
		to_const_ExprShape(x)->nelms,
		to_const_ExprShape(x)->sizes,
		origsymbolNIL
            );
	    rfre_tmstring( nm );
	    break;
	}

	case TAGExprRecord:
	{
	    type rectype = derive_type_expression( decls, originNIL, x );

	    if( rectype->tag != TAGTypeRecord ){
		internal_error( "The type of a record expression should be a record" );
	    }
	    tmsymbol recnm = get_record_name( decls, originNIL, to_TypeRecord(rectype)->fields );
	    rfre_type( rectype );
	    fprintf( f, "%s( ", recnm->name );
	    generate_expression_list( f, decls, pragmas, to_const_ExprRecord(x)->fields );
	    fputs( " )", f );
	    break;
	}

	case TAGExprGetSize:
	{
	    const_ExprGetSize szx = to_const_ExprGetSize(x);
	    type arrtype = derive_type_expression( decls, originNIL, szx->shape );

	    generate_expression( f, decls, Pragma_listNIL, szx->shape );
	    if( arrtype->tag == TAGTypeShape && szx->dim->tag == TAGExprInt ){
		unsigned int rank = to_TypeShape( arrtype )->sizes->sz;
		long int dim = (unsigned int) to_ExprInt( szx->dim )->v;

		if( rank<4 && dim>=0 && (unsigned int) dim<rank ){
		    fprintf( f, ".GetSize%ld()", dim );
		    rfre_type( arrtype );
		    break;
		}
	    }
	    fputs( ".GetSize( ", f );
	    generate_expression( f, decls, Pragma_listNIL, szx->dim );
	    fputs( " )", f );
	    rfre_type( arrtype );
	    break;
	}

	case TAGExprGetLength:
	    generate_expression( f, decls, Pragma_listNIL, to_const_ExprGetLength(x)->shape );
	    fputs( ".GetLength()", f );
	    break;

	case TAGExprGetBuf:
	    generate_expression( f, decls, Pragma_listNIL, to_const_ExprGetBuf(x)->shape );
	    fputs( ".GetBuffer()", f );
	    break;

	case TAGExprFunctionCall:
	{
	    generate_expression( f, decls, Pragma_listNIL, to_const_ExprFunctionCall(x)->function );
	    generate_actualparameter_list( f, decls, to_const_ExprFunctionCall(x)->parameters );
	    break;
	}

	case TAGExprSelection:
	    generate_selection(
		f,
		decls,
		pragmas,
		to_const_ExprSelection(x)->shape,
		to_const_ExprSelection(x)->indices
	    );
	    break;

	case TAGExprFlatSelection:
	    generate_flat_selection(
		f,
		decls,
		to_const_ExprFlatSelection(x)->shape,
		to_const_ExprFlatSelection(x)->index
	    );
	    break;

	case TAGExprUnop:
	    generate_unary_operation(
		f,
		decls,
		to_const_ExprUnop(x)->optor,
		to_const_ExprUnop(x)->operand
	    );
	    break;

	case TAGExprCheckedIndex:
	{
	    const_ExprCheckedIndex ix = to_const_ExprCheckedIndex(x);

	    fputs( "VnusCheckBound(", f );
	    generate_expression( f, decls, Pragma_listNIL, ix->val );
	    fputs( ",", f );
	    generate_expression( f, decls, Pragma_listNIL, ix->upperbound );
	    fputs( ")", f );
	    break;
	}

	case TAGExprUpperCheckedIndex:
	{
	    const_ExprUpperCheckedIndex ix = to_const_ExprUpperCheckedIndex(x);

	    fputs( "VnusUpperCheckBound(", f );
	    generate_expression( f, decls, Pragma_listNIL, ix->val );
	    fputs( ",", f );
	    generate_expression( f, decls, Pragma_listNIL, ix->upperbound );
	    fputs( ")", f );
	    break;
	}

	case TAGExprLowerCheckedIndex:
	{
	    const_ExprLowerCheckedIndex ix = to_const_ExprLowerCheckedIndex(x);

	    fputs( "VnusLowerCheckBound(", f );
	    generate_expression( f, decls, Pragma_listNIL, ix->val );
	    fputs( ")", f );
	    break;
	}

	case TAGExprIsBoundViolated:
	{
	    const_ExprIsBoundViolated ix = to_const_ExprIsBoundViolated(x);

	    fputs( "VnusIsBoundViolated(", f );
	    generate_expression( f, decls, Pragma_listNIL, ix->val );
	    fputs( ",", f );
	    generate_expression( f, decls, Pragma_listNIL, ix->upperbound );
	    fputs( ")", f );
	    break;
	}

	case TAGExprIsUpperBoundViolated:
	{
	    const_ExprIsUpperBoundViolated ix = to_const_ExprIsUpperBoundViolated(x);

	    fputs( "VnusIsUpperBoundViolated(", f );
	    generate_expression( f, decls, Pragma_listNIL, ix->val );
	    fputs( ",", f );
	    generate_expression( f, decls, Pragma_listNIL, ix->upperbound );
	    fputs( ")", f );
	    break;
	}

	case TAGExprIsLowerBoundViolated:
	{
	    const_ExprIsLowerBoundViolated ix = to_const_ExprIsLowerBoundViolated(x);

	    fputs( "VnusIsLowerBoundViolated(", f );
	    generate_expression( f, decls, Pragma_listNIL, ix->val );
	    fputs( ")", f );
	    break;
	}

	case TAGExprBinop:
	    generate_binary_operation(
		f,
		decls,
		to_const_ExprBinop(x)->left,
		to_const_ExprBinop(x)->optor,
		to_const_ExprBinop(x)->right
	    );
	    break;

	case TAGExprReduction:
	    generate_reduction_operation(
		f,
		decls,
		to_const_ExprReduction(x)->optor,
		to_const_ExprReduction(x)->operands
	    );
	    break;

	case TAGExprIf:
	    fputs( "(", f );
	    generate_expression( f, decls, Pragma_listNIL, to_const_ExprIf(x)->cond );
	    fputs( "?", f );
	    generate_expression( f, decls, Pragma_listNIL, to_const_ExprIf(x)->thenval );
	    fputs( ":", f );
	    generate_expression( f, decls, Pragma_listNIL, to_const_ExprIf(x)->elseval );
	    fputs( ")", f );
	    break;

	case TAGExprWhere:
	    internal_error( "where expression should have been rewritten" );
	    break;

	case TAGExprName:
	    fprintf( f, "%s", to_const_ExprName(x)->name->sym->name );
	    break;

	case TAGExprWrapper:
	    generate_expression( f, decls, to_const_ExprWrapper(x)->pragmas, to_const_ExprWrapper(x)->x );
	    break;

	case TAGExprCast:
	{
	    const type oldtype = derive_type_expression( decls, originNIL, to_const_ExprCast(x)->x );
	    tmstring tnm = type_string( decls, to_const_ExprCast(x)->t );
	    fprintf( f, "(%s) ", tnm );
	    rfre_tmstring( tnm );
	    if( is_complex_type( oldtype ) && !is_complex_type( to_const_ExprCast(x)->t ) ){
		fputs( "(", f );
		generate_expression( f, decls, Pragma_listNIL, to_const_ExprCast(x)->x );
		fputs( ").real()", f );
	    }
	    else {
		generate_expression( f, decls, Pragma_listNIL, to_const_ExprCast(x)->x );
	    }
	    rfre_type( oldtype );
	    break;
	}

	case TAGExprSizeof:
	{
	    tmstring tnm = type_string( decls, to_const_ExprSizeof(x)->t );
	    fprintf( f, "sizeof(%s) ", tnm );
	    rfre_tmstring( tnm );
	    break;
	}

	case TAGExprArray:
	    fputs( "{ ", f );
	    generate_expression_list( f, decls, pragmas, to_const_ExprArray(x)->elms );
	    fputs( " }", f );
	    break;

	case TAGExprField:
	{
	    const_expression rec = skip_expression_wrappers( to_const_ExprField(x)->rec );
	    const type rectype = derive_type_expression( decls, to_const_ExprField(x)->field->orig, rec );
	    bool arrow = false;

	    if( rec->tag == TAGExprDeref ){
		arrow = true;
		rec = skip_expression_wrappers( to_const_ExprDeref(rec)->ref );
	    }
	    bool brac = (rec->tag != TAGExprName && rec->tag != TAGExprFunctionCall && rec->tag != TAGExprField );
	    if( brac ){
		fputs( "(", f );
	    }
	    generate_expression( f, decls, Pragma_listNIL, rec );
	    if( brac ){
		fputs( ")", f );
	    }
	    int pos = find_field( decls, rectype, to_const_ExprField(x)->field );
	    if( pos<0 ){
		pos = 0;
	    }
	    rfre_type( rectype );
	    if( arrow ){
		fputs( "->", f );
	    }
	    else {
		fputs( ".", f );
	    }
	    fprintf( f, "field%u", pos );
	    break;
	}

	case TAGExprFieldNumber:
	{
	    const_expression rec = skip_expression_wrappers( to_const_ExprFieldNumber(x)->rec );
	    bool arrow = false;

	    if( rec->tag == TAGExprDeref ){
		arrow = true;
		rec = skip_expression_wrappers( to_const_ExprDeref(rec)->ref );
	    }
	    bool brac = (rec->tag != TAGExprName && rec->tag != TAGExprFunctionCall && rec->tag != TAGExprFieldNumber );
	    if( brac ){
		fputs( "(", f );
	    }
	    generate_expression( f, decls, Pragma_listNIL, rec );
	    if( brac ){
		fputs( ")", f );
	    }
	    int pos = to_const_ExprFieldNumber(x)->field;
	    if( pos<0 ){
		pos = 0;
	    }
	    if( arrow ){
		fputs( "->", f );
	    }
	    else {
		fputs( ".", f );
	    }
	    fprintf( f, "field%u", pos );
	    break;
	}

	case TAGExprAddress:
	{
	    const_location adr = skip_location_wrappers( to_const_ExprAddress(x)->adr );
	    const bool brac = (adr->tag != TAGLocName && adr->tag != TAGLocField);

	    fputs( "&", f );
	    if( brac ){
		fputs( "(", f );
	    }
	    generate_location( f, decls, Pragma_listNIL, adr );
	    if( brac ){
		fputs( ")", f );
	    }
	    break;
	}

	case TAGExprDeref:
	    fputs( "(*", f );
	    generate_expression( f, decls, Pragma_listNIL, to_const_ExprDeref(x)->ref );
            fputs( ")", f );
	    break;

	case TAGExprFilledNew:
	{
	    const_type t = to_const_ExprFilledNew(x)->t;
	    expression init = to_const_ExprFilledNew(x)->init;
	    origsymbol gcmarker = to_const_ExprFilledNew(x)->gcmarker;

	    generate_filled_new( f, decls, t, init, gcmarker );
	    break;
	}

	case TAGExprNulledNew:
	{
	    const_type t = to_const_ExprNulledNew(x)->t;
	    origsymbol gcmarker = to_const_ExprNulledNew(x)->gcmarker;

	    generate_nulled_new( f, decls, t, gcmarker );
	    break;
	}

	case TAGExprNew:
	{
	    const_type t = to_const_ExprNew(x)->t;

	    tmstring tnm = type_string( decls, t );
	    fprintf( f, "new %s", tnm );
	    rfre_tmstring( tnm );
	    generate_new_dimensionlist( f, decls, t );
	    break;
	}

	case TAGExprNewArray:
	{
	    const_ExprNewArray nx = to_const_ExprNewArray(x);
	    const_type t = nx->t;

	    tmstring tnm = type_string( decls, t );
	    if( nx->init->tag == TAGOptExpr ){
		fprintf( f, "VnusAllocateFilledArray<%s>(", tnm );
		generate_expression( f, decls, Pragma_listNIL, to_const_OptExpr(nx->init)->x );
		fputs( ", ", f );
		generate_expression( f, decls, Pragma_listNIL, nx->length );
		fputs( ")", f );
	    }
	    else {
		// The NULL parameter is a dummy to keep a dumb TriMedia compiler happy.
		fprintf( f, "VnusAllocateArray<%s>(", tnm );
		generate_expression( f, decls, Pragma_listNIL, nx->length );
		fputs( ",NULL)", f );
	    }
	    rfre_tmstring( tnm );
	    break;
	}

	case TAGExprNewRecord:
	{
	    const_type t = to_const_ExprNewRecord(x)->t;
	    const_expression_list fields = to_const_ExprNewRecord(x)->fields;
	    generate_new_record( f, decls, t, fields );
	    break;
	}

	case TAGExprIsRaised:
	    internal_error( "israised should have been rewritten" );
	    break;

	case TAGExprNotNullAssert:
	    fputs( "VnusNotNullAssert(", f );
	    generate_expression( f, decls, Pragma_listNIL, to_const_ExprNotNullAssert(x)->x );
	    fputs( ")", f );
	    break;

	case TAGExprIsNewOwner:
	case TAGExprIsNewMaster:
	case TAGExprGetNewOwner:
	case TAGExprGetNewMaster:
	    internal_error( "Is/GetNewOwner/Master should not have reached vnus backend" );
	    break;
    }
}

static void generate_expression_list(
 FILE *f,
 const_declaration_list decls,
 const_Pragma_list pragmas,
 const_expression_list xl
)
{
    (void) pragmas;
    for( unsigned int ix=0; ix<xl->sz; ix++ ){
	if( ix>0 ){
	    fputs( ", ", f );
	}
	generate_expression( f, decls, Pragma_listNIL, xl->arr[ix] );
    }
}

static void generate_timing_begin( FILE *f )
{
    generate_indent( f, INDENTSTEP );
    fputs("struct timeval begin_time, end_time;\n",f);
    generate_indent( f, INDENTSTEP );
    fputs("long duration_sec, duration_usec;\n",f);
    generate_indent( f, INDENTSTEP );
    fputs("char s[256];\n",f);
    generate_indent( f, INDENTSTEP );
    fputs("gettimeofday( &begin_time, NULL );\n",f);
}

static void generate_timing_end( FILE *f )
{
    generate_indent( f, INDENTSTEP );
    fputs("gettimeofday( &end_time, NULL );\n",f);
    generate_indent( f, INDENTSTEP );
    fputs("duration_sec = end_time.tv_sec - begin_time.tv_sec;\n",f);
    generate_indent( f, INDENTSTEP );
    fputs("duration_usec = end_time.tv_usec - begin_time.tv_usec;\n",f);
    generate_indent( f, INDENTSTEP );
    fputs("if( duration_usec<0 ){\n",f);
    generate_indent( f, 2*INDENTSTEP );
    fputs("duration_usec += 1000000;\n",f);
    generate_indent( f, 2*INDENTSTEP );
    fputs("duration_sec--;\n",f);
    generate_indent( f, INDENTSTEP );
    fputs("}\n\n",f);

    generate_indent( f, INDENTSTEP );
    fputs("struct utsname *buf = new struct utsname;\n",f);
    generate_indent( f, INDENTSTEP );
    fputs("int result = uname( buf );\n",f);
    generate_indent( f, INDENTSTEP );
    fputs("if( result != 0 ){\n",f);
    generate_indent( f, 2*INDENTSTEP );
    fputs("sprintf( s, \"call to uname() failed: %d\\n\", errno);\n",f);
    generate_indent_line( f, INDENTSTEP, "}" );
    generate_indent( f, INDENTSTEP );
    fputs("else {\n",f);
    generate_indent( f, 2*INDENTSTEP );
    if( program_type == PROG_DISTRIBUTED )
        fputs("sprintf( s, \"(%s, %d): Duration = %ld.%06ld sec\\n\", buf->nodename, p, duration_sec, duration_usec);\n",f);
    else
        fputs("sprintf( s, \"(%s): Duration = %ld.%06ld sec\\n\", buf->nodename, duration_sec, duration_usec );\n",f);
    generate_indent_line( f, INDENTSTEP, "}" );
    if( program_type == PROG_DISTRIBUTED ){
        generate_indent_line( f, INDENTSTEP, "vnus_send( 0, 256, s );" );
        generate_indent_line( f, INDENTSTEP, "if( p==0 ){" );
        generate_indent_line( f, 2*INDENTSTEP, "for( int i=0; i<numberOfProcessors; i++ ){" );
        generate_indent_line( f, 3*INDENTSTEP, "vnus_receive( i, 256, s );" );
        generate_indent_line( f, 3*INDENTSTEP, "fprintf( stderr, s );" );
        generate_indent_line( f, 2*INDENTSTEP, "}" );
        generate_indent_line( f, INDENTSTEP, "}" );
        generate_indent_line( f, INDENTSTEP, "vnus_wait_pending();" );
    }
    else {
        generate_indent_line( f, INDENTSTEP, "fprintf( stderr, s );\n" );
    }
}

// Given an expression, return true iff we know it will never change.
// This means that it is only contains constants and final variables.
static bool is_unchanging_expression( const_declaration_list decls, const_expression x )
{
    // TODO: be more sophisticated than this. Also allow <final var>+1
    if( x->tag == TAGExprName ){
	const_ExprName nx = to_const_ExprName( x );
	const_declaration d = lookup_declaration( decls, nx->name );
	return has_any_flag( d->flags, MOD_FINAL );
    }
    return is_constant_expression( x );
}

static secondary breakout_dangerous_secondary(
 FILE *f,
 const_declaration_list decls,
 unsigned int indent,
 secondary sec,
 bool *did_breakout
)
{
    if( !is_unchanging_expression( decls, sec->lowerbound ) ){
	tmsymbol lowerbound = gen_tmsymbol( "lowerbound" );
	generate_indent( f, indent );
	fprintf( f, "const VnusInt %s = ", lowerbound->name );
	generate_expression( f, decls, Pragma_listNIL, sec->lowerbound );
	fputs( ";\n", f );
	*did_breakout = true;
	rfre_expression( sec->lowerbound );
	sec->lowerbound = new_ExprName(
	    new_origsymbol( lowerbound, gen_origin() )
	);
    }
    if( !is_unchanging_expression( decls, sec->stride ) ){
	tmsymbol stride = gen_tmsymbol( "stride" );
	generate_indent( f, indent );
	fprintf( f, "const VnusInt %s = ", stride->name );
	generate_expression( f, decls, Pragma_listNIL, sec->stride );
	fputs( ";\n", f );
	*did_breakout = true;
	rfre_expression( sec->stride );
	sec->stride = new_ExprName( new_origsymbol( stride, gen_origin() ) );
    }
    return sec;
}

static secondary_list breakout_dangerous_secondaries(
 FILE *f,
 const_declaration_list decls,
 unsigned int indent,
 secondary_list secs,
 bool *did_breakout
)
{
    if (secs == secondary_listNIL)
	return secs;
    
    for( unsigned int ix=0; ix<secs->sz; ix++ ){
	secs->arr[ix] = breakout_dangerous_secondary(
	    f,
	    decls,
	    indent,
	    secs->arr[ix],
	    did_breakout
	);
    }
    return secs;
}

// Given a filehandle 'f', a list of declaration 'decls', an indent
// 'indent', a list of cardinalities 'cards', an index in this cardinality
// list 'cardno', and a body 'body', generate a loopnest for the
// cardinalities from 'cardno' to the end of 'cards'.
// This may involve recursive invocation of this function
static void generate_loopnest(
 FILE *f,
 const_declaration_list decls,
 unsigned int indent,
 const_cardinality_list cards,
 unsigned int cardno,
 const_block body
)
{
    if( cardno<cards->sz ){
	const cardinality c = cards->arr[cardno];
	tmsymbol upperbound = tmsymbolNIL;
	tmsymbol lowerbound = tmsymbolNIL;
	tmsymbol stride = tmsymbolNIL;
	bool did_breakout = false;
	tmsymbol s = c->name->sym;
	secondary_list seconds = c->secondaries;

	generate_indent_line( f, indent, "{" );
	indent += INDENTSTEP;
	seconds = breakout_dangerous_secondaries( f, decls, indent, seconds, &did_breakout );
	if( !is_unchanging_expression( decls, c->lowerbound ) ){
	    lowerbound = gen_tmsymbol( "lowerbound" );
	    generate_indent( f, indent );
	    fprintf( f, "const VnusInt %s = ", lowerbound->name );
	    generate_expression( f, decls, Pragma_listNIL, c->lowerbound );
	    fputs( ";\n", f );
	    did_breakout = true;
	}
	if( !is_unchanging_expression( decls, c->upperbound ) ){
	    upperbound = gen_tmsymbol( "upperbound" );
	    generate_indent( f, indent );
	    fprintf( f, "const VnusInt %s = ", upperbound->name );
	    generate_expression( f, decls, Pragma_listNIL, c->upperbound );
	    fputs( ";\n", f );
	    did_breakout = true;
	}
	if( !is_unchanging_expression( decls, c->stride ) ){
	    stride = gen_tmsymbol( "stride" );
	    generate_indent( f, indent );
	    fprintf( f, "const VnusInt %s = ", stride->name );
	    generate_expression( f, decls, Pragma_listNIL, c->stride );
	    fputs( ";\n", f );
	    did_breakout = true;
	}
	if( did_breakout ){
	    fputs( "\n", f );
	}
	generate_indent( f, indent );
	fprintf( f, "for( VnusInt %s=", s->name );
	if( lowerbound != tmsymbolNIL ){
	    generate_tmsymbol( f, lowerbound );
	}
	else {
	    generate_expression( f, decls, Pragma_listNIL, c->lowerbound );
	}
	if (seconds != secondary_listNIL){
	    for( unsigned int ix=0; ix<seconds->sz; ix++ ){
		const_secondary sec = seconds->arr[ix];
		fprintf( f, ", %s=", sec->name->sym->name );
		generate_expression( f, decls, Pragma_listNIL, sec->lowerbound );
	    }
	}
	fprintf( f, "; %s<", s->name );
	if( upperbound != tmsymbolNIL ){
	    generate_tmsymbol( f, upperbound );
	}
	else {
	    generate_expression( f, decls, Pragma_listNIL, c->upperbound );
	}
	if( stride != tmsymbolNIL ){
	    fprintf( f, "; %s+=", s->name );
	    generate_tmsymbol( f, stride );
	}
	else {
	    if( c->stride->tag == TAGExprInt && to_const_ExprInt(c->stride)->v == 1 ){
		fprintf( f, "; ++%s", s->name );
	    }
	    else {
		fprintf( f, "; %s+=", s->name );
		generate_expression( f, decls, Pragma_listNIL, c->stride );
	    }
	}
	if (seconds != secondary_listNIL)
	{
	    for( unsigned int ix=0; ix<seconds->sz; ix++ ){
		secondary sec = seconds->arr[ix];
		fprintf( f, ", %s+=", sec->name->sym->name );
		generate_expression( f, decls, Pragma_listNIL, sec->stride );
	    }
	}
	fprintf( f, " )\n" );
	generate_loopnest( f, decls, indent, cards, cardno+1, body );
	indent -= INDENTSTEP;
	generate_indent_line( f, indent, "}" );
    }
    else {
	generate_bracket_block( f, decls, indent, body );
    }
}

static void generate_for(
 FILE *f,
 const_declaration_list decls,
 unsigned int indent,
 const_cardinality_list cards,
 const_block body
)
{
    generate_loopnest( f, decls, indent, cards, 0, body );
}

static void generate_foreach(
 FILE *f,
 const_declaration_list decls,
 unsigned int indent,
 const_cardinality_list cards,
 const_block body
)
{
    generate_for( f, decls, indent, cards, body );
}

static void generate_forall(
 FILE *f,
 const_declaration_list decls,
 unsigned int indent,
 const_cardinality_list cards,
 const_block body
)
{
    generate_loopnest( f, decls, indent, cards, 0, body );
}

static void generate_each(
 FILE *f,
 const_declaration_list decls,
 unsigned int indent,
 const_statement_list statements
)
{
    generate_statements( f, decls, indent, statements );
}

static void generate_fork(
 FILE *f,
 const_declaration_list decls,
 unsigned int indent,
 const_statement_list statements
)
{
    generate_statements( f, decls, indent, statements );
}

// Given a list of wait cases, return the number of timeout cases in the list.
static int count_timeout_cases( const_waitCase_list cases )
{
    int res = 0;

    for( unsigned int ix=0; ix<cases->sz; ix++ ){
	if( cases->arr[ix]->tag == TAGWaitCaseTimeout ){
	    res++;
	}
    }
    return res;
}

static void generate_wait_statement(
 FILE *f,
 const_declaration_list decls,
 unsigned int indent,
 const_SmtWait smt
)
{
    const_waitCase_list cases = smt->cases;
    tmsymbol readfds = gen_tmsymbol( "readfds" );
    tmsymbol maxfd = gen_tmsymbol( "maxfd" );
    tmsymbol timeout = gen_tmsymbol( "timeout" );
    tmsymbol mintimeout = gen_tmsymbol( "mintimeout" );
    tmsymbol minno = gen_tmsymbol( "minno" );
    tmsymbol selectres = gen_tmsymbol( "res" );
    int timeout_cases = count_timeout_cases( cases );

    generate_indent_line( f, indent, "{" );

    // readfds declaration
    generate_indent( f, indent+INDENTSTEP );
    fputs( "fd_set ", f );
    generate_tmsymbol( f, readfds );
    fputs( ";\n", f );

    // maxfd declaration
    generate_indent( f, indent+INDENTSTEP );
    fputs( "int ", f );
    generate_tmsymbol( f, maxfd );
    fputs( " = -1;\n", f );

    if( timeout_cases>0 ){
	// timeout declaration
	generate_indent( f, indent+INDENTSTEP );
	fputs( "struct timeval ", f );
	generate_tmsymbol( f, timeout );
	fputs( ";\n", f );

	// mintimeout declaration
	generate_indent( f, indent+INDENTSTEP );
	fputs( "VnusLong ", f );
	generate_tmsymbol( f, mintimeout );
	fputs( " = -1;\n", f );
    }
    if( timeout_cases>1 ){

	// minno declaration
	generate_indent( f, indent+INDENTSTEP );
	fputs( "int ", f );
	generate_tmsymbol( f, minno );
	fputs( ";\n", f );
    }

    // Declaration/statement separator
    fputs( "\n", f );

    // Zero the set.
    generate_indent( f, indent+INDENTSTEP );
    fputs( "FD_ZERO( &", f );
    generate_tmsymbol( f, readfds );
    fputs( " );\n", f );

    // Fill the fdset, and calculate the minimum timeout.
    for( unsigned int ix=0; ix<cases->sz; ix++ ){
	const_waitCase cs = cases->arr[ix];

	switch( cs->tag ){
	    case TAGWaitCaseValue:
	    {
		const_WaitCaseValue wcv = to_const_WaitCaseValue( cs );

		// Generate statement to add this fd to the set.
		generate_indent( f, indent+INDENTSTEP );
		fputs( "FD_SET( ", f );
		generate_expression( f, decls, Pragma_listNIL, wcv->cond );
		fputs( ", &", f );
		generate_tmsymbol( f, readfds );
		fputs( " );\n", f );

		// Generate statement to maintain maxfd
		generate_indent( f, indent+INDENTSTEP );
		fputs( "if( ", f );
		generate_tmsymbol( f, maxfd );
		fputs( "<", f );
		generate_expression( f, decls, Pragma_listNIL, wcv->cond );
		fputs( " ) ", f );
		generate_tmsymbol( f, maxfd );
		fputs( " = ", f );
		generate_expression( f, decls, Pragma_listNIL, wcv->cond );
		fputs( ";\n", f );
		break;
	    }

	    case TAGWaitCaseTimeout:
	    {
		const_WaitCaseTimeout wct = to_const_WaitCaseTimeout( cs );
	        if( timeout_cases>1 ){
		    generate_indent( f, indent+INDENTSTEP );
		    fputs( "if( ", f );
		    generate_tmsymbol( f, mintimeout );
		    fputs( ">", f );
		    generate_expression( f, decls, Pragma_listNIL, wct->time );
		    fputs( " ){\n", f );
		    generate_indent( f, indent+2*INDENTSTEP );
		    generate_tmsymbol( f, mintimeout );
		    fputs( " = ", f );
		    generate_expression( f, decls, Pragma_listNIL, wct->time );
		    fputs( ";\n", f );
		    generate_indent( f, indent+2*INDENTSTEP );
		    generate_tmsymbol( f, minno );
		    fprintf( f, " = %u;\n", ix );
		    generate_indent_line( f, indent+INDENTSTEP, "}" );
		}
		else {
		    generate_indent( f, indent+INDENTSTEP );
		    generate_tmsymbol( f, mintimeout );
		    fputs( " = ", f );
		    generate_expression( f, decls, Pragma_listNIL, wct->time );
		    fputs( ";\n", f );
		}
		break;
	    }
	}

    }

    // Fill timeout variable.
    if( timeout_cases>0 ){
	generate_indent( f, indent+INDENTSTEP );
	generate_tmsymbol( f, timeout );
	fputs( ".tv_sec = ", f );
	generate_tmsymbol( f, mintimeout );
	fputs( "/1000000;\n", f );
	generate_indent( f, indent+INDENTSTEP );
	generate_tmsymbol( f, timeout );
	fputs( ".tv_usec = ", f );
	generate_tmsymbol( f, mintimeout );
	fputs( "%1000000;\n", f );
    }
   

    // select call
    generate_indent( f, indent+INDENTSTEP );
    fputs( "int ", f );
    generate_tmsymbol( f, selectres );
    fputs( " = select( ", f );
    generate_tmsymbol( f, maxfd );
    fputs( "+1, &", f );
    generate_tmsymbol( f, readfds );
    fputs( ", NULL, NULL, &", f );
    generate_tmsymbol( f, timeout );
    fputs( " );\n", f );

    for( unsigned int ix=0; ix<cases->sz; ix++ ){
	const_waitCase cs = cases->arr[ix];

	switch( cs->tag ){
	    case TAGWaitCaseValue:
	    {
		const_WaitCaseValue wcv = to_const_WaitCaseValue( cs );

		generate_indent( f, indent+INDENTSTEP );
		fputs( "if( FD_ISSET( ", f );
		generate_expression( f, decls, Pragma_listNIL, wcv->cond );
		fputs( ", & ", f );
		generate_tmsymbol( f, readfds );
		fputs( " ) )\n", f );

		// TODO: generate assignment to val.
		generate_bracket_block( f, decls, indent+INDENTSTEP, wcv->body );
		break;
	    }

	    case TAGWaitCaseTimeout:
	    {
		const_WaitCaseTimeout wct = to_const_WaitCaseTimeout( cs );

		generate_indent( f, indent+INDENTSTEP );
		fputs( "if( ", f );
		generate_tmsymbol( f, selectres );
		fputs( " == 0", f );
		if( timeout_cases>1 ){
		    fputs( " && ", f );
		    generate_tmsymbol( f, minno );
		    fprintf( f, " == %u", ix );
		}
		fputs( " )\n", f );
		generate_bracket_block( f, decls, indent+INDENTSTEP, wct->body );
		break;
	    }
	}

    }
    generate_indent_line( f, indent, "}" );
}

static const char *basetype_name[] =
{
    "Byte",	/* BT_BYTE */
    "Short",	/* BT_SHORT */
    "Int",	/* BT_INT */
    "Long",	/* BT_LONG */
    "Float",	/* BT_FLOAT */
    "Double",	/* BT_DOUBLE */
    "Complex",	/* BT_COMPLEX */
    "Char",	/* BT_CHAR */
    "String",	/* BT_STRING */
    "Boolean",	/* BT_BOOLEAN */
    "Expr"	/* BT_ABSTRACTED */
};

static void generate_print_typed_item(
 FILE *f,
 const unsigned int indent,
 const_declaration_list decls,
 const_type t,
 const_expression arg,
 const_expression fhnd
)
{
    if( t->tag == TAGTypePragmas ){
	generate_print_typed_item( f, indent, decls, to_const_TypePragmas(t)->t, arg, fhnd );
	return;
    }
    if( t->tag == TAGTypeMap ){
	generate_print_typed_item( f, indent, decls, to_const_TypeMap(t)->t, arg, fhnd );
	return;
    }
    generate_indent( f, indent );
    switch( t->tag ){
        case TAGTypeBase:
	    fprintf( f, "v__write%s( ", basetype_name[(int) to_const_TypeBase(t)->base] );
	    generate_expression( f, decls, Pragma_listNIL, fhnd );
	    fputs( ", ", f );
	    generate_expression( f, decls, Pragma_listNIL, arg );
            break;

	case TAGTypeShape:
	    generate_expression( f, decls, Pragma_listNIL, arg );
	    fputs( ".PrintInfo( ", f );
	    generate_expression( f, decls, Pragma_listNIL, fhnd );
	    break;

	case TAGTypePointer:
	    fputs( "v__writeString( ", f );
	    generate_expression( f, decls, Pragma_listNIL, fhnd );
	    fputs( ", \"pointer\"", f );
	    break;

	case TAGTypeNamedRecord:
	    fputs( "v__writeString( ", f );
	    generate_expression( f, decls, Pragma_listNIL, fhnd );
	    fprintf( f, ", \"record %s\"", to_const_TypeNamedRecord(t)->name->sym->name );
	    break;

	case TAGTypeRecord:
	    fputs( "v__writeString( ", f );
	    generate_expression( f, decls, Pragma_listNIL, fhnd );
	    fputs( ", \"record\"", f );
	    break;

	case TAGTypeNeutralPointer:
	    fputs( "v__writeString( ", f );
	    generate_expression( f, decls, Pragma_listNIL, fhnd );
	    fputs( ", \"void pointer\"", f );
	    break;

	case TAGTypeUnsizedArray:
	    fputs( "v__writeString( ", f );
	    generate_expression( f, decls, Pragma_listNIL, fhnd );
	    fputs( ", \"array\"", f );
	    break;

	case TAGTypeArray:
	    fputs( "v__writeString( ", f );
	    generate_expression( f, decls, Pragma_listNIL, fhnd );
	    fputs( ", \"array\"", f );
	    break;

	case TAGTypeFunction:
	    fputs( "v__writeString( ", f );
	    generate_expression( f, decls, Pragma_listNIL, fhnd );
	    fputs( ", \"function\"", f );
	    break;

	case TAGTypeProcedure:
	    fputs( "v__writeString( ", f );
	    generate_expression( f, decls, Pragma_listNIL, fhnd );
	    fputs( ", \"procedure\"", f );
	    break;

	case TAGTypePragmas:
	case TAGTypeMap:
	    internal_error( "We should never reach this" );
	    break;

    }
    fputs( " );\n", f );
}

static void generate_print_item(
 FILE *f,
 const unsigned int indent,
 const_declaration_list decls,
 const_origin orig,
 const_expression arg,
 const_expression fhnd
)
{
    type t = derive_type_expression( decls, orig, arg );
    generate_print_typed_item( f, indent, decls, t, arg, fhnd );
    rfre_type( t );
}

static void generate_print(
 FILE *f,
 const_declaration_list decls,
 const_origin orig,
 const unsigned int indent,
 const_expression_list l
)
{
    unsigned int start=1;
    expression fhnd;

    if( l->sz<1 ){
        return;
    }
    fhnd = l->arr[0];
    for( unsigned int ix=start; ix<l->sz; ix++ ){
	generate_print_item(
	    f,
	    indent,
	    decls,
	    orig,
	    l->arr[ix],
	    fhnd
	);
    }
}

static void generate_assignment(
 FILE *f,
 const_declaration_list decls,
 const unsigned int indent,
 const_location lhs,
 const_expression rhs
)
{
    generate_indent( f, indent );
    generate_location( f, decls, Pragma_listNIL, lhs );
    fputs( " = ", f );
    generate_expression( f, decls, Pragma_listNIL, rhs );
    fputs( ";\n", f );
}

static void generate_assignment_op(
 FILE *f,
 const_declaration_list decls,
 const unsigned int indent,
 const_SmtAssignOp smt
)
{
    generate_indent( f, indent );
    generate_location( f, decls, Pragma_listNIL, smt->lhs );
    fputs( " ", f );
    const type ta = derive_type_location( decls, originNIL, smt->lhs );
    if( ta == typeNIL ){
	fputs( "= 0;", f );
	return;
    }
    if( smt->op == BINOP_USHIFTRIGHT ){
	tmstring tnm = type_string( decls, ta );

	fprintf( f, "= ((%s) (((Unsigned%s)", tnm, tnm );
	generate_location( f, decls, Pragma_listNIL, smt->lhs );
	fputs( ")>>", f );
	generate_expression( f, decls, Pragma_listNIL, smt->rhs );
	fputs( "))", f );
	rfre_tmstring( tnm );
    }
    else if(
	smt->op == BINOP_MOD &&
	(
	    is_this_basetype( ta, BT_FLOAT ) ||
	    is_this_basetype( ta, BT_DOUBLE )
	)
    ){
	// This is a '%' operator on float or double, so generate
	// <lhs> = fmod(<lhs>,<rhs>)
	fputs( "= fmod(", f );
	generate_location( f, decls, Pragma_listNIL, smt->lhs );
	fputs( ",", f );
	generate_expression( f, decls, Pragma_listNIL, smt->rhs );
	fputs( ")", f );
    }
    else if( smt->op == BINOP_SHORTAND || smt->op == BINOP_SHORTOR ){
	fputs( "= ", f );
	generate_location( f, decls, Pragma_listNIL, smt->lhs );
	fputs( binop_strings[(int) smt->op], f );
	generate_expression( f, decls, Pragma_listNIL, smt->rhs );
    }
    else {
	// There is nothing special to this operator or operand, so
	// just generate <rhs> <op>= <lhs>;
	fprintf( f, "%s= ", binop_strings[(int) smt->op] );
	generate_expression( f, decls, Pragma_listNIL, smt->rhs );
    }
    rfre_type( ta );
    fputs( ";\n", f );
}

static void generate_block(
 FILE *f,
 const_declaration_list decls,
 const unsigned int indent,
 const_block blk
)
{
    const tmsymbol scope = blk->scope;

    if( scope != tmsymbolNIL ){
	origsymbol_list locals;

	generate_indent( f, indent );
	fprintf( f, "// Scope: %s\n", scope->name );
	locals = get_scope_locals( scope );
	if( locals != origsymbol_listNIL ){
	    if( locals->sz>0 ){
		generate_local_declarations( f, decls, indent, locals );
		fprintf( f, "\n" );
	    }
	    rfre_origsymbol_list( locals );
	}
    }
    generate_statements( f, decls, indent, blk->statements );
}

static void generate_bracket_block(
 FILE *f,
 const_declaration_list decls,
 const unsigned int indent,
 const_block blk
)
{
    generate_indent_line( f, indent, "{" );
    generate_block( f, decls, indent+INDENTSTEP, blk );
    generate_indent_line( f, indent, "}" );
}

/* Given a list of switch cases, generate code for them. */
static void generate_switchCase(
 FILE *f,
 const_declaration_list decls,
 const unsigned int indent,
 const_switchCase c
)
{
    generate_indent( f, indent );
    switch( c->tag ){
	case TAGSwitchCaseValue:
	    fprintf( f, "case %d:\n", (int) to_const_SwitchCaseValue(c)->cond );
	    break;

	case TAGSwitchCaseDefault:
	    fputs( "default:\n", f );
	    break;

    }
    generate_block( f, decls, indent+INDENTSTEP, c->body );
    if( c->body->statements->sz == 0 ){
	generate_indent_line( f, indent+INDENTSTEP, ";" );
    }
    fputs( "\n", f );
}

/* Given a list of switch cases, generate code for them. */
static void generate_switchCase_list(
 FILE *f,
 const_declaration_list decls,
 const unsigned int indent,
 const_switchCase_list l
)
{
    for( unsigned int ix=0; ix<l->sz; ix++ ){
	generate_switchCase( f, decls, indent, l->arr[ix] );
    }
}

static void generate_aggr_decl(
    FILE *f,
    const_declaration_list decls,
    const unsigned int indent,
    tmsymbol var,
    const_expression_list l
)
{
    generate_indent( f, indent );
    fprintf( f, "const VnusInt %s[] = { ", var->name );
    for( unsigned int ix=0; ix<l->sz; ix++ ){
	if( ix != 0 ){
	    fputs( ", ", f );
	}
	generate_expression( f, decls, Pragma_listNIL, l->arr[ix] );
    }
    fputs( " };\n", f );
}

static void generate_statement(
 FILE *f,
 const_declaration_list decls,
 const unsigned int indent,
 const_statement act
)
{
    const_origin orig = act->orig;

    if( act->label != origsymbolNIL ){
	generate_origsymbol( f, act->label );
	fputs( ":\n", f );
    }
    switch( act->tag ){
        case TAGSmtIf:
	{
	    const char *role = get_string_pragma( NULL, act->pragmas, "role" );
	    bool unlikely = role != NULL && (
		(strcmp( role, "static-init" ) == 0) ||
		(strcmp( role, "null-check" ) == 0) ||
		(strcmp( role, "bound-check" ) == 0)
	    );

	    generate_indent( f, indent );
	    fputs( "if( ", f );
	    if( unlikely ){
		fputs( "VNUS_UNLIKELY( ", f );
	    }
	    generate_expression( f, decls, Pragma_listNIL, to_const_SmtIf(act)->cond );
	    if( unlikely ){
		fputs( " )", f );
	    }
	    fputs( " )\n", f );
	    generate_bracket_block( f, decls, indent, to_const_SmtIf(act)->thenbody );
	    if( to_const_SmtIf(act)->elsebody->statements->sz != 0 ){
		generate_indent_line( f, indent, "else" );
		generate_bracket_block( f, decls, indent, to_const_SmtIf(act)->elsebody );
	    }
	    break;
	}

        case TAGSmtWhile:
	    generate_indent( f, indent );
	    fputs( "while( ", f );
	    generate_expression( f, decls, Pragma_listNIL, to_const_SmtWhile(act)->cond );
	    fputs( " )\n", f );
	    generate_bracket_block( f, decls, indent, to_const_SmtWhile(act)->body );
	    break;

        case TAGSmtDoWhile:
	    generate_indent_line( f, indent, "do" );
	    generate_bracket_block( f, decls, indent, to_const_SmtDoWhile(act)->body );
	    generate_indent( f, indent );
	    fputs( "while( ", f );
	    generate_expression( f, decls, Pragma_listNIL, to_const_SmtDoWhile(act)->cond );
	    fputs( " );\n", f );
	    break;

        case TAGSmtFor:
	    generate_for( f, decls, indent, to_const_SmtFor(act)->cards, to_const_SmtFor(act)->body );
	    break;

        case TAGSmtForall:
	    generate_forall(
		f,
		decls,
		indent,
		to_const_SmtForall(act)->cards,
		to_const_SmtForall(act)->body
	    );
	    break;

        case TAGSmtForkall:
	    /* forkallcard:cardinality body:[statement] | */
	    generate_indent_line( f, indent, "/* Forkall statement */" );
	    break;

        case TAGSmtForeach:
	    generate_foreach( f, decls, indent, to_const_SmtForeach(act)->cards, to_const_SmtForeach(act)->body );
	    break;

	case TAGSmtEach:
	    generate_each( f, decls, indent, to_const_SmtEach(act)->statements );
	    break;

	case TAGSmtBlock:
	    generate_bracket_block( f, decls, indent, to_const_SmtBlock(act)->body );
	    break;

	case TAGSmtPseudoBlock:
	    generate_block( f, decls, indent, to_const_SmtPseudoBlock(act)->body );
	    break;

	case TAGSmtSend:
	    generate_indent( f, indent );
	    fputs( "vnus_send( ", f );
	    generate_expression( f, decls, Pragma_listNIL, to_const_SmtSend(act)->dest );
	    fputs( ", sizeof( ", f );
	    generate_expression( f, decls, Pragma_listNIL, to_const_SmtSend(act)->elm );
	    fputs( " ), &", f );
	    generate_expression( f, decls, Pragma_listNIL, to_const_SmtSend(act)->elm );
	    fputs( " );\n", f );
	    break;

	case TAGSmtReceive:
	    generate_indent( f, indent );
	    fputs( "vnus_receive( ", f );
	    generate_expression( f, decls, Pragma_listNIL, to_const_SmtReceive(act)->src );
	    fputs( ", sizeof( ", f );
	    generate_location( f, decls, Pragma_listNIL, to_const_SmtReceive(act)->elm );
	    fputs( " ), &", f );
	    generate_location( f, decls, Pragma_listNIL, to_const_SmtReceive(act)->elm );
	    fputs( " );\n", f );
	    break;

	case TAGSmtBlocksend:
	    generate_indent( f, indent );
	    fputs( "vnus_send( ", f );
	    generate_expression( f, decls, Pragma_listNIL, to_const_SmtBlocksend(act)->dest );
	    fputs( ", ", f );
	    generate_expression( f, decls, Pragma_listNIL, to_const_SmtBlocksend(act)->nelm );
	    fputs( "* sizeof( ", f );
	    generate_expression( f, decls, Pragma_listNIL, to_const_SmtBlocksend(act)->buf );
//	    fputs( ".Getbuf()[0] ), ", f );
	    fputs( "[0] ), ", f );
	    generate_expression( f, decls, Pragma_listNIL, to_const_SmtBlocksend(act)->buf );
//	    fputs( ".Getbuf() );\n", f );
	    fputs( " );\n", f );
	    break;

	case TAGSmtBlockreceive:
	    generate_indent( f, indent );
	    fputs( "vnus_receive( ", f );
	    generate_expression( f, decls, Pragma_listNIL, to_const_SmtBlockreceive(act)->src );
	    fputs( ", ", f );
	    generate_expression( f, decls, Pragma_listNIL, to_const_SmtBlockreceive(act)->nelm );
	    fputs( "* sizeof( ", f );
	    generate_expression( f, decls, Pragma_listNIL, to_const_SmtBlockreceive(act)->buf );
//	    fputs( ".Getbuf()[0] ), ", f );
	    fputs( "[0] ), ", f );
	    generate_expression( f, decls, Pragma_listNIL, to_const_SmtBlockreceive(act)->buf );
//	    fputs( ".Getbuf() );\n", f );
	    fputs( " );\n", f );
	    break;

	case TAGSmtASend:
	    generate_indent( f, indent );
	    fputs( "vnus_asend( ", f );
	    generate_expression( f, decls, Pragma_listNIL, to_const_SmtASend(act)->dest );
	    fputs( ", sizeof( ", f );
	    generate_expression( f, decls, Pragma_listNIL, to_const_SmtASend(act)->elm );
	    fputs( " ), &", f );
	    generate_expression( f, decls, Pragma_listNIL, to_const_SmtASend(act)->elm );
	    fputs( " );\n", f );
	    break;

	case TAGSmtAReceive:
	    generate_indent( f, indent );
	    fputs( "vnus_areceive( ", f );
	    generate_expression( f, decls, Pragma_listNIL, to_const_SmtAReceive(act)->src );
	    fputs( ", sizeof( ", f );
	    generate_location( f, decls, Pragma_listNIL, to_const_SmtAReceive(act)->elm );
	    fputs( " ), &", f );
	    generate_location( f, decls, Pragma_listNIL, to_const_SmtAReceive(act)->elm );
	    fputs( " );\n", f );
	    break;

	case TAGSmtABlocksend:
	    generate_indent( f, indent );
	    fputs( "vnus_asend( ", f );
	    generate_expression( f, decls, Pragma_listNIL, to_const_SmtABlocksend(act)->dest );
	    fputs( ", ", f );
	    generate_expression( f, decls, Pragma_listNIL, to_const_SmtABlocksend(act)->nelm );
	    fputs( "* sizeof( ", f );
	    generate_expression( f, decls, Pragma_listNIL, to_const_SmtABlocksend(act)->buf );
	    fputs( ".Getbuf()[0] ), ", f );
	    generate_expression( f, decls, Pragma_listNIL, to_const_SmtABlocksend(act)->buf );
	    fputs( ".Getbuf() );\n", f );
	    break;

	case TAGSmtABlockreceive:
	    generate_indent( f, indent );
	    fputs( "vnus_areceive( ", f );
	    generate_expression( f, decls, Pragma_listNIL, to_const_SmtABlockreceive(act)->src );
	    fputs( ", ", f );
	    generate_expression( f, decls, Pragma_listNIL, to_const_SmtABlockreceive(act)->nelm );
	    fputs( "* sizeof( ", f );
	    generate_expression( f, decls, Pragma_listNIL, to_const_SmtABlockreceive(act)->buf );
	    fputs( ".Getbuf()[0] ), ", f );
	    generate_expression( f, decls, Pragma_listNIL, to_const_SmtABlockreceive(act)->buf );
	    fputs( ".Getbuf() );\n", f );
	    break;

	case TAGSmtAggrSend:
	{
	    tmsymbol strides = gen_tmsymbol( "strides" );
	    tmsymbol stridecnts = gen_tmsymbol( "stridecnts" );
	    generate_aggr_decl( f, decls, indent, strides, to_const_SmtAggrSend(act)->strides );
	    generate_aggr_decl( f, decls, indent, stridecnts, to_const_SmtAggrSend(act)->stridecnts );
	    generate_indent( f, indent );
	    fputs( "emb_send( ", f );
	    generate_expression( f, decls, Pragma_listNIL, to_const_SmtAggrSend(act)->dest );
	    fputs( ", sizeof( ", f );
	    generate_expression( f, decls, Pragma_listNIL, to_const_SmtAggrSend(act)->buf );
	    fputs( ".Getbuf()[0] ), ", f );
	    generate_expression( f, decls, Pragma_listNIL, to_const_SmtAggrSend(act)->buf );
	    fputs( ".Getbuf(), ", f );
	    generate_expression( f, decls, Pragma_listNIL, to_const_SmtAggrSend(act)->offset );
	    fputs( ", ", f );
	    generate_expression( f, decls, Pragma_listNIL, to_const_SmtAggrSend(act)->nstrides );
	    fprintf(f, ", %s, %s, ", strides->name, stridecnts->name);
	    generate_expression( f, decls, Pragma_listNIL, to_const_SmtAggrSend(act)->exclusive );
	    fputs( " );\n", f );
	    break;
	}
	
	case TAGSmtAggrReceive:
	{
	    tmsymbol strides = gen_tmsymbol( "strides" );
	    tmsymbol stridecnts = gen_tmsymbol( "stridecnts" );
	    generate_aggr_decl( f, decls, indent, strides, to_const_SmtAggrReceive(act)->strides );
	    generate_aggr_decl( f, decls, indent, stridecnts, to_const_SmtAggrReceive(act)->stridecnts );
	    generate_indent( f, indent );
	    fputs( "emb_recv( ", f );
	    generate_expression( f, decls, Pragma_listNIL, to_const_SmtAggrReceive(act)->src );
	    fputs( ", sizeof( ", f );
	    generate_expression( f, decls, Pragma_listNIL, to_const_SmtAggrReceive(act)->buf );
	    fputs( ".Getbuf()[0] ), ", f );
	    generate_expression( f, decls, Pragma_listNIL, to_const_SmtAggrReceive(act)->buf );
	    fputs( ".Getbuf(), ", f );
	    generate_expression( f, decls, Pragma_listNIL, to_const_SmtAggrReceive(act)->offset );
	    fputs( ", ", f );
	    generate_expression( f, decls, Pragma_listNIL, to_const_SmtAggrReceive(act)->nstrides );
	    fprintf(f, ", %s, %s, ", strides->name, stridecnts->name);
	    generate_expression( f, decls, Pragma_listNIL, to_const_SmtAggrReceive(act)->exclusive );
	    fputs( " );\n", f );
	    break;
	}
	    
	case TAGSmtEmpty:
	    generate_indent_line( f, indent, ";" );
	    break;

	case TAGSmtBarrier:
	    generate_indent_line( f, indent, "vnus_barrier();" );
	    break;

	case TAGSmtWaitPending:
	    generate_indent_line( f, indent, "vnus_wait_pending();" );
	    break;

        case TAGSmtAssign:
	    generate_assignment( f, decls, indent, to_const_SmtAssign(act)->lhs, to_const_SmtAssign(act)->rhs );
	    break;

        case TAGSmtAssignOp:
	    generate_assignment_op( f, decls, indent, to_const_SmtAssignOp(act) );
	    break;

        case TAGSmtPrint:
            generate_print( f, decls, orig, indent, to_const_SmtPrint(act)->argv );
            break;

        case TAGSmtPrintLn:
	{
	    expression newln = new_ExprString( new_tmstring( "\\n" ) );
	    expression_list fake_argv = rdup_expression_list(
		to_const_SmtPrintLn(act)->argv
	    );

	    fake_argv = append_expression_list( fake_argv, newln );
            generate_print( f, decls, orig, indent, fake_argv );
	    rfre_expression_list( fake_argv );
            break;
	}

        case TAGSmtProcedureCall:
	    generate_indent( f, indent );
	    generate_expression( f, decls, Pragma_listNIL, to_const_SmtProcedureCall(act)->proc );
	    generate_actualparameter_list( f, decls, to_const_SmtProcedureCall(act)->parameters );
	    fputs( ";\n", f );
	    break;

        case TAGSmtExpression:
	    generate_indent( f, indent );
	    fputs( "(void) ", f );
	    generate_expression( f, decls, Pragma_listNIL, to_const_SmtExpression(act)->x );
	    fputs( ";\n", f );
	    break;

	case TAGSmtSwitch:
	    generate_indent( f, indent );
	    fputs( "switch( ", f );
	    generate_expression( f, decls, Pragma_listNIL, to_const_SmtSwitch(act)->cond );
	    fputs( " ){\n", f );
	    generate_switchCase_list( f, decls, indent+INDENTSTEP, to_const_SmtSwitch(act)->cases );
	    generate_indent_line( f, indent, "}" );
	    break;

	case TAGSmtReturn:
	    generate_indent_line( f, indent, "return;" );
	    break;

	case TAGSmtValueReturn:
	    generate_indent( f, indent );
	    fputs( "return ", f );
	    generate_expression( f, decls, Pragma_listNIL, to_const_SmtValueReturn(act)->v );
	    fputs( ";\n", f );
	    break;

	case TAGSmtGoto:
	    generate_indent( f, indent );
	    fputs( "goto ", f );
	    generate_origsymbol( f, to_const_SmtGoto(act)->target );
	    fputs( ";\n", f );
	    break;

	case TAGSmtThrow:
	    generate_indent( f, indent );
	    fputs( "throw ", f );
	    generate_expression( f, decls, Pragma_listNIL, to_const_SmtThrow(act)->elm );
	    fputs( ";\n", f );
	    break;

	case TAGSmtRethrow:
	    generate_indent_line( f, indent, "throw;" );
	    break;

	case TAGSmtCatch:
	    generate_indent_line( f, indent, "try" );
	    generate_bracket_block( f, decls, indent, to_const_SmtCatch(act)->body );
	    generate_indent( f, indent );
	    fputs( "catch( ", f );
	    generate_formalparameter( f, decls, to_const_SmtCatch(act)->elm );
	    fputs( " )\n", f );
	    generate_bracket_block( f, decls, indent, to_const_SmtCatch(act)->handler );
	    break;

	case TAGSmtDelete:
	    generate_indent( f, indent );
	    fputs( "delete ", f );
	    generate_expression( f, decls, Pragma_listNIL, to_const_SmtDelete(act)->adr );
	    fputs( ";\n", f );
	    break;

	case TAGSmtFork:
	    generate_fork( f, decls, indent, to_const_SmtFork(act)->statements );
	    break;

	case TAGSmtGarbageCollect:
	    generate_indent_line( f, indent, "/* garbagecollect(); */" );
	    break;

	case TAGSmtWait:
	    generate_wait_statement( f, decls, indent, to_const_SmtWait(act) );
	    break;

	case TAGSmtAddTask:
	    generate_indent( f, indent );
	    fputs( "add_task( ", f );
	    generate_expression( f, decls, Pragma_listNIL, to_const_SmtAddTask(act)->id );
	    fputs( ", ", f );
	    generate_expression( f, decls, Pragma_listNIL, to_const_SmtAddTask(act)->parms );
	    fputs( " );\n", f );
	    break;
	    
	case TAGSmtExecuteTasks:
	    generate_indent( f, indent );
	    fputs( "execute_tasks();\n", f );
	    break;
	    
	case TAGSmtRegisterTask:
	    generate_indent( f, indent );
	    fputs( "register_task( ", f );
	    generate_expression( f, decls, Pragma_listNIL, to_const_SmtRegisterTask(act)->id );
	    fputs( ", ", f );
	    fputs( "(void (*)(void*, void*))", f);
	    generate_expression( f, decls, Pragma_listNIL, to_const_SmtRegisterTask(act)->proc );
	    fputs( " );\n", f );
	    break;
	    
	case TAGSmtTSend:
	    generate_indent( f, indent );
	    fputs( "tsend( ", f );
	    generate_expression( f, decls, Pragma_listNIL, to_const_SmtTSend(act)->dest );
	    fputs( ", sizeof( ", f );
	    generate_expression( f, decls, Pragma_listNIL, to_const_SmtTSend(act)->elm );
	    fputs( " ), &", f );
	    generate_expression( f, decls, Pragma_listNIL, to_const_SmtTSend(act)->elm );
	    fputs( " );\n", f );
	    break;

	case TAGSmtTReceive:
	    generate_indent( f, indent );
	    fputs( "treceive( ", f );
	    generate_expression( f, decls, Pragma_listNIL, to_const_SmtTReceive(act)->src );
	    fputs( ", sizeof( ", f );
	    generate_location( f, decls, Pragma_listNIL, to_const_SmtTReceive(act)->elm );
	    fputs( " ), &", f );
	    generate_location( f, decls, Pragma_listNIL, to_const_SmtTReceive(act)->elm );
	    fputs( " );\n", f );
	    break;

    }
}

static void generate_statements(
 FILE *f,
 const_declaration_list decls,
 const unsigned int indent,
 const_statement_list smts
)
{
    for( unsigned int ix=0; ix<smts->sz; ix++ ){
	generate_statement( f, decls, indent, smts->arr[ix] );
    }
}

static void generate_forward_function(
 FILE *f,
 const_declaration_list decls,
 const_DeclFunction fn
)
{
    tmstring tnm = type_string( decls, fn->rettype );

    if( get_flag_pragma( Pragma_listNIL, fn->pragmas, "inline" ) ){
	fputs( "inline ", f );
    }
    fprintf( f ,"%s %s", tnm, fn->name->sym->name );
    generate_formalparameter_list( f, decls, fn->parms );
    fprintf( f, ";\n" );
    rfre_tmstring( tnm );
}

static void generate_function(
 FILE *f,
 const_declaration_list decls,
 const_DeclFunction fn
)
{
    tmstring tnm = type_string( decls, fn->rettype );

    fprintf( f ,"\n" );
    if( get_flag_pragma( Pragma_listNIL, fn->pragmas, "inline" ) ){
	fputs( "inline ", f );
    }
    fprintf( f ,"%s %s", tnm, fn->name->sym->name );
    generate_formalparameter_list( f, decls, fn->parms );
    fprintf( f, "\n" );
    fprintf( f, "{\n" );
    generate_block( f, decls, INDENTSTEP, fn->body );
    fprintf( f, "}\n" );
    rfre_tmstring( tnm );
}

static void generate_external_function(
 FILE *f,
 const_declaration_list decls,
 const_origsymbol name,
 const_origsymbol_list args,
 const_type rettype
)
{
    tmstring tnm = type_string( decls, rettype );

    fprintf( f ,"// extern %s %s", tnm, name->sym->name );
    generate_formalparameter_list( f, decls, args );
    fprintf( f, ";\n" );
    rfre_tmstring( tnm );
}

static void generate_forward_procedure(
 FILE *f,
 const_declaration_list decls,
 const_DeclProcedure proc
)
{
    if( get_flag_pragma( Pragma_listNIL, proc->pragmas, "inline" ) ){
	fputs( "inline ", f );
    }
    fprintf( f ,"void %s", proc->name->sym->name );
    generate_formalparameter_list( f, decls, proc->parms );
    fprintf( f, ";\n" );
}

static void generate_procedure(
 FILE *f,
 const_declaration_list decls,
 const_DeclProcedure proc
)
{
    fprintf( f ,"\n" );
    if( get_flag_pragma( Pragma_listNIL, proc->pragmas, "inline" ) ){
	fputs( "inline ", f );
    }
    fprintf( f ,"void %s", proc->name->sym->name );
    generate_formalparameter_list( f, decls, proc->parms );
    fprintf( f, "\n" );
    fprintf( f, "{\n" );
    generate_block( f, decls, INDENTSTEP, proc->body );
    fprintf( f, "}\n" );
}

static void generate_external_procedure(
 FILE *f,
 const_declaration_list decls,
 const_origsymbol name,
 const_origsymbol_list args
)
{
    fprintf( f ,"// extern " );
    fprintf( f ,"void %s", name->sym->name );
    generate_formalparameter_list( f, decls, args );
    fprintf( f, ";\n" );
}

static void generate_forward_declaration(
 FILE *f, 
 const_declaration_list decls,
 const_declaration decl
)
{
    const origsymbol dname = decl->name;

    switch( decl->tag ){
	case TAGDeclRecord:
	    break;

	case TAGDeclGlobalVariable:
	    generate_forward_variable_declaration(
		f,
		decls,
		0,
		dname,
		decl->flags,
		to_const_DeclGlobalVariable(decl)->t
	    );
	    break;

	case TAGDeclFunction:
	    generate_forward_function(
		f,
		decls,
		to_const_DeclFunction(decl)
	    );
	    break;

	case TAGDeclProcedure:
	    generate_forward_procedure(
		f,
		decls,
		to_const_DeclProcedure(decl)
	    );
	    break;

	case TAGDeclLocalVariable:
	case TAGDeclFormalVariable:
	case TAGDeclCardinalityVariable:
	    break;

	case TAGDeclExternalVariable:
	    generate_external_variable_declaration(
		f,
		decls,
		dname,
		decl->flags,
		to_const_DeclExternalVariable(decl)->t
	    );
	    break;

	case TAGDeclExternalFunction:
	    generate_external_function(
		f,
		decls,
		dname,
		to_const_DeclExternalFunction(decl)->parms,
		to_const_DeclExternalFunction(decl)->rettype
	    );
	    break;

	case TAGDeclExternalProcedure:
	    generate_external_procedure(
		f,
		decls,
		dname,
		to_const_DeclExternalProcedure(decl)->parms
	    );
	    break;
    }
}

static void generate_declaration(
 FILE *f,
 const_declaration_list decls,
 const_declaration decl
)
{
    const_origsymbol dname = decl->name;

    switch( decl->tag ){
	case TAGDeclGlobalVariable:
	    generate_variable_declaration(
		f,
		decls,
		0,
		dname,
		to_const_DeclGlobalVariable(decl)->t,
		decl->flags,
		to_const_DeclGlobalVariable(decl)->init
	    );
	    break;

	case TAGDeclExternalVariable:
	case TAGDeclExternalFunction:
	case TAGDeclExternalProcedure:
	    /* done in forward declarations section. */
	    break;

	case TAGDeclFunction:
	    generate_function(
		f,
		decls,
		to_const_DeclFunction( decl )
	    );
	    break;

	case TAGDeclProcedure:
	    generate_procedure(
		f,
		decls,
		to_const_DeclProcedure( decl )
	    );
	    break;

	case TAGDeclLocalVariable:
	case TAGDeclFormalVariable:
	case TAGDeclCardinalityVariable:
	case TAGDeclRecord:
	    break;

    }
}

/* Forward declarations for functions are generated first, and
 * declarations for variables are generated after that.
 *
 * This because variable initializations may contain function invocations.
 */
static void generate_declarations( FILE *f, const_declaration_list declarations )
{
    unsigned int ix;

    for( ix=0; ix<declarations->sz; ix++ ){
	declaration d = declarations->arr[ix];

	if( is_RoutineDeclaration( d ) ){
	    generate_forward_declaration( f, declarations, d );
	}
    }
    fputs( "/* ------ end of forward function declarations ------ */\n", f );
    for( ix=0; ix<declarations->sz; ix++ ){
	declaration d = declarations->arr[ix];

	if( !is_RoutineDeclaration( d ) ){
	    generate_forward_declaration( f, declarations, d );
	}
    }
    fputs( "/* ------ end of forward variable declarations ------ */\n", f );
    for( ix=0; ix<declarations->sz; ix++ ){
	declaration d = declarations->arr[ix];

	if( !is_RoutineDeclaration( d ) ){
	    generate_declaration( f, declarations, d );
	}
    }
    for( ix=0; ix<declarations->sz; ix++ ){
	declaration d = declarations->arr[ix];

	if( is_RoutineDeclaration( d ) ){
	    generate_declaration( f, declarations, d );
	}
    }
}

static void generate_headertext( FILE *f, const_tmstring_list includes )
{
    fputs( "/* This code was generated by the ParTool Vnus Backend. */\n", f );
    fputs( "\n", f );
    fputs( "#include <stdlib.h>\n", f );
    fputs( "#include <stdio.h>\n", f );
    fputs( "#include <errno.h>\n", f );
    fputs( "#include <string.h>\n", f );
    fputs( "#include <vnuscomplex.h>\n", f );
    fputs( "#include <sys/time.h>\n", f );
    fputs( "#include <sys/types.h>\n", f );
    fputs( "#include <unistd.h>\n", f );
    if( generateTimingCode ){
        fputs( "#include <sys/utsname.h>\n", f );
    }
    fputs( "#include \"vnusstio.h\"\n", f );
    if( program_type == PROG_DISTRIBUTED ){
        fputs( "#include \"vnusprocdefs.h\"\n", f );
    }
    fputs( "#include \"vnusbaseclass.h\"\n", f );
    fputs( "#include \"arrayalloc.h\"\n", f );
    fputs( "#include \"vnusservice.h\"\n", f );
    fputs( "#include \"shape.h\"\n", f );
    fputs( "#include \"vnusrtl.h\"\n", f );
    fputs( "#include \"arrayboundviolated.h\"\n", f );
    fputs( "#include \"notnullassert.h\"\n", f );
    if( longAsClass ){
      fputs( "#include \"longlong.h\"\n", f);
    }
    for( unsigned int i=0; i<includes->sz; i++ ){
	fprintf( f, "#include <%s>\n", includes->arr[i] );
    }
    fputs( "\n", f );
    if( program_type == PROG_SEQUENTIAL ){
	fputs( "VnusInt numberOfProcessors = 1;\n", f );
    }
    else {
	fprintf(
	    f,
	    "VnusInt numberOfProcessors = %d;\n",
	    numberOfProcessors
	);
	if( processor_array->sz == 0 ){
	    processor_array = append_tmuint_list( processor_array, 1 );
	}
	fprintf( f, "VnusInt processorArrayRank = %d;\n", processor_array->sz );
	fprintf(
	    f,
	    "VnusInt processorArrayDims[] = { "
	);
	fprintf( f, "%u", processor_array->arr[0] );
	for( unsigned int ix=1; ix<processor_array->sz; ix++ ){
	    fprintf( f, ", %u", processor_array->arr[ix] );
	}
	fputs( " };\n\n", f );
	}
    fputs( "int vnus_argc;\n", f );
    fputs( "char **vnus_argv;\n", f );
}

/* Given a top level execution list, distil the name used for 'me',
 * and assign it to 'me_name'.
 */
static void distil_me_name( const_block blk )
{
    statement forkall;
    cardinality_list cl;
    statement_list stmts = blk->statements;

    if( stmts->sz != 1 || stmts->arr[0]->tag != TAGSmtForkall ){
	internal_error( "top level for threaded execution should contain exactly one forkall" );
    }
    forkall = stmts->arr[0];
    cl = to_SmtForkall(forkall)->cards;
    if( cl->sz != 1 ){
	internal_error( "forkall for threaded execution should have cardinality list of size 1" );
    }
    me_name = cl->arr[0]->name->sym;
}

/* Given a top level execution list, generate a thread function. */
static void generate_parallel_function( FILE *f, const_declaration_list decls, const_block blk )
{
    block body;
    statement_list smts = blk->statements;

    if( smts->sz != 1 || smts->arr[0]->tag != TAGSmtForkall ){
	internal_error( "top level for parallel execution should contain exactly one forkall" );
    }
    body = to_SmtForkall(smts->arr[0])->body;
    fputs( "\n", f );
    fprintf( f, "static void %s( VnusInt %s )\n", PARALLEL_FUNCTION_NAME, me_name->name );
    generate_bracket_block( f, decls, 0, body );
    fputs( "\n", f );
    fprintf( f, "static void %s( int p, int argc, char **argv )\n", PARALLEL_FUNCTION_WRAPPER_NAME );
    fputs( "{\n", f );
    generate_indent_line( f, INDENTSTEP, "vnus_argc = argc;" );
    generate_indent_line( f, INDENTSTEP, "vnus_argv = argv;" );
    if( generateTimingCode ){
        generate_timing_begin( f );
    }
    generate_indent( f, INDENTSTEP );
    fprintf( f, "%s( (VnusInt) p );\n", PARALLEL_FUNCTION_NAME );
    if( generateTimingCode ){
        generate_timing_end( f );
    }
    fputs( "}\n", f );
}

static void generate_distributed_main( FILE *f, const unsigned int indent, const_statement_list smts )
{
    if( smts->sz != 1 || smts->arr[0]->tag != TAGSmtForkall ){
	internal_error( "top level for distributed execution should contain exactly one forkall" );
    }
    generate_indent( f, indent );
    fprintf(
	f,
	"vnus_exec_forkall( %s, numberOfProcessors, argc, argv );\n",
	PARALLEL_FUNCTION_WRAPPER_NAME
    );
}

static void generate_main( FILE *f, const_declaration_list decls, const_block blk, bool dump_gc_stats )
{
    const_statement_list l = blk->statements;

    fputs( "\n", f );
    fputs( "int main( int argc, char **argv )\n", f );
    fputs( "{\n", f );
    switch( program_type ){
	case PROG_SEQUENTIAL:
        generate_indent_line( f, INDENTSTEP, "vnus_argc = argc;" );
        generate_indent_line( f, INDENTSTEP, "vnus_argv = argv;" );
	    if( generateTimingCode ){
		generate_timing_begin( f );
	    }
	    generate_statements( f, decls, INDENTSTEP, l );
	    if( generateTimingCode ){
		generate_timing_end( f );
	    }
	    break;

	case PROG_DISTRIBUTED:
	    generate_distributed_main( f, INDENTSTEP, l );
	    break;
    }
    if( dump_gc_stats ){
	generate_indent_line( f, INDENTSTEP, "vnus_report_garbagecollector();" );
    }
    generate_indent_line( f, INDENTSTEP, "exit( 0 );" );
    generate_indent_line( f, INDENTSTEP, "return 0;" );
    fputs( "}\n", f );
}

void generate_vnusprog( FILE *f, const_tmstring_list includes, const_vnusprog theprog, bool dump_gc_stats )
{
    global_pragmas = theprog->pragmas;
    if( program_type != PROG_SEQUENTIAL ){
        distil_me_name( theprog->statements );
    }
    generate_headertext( f, includes );
    generate_record_definitions( f, theprog->declarations );
    generate_function_types( f, theprog->declarations );
    generate_declarations( f, theprog->declarations );
    switch( program_type ){
	case PROG_SEQUENTIAL:
	    break;

	case PROG_DISTRIBUTED:
	    generate_parallel_function( f, theprog->declarations, theprog->statements );
	    break;
    }
    generate_main( f, theprog->declarations, theprog->statements, dump_gc_stats );
}
