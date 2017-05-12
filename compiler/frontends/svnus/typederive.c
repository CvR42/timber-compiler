/* File: typederive.c
 *
 * Routines to derive the type of parts of the parse tree.
 */

#include <tmc.h>

#include "defs.h"
#include "vnus_types.h"
#include "tmadmin.h"
#include "error.h"
#include "symbol_table.h"
#include "typederive.h"
#include "rewrite.h"
#include "service.h"

/* Forward declarations. */
static bool is_equivalent_type_list( const type_list la, const type_list lb );

/* Given two types 'a' and 'b', that presumably are base types, return
 * a new type expression containing the 'maximum' of these two types,
 * where the maximum is the one with the largest number of bytes,
 * Also, the 'float' type is larger than the 'long' type.
 */
static BASETYPE max_basetypes( const BASETYPE ta, const BASETYPE tb )
{
    if( ta<tb ){
	return tb;
    }
    else {
	return ta;
    }
}

/* Given two types 'a' and 'b', that presumably are base types, return
 * a new type expression containing the 'maximum' of these two types,
 * where the maximum is the one with the largest number of bytes,
 * Also, the 'float' type is larger than the 'long' type.
 */
type max_types( const type ta, const type tb )
{
    if( ta->tag == TAGTypeNeutralPointer && tb->tag == TAGTypePointer ){
	return rdup_type( tb );
    }
    if( ta->tag == TAGTypePointer && tb->tag == TAGTypeNeutralPointer ){
	return rdup_type( ta );
    }
    if( ta->tag != TAGTypeBase ){
	return rdup_type( tb );
    }
    if( tb->tag != TAGTypeBase ){
	return rdup_type( ta );
    }
    return new_TypeBase( max_basetypes( to_TypeBase(ta)->base, to_TypeBase(tb)->base ) );
}

/* Given a symbol 'nm' return the type of that variable. */
type derive_type_name( const origsymbol nm )
{
    return rewrite_type( lookup_vartype( nm ) );
}

static type derive_type_selection( const expression selected )
{
    type ans = typeNIL;
    type t = derive_type_expression( selected );

    if( t == typeNIL ){
	return t;
    }
    switch( t->tag ){
	case TAGTypeBase:
	case TAGTypePointer:
	case TAGTypeRecord:
	case TAGTypeNeutralPointer:
	case TAGTypeFunction:
	case TAGTypeProcedure:
	    internal_botch( "Cannot select element" );
	    break;

	case TAGTypeShape:
	    ans = rdup_type( to_TypeShape(t)->elmtype );
	    break;

	case TAGTypeView:
	    ans = rdup_type( to_TypeView(t)->elmtype );
	    break;

    }
    rfre_type( t );
    return ans;
}

/* Given a function call 'fc', return the type of the function. This
 * function does not check the input parameters, since this is supposed to
 * happen elsewere.
 */
static type derive_type_functioncall( ExprFunctionCall fc )
{
    type fnt = derive_type_expression( to_ExprFunctionCall(fc)->function );
    type ans;

    switch( fnt->tag ){
	case TAGTypeBase:
	case TAGTypeRecord:
	case TAGTypeShape:
	case TAGTypeView:
	case TAGTypeNeutralPointer:
	case TAGTypeProcedure:
	case TAGTypePointer:
	    internal_botch( "Cannot determine return type" );
	    break;

	case TAGTypeFunction:
	    ans = rewrite_type( rdup_type( to_TypeFunction(fnt)->retval ) );
	    break;
    }
    rfre_type( fnt );
    return ans;
}

/* Given an expression, convert it to a size. */
static size derive_size_expression( expression x )
{
    size ans = sizeNIL;

    switch( x->tag ){
        case TAGExprPragma:
            ans = derive_size_expression( to_ExprPragma(x)->x );
            break;

        case TAGExprIsOwner:
        case TAGExprBoolean:
        case TAGExprString:
        case TAGExprComplex:
        case TAGExprView:
        case TAGExprByte:
        case TAGExprCast:
        case TAGExprShort:
        case TAGExprInt:
        case TAGExprLong:
        case TAGExprFloat:
        case TAGExprDouble:
        case TAGExprChar:
        case TAGExprGetSize:
        case TAGExprFunctionCall:
        case TAGExprSelection:
        case TAGExprUnop:
        case TAGExprBinop:
        case TAGExprGetBlocksize:
        case TAGExprOwner:
        case TAGExprSender:
        case TAGExprIf:
        case TAGExprName:
        case TAGExprField:
        case TAGExprNull:
        case TAGExprDeref:
        case TAGExprAddress:
        case TAGExprNew:
        case TAGExprRecord:
        case TAGExprIsMultidimDist:
        case TAGExprGetRoom:
	    ans = new_SizeExpression( rdup_expression( x ) );
	    break;
    }
    return ans;
}

/* Given a view 'v', return the type of the expression. */
static type derive_type_view( const view v )
{
    type seltype;

    seltype = derive_type_expression( v->loc );
    return new_TypeView( v->cardinalities->sz, seltype );
}

/* Given two operands 'opa' and 'opb', and an operand 'op', return the
 * type of the binary operator expression. This function is rather sloppy
 * about checking the type of the operators, since this is supposed to
 * happen elsewere.
 */
static type derive_type_binop(
    const expression opa,
    const BINOP op,
    const expression opb
)
{
    switch( op ){
	/* Arithmetic and boolean/bitwise operators. */
        case BINOP_AND:
        case BINOP_OR:
        case BINOP_XOR:
        case BINOP_MOD:
        case BINOP_PLUS:
        case BINOP_MINUS:
        case BINOP_TIMES:
        case BINOP_DIVIDE:
	{
	    type ta = derive_type_expression( opa );
	    type tb = derive_type_expression( opb );
	    type tx = max_types( ta, tb );

	    rfre_type( ta );
	    rfre_type( tb );
	    return tx;
	}

	/* Comparison operators. */
        case BINOP_EQUAL:
        case BINOP_NOTEQUAL:
        case BINOP_LESS:
        case BINOP_LESSEQUAL:
        case BINOP_GREATER:
        case BINOP_GREATEREQUAL:
	    return new_TypeBase( BT_BOOLEAN );

	/* Shift operators. */
	case BINOP_SHIFTLEFT:
	case BINOP_SHIFTRIGHT:
	case BINOP_USHIFTRIGHT:
	    return derive_type_expression( opa );

    }
    return typeNIL;
}

static field_list derive_fields_expression_list( const expression_list l )
{
    unsigned int ix;
    field_list res;

    res = setroom_field_list( new_field_list(), l->sz );
    for( ix=0; ix<l->sz; ix++ ){
	res = append_field_list(
	    res,
	    new_field(
		origsymbolNIL,
		derive_type_expression( l->arr[ix] )
	    )
	);
    }
    return res;
}

/* Given an expression 'x', return the type of that expression.
 * The returned type tree must be deleted by the caller.
 */
type derive_type_expression( const expression x )
{
    type ans;

    ans = typeNIL;
    switch( x->tag ){
	case TAGExprNull:
	    ans = new_TypeNeutralPointer();
	    break;

        case TAGExprByte:
	    ans = new_TypeBase( BT_BYTE );
	    break;

        case TAGExprShort:
	    ans = new_TypeBase( BT_SHORT );
	    break;

        case TAGExprInt:
	    ans = new_TypeBase( BT_INT );
	    break;

        case TAGExprLong:
	    ans = new_TypeBase( BT_LONG );
	    break;

        case TAGExprFloat:
	    ans = new_TypeBase( BT_FLOAT );
	    break;

        case TAGExprDouble:
	    ans = new_TypeBase( BT_DOUBLE );
	    break;

        case TAGExprString:
	    ans = new_TypeBase( BT_STRING );
	    break;

        case TAGExprChar:
	    ans = new_TypeBase( BT_CHAR );
	    break;

        case TAGExprBoolean:
	    ans = new_TypeBase( BT_BOOLEAN );
	    break;

	case TAGExprCast:
	    ans = rdup_type( to_ExprCast(x)->t );
	    break;

        case TAGExprComplex:
	    ans = new_TypeBase( BT_COMPLEX );
	    break;

	case TAGExprRecord:
	    ans = new_TypeRecord(
		derive_fields_expression_list( to_ExprRecord(x)->fields )
	    );
	    break;

        case TAGExprGetSize:
	    ans = new_TypeBase( BT_INT );
	    break;

        case TAGExprGetRoom:
	    ans = new_TypeBase( BT_INT );
	    break;

        case TAGExprFunctionCall:
	    ans = derive_type_functioncall( to_ExprFunctionCall(x) );
	    break;

        case TAGExprSelection:
	    ans = derive_type_selection( to_ExprSelection(x)->selected );
	    break;

        case TAGExprView:
	    ans = derive_type_view( to_ExprView(x)->v );
	    break;

        case TAGExprUnop:
	    ans = derive_type_expression( to_ExprUnop(x)->operand );
	    break;

        case TAGExprPragma:
            ans = derive_type_expression( to_ExprPragma(x)->x );
            break;

        case TAGExprBinop:
	    ans = derive_type_binop(
	        to_ExprBinop(x)->operanda,
		to_ExprBinop(x)->optor,
		to_ExprBinop(x)->operandb
	    );
	    break;

        case TAGExprGetBlocksize:
	    ans = new_TypeBase( BT_INT );
	    break;

        case TAGExprSender:
	    ans = new_TypeBase( BT_INT );
	    break;

        case TAGExprOwner:
	    ans = new_TypeBase( BT_INT );
	    break;

        case TAGExprIsOwner:
	    ans = new_TypeBase( BT_BOOLEAN );
	    break;

        case TAGExprIsMultidimDist:
	    ans = new_TypeBase( BT_BOOLEAN );
	    break;

        case TAGExprIf:
	    ans = derive_type_expression( to_ExprIf(x)->thenval );
	    break;

        case TAGExprName:
	    ans = derive_type_name( to_ExprName(x)->name );
	    break;

	case TAGExprField:
	{
	    type t = derive_type_expression( to_ExprField(x)->rec );

	    if( t == typeNIL ){
		break;
	    }
	    switch( t->tag ){
		case TAGTypeBase:
		case TAGTypeShape:
		case TAGTypeView:
		case TAGTypePointer:
		case TAGTypeNeutralPointer:
		case TAGTypeFunction:
		case TAGTypeProcedure:
		    internal_botch( "Cannot select element" );
		    break;

		case TAGTypeRecord:
		{
		    int pos = find_field( t, to_ExprField(x)->field );
		    if( pos<0 ){
			internal_botch( "no such field in this record" );
		    }
		    else {
			ans = rdup_type( to_TypeRecord(t)->fields->arr[pos]->elmtype );
		    }
		    break;
		}
	    }
	    rfre_type( t );
	    break;
	}

	case TAGExprDeref:
	{
	    type t = derive_type_expression( to_ExprDeref(x)->ref );

	    if( t == typeNIL ){
		break;
	    }
	    switch( t->tag ){
		case TAGTypeBase:
		case TAGTypeRecord:
		case TAGTypeShape:
		case TAGTypeView:
		case TAGTypeNeutralPointer:
		case TAGTypeFunction:
		case TAGTypeProcedure:
		    internal_botch( "Cannot select element" );
		    break;

		case TAGTypePointer:
		    ans = rdup_type( to_TypePointer(t)->elmtype );
		    break;
	    }
	    rfre_type( t );
	    break;
	}
	    
	case TAGExprAddress:
	    ans = new_TypePointer(
	       derive_type_expression( to_ExprAddress(x)->adr )
	    );
	    break;

	case TAGExprNew:
	    ans = new_TypePointer( rdup_type( to_ExprNew(x)->t ) );
	    break;
    }
    return ans;
}

type derive_type_optexpression( const optexpression x )
{
    type ans = typeNIL;

    switch( x->tag ){
	case TAGOptExprNone:
	    ans = typeNIL;
	    break;

	case TAGOptExpr:
	    ans = derive_type_expression( to_OptExpr(x)->x );
	    break;
    }
    return ans;
}

type derive_type_formalParameter( const formalParameter arg )
{
    return rewrite_type( rdup_type( lookup_parmtype( arg->name ) ) );
}

type_list derive_type_formalParameter_list( const formalParameter_list pl )
{
    unsigned int ix;
    type_list res;

    res = setroom_type_list( new_type_list(), pl->sz );
    for( ix=0; ix<pl->sz; ix++ ){
	res = append_type_list( res, derive_type_formalParameter( pl->arr[ix] ) );
    }
    return res;
}

/***** Some type classification functions. *****/

bool is_scalar_type( const type t )
{
    return t->tag == TAGTypeBase;
}

static bool is_equivalent_field( const field a, const field b )
{
    return is_equivalent_type( a->elmtype, b->elmtype );
}

static bool is_equivalent_field_list( const field_list la, const field_list lb )
{
    unsigned int ix;

    if( la->sz != lb->sz ){
	return FALSE;
    }
    for( ix=0; ix<la->sz; ix++ ){
	if( !is_equivalent_field( la->arr[ix], lb->arr[ix] ) ){
	    return FALSE;
	}
    }
    return TRUE;
}

/* Given a type 'a' and a type 'b', return TRUE iff the types are
 * equivalent.
 */
bool is_equivalent_type( const type a, const type b )
{
    if( a->tag != b->tag ){
	return FALSE;
    }
    switch( a->tag ){
	case TAGTypeBase:
	    return (to_TypeBase(a)->base == to_TypeBase(b)->base);

	case TAGTypeView:
	    return is_equivalent_type(
		to_TypeView(a)->elmtype,
		to_TypeView(b)->elmtype
	    );

	case TAGTypeShape:
	    return is_equivalent_type(
		to_TypeShape(a)->elmtype,
		to_TypeShape(b)->elmtype
	    );

	case TAGTypePointer:
	    return is_equivalent_type(
		to_TypePointer(a)->elmtype,
		to_TypePointer(b)->elmtype
	    );

	case TAGTypeRecord:
	    return is_equivalent_field_list(
		to_TypeRecord(a)->fields,
		to_TypeRecord(b)->fields
	    );

	case TAGTypeNeutralPointer:
	    return TRUE;

	case TAGTypeFunction:
	    return is_equivalent_type_list(
		    to_TypeFunction(a)->formals,
		    to_TypeFunction(b)->formals
		) &&
	        is_equivalent_type(
		    to_TypeFunction(a)->retval,
		    to_TypeFunction(b)->retval
		);

	case TAGTypeProcedure:
	    return is_equivalent_type_list(
		to_TypeProcedure(a)->formals,
		to_TypeProcedure(b)->formals
	    );

    }
    return FALSE;
}

static bool is_equivalent_type_list( const type_list la, const type_list lb )
{
    unsigned int ix;

    if( la->sz != lb->sz ){
	return FALSE;
    }
    for( ix=0; ix<la->sz; ix++ ){
	if( !is_equivalent_type( la->arr[ix], lb->arr[ix] ) ){
	    return FALSE;
	}
    }
    return TRUE;
}
