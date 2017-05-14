/* File: typederive.c
 *
 * Routines to derive the type of parts of the parse tree.
 */

#include <tmc.h>
#include <assert.h>

#include "defs.h"
#include "tmadmin.h"
#include "error.h"
#include "symbol_table.h"
#include "typederive.h"
#include "service.h"
#include "collect.h"

/* Forward declarations. */
static bool is_identity_conv_type_list( const_Entry_list symtab, const_type_list la, const_type_list lb );
static bool is_implemented( const_TypeEntry_list typelist, const_type sup, const_type inf );
static bool is_widening_tuple_type( const_Entry_list symtab, const_TypeEntry_list, const_type from, const_type to );
static bool is_narrowing_tuple_type( const_Entry_list symtab, const_TypeEntry_list, const_type from, const_type to );

/* Given two basetypes 'a' and 'b', return
 * a new type expression containing the 'maximum' of these two types,
 * as defined in JLS2 5.6.2
 */
static BASETYPE max_basetypes( const BASETYPE ta, const BASETYPE tb )
{
    if( ta == BT_BOOLEAN || tb == BT_BOOLEAN ){
	// In fact, both should be boolean, but we'll cover is elsewhere
	return BT_BOOLEAN;
    }
    /* This one is only valid for '+', but we'll cover it elsewhere */
    if( ta == BT_STRING || tb == BT_STRING ){
	return BT_STRING;
    }
    if( ta == BT_COMPLEX || tb == BT_COMPLEX ){
	return BT_COMPLEX;
    }
    if( ta == BT_DOUBLE || tb == BT_DOUBLE ){
	return BT_DOUBLE;
    }
    if( ta == BT_FLOAT || tb == BT_FLOAT ){
	return BT_FLOAT;
    }
    if( ta == BT_LONG || tb == BT_LONG ){
	return BT_LONG;
    }
    return BT_INT;
}

// Given a type 't' that presumably is a primitive type, return its BASETYPE.
BASETYPE get_BASETYPE( const_type t )
{
    if( t == typeNIL ){
	return BT_INT;
    }
    if( t->tag == TAGPrimitiveType ){
	return to_const_PrimitiveType(t)->base;
    }
    if( t->tag == TAGPragmaType ){
	return get_BASETYPE( to_const_PragmaType(t)->t );
    }
    internal_error( "not a primitive type" );
    return BT_INT;
}

/* Given two types 'a' and 'b', that presumably are base types, return
 * a new type expression containing the 'maximum' of these two types,
 * where the maximum is the one with the largest number of bytes,
 * Also, the 'float' type is larger than the 'long' type.
 */
type max_types( const_type ta, const_type tb )
{
    if( ta == typeNIL || (ta->tag == TAGNullType && is_ReferenceType( tb )) ){
	return rdup_type( tb );
    }
    if( ta->tag == TAGPragmaType ){
	return max_types( to_const_PragmaType(ta)->t, tb );
    }
    if( tb == typeNIL ){
	return rdup_type( ta );
    }
    if( tb->tag == TAGPragmaType ){
	return max_types( ta, to_const_PragmaType(tb)->t );
    }
    if( is_ReferenceType( ta ) && tb->tag == TAGNullType ){
	return rdup_type( ta );
    }
    if( ta->tag != TAGPrimitiveType ){
	return rdup_type( tb );
    }
    if( tb->tag != TAGPrimitiveType ){
	return rdup_type( ta );
    }
    return new_PrimitiveType( max_basetypes( to_const_PrimitiveType(ta)->base, to_const_PrimitiveType(tb)->base ) );
}

/* Given a symbol 'nm' return the type of that variable. */
type derive_type_name( const_Entry_list symtab, const_origsymbol nm )
{
    return lookup_vartype( symtab, nm );
}

static type derive_type_getbuf( const_Entry_list symtab, const_expression arr )
{
    type t = derive_type_expression( symtab, arr );

    if( t->tag != TAGArrayType ){
	// This expression is wrong, but let somebody else report it.
	rfre_type( t );
	return typeNIL;
    }
    ArrayType at = to_ArrayType(t);
    type res = new_PrimArrayType( at->elmtype );
    at->elmtype = typeNIL;
    rfre_type( t );
    return res;
}

static type derive_type_selection( const_Entry_list symtab, const_type t )
{
    type ans = typeNIL;

    if( t == typeNIL ){
	return typeNIL;
    }
    switch( t->tag ){
	case TAGPrimitiveType:
	case TAGTypeType:
	case TAGNullType:
	case TAGFunctionType:
	case TAGObjectType:
	case TAGGenericObjectType:
	case TAGVoidType:
	case TAGExceptionVariableType:
	case TAGGCRefLinkType:
	case TAGGCTopRefLinkType:
	    // This expression is wrong, but let somebody else report it.
	    return typeNIL;

	case TAGTupleType:
	    // TODO: allow this.
	    internal_error( "Cannot select element of a tuple" );
	    break;

	case TAGVectorType:
	    ans = rdup_type( to_const_VectorType(t)->elmtype );
	    break;

	case TAGArrayType:
	    ans = rdup_type( to_const_ArrayType(t)->elmtype );
	    break;

	case TAGPrimArrayType:
	    ans = rdup_type( to_const_PrimArrayType(t)->elmtype );
	    break;

	case TAGPragmaType:
	    ans = derive_type_selection( symtab, to_const_PragmaType(t)->t );
	    break;

	case TAGTypeOf:
	{
	    type nt = derive_type_expression( symtab, to_const_TypeOf(t)->x );
	    ans = derive_type_selection( symtab, nt );
	    rfre_type( nt );
	    break;
	}

	case TAGTypeOfIf:
	{
	    type nt = calculate_ifexpression_type(
		symtab,
		to_const_TypeOfIf(t)->org,
		to_const_TypeOfIf(t)->thenval,
		to_const_TypeOfIf(t)->elseval
	    );
	    ans = derive_type_selection( symtab, nt );
	    rfre_type( nt );
	    break;
	}

    }
    return ans;
}

static type derive_type_selection( const_Entry_list symtab, const expression selected )
{
    type t = derive_type_expression( symtab, selected );
    type ans = derive_type_selection( symtab, t );
    rfre_type( t );
    return ans;
}

/* Given a type 'fnt' that presumably is the type of a function, return the
 * type of the return value of that function.
 */
static type derive_type_functioncall( const_Entry_list symtab, const_type fnt )
{
    type ans = typeNIL;

    if( fnt == typeNIL ){
	return typeNIL;
    }
    switch( fnt->tag ){
	case TAGPrimitiveType:
	case TAGTypeType:
	case TAGTupleType:
	case TAGVectorType:
	case TAGArrayType:
	case TAGPrimArrayType:
	case TAGNullType:
	case TAGObjectType:
	case TAGGenericObjectType:
	case TAGVoidType:
	case TAGExceptionVariableType:
	case TAGGCRefLinkType:
	case TAGGCTopRefLinkType:
	    internal_error( "Cannot determine return type" );
	    break;

	case TAGTypeOf:
	{
	    type nt = derive_type_expression( symtab, to_const_TypeOf(fnt)->x );
	    ans = derive_type_functioncall( symtab, nt );
	    rfre_type( nt );
	    break;
	}

	case TAGTypeOfIf:
	{
	    type nt = calculate_ifexpression_type(
		symtab,
		to_const_TypeOfIf(fnt)->org,
		to_const_TypeOfIf(fnt)->thenval,
		to_const_TypeOfIf(fnt)->elseval
	    );
	    ans = derive_type_functioncall( symtab, nt );
	    rfre_type( nt );
	    break;
	}

	case TAGPragmaType:
	    ans = derive_type_functioncall( symtab, to_const_PragmaType(fnt)->t );
	    break;

	case TAGFunctionType:
	    ans = rdup_type( to_const_FunctionType(fnt)->rettype );
	    break;
    }
    return ans;
}

/* Given a function call 'fc', return the type of the function. This
 * function does not check the input parameters, since this is supposed to
 * happen elsewere.
 */
static type derive_type_functioncall( const_Entry_list symtab, const_MethodInvocationExpression fc )
{
    type fnt = derive_type_name( symtab, fc->invocation->name );
    type ans = derive_type_functioncall( symtab, fnt );
    rfre_type( fnt );
    return ans;
}

#if 0
/* Given a function call 'fc', return the type of the function. This
 * function does not check the input parameters, since this is supposed to
 * happen elsewere.
 */
static type derive_type_virtualfunctioncall( const_Entry_list symtab, const_type fnt )
{
    type ans = typeNIL;

    if( fnt == typeNIL ){
	return typeNIL;
    }
    switch( fnt->tag ){
	case TAGPrimitiveType:
	case TAGTypeType:
	case TAGTupleType:
	case TAGVectorType:
	case TAGArrayType:
	case TAGPrimArrayType:
	case TAGNullType:
	case TAGObjectType:
	case TAGGenericObjectType:
	case TAGVoidType:
	case TAGExceptionVariableType:
	case TAGGCRefLinkType:
	case TAGGCTopRefLinkType:
	    internal_error( "Cannot determine return type" );
	    break;

	case TAGTypeOf:
	{
	    type nt = derive_type_expression( symtab, to_const_TypeOf(fnt)->x );
	    ans = derive_type_virtualfunctioncall( symtab, nt );
	    rfre_type( nt );
	    break;
	}

	case TAGPragmaType:
	    ans = derive_type_virtualfunctioncall( symtab, to_const_PragmaType(fnt)->t );
	    break;


	case TAGFunctionType:
	    ans = rdup_type( to_const_FunctionType(fnt)->rettype );
	    break;
    }
    return ans;
}
#endif

/* Given two operands 'opa' and 'opb', and an operand 'op', return the
 * type of the binary operator expression. This function is rather sloppy
 * about checking the type of the operators, since this is supposed to
 * happen elsewere.
 */
static type derive_type_binop(
    const_Entry_list symtab,
    const_expression opa,
    const BINOP op,
    const_expression opb
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
	    type ta = derive_type_expression( symtab, opa );
	    type tb = derive_type_expression( symtab, opb );
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
	    return new_PrimitiveType( BT_BOOLEAN );

	/* Shift operators. */
	case BINOP_SHIFTLEFT:
	case BINOP_SHIFTRIGHT:
	case BINOP_USHIFTRIGHT:
	    return derive_type_expression( symtab, opa );

    }
    return typeNIL;
}

type_list derive_type_expression_list( const_Entry_list symtab, const_expression_list l )
{
    type_list res = setroom_type_list( new_type_list(), l->sz );
    for( unsigned int ix=0; ix<l->sz; ix++ ){
	res = append_type_list(
	    res,
	    derive_type_expression( symtab, l->arr[ix] )
	);
    }
    return res;
}

/* Given a type 't' and a vector, return 't' wrapped in
 * an ArrayType with the rank of the vector.
 */
static type wrap_size( const_Entry_list symtab, type t, ActualDim dim )
{
    expression vec = rdup_expression( dim->vector );
    type vt = derive_type_expression( symtab, vec );
    unsigned int sz;

    if( !extract_tuple_length( originNIL, vt, &sz ) ){
	sz = 0;
    }
    rfre_type( vt );
    rfre_expression( vec );
    return new_ArrayType( t, new_IntExpression( (int) sz ) );
}

/* Given a type 't' and a list of vectors, return 't' wrapped
 * in ArrayType's with the ranks of the vectors.
 */
static type wrap_sizes( const_Entry_list symtab, type t, ActualDim_list vectors )
{
    for( unsigned int ix=0; ix<vectors->sz; ix++ ){
	t = wrap_size( symtab, t, vectors->arr[ix] );
    }
    return t;
}

/* Given a basetype 'bt', return true iff this is a subject of unary 
 * numeric promotion according to JLS2 5.6.1.
 * 'from' to type 'to' is a widening primitive conversion as defined in
 * JLS2 5.1.2.
 */
static bool is_unary_promotion_basetype( const BASETYPE bt )
{
    return bt == BT_BYTE || bt == BT_SHORT || bt == BT_CHAR;
}

/* Given a type 't', return the type with possibly unary numeric
 * promotion according to JLS2 5.6.1 applied.
 *
 * TODO: apply this to elements of tuples when necessary.
 */
static type unary_promote_type( type t )
{
    assert( t->tag != TAGTypeOf );
    if( t->tag == TAGPragmaType ){
	to_PragmaType(t)->t = unary_promote_type( to_PragmaType(t)->t );
	return t;
    }
    if( t->tag == TAGPrimitiveType ){
	if( is_unary_promotion_basetype( to_PrimitiveType(t)->base ) ){
	    to_PrimitiveType(t)->base = BT_INT;
	}
    }
    return t;
}

// Given a unary operator op, return true iff this operator does
// unary numeric promotion.
static bool is_unary_promotion_operator( UNOP op )
{
    return (op == UNOP_PLUS) || (op == UNOP_NEGATE) || (op == UNOP_NOT);
}

/* Given an expression 'x', return the type of that expression.
 * The returned type tree must be deleted by the caller.
 */
type derive_type_expression( const_Entry_list symtab, const_expression x )
{
    type ans = typeNIL;

    if( x == expressionNIL ){
	return ans;
    }
    switch( x->tag ){
	case TAGDefaultValueExpression:
	    ans = rdup_type( to_const_DefaultValueExpression(x)->t );
	    break;

	case TAGNullExpression:
	    ans = new_NullType();
	    break;

        case TAGByteExpression:
	    ans = new_PrimitiveType( BT_BYTE );
	    break;

        case TAGShortExpression:
	    ans = new_PrimitiveType( BT_SHORT );
	    break;

        case TAGClassIdExpression:
        case TAGSizeofExpression:
        case TAGIntExpression:
	    ans = new_PrimitiveType( BT_INT );
	    break;

        case TAGLongExpression:
	    ans = new_PrimitiveType( BT_LONG );
	    break;

        case TAGFloatExpression:
	    ans = new_PrimitiveType( BT_FLOAT );
	    break;

        case TAGDoubleExpression:
	    ans = new_PrimitiveType( BT_DOUBLE );
	    break;

        case TAGStringExpression:
	    ans = make_String_type();
	    break;

        case TAGCharExpression:
	    ans = new_PrimitiveType( BT_CHAR );
	    break;

        case TAGBooleanExpression:
	    ans = new_PrimitiveType( BT_BOOLEAN );
	    break;

	case TAGCastExpression:
	    ans = rdup_type( to_const_CastExpression(x)->t );
	    break;

	case TAGForcedCastExpression:
	    ans = rdup_type( to_const_ForcedCastExpression(x)->t );
	    break;

	case TAGPostIncrementExpression:
	    ans = derive_type_expression( symtab, to_const_PostIncrementExpression(x)->x );
	    break;

	case TAGPostDecrementExpression:
	    ans = derive_type_expression( symtab, to_const_PostDecrementExpression(x)->x );
	    break;

	case TAGPreIncrementExpression:
	    ans = derive_type_expression( symtab, to_const_PreIncrementExpression(x)->x );
	    break;

	case TAGPreDecrementExpression:
	    ans = derive_type_expression( symtab, to_const_PreDecrementExpression(x)->x );
	    break;

        case TAGComplexExpression:
	    ans = new_PrimitiveType( BT_COMPLEX );
	    break;

        case TAGTypeExpression:
	    ans = new_TypeType();
	    break;

	case TAGOuterThisExpression:
	    ans = rdup_type( to_const_OuterThisExpression(x)->t );
	    break;

	case TAGVectorExpression:
	    ans = new_TupleType(
		derive_type_expression_list( symtab, to_const_VectorExpression(x)->fields )
	    );
	    break;

        case TAGClassInstanceOfExpression:
        case TAGInstanceOfExpression:
        case TAGInterfaceInstanceOfExpression:
        case TAGTypeInstanceOfExpression:
	    ans = new_PrimitiveType( BT_BOOLEAN );
	    break;

        case TAGGetSizeExpression:
        case TAGGetLengthExpression:
	    ans = new_PrimitiveType( BT_INT );
	    break;

        case TAGFieldInvocationExpression:
        case TAGTypeInvocationExpression:
        case TAGOuterSuperInvocationExpression:
        case TAGSuperInvocationExpression:
	    internal_error( "untranslated invocation" );
	    break;

        case TAGMethodInvocationExpression:
	    ans = derive_type_functioncall(
		symtab,
		to_const_MethodInvocationExpression( x )
	    );
	    break;

        case TAGVectorSubscriptExpression:
	{
	    type vt = derive_type_expression( symtab, to_const_VectorSubscriptExpression(x)->vector );
	    unsigned int ix = to_const_VectorSubscriptExpression(x)->subscript;

	    switch( vt->tag ){
		case TAGVectorType:
		    ans = rdup_type( to_const_VectorType(vt)->elmtype );
		    break;

		case TAGTupleType:
		{
		    type_list tl = to_const_TupleType(vt)->fields;

		    if( ix>=tl->sz ){
			internal_error( "vector subscript out of range" );
		    }
		    else {
			ans = rdup_type( tl->arr[ix] );
		    }
		    break;
		}

		default:
		    internal_error( "vector subscript not on a vector" );
		    break;
	    }
	    rfre_type( vt );
	    break;
	}

        case TAGGetBufExpression:
	    ans = derive_type_getbuf( symtab, to_const_GetBufExpression(x)->array );
	    break;


        case TAGSubscriptExpression:
	    ans = derive_type_selection( symtab, to_const_SubscriptExpression(x)->array );
	    break;

        case TAGUnopExpression:
	    ans = derive_type_expression( symtab, to_const_UnopExpression(x)->operand );
	    if( is_unary_promotion_operator( to_const_UnopExpression(x)->optor ) ){
	        ans = unary_promote_type( ans );
	    }
	    break;

        case TAGBracketExpression:
            ans = derive_type_expression( symtab, to_const_BracketExpression(x)->x );
            break;

        case TAGAnnotationExpression:
            ans = derive_type_expression( symtab, to_const_AnnotationExpression(x)->x );
            break;

        case TAGBinopExpression:
	    ans = derive_type_binop(
		symtab,
		to_const_BinopExpression(x)->left,
		to_const_BinopExpression(x)->optor,
		to_const_BinopExpression(x)->right
	    );
	    break;

        case TAGShortopExpression:
	    ans = new_PrimitiveType( BT_BOOLEAN );
	    break;

        case TAGIfExpression:
	{
	    const_IfExpression ifx = to_const_IfExpression(x);

	    ans = calculate_ifexpression_type( symtab, ifx->org, ifx->thenval, ifx->elseval );
	    break;
	}

        case TAGWhereExpression:
	    ans = derive_type_expression( symtab, to_const_WhereExpression(x)->x );
	    break;

        case TAGVariableNameExpression:
	    ans = derive_type_name( symtab, to_const_VariableNameExpression(x)->name );
	    break;

	case TAGFieldExpression:
	    ans = derive_type_name( symtab, to_const_FieldExpression(x)->field );
	    break;

	case TAGClassExpression:
	    ans = new_ObjectType( add_origsymbol( "java.lang.Class" ) );
	    break;

	case TAGInternalizeExpression:
	    ans = new_ObjectType( add_origsymbol( "java.lang.String" ) );
	    break;

	case TAGOuterSuperFieldExpression:
	case TAGSuperFieldExpression:
	    internal_error( "unhandled super field" );
	    break;

	case TAGTypeFieldExpression:
	    internal_error( "unhandled type field" );
	    break;

	case TAGAssignOpExpression:
	    ans = derive_type_expression( symtab, to_const_AssignOpExpression(x)->left );
	    break;

	case TAGNewInitArrayExpression:
	    ans = rdup_type( to_const_NewArrayExpression(x)->t );
	    break;

	case TAGNewArrayExpression:
	    ans = wrap_sizes(
		symtab,
		rdup_type( to_const_NewArrayExpression(x)->t ),
		to_const_NewArrayExpression(x)->sizes
	    );
	    break;

	case TAGNewClassExpression:
	    ans = rdup_type( to_const_NewClassExpression(x)->cl );
	    break;

	case TAGArrayInitExpression:
	    if( to_const_ArrayInitExpression(x)->t == typeNIL ){
		internal_error( "Array initializer was not typed" );
	    }
	    else {
		ans = rdup_type( to_const_ArrayInitExpression(x)->t );
	    }
	    break;

	case TAGNotNullAssertExpression:
	    ans = derive_type_expression( symtab, to_const_NotNullAssertExpression(x)->x );
	    break;

	case TAGNewRecordExpression:
	    ans = rdup_type( to_const_NewRecordExpression(x)->cl );
	    break;
    }
    return ans;
}

static type derive_type_FormalParameter( const_FormalParameter arg )
{
    if( arg == FormalParameterNIL ){
	return typeNIL;
    }
    return rdup_type( arg->t );
}

type_list derive_type_FormalParameter_list( const_FormalParameter_list pl )
{
    unsigned int ix;
    type_list res;

    res = setroom_type_list( new_type_list(), pl->sz );
    for( ix=0; ix<pl->sz; ix++ ){
	res = append_type_list( res, derive_type_FormalParameter( pl->arr[ix] ) );
    }
    return res;
}

/***** Some type classification functions. *****/

static bool is_subclass_name( const_TypeEntry_list typelist, const_type sub, const_origsymbol sup );

/* Given a list of types 'typelist', a potential subclass 'sub' and a
 * potential superclass 'sup', return true iff 'sub' is a subclass of 'sup'.
 */
static bool is_subclass_name( const_TypeEntry_list typelist, const_origsymbol sub, const_origsymbol sup )
{
    bool res;

    if( sub == origsymbolNIL || sup == origsymbolNIL ){
	return false;
    }
    TypeEntry sub_entry = search_TypeEntry( typelist, sub->sym );
    TypeEntry sup_entry = search_TypeEntry( typelist, sup->sym );
    if( sub_entry == TypeEntryNIL || sup_entry == TypeEntryNIL ){
	return false;
    }
    if( sub_entry->tag != TAGClassEntry || sup_entry->tag != TAGClassEntry ){
	return false;
    }
    if( sub->sym == sup->sym ){
	return true;
    }
    res = is_subclass_name( typelist, to_ClassEntry(sub_entry)->super, sup );
    return res;
}

static bool is_subclass_name( const_TypeEntry_list typelist, const_type sub, const_origsymbol sup )
{
    if( sub == typeNIL ){
	return false;
    }
    assert( sub->tag == TAGObjectType );
    return is_subclass_name( typelist, to_const_ObjectType(sub)->name, sup );
}

/* Given a list of types 'typelist', a potential subclass 'sub' and a
 * potential superclass 'sup', return true iff 'sub' is a subclass of 'sup'.
 */
static bool is_subclass_type( const_TypeEntry_list typelist, const_type sub, const_type sup )
{
    if( sub->tag != TAGObjectType || sup->tag != TAGObjectType ){
	return false;
    }
    return is_subclass_name(
	typelist,
	to_const_ObjectType( sub )->name,
	to_const_ObjectType( sup )->name
    );
}

// Forward declaration.
static bool is_implemented( const_TypeEntry_list typelist, const_type sup, const_origsymbol inf );

/* Given a list of types 'typelist', a potential interface 'inf' and a
 * potential class or interface 'obj' that may have implemented it, return
 * true iff 'inf' is implemented by 'obj'.
 */
static bool is_implemented( const_TypeEntry_list typelist, const_origsymbol obj, const_origsymbol inf )
{
    if( inf == origsymbolNIL || obj == origsymbolNIL ){
	return false;
    }
    const_TypeEntry inf_entry = search_TypeEntry( typelist, inf->sym );
    const_TypeEntry obj_entry = search_TypeEntry( typelist, obj->sym );
    if( inf_entry == TypeEntryNIL || obj_entry == TypeEntryNIL ){
	return false;
    }
    if( inf_entry->tag != TAGInterfaceEntry ){
	return false;
    }
    if( inf->sym == obj->sym ){
	return true;
    }
    switch( obj_entry->tag ){
	case TAGClassEntry:
	{
	    const_ClassEntry cl = to_const_ClassEntry( obj_entry );
	    type_list interfaces = cl->interfaces;

	    for( unsigned int ix=0; ix<interfaces->sz; ix++ ){
		if( is_implemented( typelist, interfaces->arr[ix], inf ) ){
		    return true;
		}
	    }
	    if( cl->super != typeNIL && is_implemented( typelist, cl->super, inf ) ){
		return true;
	    }
	    break;
	}

	case TAGInterfaceEntry:
	{
	    type_list interfaces = to_const_InterfaceEntry( obj_entry )->interfaces;

	    for( unsigned int ix=0; ix<interfaces->sz; ix++ ){
		if( is_implemented( typelist, interfaces->arr[ix], inf ) ){
		    return true;
		}
	    }
	    break;
	}

	case TAGForwardObjectEntry:
	case TAGPackageEntry:
	    break;
    }
    return false;
}

static bool is_implemented( const_TypeEntry_list typelist, const_type sup, const_origsymbol inf )
{
    if( sup->tag != TAGObjectType ){
	return false;
    }
    return is_implemented(
	typelist,
	to_const_ObjectType( sup )->name,
	inf
    );
}

/* Given a list of types 'typelist', a potential implemented interface 'inf'
 * and a class or interface 'sup' that may have implemented it, return true
 * iff 'inf' is implemented by 'sup'.
 */
static bool is_implemented( const_TypeEntry_list typelist, const_type sup, const_type inf )
{
    if( inf->tag != TAGObjectType ){
	return false;
    }
    return is_implemented(
	typelist,
	sup,
	to_const_ObjectType( inf )->name
    );
}

/* Given a type entry 'e', return true if it is an interface type.  */
static bool is_interface_type( const_TypeEntry e )
{
    if( e == TypeEntryNIL ){
	return false;
    }
    return (e->tag == TAGInterfaceEntry);
}

/* Given a list of types 'typelist' and a type 't', return true if it is
 * an interface type.
 */
bool is_interface_type( const_TypeEntry_list typelist, const_origsymbol name )
{
    TypeEntry e = lookup_TypeEntry( typelist, name );
    return is_interface_type( e );
}

/* Given a list of types 'typelist' and a type 't', return true if it is
 * an interface type.
 */
static bool is_interface_type( const_TypeEntry_list typelist, const_type t )
{
    if( t->tag != TAGObjectType ){
	return false;
    }
    return is_interface_type( typelist, to_const_ObjectType( t )->name );
}

/* Given a list of types 'typelist' and a type 't', return true if it
 * is a final type.
 */
static bool is_final_type( const_TypeEntry e )
{
    if( e == TypeEntryNIL || !is_ConcreteTypeEntry( e ) ){
	return false;
    }
    return (to_const_ConcreteTypeEntry(e)->flags & ACC_FINAL) != 0;
}

/* Given a list of types 'typelist' and a type 't', return true if it
 * is a final type.
 */
static bool is_final_type( const_TypeEntry_list typelist, const_type t )
{
    if( t->tag != TAGObjectType ){
	return false;
    }
    TypeEntry e = lookup_TypeEntry( typelist, to_const_ObjectType( t )->name );
    return is_final_type( e );
}

static type build_TupleType( type basetype, expression power )
{
    unsigned int sz;

    if( extract_vector_power( originNIL, power, &sz ) ){
	type_list tl = setroom_type_list( new_type_list(), sz );
	for( unsigned int n=0; n<sz; n++ ){
	    tl = append_type_list( tl, rdup_type( basetype ) );
	}
	return new_TupleType( tl );
    }
    return typeNIL;
}

/* Given a type 'a' and a type 'b', return true iff the types are
 * identical.
 */
bool is_identity_conv_type( const_Entry_list symtab, const_type a, const_type b )
{
    if( a == typeNIL || b == typeNIL ){
	return true;
    }
    if( a->tag == TAGTypeOf ){
	type ta = derive_type_expression( symtab, to_const_TypeOf(a)->x );
	bool res = is_identity_conv_type( symtab, ta, b );

	rfre_type( ta );
	return res;
    }
    if( b->tag == TAGTypeOf ){
	type tb = derive_type_expression( symtab, to_const_TypeOf(b)->x );
	bool res = is_identity_conv_type( symtab, a, tb );

	rfre_type( tb );
	return res;
    }
    if( b->tag == TAGPragmaType ){
	return is_identity_conv_type( symtab, a, to_const_PragmaType(b)->t );
    }
    if( a->tag == TAGPragmaType ){
	return is_identity_conv_type( symtab, to_const_PragmaType(a)->t, b );
    }
    if( a->tag == TAGVectorType && a->tag != b->tag ){
	const_VectorType pt = to_const_VectorType(a);
	type vt = build_TupleType( pt->elmtype, pt->power );
	bool res = is_identity_conv_type( symtab, vt, b );
	rfre_type( vt );
	return res;
    }
    if( b->tag == TAGVectorType && a->tag != b->tag ){
	const_VectorType pt = to_const_VectorType(b);
	type vt = build_TupleType( pt->elmtype, pt->power );
	bool res = is_identity_conv_type( symtab, a, vt );
	rfre_type( vt );
	return res;
    }
    if( a->tag != b->tag ){
	return false;
    }
    switch( a->tag ){
	case TAGPragmaType:
	    return is_identity_conv_type( symtab, to_const_PragmaType(a)->t, b );

	case TAGPrimitiveType:
	{
	    const_PrimitiveType bta = to_const_PrimitiveType(a);
	    const_PrimitiveType btb = to_const_PrimitiveType(b);
	    return (bta->base == btb->base);
	}

	case TAGTypeType:
	    return true;

	case TAGArrayType:
	{
	    const_ArrayType aa = to_const_ArrayType(a);
	    const_ArrayType ab = to_const_ArrayType(b);

	    // TODO: check ranks of arrays
	    return
		isequal_expression( aa->rank, ab->rank )
		&&
		is_identity_conv_type(
		    symtab,
		    aa->elmtype,
		    ab->elmtype
		);
	}

	case TAGPrimArrayType:
	    return is_identity_conv_type(
		symtab,
		to_const_PrimArrayType(a)->elmtype,
		to_const_PrimArrayType(b)->elmtype
	    );

	case TAGTupleType:
	    return is_identity_conv_type_list(
		symtab,
		to_const_TupleType(a)->fields,
		to_const_TupleType(b)->fields
	    );

	case TAGVectorType:
	    return is_identity_conv_type(
		symtab,
		to_const_VectorType(a)->elmtype,
		to_const_VectorType(b)->elmtype
	    ) &&
	    (
		isequal_expression( 
		    to_const_VectorType(a)->power,
		    to_const_VectorType(b)->power
		)
	    )
	    ;

	case TAGNullType:
	case TAGVoidType:
	case TAGExceptionVariableType:
	case TAGGCRefLinkType:
	case TAGGCTopRefLinkType:
	    return true;

	case TAGFunctionType:
	    return is_identity_conv_type_list(
		    symtab,
		    to_const_FunctionType(a)->formals,
		    to_const_FunctionType(b)->formals
		) &&
	        is_identity_conv_type(
		    symtab,
		    to_const_FunctionType(a)->rettype,
		    to_const_FunctionType(b)->rettype
		);

	case TAGGenericObjectType:
	    internal_error( "unhandled generic object type" );
	    break;

	case TAGObjectType:
	    return (
		to_const_ObjectType(a)->name != origsymbolNIL &&
		to_const_ObjectType(b)->name != origsymbolNIL &&
		to_const_ObjectType(a)->name->sym == to_const_ObjectType(b)->name->sym
	    );

	case TAGTypeOf:
	    internal_error( "unhandled typeof" );
	    break;

	case TAGTypeOfIf:
	    internal_error( "unhandled typeofif" );
	    break;
    }
    return false;
}

static bool is_identity_conv_type_list( const_Entry_list symtab, const_type_list la, const_type_list lb )
{
    if( la->sz != lb->sz ){
	return false;
    }
    for( unsigned int ix=0; ix<la->sz; ix++ ){
	if( !is_identity_conv_type( symtab, la->arr[ix], lb->arr[ix] ) ){
	    return false;
	}
    }
    return true;
}

bool is_equivalent_type( const_type a, const_type b )
{
    if( a->tag == TAGPragmaType ){
	return is_equivalent_type( to_const_PragmaType(a)->t, b );
    }
    if( b->tag == TAGPragmaType ){
	return is_equivalent_type( a, to_const_PragmaType(b)->t );
    }
    if( a->tag == TAGTupleType && b->tag == TAGVectorType ){
	unsigned int sz_b;
	const_TupleType ta = to_const_TupleType(a);
	const_VectorType vb = to_const_VectorType(b);

	if( !extract_vector_power( originNIL, vb->power, &sz_b ) ){
	    return false;
	}
	if( sz_b != ta->fields->sz ){
	    return false;
	}
	for( unsigned int ix=0; ix<sz_b; ix++ ){
	    if( !is_equivalent_type( ta->fields->arr[ix], vb->elmtype ) ){
		return false;
	    }
	}
	return true;
    }
    if( a->tag == TAGVectorType && b->tag == TAGTupleType ){
	unsigned int sz_a;
	const_TupleType tb = to_const_TupleType(b);
	const_VectorType va = to_const_VectorType(a);

	if( !extract_vector_power( originNIL, va->power, &sz_a ) ){
	    return false;
	}
	if( sz_a != tb->fields->sz ){
	    return false;
	}
	for( unsigned int ix=0; ix<sz_a; ix++ ){
	    if( !is_equivalent_type( va->elmtype, tb->fields->arr[ix] ) ){
		return false;
	    }
	}
	return true;
    }
    return isequal_type( a, b );
}

bool is_invocationequivalent_type_list( const_Entry_list symtab, const_TypeEntry_list typelist, const_type_list la, const_type_list lb )
{
    if( la->sz != lb->sz ){
	return false;
    }
    for( unsigned int ix=0; ix<la->sz; ix++ ){
	if( !is_invocationequivalent_type( symtab, typelist, la->arr[ix], lb->arr[ix] ) ){
	    return false;
	}
    }
    return true;
}

/* Given a type 't', return true if it is type 'complex'. */
bool is_complex_type( const_Entry_list symtab, const_type t )
{
    if( t == typeNIL ){
	return false;
    }
    switch( t->tag ){
        case TAGPrimitiveType:
            return to_const_PrimitiveType(t)->base == BT_COMPLEX;

	case TAGTypeOf:
	{
	    type tx = derive_type_expression( symtab, to_const_TypeOf(t)->x );
	    bool res = is_complex_type( symtab, tx );

	    rfre_type( tx );
	    return res;
	}

	case TAGTypeOfIf:
	{
	    type tx = calculate_ifexpression_type(
		symtab,
		to_const_TypeOfIf(t)->org,
		to_const_TypeOfIf(t)->thenval,
		to_const_TypeOfIf(t)->elseval
	    );
	    bool res = is_complex_type( symtab, tx );

	    rfre_type( tx );
	    return res;
	}

	case TAGPragmaType:
	    return is_complex_type( symtab, to_const_PragmaType(t)->t );

	case TAGExceptionVariableType:
	case TAGGCRefLinkType:
	case TAGGCTopRefLinkType:
        case TAGArrayType:
        case TAGFunctionType:
        case TAGGenericObjectType:
        case TAGNullType:
        case TAGObjectType:
        case TAGPrimArrayType:
        case TAGTupleType:
        case TAGTypeType:
        case TAGVectorType:
        case TAGVoidType:
            return false;

    }
    return false;
}

/* Given a type 't', return true if it is type 'boolean'. */
bool is_boolean_type( const_Entry_list symtab, const_type t )
{
    if( t == typeNIL ){
	return false;
    }
    switch( t->tag ){
        case TAGPrimitiveType:
            return to_const_PrimitiveType(t)->base == BT_BOOLEAN;

	case TAGTypeOf:
	{
	    type tx = derive_type_expression( symtab, to_const_TypeOf(t)->x );
	    bool res = is_boolean_type( symtab, tx );

	    rfre_type( tx );
	    return res;
	}

	case TAGTypeOfIf:
	{
	    type tx = calculate_ifexpression_type(
		symtab,
		to_const_TypeOfIf(t)->org,
		to_const_TypeOfIf(t)->thenval,
		to_const_TypeOfIf(t)->elseval
	    );
	    bool res = is_boolean_type( symtab, tx );

	    rfre_type( tx );
	    return res;
	}

	case TAGPragmaType:
	    return is_boolean_type( symtab, to_const_PragmaType(t)->t );

	case TAGExceptionVariableType:
	case TAGGCRefLinkType:
	case TAGGCTopRefLinkType:
        case TAGArrayType:
        case TAGPrimArrayType:
        case TAGFunctionType:
        case TAGGenericObjectType:
        case TAGNullType:
        case TAGObjectType:
        case TAGTupleType:
        case TAGTypeType:
        case TAGVectorType:
        case TAGVoidType:
            return false;

    }
    return false;
}

/* Given a basetype 't', return true if it is a integral type. */
static bool is_integral_basetype( const BASETYPE t )
{
    switch( t ){
	case BT_BYTE:
	case BT_SHORT:
	case BT_INT:
	case BT_LONG:
	case BT_CHAR:
	    return true;

	case BT_COMPLEX:
	case BT_FLOAT:
	case BT_DOUBLE:
	case BT_STRING:
	case BT_BOOLEAN:
	    break;
    }
    return false;
}

/* Given a type 't', return true if it is a integral type. */
bool is_integral_type( const_Entry_list symtab, const_type t )
{
    if( t == typeNIL ){
        return true;
    }
    switch( t->tag ){
        case TAGPrimitiveType:
            return is_integral_basetype( to_const_PrimitiveType(t)->base );

	case TAGTypeOf:
	{
	    type tx = derive_type_expression( symtab, to_const_TypeOf(t)->x );
	    bool res = is_integral_type( symtab, tx );

	    rfre_type( tx );
	    return res;
	}

	case TAGTypeOfIf:
	{
	    type tx = calculate_ifexpression_type(
		symtab,
		to_const_TypeOfIf(t)->org,
		to_const_TypeOfIf(t)->thenval,
		to_const_TypeOfIf(t)->elseval
	    );
	    bool res = is_integral_type( symtab, tx );

	    rfre_type( tx );
	    return res;
	}

	case TAGPragmaType:
	    return is_integral_type( symtab, to_const_PragmaType(t)->t );

	case TAGExceptionVariableType:
	case TAGGCRefLinkType:
	case TAGGCTopRefLinkType:
        case TAGArrayType:
        case TAGPrimArrayType:
        case TAGFunctionType:
        case TAGGenericObjectType:
        case TAGNullType:
        case TAGObjectType:
        case TAGTupleType:
        case TAGTypeType:
        case TAGVectorType:
        case TAGVoidType:
            return false;

    }
    return false;
}

/* Given a basetype 't', return true if it is a numeric type. */
static bool is_numeric_basetype( const BASETYPE t )
{
    switch( t ){
	case BT_BYTE:
	case BT_SHORT:
	case BT_INT:
	case BT_LONG:
	case BT_FLOAT:
	case BT_DOUBLE:
	case BT_CHAR:
	case BT_COMPLEX:
	    return true;

	case BT_STRING:
	case BT_BOOLEAN:
	    break;
    }
    return false;
}

/* Given a type 't', return true if it is a numeric type. */
bool is_numeric_type( const_Entry_list symtab, const_type t )
{
    if( t == typeNIL ){
        return true;
    }
    switch( t->tag ){
        case TAGPrimitiveType:
            return is_numeric_basetype( to_const_PrimitiveType(t)->base );

	case TAGTypeOf:
	{
	    type tx = derive_type_expression( symtab, to_const_TypeOf(t)->x );
	    bool res = is_numeric_type( symtab, tx );

	    rfre_type( tx );
	    return res;
	}

	case TAGTypeOfIf:
	{
	    type tx = calculate_ifexpression_type(
		symtab,
		to_const_TypeOfIf(t)->org,
		to_const_TypeOfIf(t)->thenval,
		to_const_TypeOfIf(t)->elseval
	    );
	    bool res = is_numeric_type( symtab, tx );

	    rfre_type( tx );
	    return res;
	}

	case TAGPragmaType:
	    return is_numeric_type( symtab, to_const_PragmaType(t)->t );

	case TAGExceptionVariableType:
	case TAGGCRefLinkType:
	case TAGGCTopRefLinkType:
        case TAGArrayType:
        case TAGPrimArrayType:
        case TAGFunctionType:
        case TAGGenericObjectType:
        case TAGNullType:
        case TAGObjectType:
        case TAGTupleType:
        case TAGTypeType:
        case TAGVectorType:
        case TAGVoidType:
            return false;

    }
    return false;
}

/* Given two basetypes 'from' and 'to', return true if conversion of type
 * 'from' to type 'to' is a widening primitive conversion as defined in
 * JLS2 5.1.2.
 */
bool is_widening_prim_basetype( const BASETYPE from, const BASETYPE to )
{
    switch( from ){
	case BT_BYTE:
	    return (to==BT_SHORT||to==BT_INT||to==BT_LONG||to==BT_FLOAT||to==BT_DOUBLE||to==BT_COMPLEX);

	case BT_CHAR:
	case BT_SHORT:
	    return (to==BT_INT||to==BT_LONG||to==BT_FLOAT||to==BT_DOUBLE||to==BT_COMPLEX);

	case BT_INT:
	    return (to==BT_LONG||to==BT_FLOAT||to==BT_DOUBLE||to==BT_COMPLEX);

	case BT_LONG:
	    return (to==BT_FLOAT||to==BT_DOUBLE||to==BT_COMPLEX);

	case BT_FLOAT:
	    return (to==BT_DOUBLE||to==BT_COMPLEX);

	case BT_DOUBLE:
	    return (to==BT_COMPLEX);

        case BT_COMPLEX:
	case BT_STRING:
	case BT_BOOLEAN:
	    break;

    }
    return false;
}

/* Given two types 'from' and 'to', return true if conversion of type
 * 'from' to type 'to' is a widening primitive conversion as defined in
 * JLS2 5.1.2.
 */
static bool is_widening_prim_type( const_Entry_list symtab, const_type from, const_type to )
{
    if( to->tag == TAGTypeOf ){
	type tto = derive_type_expression( symtab, to_const_TypeOf(to)->x );
	bool res = is_widening_prim_type( symtab, tto, from );

	rfre_type( tto );
	return res;
    }
    if( from->tag == TAGTypeOf ){
	type tfrom = derive_type_expression( symtab, to_const_TypeOf(from)->x );
	bool res = is_widening_prim_type( symtab, to, tfrom );

	rfre_type( tfrom );
	return res;
    }
    if( from->tag == TAGPragmaType ){
	return is_widening_prim_type( symtab, to_const_PragmaType(from)->t, to );
    }
    if( to->tag == TAGPragmaType ){
	return is_widening_prim_type( symtab, from, to_const_PragmaType(to)->t );
    }
    if( from->tag != TAGPrimitiveType || to->tag != TAGPrimitiveType ){
	return false;
    }
    return is_widening_prim_basetype(
	to_const_PrimitiveType(from)->base,
	to_const_PrimitiveType(to)->base
    );
}

/* Given two basetypes 'from' and 'to', return true if conversion of type
 * 'from' to type 'to' is a narrowing primitive conversion as defined in
 * JLS2 5.1.3.
 */
static bool is_narrowing_prim_basetype( const BASETYPE from, const BASETYPE to )
{
    switch( from ){
	case BT_BYTE:
	    return (to==BT_CHAR);

	case BT_SHORT:
	    return (to==BT_BYTE||to==BT_CHAR);

	case BT_CHAR:
	    return (to==BT_BYTE||to==BT_SHORT);

	case BT_INT:
	    return (to==BT_BYTE||to==BT_SHORT||to==BT_CHAR);

	case BT_LONG:
	    return (to==BT_BYTE||to==BT_SHORT||to==BT_CHAR||to==BT_INT);

	case BT_FLOAT:
	    return (to==BT_BYTE||to==BT_SHORT||to==BT_CHAR||to==BT_INT||to==BT_LONG);

	case BT_DOUBLE:
	    return (to==BT_BYTE||to==BT_SHORT||to==BT_CHAR||to==BT_INT||to==BT_LONG||to==BT_FLOAT);

        case BT_COMPLEX:
	    return (to==BT_BYTE||to==BT_SHORT||to==BT_CHAR||to==BT_INT||to==BT_LONG||to==BT_FLOAT||to==BT_DOUBLE);

	case BT_STRING:
	case BT_BOOLEAN:
	    break;

    }
    return false;
}

/* Given two types 'from' and 'to', return true if conversion of type
 * 'from' to type 'to' is a narrowing primitive conversion as defined in
 * JLS2 5.1.3.
 */
static bool is_narrowing_prim_type( const_Entry_list symtab, const_type from, const_type to )
{
    if( to->tag == TAGTypeOf ){
	type tto = derive_type_expression( symtab, to_const_TypeOf(to)->x );
	bool res = is_narrowing_prim_type( symtab, tto, from );

	rfre_type( tto );
	return res;
    }
    if( from->tag == TAGTypeOf ){
	type tfrom = derive_type_expression( symtab, to_const_TypeOf(from)->x );
	bool res = is_narrowing_prim_type( symtab, to, tfrom );

	rfre_type( tfrom );
	return res;
    }
    if( from->tag == TAGPragmaType ){
	return is_narrowing_prim_type( symtab, to_const_PragmaType(from)->t, to );
    }
    if( to->tag == TAGPragmaType ){
	return is_narrowing_prim_type( symtab, from, to_const_PragmaType(to)->t );
    }
    if( from->tag != TAGPrimitiveType || to->tag != TAGPrimitiveType ){
	return false;
    }
    return is_narrowing_prim_basetype(
	to_const_PrimitiveType(from)->base,
	to_const_PrimitiveType(to)->base
    );
}

/* Return true iff 'from' can be converted to 'to' with a widening
 * reference conversion according to JLS2 5.1.4.
 */
static bool is_widening_ref_type( const_Entry_list symtab, const_TypeEntry_list typelist, const_type from, const_type to )
{
    if( to->tag == TAGTypeOf ){
	type tto = derive_type_expression( symtab, to_const_TypeOf(to)->x );
	bool res = is_widening_ref_type( symtab, typelist, tto, from );

	rfre_type( tto );
	return res;
    }
    if( from->tag == TAGTypeOf ){
	type tfrom = derive_type_expression( symtab, to_const_TypeOf(from)->x );
	bool res = is_widening_ref_type( symtab, typelist, to, tfrom );

	rfre_type( tfrom );
	return res;
    }
    if( from->tag == TAGPragmaType ){
	return is_widening_ref_type( symtab, typelist, to_const_PragmaType(from)->t, to );
    }
    if( to->tag == TAGPragmaType ){
	return is_widening_ref_type( symtab, typelist, from, to_const_PragmaType(to)->t );
    }
    if( from->tag == TAGNullType && is_ReferenceType( to ) ){
	return true;
    }
    // This is an addition to the Java rules to accomodate our internal
    // handling of Vnus strings.
    //
    // Conversion from __string to java.lang.String or Object is considered a
    // widening conversion.
    // The subsequent cast should be implemented as the internalization
    // of strings as prescribed by Java.
    if(
	is_vnusstring_type( from ) &&
	(is_String_type( to ) || is_Object_type( to ))
    ){
	return true;
    }
    if( is_subclass_type( typelist, from, to ) ){
	return true;
    }
    if( is_implemented( typelist, from, to ) ){
	return true;
    }
    if(
	to->tag == TAGObjectType &&
	to_const_ObjectType( to )->name != origsymbolNIL &&
	to_const_ObjectType( to )->name->sym == add_tmsymbol( "java.lang.Object" )
    ){
	// TODO: this is in fact not correct, since it *is* possible to
	// have a package that resolves Object to another type.
	if( from->tag == TAGArrayType ){
	    return true;
	}
	if( is_interface_type( typelist, from ) ){
	    return true;
	}
    }
    if(
	// TODO: this is in fact not correct, since it *is* possible to
	// have a package that resolves Clonable to another type.
	to->tag == TAGObjectType &&
	to_const_ObjectType( to )->name != origsymbolNIL &&
	to_const_ObjectType( to )->name->sym == add_tmsymbol( "java.lang.Clonable" )
    ){
	if( from->tag == TAGArrayType ){
	    return true;
	}
    }
    if( from->tag == TAGArrayType && to->tag == TAGArrayType ){
	const_ArrayType from_array = to_const_ArrayType( from );
	const_ArrayType to_array = to_const_ArrayType( to );

	if( isequal_expression( from_array->rank, to_array->rank ) ){
	    if( is_widening_ref_type( symtab, typelist, from_array->elmtype, to_array->elmtype ) ){
		return true;
	    }
	}
    }
    return false;
}

/* Return true iff 'from' can be converted to 'to' with a narrowing
 * reference conversion according to JLS2 5.1.5.
 */
static bool is_narrowing_ref_type( const_Entry_list symtab, const_TypeEntry_list typelist, const_type from, const_type to )
{
    if( to->tag == TAGTypeOf ){
	type tto = derive_type_expression( symtab, to_const_TypeOf(to)->x );
	bool res = is_narrowing_ref_type( symtab, typelist, tto, from );

	rfre_type( tto );
	return res;
    }
    if( from->tag == TAGTypeOf ){
	type tfrom = derive_type_expression( symtab, to_const_TypeOf(from)->x );
	bool res = is_narrowing_ref_type( symtab, typelist, to, tfrom );

	rfre_type( tfrom );
	return res;
    }
    if( from->tag == TAGPragmaType ){
	return is_narrowing_ref_type( symtab, typelist, to_const_PragmaType(from)->t, to );
    }
    if( to->tag == TAGPragmaType ){
	return is_narrowing_ref_type( symtab, typelist, from, to_const_PragmaType(to)->t );
    }
    if( is_subclass_type( typelist, to, from ) ){
	return true;
    }

    if( is_Object_type( from ) && to->tag == TAGArrayType ){
	return true;
    }

    // From Object to any interface type. (CvR: huh?)
    if( is_Object_type( from ) && is_interface_type( typelist, to ) ){
	return true;
    }

    // From any interface type J to any class type T that is not final.
    //
    // From any interface type J to any class type T that is final,
    // provided that T implements J.
    if( is_interface_type( typelist, from ) ){
	if( !is_final_type( typelist, to ) || is_implemented( typelist, to, from ) ){
	    return true;
	}
    }
    else {
	// From any class S to any interface type K, provided that S is not
	// final and does not implement K. (e.g. Object -> interface type).
	if(
	    is_interface_type( typelist, to ) &&
	    !is_final_type( typelist, from ) &&
	    !is_implemented( typelist, from, to )
	){
	    return true;
	}
    }

    // From any interface type J to any inteface type K, provided that
    // J is not a subinterface of K and there is no method name m such that
    // J and K both declare a method named m with the same signature
    // but different return types.
    if( from->tag == TAGArrayType && to->tag == TAGArrayType ){
	const_ArrayType from_array = to_const_ArrayType( from );
	const_ArrayType to_array = to_const_ArrayType( to );

	if( isequal_expression( from_array->rank, to_array->rank ) ){
	    if(
		is_narrowing_ref_type(
		    symtab,
		    typelist,
		    from_array->elmtype,
		    to_array->elmtype
		)
	    ){
		return true;
	    }
	}
    }
    return false;
}

/* Given the type of the lhs and the type of the rhs, return true if they are
 * assignment compatible.
 * This implements JLS2 5.2.
 */
static bool is_narrowing_conversion( const_Entry_list symtab, const_TypeEntry_list typelist, const_type from, const_type to )
{
    if( is_narrowing_prim_type( symtab, from, to ) ){
	return true;
    }
    if( is_narrowing_tuple_type( symtab, typelist, from, to ) ){
	return true;
    }
    return is_narrowing_ref_type( symtab, typelist, from, to );
}

/* Given the type of the lhs and the type of the rhs, return true if they are
 * assignment compatible.
 * This implements JLS2 5.2.
 */
static bool is_widening_conversion( const_Entry_list symtab, const_TypeEntry_list typelist, const_type from, const_type to )
{
    if( is_widening_prim_type( symtab, from, to ) ){
	return true;
    }
    if( is_widening_tuple_type( symtab, typelist, from, to ) ){
	return true;
    }
    return is_widening_ref_type( symtab, typelist, from, to );
}

/* Given the type of the lhs and the type of the rhs, return true if they form
 * a widening tuple conversion.
 */
static bool is_widening_tuple_type( const_Entry_list symtab, const_TypeEntry_list typelist, const_type from, const_type to )
{
    if( from->tag == TAGTupleType ){
	const_TupleType t_from = to_const_TupleType( from );

	if( to->tag == TAGTupleType ){
	    const_TupleType t_to = to_const_TupleType( to );
	    type_list tl_from = t_from->fields;
	    type_list tl_to = t_to->fields;
	    bool any_widening = false;

	    if( tl_from->sz != tl_to->sz ){
		return false;
	    }
	    for( unsigned int ix=0; ix<tl_to->sz; ix++ ){
		if( is_narrowing_conversion( symtab, typelist, tl_from->arr[ix], tl_to->arr[ix] ) ){
		    return false;
		}
		if(
		    !any_widening && 
		    is_widening_conversion( symtab, typelist, tl_from->arr[ix], tl_to->arr[ix] )
		){
		    any_widening = true;
		}
	    }
	    return any_widening;
	}
	else if( to->tag == TAGVectorType ){
	    const_VectorType v_to = to_const_VectorType( to );
	    type_list tl_from = t_from->fields;
	    unsigned int sz_to;
	    bool any_widening = false;

	    if( !extract_vector_power( originNIL, v_to->power, &sz_to ) ){
		return false;
	    }
	    if( sz_to != tl_from->sz ){
		return false;
	    }
	    for( unsigned int ix=0; ix<sz_to; ix++ ){
		if( is_narrowing_conversion( symtab, typelist, tl_from->arr[ix], v_to->elmtype ) ){
		    return false;
		}
		if(
		    !any_widening && 
		    is_widening_conversion( symtab, typelist, tl_from->arr[ix], v_to->elmtype )
		){
		    any_widening = true;
		}
	    }
	    return any_widening;
	}
    }
    else if( from->tag == TAGVectorType ){
	const_VectorType v_from = to_const_VectorType( from );
	unsigned int sz_from;

	if( !extract_vector_power( originNIL, v_from->power, &sz_from ) ){
	    return false;
	}
	if( to->tag == TAGTupleType ){
	    const_TupleType t_to = to_const_TupleType( to );
	    type_list tl_to = t_to->fields;
	    bool any_widening = false;

	    if( sz_from != tl_to->sz ){
		return false;
	    }
	    for( unsigned int ix=0; ix<sz_from; ix++ ){
		if( is_narrowing_conversion( symtab, typelist, v_from->elmtype, tl_to->arr[ix] ) ){
		    return false;
		}
		if(
		    !any_widening && 
		    is_widening_conversion( symtab, typelist, v_from->elmtype, tl_to->arr[ix] )
		){
		    any_widening = true;
		}
	    }
	    return any_widening;
	}
	else if( to->tag == TAGVectorType ){
	    const_VectorType v_to = to_const_VectorType( to );
	    unsigned int sz_to;

	    if( !extract_vector_power( originNIL, v_to->power, &sz_to ) ){
		return false;
	    }
	    // Normally, it is enough that one of the elements causes
	    // a widening. Since both sides are vectors, this implies that
	    // *all* elements cause widening conversion.
	    if(
		sz_from == sz_to &&
		is_widening_conversion( symtab, typelist, v_from->elmtype, v_to->elmtype )
	    ){
		return true;
	    }
	}
    }
    else {
	if( to->tag == TAGTupleType ){
	    type_list tl = to_const_TupleType( to )->fields;

	    for( unsigned int ix=0; ix<tl->sz; ix++ ){
		if( !is_valid_cast( symtab, typelist, from, tl->arr[ix] ) ){
		    return false;
		}
	    }
	    return true;
	}
	else if( to->tag == TAGVectorType ){
	    return is_valid_cast( symtab, typelist, from, to_const_VectorType(to)->elmtype );
	}
    }
    return false;
}

/* Given the type of the lhs and the type of the rhs, return true if they form
 * a widening tuple conversion.
 */
static bool is_narrowing_tuple_type(
 const_Entry_list symtab,
 const_TypeEntry_list typelist,
 const_type from,
 const_type to
)
{
    if( from->tag == TAGTupleType ){
	const_TupleType t_from = to_const_TupleType( from );

	if( to->tag == TAGTupleType ){
	    const_TupleType t_to = to_const_TupleType( to );
	    type_list tl_from = t_from->fields;
	    type_list tl_to = t_to->fields;

	    if( tl_from->sz != tl_to->sz ){
		return false;
	    }
	    for( unsigned int ix=0; ix<tl_to->sz; ix++ ){
		if( is_narrowing_conversion( symtab, typelist, tl_from->arr[ix], tl_to->arr[ix] ) ){
		    return true;
		}
	    }
	    return false;
	}
	else if( to->tag == TAGVectorType ){
	    const_VectorType v_to = to_const_VectorType( to );
	    type_list tl_from = t_from->fields;
	    unsigned int sz_to;

	    if( !extract_vector_power( originNIL, v_to->power, &sz_to ) ){
		return false;
	    }
	    if( sz_to != tl_from->sz ){
		return false;
	    }
	    for( unsigned int ix=0; ix<sz_to; ix++ ){
		if( is_narrowing_conversion( symtab, typelist, tl_from->arr[ix], v_to->elmtype ) ){
		    return true;
		}
	    }
	    return false;
	}
    }
    else if( from->tag == TAGVectorType ){
	const_VectorType v_from = to_const_VectorType( from );
	unsigned int sz_from;

	if( !extract_vector_power( originNIL, v_from->power, &sz_from ) ){
	    return false;
	}
	if( to->tag == TAGTupleType ){
	    const_TupleType t_to = to_const_TupleType( to );
	    type_list tl_to = t_to->fields;

	    if( sz_from != tl_to->sz ){
		return false;
	    }
	    for( unsigned int ix=0; ix<sz_from; ix++ ){
		if( is_narrowing_conversion( symtab, typelist, v_from->elmtype, tl_to->arr[ix] ) ){
		    return true;
		}
	    }
	    return false;
	}
	else if( to->tag == TAGVectorType ){
	    const_VectorType v_to = to_const_VectorType( to );
	    unsigned int sz_to;

	    if( !extract_vector_power( originNIL, v_to->power, &sz_to ) ){
		return false;
	    }
	    // Normally, it is enough that one of the elements causes
	    // a widening. Since both sides are vectors, this implies that
	    // *all* elements cause widening conversion.
	    if(
		sz_from == sz_to &&
		is_narrowing_conversion( symtab, typelist, v_from->elmtype, v_to->elmtype )
	    ){
		return true;
	    }
	}
    }
    return false;
}

/* Given the type of the lhs and the type of the rhs, return true if they are
 * assignment compatible.
 * This implements JLS2 5.2.
 */
bool is_assignequivalent_type(
 const_Entry_list symtab,
 const_TypeEntry_list typelist,
 const_type from,
 const_type to
)
{
    if( from == typeNIL || to == typeNIL ){
	/* We're in trouble anyway, don't add to it. */
	return true;
    }
    if( is_identity_conv_type( symtab, from, to ) ){
	return true;
    }
    if( is_widening_prim_type( symtab, from, to ) ){
	return true;
    }
    if( is_widening_tuple_type( symtab, typelist, from, to ) ){
	return true;
    }
    if( is_widening_ref_type( symtab, typelist, from, to ) ){
	return true;
    }
    /* Some constant conversions are also allowed. Pitty they cannot be
     * checked here.
     */
    return false;
}

/* Given the type of the lhs and the type of the rhs, return true if they are
 * invocation compatible.
 * This implements JLS2 5.2.
 */
bool is_invocationequivalent_type( const_Entry_list symtab, const_TypeEntry_list typelist, const_type from, const_type to )
{
    if( from == typeNIL || to == typeNIL ){
	/* We're in trouble anyway, don't add to it. */
	return true;
    }
    if( is_identity_conv_type( symtab, from, to ) ){
	return true;
    }
    if( is_widening_prim_type( symtab, from, to ) ){
	return true;
    }
    if( is_widening_tuple_type( symtab, typelist, from, to ) ){
	return true;
    }
    if( is_widening_ref_type( symtab, typelist, from, to ) ){
	return true;
    }
    /* Some constant conversions are also allowed. Pitty they cannot be
     * checked here.
     */
    return false;
}

/* Given a type 'from' and a type 'to', return true iff this is a conversion
 * between primitive arrays.
 *
 */
static bool is_primarray_conversion( const_type from, const_type to )
{
    return( from->tag == TAGPrimArrayType && to->tag ==  TAGPrimArrayType );
}

/* Given a type 'from' and a type 'to', return true iff it is allowed
 * to cast type 'from' to type 'to'.
 *
 * This implements JLS2 5.5
 */
bool is_valid_cast( const_Entry_list symtab, const_TypeEntry_list typelist, const_type from, const_type to )
{
    if( from == typeNIL || to == typeNIL ){
	/* We're in trouble anyway, don't add to it. */
	return true;
    }
    if( is_identity_conv_type( symtab, from, to ) ){
	return true;
    }
    if( is_widening_prim_type( symtab, from, to ) ){
	return true;
    }
    if( is_narrowing_prim_type( symtab, from, to ) ){
	return true;
    }
    if( is_widening_tuple_type( symtab, typelist, from, to ) ){
	return true;
    }
    if( is_narrowing_tuple_type( symtab, typelist, from, to ) ){
	return true;
    }
    if( is_widening_ref_type( symtab, typelist, from, to ) ){
	return true;
    }
    if( is_narrowing_ref_type( symtab, typelist, from, to ) ){
	return true;
    }
    if( is_primarray_conversion( from, to ) ){
	return true;
    }
    return false;
}

// Given two expressions 'thenval' and 'elseval' 
// calculate the type of the ?:
// expression of these types.
type calculate_ifexpression_type( const_Entry_list symtab, const_origin org, const_expression thenval, const_expression elseval )
{
    type tthen = derive_type_expression( symtab, thenval );
    type telse = derive_type_expression( symtab, elseval );
    type res = typeNIL;

    if( tthen == typeNIL || telse == typeNIL ){
	rfre_type( tthen );
	rfre_type( telse );
	return typeNIL;
    }
    (void) check_IfExpression_types( symtab, org, tthen, telse );
    // TODO: things are not nearly as simple as this. Make this more
    // sophisticated.
    if( is_numeric_type( symtab, tthen ) && is_numeric_type( symtab, telse ) ){
        // Enforce part of JLS 15.25: if one branch is an int
	// constant that fits, and the other branch is a byte, short or
	// char, the entire expression is the small type.
	// To avoid confusion elsewhere, here we force the int constant
	// to the correct type.
	BASETYPE bt_then = get_BASETYPE( tthen );
	BASETYPE bt_else = get_BASETYPE( telse );
	if( elseval->tag == TAGIntExpression ){
	    vnus_int n = to_const_IntExpression(elseval)->v;

	    if( bt_then == BT_SHORT && n>=VNUS_SHORT_MIN && n<=VNUS_SHORT_MAX ){
		res = rdup_type( tthen );
	    }
	    if( bt_then == BT_CHAR && n>=VNUS_CHAR_MIN && n<=VNUS_CHAR_MAX ){
		res = rdup_type( tthen );
	    }
	    if( bt_then == BT_BYTE && n>=VNUS_BYTE_MIN && n<=VNUS_BYTE_MAX ){
		res = rdup_type( tthen );
	    }
	}
	if( thenval->tag == TAGIntExpression ){
	    vnus_int n = to_const_IntExpression(thenval)->v;

	    if( bt_else == BT_SHORT && n>=VNUS_SHORT_MIN && n<=VNUS_SHORT_MAX ){
		res = rdup_type( telse );
	    }
	    if( bt_else == BT_CHAR && n>=VNUS_CHAR_MIN && n<=VNUS_CHAR_MAX ){
		res = rdup_type( telse );
	    }
	    if( bt_else == BT_BYTE && n>=VNUS_BYTE_MIN && n<=VNUS_BYTE_MAX ){
		res = rdup_type( telse );
	    }
	}
	if( bt_then == BT_SHORT && bt_else == BT_BYTE ){
	    res = rdup_type( tthen );
	}
	if( bt_then == BT_BYTE && bt_else == BT_SHORT ){
	    res = rdup_type( telse );
	}
	if( bt_then == bt_else ){
	    res = rdup_type( telse );
	}
    }
    if( is_vnusstring_type( telse ) && is_String_type( tthen ) ){
	res = rdup_type( tthen );
    }
    if( is_String_type( telse ) && is_vnusstring_type( tthen ) ){
	res = rdup_type( telse );
    }
    if(
	(is_vnusstring_type( telse ) && tthen->tag == TAGNullType ) ||
	(telse->tag == TAGNullType  && is_vnusstring_type( tthen ) )
    ){
	res = new_ObjectType( add_origsymbol( "java.lang.String" ) );
    }
    if( res == typeNIL ){
	// Nobody yet had a strong opinion about the result type, so
	// apply a 'max' operator on it.
	res =  max_types( tthen, telse );
    }
    rfre_type( tthen );
    rfre_type( telse );
    return res;
}
