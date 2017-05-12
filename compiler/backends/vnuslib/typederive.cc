/* File: typederive.cc
 *
 * Type derivation functions.
 */

#include <assert.h>
#include <tmc.h>

#include "vnusbasetypes.h"
#include "tmadmin.h"
#include "vnusctl.h"

/* Forward declarations. */
field_list derive_fields_expression_list( const_declaration_list decls, const_origin org, const_expression_list l );

/* Given a symbol 'nm' return the type of that variable. */
static type derive_type_name( const_declaration_list decls, const_origsymbol nm )
{
    return lookup_vartype( decls, nm );
}

// Given the type of a shape or array, return the type of an element of it.
static type derive_type_selection( const_type t )
{
    type ans = typeNIL;
    if( t == typeNIL ){
	return typeNIL;
    }
    switch( t->tag ){
	case TAGTypeBase:
	case TAGTypePointer:
	case TAGTypeRecord:
	case TAGTypeNamedRecord:
	case TAGTypeNeutralPointer:
	case TAGTypeFunction:
	case TAGTypeProcedure:
	    internal_error( "Cannot select element" );
	    break;

	case TAGTypePragmas:
	    ans = derive_type_selection( to_const_TypePragmas(t)->t );
	    break;

	case TAGTypeMap:
	    ans = derive_type_selection( to_const_TypeMap(t)->t );
	    break;

	case TAGTypeShape:
	    ans = rdup_type( to_const_TypeShape(t)->elmtype );
	    break;

	case TAGTypeUnsizedArray:
	    ans = rdup_type( to_const_TypeUnsizedArray(t)->elmtype );
	    break;

	case TAGTypeArray:
	    ans = rdup_type( to_const_TypeArray(t)->elmtype );
	    break;


    }
    return ans;
}

// Given a list of declarations, the origin of the expression, and an
// expression representing a shape or array, return the type of an element
// of the shape or array.
static type derive_type_selection( const_declaration_list decls, const_origin org, const_expression selected )
{
    type ans = typeNIL;
    type t = derive_type_expression( decls, org, selected );

    ans = derive_type_selection( t );
    rfre_type( t );
    return ans;
}

// Given the type of a shape or array, return the type of an element of it.
static type derive_type_getbuf( const_type t )
{
    type ans = typeNIL;
    if( t == typeNIL ){
	return typeNIL;
    }
    switch( t->tag ){
	case TAGTypeBase:
	case TAGTypePointer:
	case TAGTypeRecord:
	case TAGTypeNamedRecord:
	case TAGTypeNeutralPointer:
	case TAGTypeFunction:
	case TAGTypeProcedure:
	case TAGTypeUnsizedArray:
	case TAGTypeArray:
	    internal_error( "get buffer of this type" );
	    break;

	case TAGTypePragmas:
	    ans = derive_type_getbuf( to_const_TypePragmas(t)->t );
	    break;

	case TAGTypeMap:
	    ans = derive_type_getbuf( to_const_TypeMap(t)->t );
	    break;

	case TAGTypeShape:
	    ans = new_TypeUnsizedArray( rdup_type( to_const_TypeShape(t)->elmtype ) );
	    break;

    }
    return ans;
}

// Given a list of declarations, the origin of the expression, and an
// expression representing a shape or array, return the type of an element
// of the shape or array.
static type derive_type_getbuf( const_declaration_list decls, const_origin org, const_expression selected )
{
    type ans = typeNIL;
    type t = derive_type_expression( decls, org, selected );

    ans = derive_type_getbuf( t );
    rfre_type( t );
    return ans;
}

// Given the type of an expression that is being dereferenced, return
// the type of the dereferenced element.
static type derive_type_deref( const_type t )
{
    type ans = typeNIL;

    if( t == typeNIL ){
	return typeNIL;
    }
    switch( t->tag ){
	case TAGTypeBase:
	case TAGTypeRecord:
	case TAGTypeNamedRecord:
	case TAGTypeShape:
	case TAGTypeUnsizedArray:
	case TAGTypeArray:
	case TAGTypeNeutralPointer:
	case TAGTypeFunction:
	case TAGTypeProcedure:
	    internal_error( "Cannot dereference" );
	    break;

	case TAGTypeMap:
	    ans = derive_type_deref( to_const_TypeMap(t)->t );
	    break;

	case TAGTypePragmas:
	    ans = derive_type_deref( to_const_TypePragmas(t)->t );
	    break;

	case TAGTypePointer:
	    ans = rdup_type( to_const_TypePointer(t)->elmtype );
	    break;
    }
    return ans;
}

// Given a type 't' that should represent a record, and a field name,
// return the type of that field.
static type derive_type_field( const_declaration_list decls, const_type t, origsymbol field )
{
    type ans = typeNIL;

    switch( t->tag ){
	case TAGTypeBase:
	case TAGTypePointer:
	case TAGTypeUnsizedArray:
	case TAGTypeArray:
	case TAGTypeNeutralPointer:
	case TAGTypeFunction:
	case TAGTypeProcedure:
	case TAGTypeShape:
	    internal_error( "Cannot select element" );
	    break;

	case TAGTypeMap:
	    ans = derive_type_field( decls, to_const_TypeMap(t)->t, field );
	    break;

	case TAGTypePragmas:
	    ans = derive_type_field( decls, to_const_TypePragmas(t)->t, field );
	    break;

	case TAGTypeNamedRecord:
	{
	    const_field_list fl = lookup_NamedRecord_fields( decls, to_const_TypeNamedRecord(t)->name );
	    int pos = find_field_fields( fl, field );
	    if( pos<0 ){
		internal_error( "no such field in this record" );
	    }
	    else {
		ans = rdup_type( fl->arr[pos]->elmtype );
	    }
	    break;
	}

	case TAGTypeRecord:
	{
	    const_field_list fl = to_const_TypeRecord(t)->fields;
	    int pos = find_field_fields( fl, field );
	    if( pos<0 ){
		internal_error( "no such field in this record" );
	    }
	    else {
		ans = rdup_type( fl->arr[pos]->elmtype );
	    }
	    break;
	}
    }
    return ans;
}

static type derive_type_fieldnumber( const_declaration_list decls, type t, int pos )
{
    type ans = typeNIL;

    switch( t->tag ){
	case TAGTypeBase:
	case TAGTypeShape:
	case TAGTypePointer:
	case TAGTypeUnsizedArray:
	case TAGTypeArray:
	case TAGTypeNeutralPointer:
	case TAGTypeFunction:
	case TAGTypeProcedure:
	    internal_error( "Cannot select element" );
	    break;

	case TAGTypeMap:
	    ans = derive_type_fieldnumber( decls, to_TypeMap(t)->t, pos );
	    break;

	case TAGTypePragmas:
	    ans = derive_type_fieldnumber( decls, to_TypePragmas(t)->t, pos );
	    break;

	case TAGTypeNamedRecord:
	{
	    const_field_list fl = lookup_NamedRecord_fields( decls, to_TypeNamedRecord(t)->name );
	    ans = rdup_type( fl->arr[pos]->elmtype );
	    break;
	}

	case TAGTypeRecord:
	{
	    const_TypeRecord rec = to_const_TypeRecord(t);
	    const_field_list fl = rec->fields;

	    assert( (unsigned int) pos<fl->sz );
	    ans = rdup_type( fl->arr[pos]->elmtype );
	    break;
	}
    }
    return ans;
}

/* Given a location 'sel', return the type of the expression.
 * Since a location is supposed to select a single element the
 * returned type is a basetype.
 *
 * Whether the location really does select a single element is
 * not checked here.
 */
type derive_type_location( const_declaration_list decls, const_origin org, const_location sel )
{
    type ans = typeNIL;

    switch( sel->tag ){
	case TAGLocName:
	    ans = derive_type_name( decls, to_const_LocName(sel)->name );
	    break;

	case TAGLocSelection:
	    ans = derive_type_selection( decls, org, to_const_LocSelection(sel)->shape );
	    break;

	case TAGLocFlatSelection:
	    ans = derive_type_selection( decls, org, to_const_LocFlatSelection(sel)->shape );
	    break;

	case TAGLocDeref:
	{
	    type t = derive_type_expression( decls, org, to_const_LocDeref(sel)->ref );

	    ans = derive_type_deref( t );
	    rfre_type( t );
	    break;
	}

	case TAGLocField:
	{
	    type t = derive_type_expression( decls, org, to_const_LocField(sel)->rec );
	    if( t == typeNIL ){
		break;
	    }
	    ans = derive_type_field( decls, t, to_const_LocField(sel)->field );
	    rfre_type( t );
	    break;
	}

	case TAGLocFieldNumber:
	{
	    const_LocFieldNumber fn = to_const_LocFieldNumber(sel);
	    type t = derive_type_expression( decls, org, fn->rec );
	    if( t == typeNIL ){
		break;
	    }

	    vnus_int pos = fn->field;
	    if( pos<0 ){
		internal_error( "Negative field number" );
		break;
	    }

	    ans = derive_type_fieldnumber( decls, t, pos );
	    rfre_type( t );
	    break;
	}

	case TAGLocWhere:
	    ans = derive_type_location( decls, org, to_const_LocWhere(sel)->l );
	    break;

	case TAGLocWrapper:
	    ans = derive_type_location( decls, org, to_const_LocWrapper(sel)->l );
	    break;

	case TAGLocNotNullAssert:
	    ans = derive_type_location( decls, org, to_const_LocNotNullAssert(sel)->l );
	    break;
    }
    return ans;
}

/* Given two operands 'opa' and 'opb', and an operator 'op', return the
 * type of the binary operator expression. This function is rather sloppy
 * about checking the type of the operators, since this is supposed to
 * happen elsewere.
 */
static type derive_type_op(
    const_type ta,
    BINOP op
)
{
    BASETYPE res = BT_BOOLEAN;

    if( ta == typeNIL ){
	return typeNIL;
    }
    if( ta->tag == TAGTypePragmas ){
	return derive_type_op( to_const_TypePragmas(ta)->t, op );
    }
    if( ta->tag == TAGTypeMap ){
	return derive_type_op( to_const_TypeMap(ta)->t, op );
    }
    if( is_equalitycompare_operator( op ) ){
	return new_TypeBase( BT_BOOLEAN );
    }
    if( ta->tag != TAGTypeBase ){
	/* This is wrong, but that is reported elsewhere */
	return typeNIL;
    }
    BASETYPE bta = to_const_TypeBase(ta)->base;
    switch( op ){
	/* symmetrical operators. */
        case BINOP_AND:
        case BINOP_OR:
        case BINOP_XOR:
        case BINOP_MOD:
        case BINOP_PLUS:
        case BINOP_MINUS:
        case BINOP_TIMES:
        case BINOP_DIVIDE:
	    /* We require that both operands are of the same type, but
	     * that is checked elsewhere. Here we blindly assume this
	     * is true.
	     */
	    res = bta;
	    break;

	/* Comparison operators. */
	case BINOP_SHORTAND:
	case BINOP_SHORTOR:
        case BINOP_EQUAL:
        case BINOP_NOTEQUAL:
        case BINOP_LESS:
        case BINOP_LESSEQUAL:
        case BINOP_GREATER:
        case BINOP_GREATEREQUAL:
	    res = BT_BOOLEAN;
	    break;

	/* Shift operators. */
	case BINOP_SHIFTLEFT:
	case BINOP_SHIFTRIGHT:
	case BINOP_USHIFTRIGHT:
	    res = bta;
	    break;

    }
    return new_TypeBase( res );
}

/* Given two operands 'opa' and 'opb', and an operator 'op', return the
 * type of the binary operator expression. This function is rather sloppy
 * about checking the type of the operators, since this is supposed to
 * happen elsewere.
 */
static type derive_type_op(
    const_declaration_list decls,
    const_origin org,
    const_expression opa,
    BINOP op
)
{
    type ta = derive_type_expression( decls, org, opa );

    type res = derive_type_op( ta, op );
    rfre_type( ta );
    return res;
}

// Given a type of what should be a function, return a copy of the return
// type of that function.
static type derive_type_functioncall( type t )
{
    type ans = typeNIL;

    switch( t->tag ){
	case TAGTypeBase:
	case TAGTypeRecord:
	case TAGTypeNamedRecord:
	case TAGTypeShape:
	case TAGTypeArray:
	case TAGTypeUnsizedArray:
	case TAGTypeNeutralPointer:
	case TAGTypeProcedure:
	case TAGTypePointer:
	    internal_error( "Cannot determine return type" );
	    break;

	case TAGTypeMap:
	    ans = derive_type_functioncall( to_TypeMap(t)->t );
	    break;

	case TAGTypePragmas:
	    ans = derive_type_functioncall( to_TypePragmas(t)->t );
	    break;

	case TAGTypeFunction:
	    ans = rdup_type( to_TypeFunction(t)->rettype );
	    break;
    }
    return ans;
}

/* Given an expression 'x', return the type of that expression.
 * The returned type tree must be deleted by the caller.
 */
type derive_type_expression( const_declaration_list decls, const_origin org, const_expression x )
{
    type ans = typeNIL;

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
        case TAGExprSizeof:
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

        case TAGExprComplex:
	    ans = new_TypeBase( BT_COMPLEX );
	    break;

        case TAGExprGetSize:
        case TAGExprGetLength:
	    ans = new_TypeBase( BT_INT );
	    break;

        case TAGExprFunctionCall:
	{
	    type t = derive_type_expression( decls, org, to_const_ExprFunctionCall(x)->function );
	    if( t != typeNIL ){
		ans = derive_type_functioncall( t );
		rfre_type( t );
	    }
	    break;
	}

        case TAGExprGetBuf:
	    ans = derive_type_getbuf( decls, org, to_const_ExprGetBuf(x)->shape );
	    break;

        case TAGExprSelection:
	    ans = derive_type_selection( decls, org, to_const_ExprSelection(x)->shape );
	    break;

        case TAGExprFlatSelection:
	    ans = derive_type_selection( decls, org, to_const_ExprFlatSelection(x)->shape );
	    break;

	case TAGExprDeref:
	{
	    type t = derive_type_expression( decls, org, to_const_ExprDeref(x)->ref );

 	    ans = derive_type_deref( t );
	    rfre_type( t );
	    break;
	}

        case TAGExprUnop:
	    ans = derive_type_expression( decls, org, to_const_ExprUnop(x)->operand );
	    break;

        case TAGExprBinop:
	    ans = derive_type_op(
		decls,
		org,
		to_const_ExprBinop(x)->left,
		to_const_ExprBinop(x)->optor
	    );
	    break;

        case TAGExprReduction:
	{
	    const_ExprReduction rx = to_const_ExprReduction(x);

	    if( rx->operands->sz == 0 ){
		internal_error( "A reduction expression on an empty list is not allowed" );
		return typeNIL;
	    }
	    ans = derive_type_op(
		decls,
		org,
		rx->operands->arr[0],
		rx->optor
	    );
	    break;
	}

	case TAGExprIf:
	    /* Both sides of the if should have the same type, so we
	     * simply take one.
	     */
	    ans = derive_type_expression( decls, org, to_const_ExprIf(x)->thenval );
	    break;

	case TAGExprWhere:
	    ans = derive_type_expression( decls, org, to_const_ExprWhere(x)->x );
	    break;

        case TAGExprIsRaised:
	case TAGExprIsNewOwner:
	case TAGExprIsNewMaster:
	case TAGExprIsBoundViolated:
	case TAGExprIsUpperBoundViolated:
	case TAGExprIsLowerBoundViolated:
	    ans = new_TypeBase( BT_BOOLEAN );
	    break;

	case TAGExprGetNewOwner:
	case TAGExprGetNewMaster:
	case TAGExprCheckedIndex:
	case TAGExprUpperCheckedIndex:
	case TAGExprLowerCheckedIndex:
	    ans = new_TypeBase( BT_INT );
	    break;

        case TAGExprName:
	    ans = derive_type_name( decls, to_const_ExprName(x)->name );
	    break;

	case TAGExprWrapper:
	    ans = derive_type_expression( decls, org, to_const_ExprWrapper(x)->x );
	    break;

	case TAGExprCast:
	    ans = rdup_type( to_const_ExprCast(x)->t );
	    break;

	case TAGExprArray:
	{
	    type elmtype = rdup_type( to_const_ExprArray(x)->elmtype );
	    const_expression_list elms = to_const_ExprArray(x)->elms;

	    ans = new_TypeArray( (int) elms->sz, elmtype );
	    break;
	}

	case TAGExprShape:
	    ans = new_TypeShape(
		rdup_size_list( to_const_ExprShape(x)->sizes ),
		rdup_type( to_const_ExprShape(x)->elmtype )
	    );
	    break;

	case TAGExprRecord:
	    ans = new_TypeRecord(
		derive_fields_expression_list( decls, org, to_const_ExprRecord(x)->fields )
	    );
	    break;

	case TAGExprField:
	{
	    type t = derive_type_expression( decls, org, to_const_ExprField(x)->rec );

	    if( t == typeNIL ){
		break;
	    }
	    ans = derive_type_field( decls, t, to_const_ExprField(x)->field );
	    rfre_type( t );
	    break;
	}

	case TAGExprFieldNumber:
	{
	    const_ExprFieldNumber fn = to_const_ExprFieldNumber(x);
	    vnus_int pos = fn->field;
	    type t = derive_type_expression( decls, org, fn->rec );

	    if( t == typeNIL ){
		break;
	    }
	    if( pos<0 ){
		internal_error( "negative field number" );
	    }
	    ans = derive_type_fieldnumber( decls, t, pos );
	    rfre_type( t );
	    break;
	}

	case TAGExprAddress:
	    ans = new_TypePointer(
	       derive_type_location( decls, org, to_const_ExprAddress(x)->adr )
	    );
	    break;

	case TAGExprNulledNew:
	    ans = new_TypePointer( rdup_type( to_const_ExprNulledNew(x)->t ) );
	    break;

	case TAGExprFilledNew:
	    ans = new_TypePointer( rdup_type( to_const_ExprFilledNew(x)->t ) );
	    break;

	case TAGExprNewArray:
	    ans = new_TypeUnsizedArray( rdup_type( to_const_ExprNew(x)->t ) );
	    break;

	case TAGExprNew:
	    ans = new_TypePointer( rdup_type( to_const_ExprNew(x)->t ) );
	    break;

	case TAGExprNewRecord:
	    ans = new_TypePointer( rdup_type( to_const_ExprNewRecord(x)->t ) );
	    break;

	case TAGExprNotNullAssert:
	    ans = derive_type_expression( decls, org, to_const_ExprNotNullAssert(x)->x );
	    break;
    }
    return ans;
}

field_list derive_fields_expression_list(
    const_declaration_list decls,
    const_origin org,
    const_expression_list l
)
{
    field_list res = setroom_field_list( new_field_list(), l->sz );
    for( unsigned int ix=0; ix<l->sz; ix++ ){
	res = append_field_list(
	    res,
	    new_field(
		gen_origsymbol( "f" ),
		derive_type_expression( decls, org, l->arr[ix] )
	    )
	);
    }
    return res;

}

type derive_type_optexpression( const_declaration_list decls, const_origin org, const_optexpression x )
{
    type ans = typeNIL;

    switch( x->tag ){
	case TAGOptExprNone:
	    break;

	case TAGOptExpr:
	    ans = derive_type_expression( decls, org, to_const_OptExpr(x)->x );
	    break;
    }
    return ans;
}


type derive_type_formalParameter( const_declaration_list decls, const_origsymbol arg )
{
    return rdup_type( lookup_argtype( decls, arg ) );
}

type_list derive_type_expression_list( const_declaration_list decls, const_origin org, const_expression_list xl )
{
    type_list res = setroom_type_list( new_type_list(), xl->sz );

    for( unsigned int ix=0; ix<xl->sz; ix++ ){
	res = append_type_list(
	    res,
	    derive_type_expression( decls, org, xl->arr[ix] )
	);
    }
    return res;
}

