/*
  Equivalency test routines.
  All code below has been generated isequal code, and has then been rewritten
  by hand into is_equivalent code.
  Note: all default cases have been removed.
  The following are not present, use isequal instead:
      tmsymbol, int, vnus_boolean, vnus_short, vnus_byte, vnus_long, vnus_char
      BASETYPE, tmstring, UNOP, BINOP
  Special cases:
      origsymbol: code copied from isequal in vnus backend
      expression, location: ignore wrappers
  
  */

#include <tmc.h>
#include <string.h>

#include "vnusbasetypes.h"
#include "tmadmin.h"
#include "vnusctl.h"

tmbool is_equivalent_origsymbol( const const_origsymbol a, const const_origsymbol b )
{
    if( a == b ){
        return TMTRUE;
    }
    if( a == origsymbolNIL || b == origsymbolNIL ){
        return TMFALSE;
    }
    if( !isequal_tmsymbol( a->sym, b->sym ) ){
        return TMFALSE;
    }
    return TMTRUE;
}


/**************************************************
 *    Equality test routines.                     *
 **************************************************/

static tmbool is_equivalent_OwnerExpr_list( const const_OwnerExpr_list, const const_OwnerExpr_list );
static tmbool is_equivalent_Pragma_list( const const_Pragma_list, const const_Pragma_list );
static tmbool is_equivalent_expression_list( const const_expression_list, const const_expression_list );
static tmbool is_equivalent_size_list( const const_size_list, const const_size_list );
static tmbool is_equivalent_PragmaExpression_list( const const_PragmaExpression_list, const const_PragmaExpression_list );
static tmbool is_equivalent_DistrExpr_list( const const_DistrExpr_list, const const_DistrExpr_list );
static tmbool is_equivalent_field_list( const const_field_list, const const_field_list );
static tmbool is_equivalent_origsymbol_list( const const_origsymbol_list, const const_origsymbol_list );
static tmbool is_equivalent_type_list( const const_type_list, const const_type_list );
static tmbool is_equivalent_OwnerExpr( const const_OwnerExpr, const const_OwnerExpr );
static tmbool is_equivalent_location( const const_location, const const_location );
static tmbool is_equivalent_optexpression( const const_optexpression, const const_optexpression );
static tmbool is_equivalent_type( const const_type, const const_type );
static tmbool is_equivalent_Pragma( const const_Pragma, const const_Pragma );
static tmbool is_equivalent_size( const const_size, const const_size );
static tmbool is_equivalent_PragmaExpression( const const_PragmaExpression, const const_PragmaExpression );
static tmbool is_equivalent_origin( const const_origin, const const_origin );
static tmbool is_equivalent_BooleanPragmaExpression( const const_BooleanPragmaExpression, const const_BooleanPragmaExpression );
static tmbool is_equivalent_ExternalNamePragmaExpression( const const_ExternalNamePragmaExpression, const const_ExternalNamePragmaExpression );
static tmbool is_equivalent_ListPragmaExpression( const const_ListPragmaExpression, const const_ListPragmaExpression );
static tmbool is_equivalent_NamePragmaExpression( const const_NamePragmaExpression, const const_NamePragmaExpression );
static tmbool is_equivalent_NumberPragmaExpression( const const_NumberPragmaExpression, const const_NumberPragmaExpression );
static tmbool is_equivalent_StringPragmaExpression( const const_StringPragmaExpression, const const_StringPragmaExpression );
static tmbool is_equivalent_FlagPragma( const const_FlagPragma, const const_FlagPragma );
static tmbool is_equivalent_SizeDontcare( const const_SizeDontcare, const const_SizeDontcare );
static tmbool is_equivalent_SizeExpression( const const_SizeExpression, const const_SizeExpression );
static tmbool is_equivalent_ValuePragma( const const_ValuePragma, const const_ValuePragma );
static tmbool is_equivalent_field( const const_field, const const_field );
static tmbool is_equivalent_LocDeref( const const_LocDeref, const const_LocDeref );
static tmbool is_equivalent_LocField( const const_LocField, const const_LocField );
static tmbool is_equivalent_LocFieldNumber( const const_LocFieldNumber, const const_LocFieldNumber );
static tmbool is_equivalent_LocFlatSelection( const const_LocFlatSelection, const const_LocFlatSelection );
static tmbool is_equivalent_LocName( const const_LocName, const const_LocName );
static tmbool is_equivalent_LocNotNullAssert( const const_LocNotNullAssert, const const_LocNotNullAssert );
static tmbool is_equivalent_LocSelection( const const_LocSelection, const const_LocSelection );
static tmbool is_equivalent_LocWhere( const const_LocWhere, const const_LocWhere );
static tmbool is_equivalent_LocWrapper( const const_LocWrapper, const const_LocWrapper );
static tmbool is_equivalent_NewOwnDontCare( const const_NewOwnDontCare, const const_NewOwnDontCare );
static tmbool is_equivalent_NewOwnLocation( const const_NewOwnLocation, const const_NewOwnLocation );
static tmbool is_equivalent_NewOwnMap( const const_NewOwnMap, const const_NewOwnMap );
static tmbool is_equivalent_NewOwnReplicated( const const_NewOwnReplicated, const const_NewOwnReplicated );
static tmbool is_equivalent_OptExpr( const const_OptExpr, const const_OptExpr );
static tmbool is_equivalent_OptExprNone( const const_OptExprNone, const const_OptExprNone );
static tmbool is_equivalent_TypeArray( const const_TypeArray, const const_TypeArray );
static tmbool is_equivalent_TypeBase( const const_TypeBase, const const_TypeBase );
static tmbool is_equivalent_TypeFunction( const const_TypeFunction, const const_TypeFunction );
static tmbool is_equivalent_TypeMap( const const_TypeMap, const const_TypeMap );
static tmbool is_equivalent_TypeNamedRecord( const const_TypeNamedRecord, const const_TypeNamedRecord );
static tmbool is_equivalent_TypeNeutralPointer( const const_TypeNeutralPointer, const const_TypeNeutralPointer );
static tmbool is_equivalent_TypePointer( const const_TypePointer, const const_TypePointer );
static tmbool is_equivalent_TypePragmas( const const_TypePragmas, const const_TypePragmas );
static tmbool is_equivalent_TypeProcedure( const const_TypeProcedure, const const_TypeProcedure );
static tmbool is_equivalent_TypeRecord( const const_TypeRecord, const const_TypeRecord );
static tmbool is_equivalent_TypeShape( const const_TypeShape, const const_TypeShape );
static tmbool is_equivalent_TypeUnsizedArray( const const_TypeUnsizedArray, const const_TypeUnsizedArray );
static tmbool is_equivalent_ExprAddress( const const_ExprAddress, const const_ExprAddress );
static tmbool is_equivalent_ExprArray( const const_ExprArray, const const_ExprArray );
static tmbool is_equivalent_ExprBinop( const const_ExprBinop, const const_ExprBinop );
static tmbool is_equivalent_ExprBoolean( const const_ExprBoolean, const const_ExprBoolean );
static tmbool is_equivalent_ExprByte( const const_ExprByte, const const_ExprByte );
static tmbool is_equivalent_ExprCast( const const_ExprCast, const const_ExprCast );
static tmbool is_equivalent_ExprChar( const const_ExprChar, const const_ExprChar );
static tmbool is_equivalent_ExprCheckedIndex( const const_ExprCheckedIndex, const const_ExprCheckedIndex );
static tmbool is_equivalent_ExprComplex( const const_ExprComplex, const const_ExprComplex );
static tmbool is_equivalent_ExprDeref( const const_ExprDeref, const const_ExprDeref );
static tmbool is_equivalent_ExprDouble( const const_ExprDouble, const const_ExprDouble );
static tmbool is_equivalent_ExprField( const const_ExprField, const const_ExprField );
static tmbool is_equivalent_ExprFieldNumber( const const_ExprFieldNumber, const const_ExprFieldNumber );
static tmbool is_equivalent_ExprFilledNew( const const_ExprFilledNew, const const_ExprFilledNew );
static tmbool is_equivalent_ExprFlatSelection( const const_ExprFlatSelection, const const_ExprFlatSelection );
static tmbool is_equivalent_ExprFloat( const const_ExprFloat, const const_ExprFloat );
static tmbool is_equivalent_ExprFunctionCall( const const_ExprFunctionCall, const const_ExprFunctionCall );
static tmbool is_equivalent_ExprGetBuf( const const_ExprGetBuf, const const_ExprGetBuf );
static tmbool is_equivalent_ExprGetLength( const const_ExprGetLength, const const_ExprGetLength );
static tmbool is_equivalent_ExprGetNewMaster( const const_ExprGetNewMaster, const const_ExprGetNewMaster );
static tmbool is_equivalent_ExprGetNewOwner( const const_ExprGetNewOwner, const const_ExprGetNewOwner );
static tmbool is_equivalent_ExprGetSize( const const_ExprGetSize, const const_ExprGetSize );
static tmbool is_equivalent_ExprIf( const const_ExprIf, const const_ExprIf );
static tmbool is_equivalent_ExprInt( const const_ExprInt, const const_ExprInt );
static tmbool is_equivalent_ExprIsBoundViolated( const const_ExprIsBoundViolated, const const_ExprIsBoundViolated );
static tmbool is_equivalent_ExprIsLowerBoundViolated( const const_ExprIsLowerBoundViolated, const const_ExprIsLowerBoundViolated );
static tmbool is_equivalent_ExprIsNewMaster( const const_ExprIsNewMaster, const const_ExprIsNewMaster );
static tmbool is_equivalent_ExprIsNewOwner( const const_ExprIsNewOwner, const const_ExprIsNewOwner );
static tmbool is_equivalent_ExprIsRaised( const const_ExprIsRaised, const const_ExprIsRaised );
static tmbool is_equivalent_ExprIsUpperBoundViolated( const const_ExprIsUpperBoundViolated, const const_ExprIsUpperBoundViolated );
static tmbool is_equivalent_ExprLong( const const_ExprLong, const const_ExprLong );
static tmbool is_equivalent_ExprLowerCheckedIndex( const const_ExprLowerCheckedIndex, const const_ExprLowerCheckedIndex );
static tmbool is_equivalent_ExprName( const const_ExprName, const const_ExprName );
static tmbool is_equivalent_ExprNew( const const_ExprNew, const const_ExprNew );
static tmbool is_equivalent_ExprNewArray( const const_ExprNewArray, const const_ExprNewArray );
static tmbool is_equivalent_ExprNewRecord( const const_ExprNewRecord, const const_ExprNewRecord );
static tmbool is_equivalent_ExprNotNullAssert( const const_ExprNotNullAssert, const const_ExprNotNullAssert );
static tmbool is_equivalent_ExprNull( const const_ExprNull, const const_ExprNull );
static tmbool is_equivalent_ExprNulledNew( const const_ExprNulledNew, const const_ExprNulledNew );
static tmbool is_equivalent_ExprRecord( const const_ExprRecord, const const_ExprRecord );
static tmbool is_equivalent_ExprReduction( const const_ExprReduction, const const_ExprReduction );
static tmbool is_equivalent_ExprSelection( const const_ExprSelection, const const_ExprSelection );
static tmbool is_equivalent_ExprShape( const const_ExprShape, const const_ExprShape );
static tmbool is_equivalent_ExprShort( const const_ExprShort, const const_ExprShort );
static tmbool is_equivalent_ExprSizeof( const const_ExprSizeof, const const_ExprSizeof );
static tmbool is_equivalent_ExprString( const const_ExprString, const const_ExprString );
static tmbool is_equivalent_ExprUnop( const const_ExprUnop, const const_ExprUnop );
static tmbool is_equivalent_ExprUpperCheckedIndex( const const_ExprUpperCheckedIndex, const const_ExprUpperCheckedIndex );
static tmbool is_equivalent_ExprWhere( const const_ExprWhere, const const_ExprWhere );
static tmbool is_equivalent_ExprWrapper( const const_ExprWrapper, const const_ExprWrapper );
static tmbool is_equivalent_NewDistAlign( const const_NewDistAlign, const const_NewDistAlign );
static tmbool is_equivalent_NewDistBC( const const_NewDistBC, const const_NewDistBC );
static tmbool is_equivalent_NewDistBlock( const const_NewDistBlock, const const_NewDistBlock );
static tmbool is_equivalent_NewDistCyclic( const const_NewDistCyclic, const const_NewDistCyclic );
static tmbool is_equivalent_NewDistDontCare( const const_NewDistDontCare, const const_NewDistDontCare );
static tmbool is_equivalent_NewDistLocal( const const_NewDistLocal, const const_NewDistLocal );
static tmbool is_equivalent_NewDistReplicated( const const_NewDistReplicated, const const_NewDistReplicated );
/* Compare two DistrExpr class,
 * and return TMTRUE iff they are equal.
 */
tmbool is_equivalent_DistrExpr( const const_DistrExpr a, const const_DistrExpr b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == DistrExprNIL || b == DistrExprNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    switch( a->tag )
    {
	case TAGNewDistDontCare:
	    if( !is_equivalent_NewDistDontCare( to_const_NewDistDontCare(a), to_const_NewDistDontCare(b) ) ){
		return TMFALSE;
	    }
	    break;

	case TAGNewDistBlock:
	    if( !is_equivalent_NewDistBlock( to_const_NewDistBlock(a), to_const_NewDistBlock(b) ) ){
		return TMFALSE;
	    }
	    break;

	case TAGNewDistCyclic:
	    if( !is_equivalent_NewDistCyclic( to_const_NewDistCyclic(a), to_const_NewDistCyclic(b) ) ){
		return TMFALSE;
	    }
	    break;

	case TAGNewDistBC:
	    if( !is_equivalent_NewDistBC( to_const_NewDistBC(a), to_const_NewDistBC(b) ) ){
		return TMFALSE;
	    }
	    break;

	case TAGNewDistReplicated:
	    if( !is_equivalent_NewDistReplicated( to_const_NewDistReplicated(a), to_const_NewDistReplicated(b) ) ){
		return TMFALSE;
	    }
	    break;

	case TAGNewDistLocal:
	    if( !is_equivalent_NewDistLocal( to_const_NewDistLocal(a), to_const_NewDistLocal(b) ) ){
		return TMFALSE;
	    }
	    break;

	case TAGNewDistAlign:
	    if( !is_equivalent_NewDistAlign( to_const_NewDistAlign(a), to_const_NewDistAlign(b) ) ){
		return TMFALSE;
	    }
	    break;
    }
    return TMTRUE;
}

/* Compare two expression class,
 * and return TMTRUE iff they are equal.
 */
tmbool is_equivalent_expression( const const_expression a, const const_expression b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == expressionNIL || b == expressionNIL ){
	return TMFALSE;
    }
	// Extra code
    if( a->tag == TAGExprWrapper )
    {
	return is_equivalent_expression(to_const_ExprWrapper(a)->x,b);
    }
    if( b->tag == TAGExprWrapper )
    {
	return is_equivalent_expression(a,to_const_ExprWrapper(b)->x);
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    switch( a->tag )
    {
	case TAGExprByte:
	    if( !is_equivalent_ExprByte( to_const_ExprByte(a), to_const_ExprByte(b) ) ){
		return TMFALSE;
	    }
	    break;

	case TAGExprShort:
	    if( !is_equivalent_ExprShort( to_const_ExprShort(a), to_const_ExprShort(b) ) ){
		return TMFALSE;
	    }
	    break;

	case TAGExprInt:
	    if( !is_equivalent_ExprInt( to_const_ExprInt(a), to_const_ExprInt(b) ) ){
		return TMFALSE;
	    }
	    break;

	case TAGExprLong:
	    if( !is_equivalent_ExprLong( to_const_ExprLong(a), to_const_ExprLong(b) ) ){
		return TMFALSE;
	    }
	    break;

	case TAGExprFloat:
	    if( !is_equivalent_ExprFloat( to_const_ExprFloat(a), to_const_ExprFloat(b) ) ){
		return TMFALSE;
	    }
	    break;

	case TAGExprDouble:
	    if( !is_equivalent_ExprDouble( to_const_ExprDouble(a), to_const_ExprDouble(b) ) ){
		return TMFALSE;
	    }
	    break;

	case TAGExprChar:
	    if( !is_equivalent_ExprChar( to_const_ExprChar(a), to_const_ExprChar(b) ) ){
		return TMFALSE;
	    }
	    break;

	case TAGExprBoolean:
	    if( !is_equivalent_ExprBoolean( to_const_ExprBoolean(a), to_const_ExprBoolean(b) ) ){
		return TMFALSE;
	    }
	    break;

	case TAGExprString:
	    if( !is_equivalent_ExprString( to_const_ExprString(a), to_const_ExprString(b) ) ){
		return TMFALSE;
	    }
	    break;

	case TAGExprNull:
	    if( !is_equivalent_ExprNull( to_const_ExprNull(a), to_const_ExprNull(b) ) ){
		return TMFALSE;
	    }
	    break;

	case TAGExprSizeof:
	    if( !is_equivalent_ExprSizeof( to_const_ExprSizeof(a), to_const_ExprSizeof(b) ) ){
		return TMFALSE;
	    }
	    break;

	case TAGExprName:
	    if( !is_equivalent_ExprName( to_const_ExprName(a), to_const_ExprName(b) ) ){
		return TMFALSE;
	    }
	    break;

	case TAGExprSelection:
	    if( !is_equivalent_ExprSelection( to_const_ExprSelection(a), to_const_ExprSelection(b) ) ){
		return TMFALSE;
	    }
	    break;

	case TAGExprFlatSelection:
	    if( !is_equivalent_ExprFlatSelection( to_const_ExprFlatSelection(a), to_const_ExprFlatSelection(b) ) ){
		return TMFALSE;
	    }
	    break;

	case TAGExprField:
	    if( !is_equivalent_ExprField( to_const_ExprField(a), to_const_ExprField(b) ) ){
		return TMFALSE;
	    }
	    break;

	case TAGExprFieldNumber:
	    if( !is_equivalent_ExprFieldNumber( to_const_ExprFieldNumber(a), to_const_ExprFieldNumber(b) ) ){
		return TMFALSE;
	    }
	    break;

	case TAGExprDeref:
	    if( !is_equivalent_ExprDeref( to_const_ExprDeref(a), to_const_ExprDeref(b) ) ){
		return TMFALSE;
	    }
	    break;

	case TAGExprFunctionCall:
	    if( !is_equivalent_ExprFunctionCall( to_const_ExprFunctionCall(a), to_const_ExprFunctionCall(b) ) ){
		return TMFALSE;
	    }
	    break;

	case TAGExprComplex:
	    if( !is_equivalent_ExprComplex( to_const_ExprComplex(a), to_const_ExprComplex(b) ) ){
		return TMFALSE;
	    }
	    break;

	case TAGExprShape:
	    if( !is_equivalent_ExprShape( to_const_ExprShape(a), to_const_ExprShape(b) ) ){
		return TMFALSE;
	    }
	    break;

	case TAGExprRecord:
	    if( !is_equivalent_ExprRecord( to_const_ExprRecord(a), to_const_ExprRecord(b) ) ){
		return TMFALSE;
	    }
	    break;

	case TAGExprAddress:
	    if( !is_equivalent_ExprAddress( to_const_ExprAddress(a), to_const_ExprAddress(b) ) ){
		return TMFALSE;
	    }
	    break;

	case TAGExprNewArray:
	    if( !is_equivalent_ExprNewArray( to_const_ExprNewArray(a), to_const_ExprNewArray(b) ) ){
		return TMFALSE;
	    }
	    break;

	case TAGExprNew:
	    if( !is_equivalent_ExprNew( to_const_ExprNew(a), to_const_ExprNew(b) ) ){
		return TMFALSE;
	    }
	    break;

	case TAGExprFilledNew:
	    if( !is_equivalent_ExprFilledNew( to_const_ExprFilledNew(a), to_const_ExprFilledNew(b) ) ){
		return TMFALSE;
	    }
	    break;

	case TAGExprNulledNew:
	    if( !is_equivalent_ExprNulledNew( to_const_ExprNulledNew(a), to_const_ExprNulledNew(b) ) ){
		return TMFALSE;
	    }
	    break;

	case TAGExprNewRecord:
	    if( !is_equivalent_ExprNewRecord( to_const_ExprNewRecord(a), to_const_ExprNewRecord(b) ) ){
		return TMFALSE;
	    }
	    break;

	case TAGExprCast:
	    if( !is_equivalent_ExprCast( to_const_ExprCast(a), to_const_ExprCast(b) ) ){
		return TMFALSE;
	    }
	    break;

	case TAGExprIf:
	    if( !is_equivalent_ExprIf( to_const_ExprIf(a), to_const_ExprIf(b) ) ){
		return TMFALSE;
	    }
	    break;

	case TAGExprWhere:
	    if( !is_equivalent_ExprWhere( to_const_ExprWhere(a), to_const_ExprWhere(b) ) ){
		return TMFALSE;
	    }
	    break;

	case TAGExprUnop:
	    if( !is_equivalent_ExprUnop( to_const_ExprUnop(a), to_const_ExprUnop(b) ) ){
		return TMFALSE;
	    }
	    break;

	case TAGExprBinop:
	    if( !is_equivalent_ExprBinop( to_const_ExprBinop(a), to_const_ExprBinop(b) ) ){
		return TMFALSE;
	    }
	    break;

	case TAGExprReduction:
	    if( !is_equivalent_ExprReduction( to_const_ExprReduction(a), to_const_ExprReduction(b) ) ){
		return TMFALSE;
	    }
	    break;

	case TAGExprIsBoundViolated:
	    if( !is_equivalent_ExprIsBoundViolated( to_const_ExprIsBoundViolated(a), to_const_ExprIsBoundViolated(b) ) ){
		return TMFALSE;
	    }
	    break;

	case TAGExprIsUpperBoundViolated:
	    if( !is_equivalent_ExprIsUpperBoundViolated( to_const_ExprIsUpperBoundViolated(a), to_const_ExprIsUpperBoundViolated(b) ) ){
		return TMFALSE;
	    }
	    break;

	case TAGExprIsLowerBoundViolated:
	    if( !is_equivalent_ExprIsLowerBoundViolated( to_const_ExprIsLowerBoundViolated(a), to_const_ExprIsLowerBoundViolated(b) ) ){
		return TMFALSE;
	    }
	    break;

	case TAGExprCheckedIndex:
	    if( !is_equivalent_ExprCheckedIndex( to_const_ExprCheckedIndex(a), to_const_ExprCheckedIndex(b) ) ){
		return TMFALSE;
	    }
	    break;

	case TAGExprUpperCheckedIndex:
	    if( !is_equivalent_ExprUpperCheckedIndex( to_const_ExprUpperCheckedIndex(a), to_const_ExprUpperCheckedIndex(b) ) ){
		return TMFALSE;
	    }
	    break;

	case TAGExprLowerCheckedIndex:
	    if( !is_equivalent_ExprLowerCheckedIndex( to_const_ExprLowerCheckedIndex(a), to_const_ExprLowerCheckedIndex(b) ) ){
		return TMFALSE;
	    }
	    break;

	case TAGExprGetBuf:
	    if( !is_equivalent_ExprGetBuf( to_const_ExprGetBuf(a), to_const_ExprGetBuf(b) ) ){
		return TMFALSE;
	    }
	    break;

	case TAGExprGetSize:
	    if( !is_equivalent_ExprGetSize( to_const_ExprGetSize(a), to_const_ExprGetSize(b) ) ){
		return TMFALSE;
	    }
	    break;

	case TAGExprGetLength:
	    if( !is_equivalent_ExprGetLength( to_const_ExprGetLength(a), to_const_ExprGetLength(b) ) ){
		return TMFALSE;
	    }
	    break;

	case TAGExprNotNullAssert:
	    if( !is_equivalent_ExprNotNullAssert( to_const_ExprNotNullAssert(a), to_const_ExprNotNullAssert(b) ) ){
		return TMFALSE;
	    }
	    break;

	case TAGExprWrapper:
	    if( !is_equivalent_ExprWrapper( to_const_ExprWrapper(a), to_const_ExprWrapper(b) ) ){
		return TMFALSE;
	    }
	    break;

	case TAGExprIsRaised:
	    if( !is_equivalent_ExprIsRaised( to_const_ExprIsRaised(a), to_const_ExprIsRaised(b) ) ){
		return TMFALSE;
	    }
	    break;

	case TAGExprArray:
	    if( !is_equivalent_ExprArray( to_const_ExprArray(a), to_const_ExprArray(b) ) ){
		return TMFALSE;
	    }
	    break;

	case TAGExprIsNewOwner:
	    if( !is_equivalent_ExprIsNewOwner( to_const_ExprIsNewOwner(a), to_const_ExprIsNewOwner(b) ) ){
		return TMFALSE;
	    }
	    break;

	case TAGExprIsNewMaster:
	    if( !is_equivalent_ExprIsNewMaster( to_const_ExprIsNewMaster(a), to_const_ExprIsNewMaster(b) ) ){
		return TMFALSE;
	    }
	    break;

	case TAGExprGetNewOwner:
	    if( !is_equivalent_ExprGetNewOwner( to_const_ExprGetNewOwner(a), to_const_ExprGetNewOwner(b) ) ){
		return TMFALSE;
	    }
	    break;

	case TAGExprGetNewMaster:
	    if( !is_equivalent_ExprGetNewMaster( to_const_ExprGetNewMaster(a), to_const_ExprGetNewMaster(b) ) ){
		return TMFALSE;
	    }
	    break;
    }
    return TMTRUE;
}

/* Compare two OwnerExpr class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_OwnerExpr( const const_OwnerExpr a, const const_OwnerExpr b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == OwnerExprNIL || b == OwnerExprNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    switch( a->tag )
    {
	case TAGNewOwnDontCare:
	    if( !is_equivalent_NewOwnDontCare( to_const_NewOwnDontCare(a), to_const_NewOwnDontCare(b) ) ){
		return TMFALSE;
	    }
	    break;

	case TAGNewOwnReplicated:
	    if( !is_equivalent_NewOwnReplicated( to_const_NewOwnReplicated(a), to_const_NewOwnReplicated(b) ) ){
		return TMFALSE;
	    }
	    break;

	case TAGNewOwnLocation:
	    if( !is_equivalent_NewOwnLocation( to_const_NewOwnLocation(a), to_const_NewOwnLocation(b) ) ){
		return TMFALSE;
	    }
	    break;

	case TAGNewOwnMap:
	    if( !is_equivalent_NewOwnMap( to_const_NewOwnMap(a), to_const_NewOwnMap(b) ) ){
		return TMFALSE;
	    }
	    break;
    }
    return TMTRUE;
}

/* Compare two location class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_location( const const_location a, const const_location b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == locationNIL || b == locationNIL ){
	return TMFALSE;
    }
	// Extra code
    if( a->tag == TAGLocWrapper )
    {
	return is_equivalent_location(to_const_LocWrapper(a)->l,b);
    }
    if( b->tag == TAGLocWrapper )
    {
	return is_equivalent_location(a,to_const_LocWrapper(b)->l);
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    switch( a->tag )
    {
	case TAGLocName:
	    if( !is_equivalent_LocName( to_const_LocName(a), to_const_LocName(b) ) ){
		return TMFALSE;
	    }
	    break;

	case TAGLocField:
	    if( !is_equivalent_LocField( to_const_LocField(a), to_const_LocField(b) ) ){
		return TMFALSE;
	    }
	    break;

	case TAGLocFieldNumber:
	    if( !is_equivalent_LocFieldNumber( to_const_LocFieldNumber(a), to_const_LocFieldNumber(b) ) ){
		return TMFALSE;
	    }
	    break;

	case TAGLocSelection:
	    if( !is_equivalent_LocSelection( to_const_LocSelection(a), to_const_LocSelection(b) ) ){
		return TMFALSE;
	    }
	    break;

	case TAGLocFlatSelection:
	    if( !is_equivalent_LocFlatSelection( to_const_LocFlatSelection(a), to_const_LocFlatSelection(b) ) ){
		return TMFALSE;
	    }
	    break;

	case TAGLocWrapper:
	    if( !is_equivalent_LocWrapper( to_const_LocWrapper(a), to_const_LocWrapper(b) ) ){
		return TMFALSE;
	    }
	    break;

	case TAGLocDeref:
	    if( !is_equivalent_LocDeref( to_const_LocDeref(a), to_const_LocDeref(b) ) ){
		return TMFALSE;
	    }
	    break;

	case TAGLocWhere:
	    if( !is_equivalent_LocWhere( to_const_LocWhere(a), to_const_LocWhere(b) ) ){
		return TMFALSE;
	    }
	    break;

	case TAGLocNotNullAssert:
	    if( !is_equivalent_LocNotNullAssert( to_const_LocNotNullAssert(a), to_const_LocNotNullAssert(b) ) ){
		return TMFALSE;
	    }
	    break;
    }
    return TMTRUE;
}

/* Compare two optexpression class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_optexpression( const const_optexpression a, const const_optexpression b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == optexpressionNIL || b == optexpressionNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    switch( a->tag )
    {
	case TAGOptExprNone:
	    if( !is_equivalent_OptExprNone( to_const_OptExprNone(a), to_const_OptExprNone(b) ) ){
		return TMFALSE;
	    }
	    break;

	case TAGOptExpr:
	    if( !is_equivalent_OptExpr( to_const_OptExpr(a), to_const_OptExpr(b) ) ){
		return TMFALSE;
	    }
	    break;
    }
    return TMTRUE;
}

/* Compare two type class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_type( const const_type a, const const_type b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == typeNIL || b == typeNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    switch( a->tag )
    {
	case TAGTypeMap:
	    if( !is_equivalent_TypeMap( to_const_TypeMap(a), to_const_TypeMap(b) ) ){
		return TMFALSE;
	    }
	    break;

	case TAGTypeBase:
	    if( !is_equivalent_TypeBase( to_const_TypeBase(a), to_const_TypeBase(b) ) ){
		return TMFALSE;
	    }
	    break;

	case TAGTypeShape:
	    if( !is_equivalent_TypeShape( to_const_TypeShape(a), to_const_TypeShape(b) ) ){
		return TMFALSE;
	    }
	    break;

	case TAGTypeUnsizedArray:
	    if( !is_equivalent_TypeUnsizedArray( to_const_TypeUnsizedArray(a), to_const_TypeUnsizedArray(b) ) ){
		return TMFALSE;
	    }
	    break;

	case TAGTypePointer:
	    if( !is_equivalent_TypePointer( to_const_TypePointer(a), to_const_TypePointer(b) ) ){
		return TMFALSE;
	    }
	    break;

	case TAGTypeNeutralPointer:
	    if( !is_equivalent_TypeNeutralPointer( to_const_TypeNeutralPointer(a), to_const_TypeNeutralPointer(b) ) ){
		return TMFALSE;
	    }
	    break;

	case TAGTypeArray:
	    if( !is_equivalent_TypeArray( to_const_TypeArray(a), to_const_TypeArray(b) ) ){
		return TMFALSE;
	    }
	    break;

	case TAGTypeRecord:
	    if( !is_equivalent_TypeRecord( to_const_TypeRecord(a), to_const_TypeRecord(b) ) ){
		return TMFALSE;
	    }
	    break;

	case TAGTypeNamedRecord:
	    if( !is_equivalent_TypeNamedRecord( to_const_TypeNamedRecord(a), to_const_TypeNamedRecord(b) ) ){
		return TMFALSE;
	    }
	    break;

	case TAGTypeProcedure:
	    if( !is_equivalent_TypeProcedure( to_const_TypeProcedure(a), to_const_TypeProcedure(b) ) ){
		return TMFALSE;
	    }
	    break;

	case TAGTypeFunction:
	    if( !is_equivalent_TypeFunction( to_const_TypeFunction(a), to_const_TypeFunction(b) ) ){
		return TMFALSE;
	    }
	    break;

	case TAGTypePragmas:
	    if( !is_equivalent_TypePragmas( to_const_TypePragmas(a), to_const_TypePragmas(b) ) ){
		return TMFALSE;
	    }
	    break;
    }
    return TMTRUE;
}

/* Compare two Pragma class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_Pragma( const const_Pragma a, const const_Pragma b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == PragmaNIL || b == PragmaNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    switch( a->tag )
    {
	case TAGFlagPragma:
	    if( !is_equivalent_FlagPragma( to_const_FlagPragma(a), to_const_FlagPragma(b) ) ){
		return TMFALSE;
	    }
	    break;

	case TAGValuePragma:
	    if( !is_equivalent_ValuePragma( to_const_ValuePragma(a), to_const_ValuePragma(b) ) ){
		return TMFALSE;
	    }
	    break;
    }
    return TMTRUE;
}

/* Compare two size class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_size( const const_size a, const const_size b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == sizeNIL || b == sizeNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    switch( a->tag )
    {
	case TAGSizeDontcare:
	    if( !is_equivalent_SizeDontcare( to_const_SizeDontcare(a), to_const_SizeDontcare(b) ) ){
		return TMFALSE;
	    }
	    break;

	case TAGSizeExpression:
	    if( !is_equivalent_SizeExpression( to_const_SizeExpression(a), to_const_SizeExpression(b) ) ){
		return TMFALSE;
	    }
	    break;
    }
    return TMTRUE;
}

/* Compare two PragmaExpression class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_PragmaExpression( const const_PragmaExpression a, const const_PragmaExpression b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == PragmaExpressionNIL || b == PragmaExpressionNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    switch( a->tag )
    {
	case TAGListPragmaExpression:
	    if( !is_equivalent_ListPragmaExpression( to_const_ListPragmaExpression(a), to_const_ListPragmaExpression(b) ) ){
		return TMFALSE;
	    }
	    break;

	case TAGNumberPragmaExpression:
	    if( !is_equivalent_NumberPragmaExpression( to_const_NumberPragmaExpression(a), to_const_NumberPragmaExpression(b) ) ){
		return TMFALSE;
	    }
	    break;

	case TAGStringPragmaExpression:
	    if( !is_equivalent_StringPragmaExpression( to_const_StringPragmaExpression(a), to_const_StringPragmaExpression(b) ) ){
		return TMFALSE;
	    }
	    break;

	case TAGBooleanPragmaExpression:
	    if( !is_equivalent_BooleanPragmaExpression( to_const_BooleanPragmaExpression(a), to_const_BooleanPragmaExpression(b) ) ){
		return TMFALSE;
	    }
	    break;

	case TAGNamePragmaExpression:
	    if( !is_equivalent_NamePragmaExpression( to_const_NamePragmaExpression(a), to_const_NamePragmaExpression(b) ) ){
		return TMFALSE;
	    }
	    break;

	case TAGExternalNamePragmaExpression:
	    if( !is_equivalent_ExternalNamePragmaExpression( to_const_ExternalNamePragmaExpression(a), to_const_ExternalNamePragmaExpression(b) ) ){
		return TMFALSE;
	    }
	    break;
    }
    return TMTRUE;
}

/* Compare two origin tuple,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_origin( const const_origin a, const const_origin b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == originNIL || b == originNIL ){
	return TMFALSE;
    }
    if( !isequal_tmsymbol( a->file, b->file ) ){
	return TMFALSE;
    }
    if( !isequal_int( a->line, b->line ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two BooleanPragmaExpression class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_BooleanPragmaExpression( const const_BooleanPragmaExpression a, const const_BooleanPragmaExpression b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == BooleanPragmaExpressionNIL || b == BooleanPragmaExpressionNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !isequal_vnus_boolean( a->b, b->b ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two ExternalNamePragmaExpression class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_ExternalNamePragmaExpression( const const_ExternalNamePragmaExpression a, const const_ExternalNamePragmaExpression b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == ExternalNamePragmaExpressionNIL || b == ExternalNamePragmaExpressionNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !is_equivalent_origsymbol( a->name, b->name ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two ListPragmaExpression class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_ListPragmaExpression( const const_ListPragmaExpression a, const const_ListPragmaExpression b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == ListPragmaExpressionNIL || b == ListPragmaExpressionNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !is_equivalent_PragmaExpression_list( a->l, b->l ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two NamePragmaExpression class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_NamePragmaExpression( const const_NamePragmaExpression a, const const_NamePragmaExpression b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == NamePragmaExpressionNIL || b == NamePragmaExpressionNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !is_equivalent_origsymbol( a->name, b->name ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two NumberPragmaExpression class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_NumberPragmaExpression( const const_NumberPragmaExpression a, const const_NumberPragmaExpression b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == NumberPragmaExpressionNIL || b == NumberPragmaExpressionNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !isequal_vnus_double( a->v, b->v ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two StringPragmaExpression class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_StringPragmaExpression( const const_StringPragmaExpression a, const const_StringPragmaExpression b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == StringPragmaExpressionNIL || b == StringPragmaExpressionNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !isequal_tmstring( a->s, b->s ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two FlagPragma class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_FlagPragma( const const_FlagPragma a, const const_FlagPragma b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == FlagPragmaNIL || b == FlagPragmaNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !is_equivalent_origsymbol( a->name, b->name ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two SizeDontcare class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_SizeDontcare( const const_SizeDontcare a, const const_SizeDontcare b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == SizeDontcareNIL || b == SizeDontcareNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !is_equivalent_origin( a->orig, b->orig ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two SizeExpression class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_SizeExpression( const const_SizeExpression a, const const_SizeExpression b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == SizeExpressionNIL || b == SizeExpressionNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !is_equivalent_expression( a->x, b->x ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two ValuePragma class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_ValuePragma( const const_ValuePragma a, const const_ValuePragma b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == ValuePragmaNIL || b == ValuePragmaNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !is_equivalent_origsymbol( a->name, b->name ) ){
	return TMFALSE;
    }
    if( !is_equivalent_PragmaExpression( a->x, b->x ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two field tuple,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_field( const const_field a, const const_field b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == fieldNIL || b == fieldNIL ){
	return TMFALSE;
    }
    if( !is_equivalent_origsymbol( a->name, b->name ) ){
	return TMFALSE;
    }
    if( !is_equivalent_type( a->elmtype, b->elmtype ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two LocDeref class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_LocDeref( const const_LocDeref a, const const_LocDeref b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == LocDerefNIL || b == LocDerefNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !is_equivalent_expression( a->ref, b->ref ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two LocField class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_LocField( const const_LocField a, const const_LocField b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == LocFieldNIL || b == LocFieldNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !is_equivalent_expression( a->rec, b->rec ) ){
	return TMFALSE;
    }
    if( !is_equivalent_origsymbol( a->field, b->field ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two LocFieldNumber class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_LocFieldNumber( const const_LocFieldNumber a, const const_LocFieldNumber b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == LocFieldNumberNIL || b == LocFieldNumberNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !is_equivalent_expression( a->rec, b->rec ) ){
	return TMFALSE;
    }
    if( !isequal_vnus_int( a->field, b->field ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two LocFlatSelection class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_LocFlatSelection( const const_LocFlatSelection a, const const_LocFlatSelection b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == LocFlatSelectionNIL || b == LocFlatSelectionNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !is_equivalent_expression( a->shape, b->shape ) ){
	return TMFALSE;
    }
    if( !is_equivalent_expression( a->index, b->index ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two LocName class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_LocName( const const_LocName a, const const_LocName b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == LocNameNIL || b == LocNameNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !is_equivalent_origsymbol( a->name, b->name ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two LocNotNullAssert class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_LocNotNullAssert( const const_LocNotNullAssert a, const const_LocNotNullAssert b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == LocNotNullAssertNIL || b == LocNotNullAssertNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !is_equivalent_location( a->l, b->l ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two LocSelection class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_LocSelection( const const_LocSelection a, const const_LocSelection b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == LocSelectionNIL || b == LocSelectionNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !is_equivalent_expression( a->shape, b->shape ) ){
	return TMFALSE;
    }
    if( !is_equivalent_expression_list( a->indices, b->indices ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two LocWhere class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_LocWhere( const const_LocWhere a, const const_LocWhere b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == LocWhereNIL || b == LocWhereNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !isequal_tmsymbol( a->scope, b->scope ) ){
	return TMFALSE;
    }
    if( !is_equivalent_location( a->l, b->l ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two LocWrapper class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_LocWrapper( const const_LocWrapper a, const const_LocWrapper b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == LocWrapperNIL || b == LocWrapperNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !is_equivalent_Pragma_list( a->pragmas, b->pragmas ) ){
	return TMFALSE;
    }
    if( !is_equivalent_OwnerExpr_list( a->on, b->on ) ){
	return TMFALSE;
    }
    if( !is_equivalent_location( a->l, b->l ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two NewOwnDontCare class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_NewOwnDontCare( const const_NewOwnDontCare a, const const_NewOwnDontCare b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == NewOwnDontCareNIL || b == NewOwnDontCareNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two NewOwnLocation class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_NewOwnLocation( const const_NewOwnLocation a, const const_NewOwnLocation b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == NewOwnLocationNIL || b == NewOwnLocationNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !is_equivalent_expression( a->proc, b->proc ) ){
	return TMFALSE;
    }
    if( !is_equivalent_DistrExpr_list( a->distr, b->distr ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two NewOwnMap class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_NewOwnMap( const const_NewOwnMap a, const const_NewOwnMap b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == NewOwnMapNIL || b == NewOwnMapNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !is_equivalent_origsymbol_list( a->parms, b->parms ) ){
	return TMFALSE;
    }
    if( !is_equivalent_OwnerExpr( a->on, b->on ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two NewOwnReplicated class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_NewOwnReplicated( const const_NewOwnReplicated a, const const_NewOwnReplicated b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == NewOwnReplicatedNIL || b == NewOwnReplicatedNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two OptExpr class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_OptExpr( const const_OptExpr a, const const_OptExpr b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == OptExprNIL || b == OptExprNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !is_equivalent_expression( a->x, b->x ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two OptExprNone class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_OptExprNone( const const_OptExprNone a, const const_OptExprNone b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == OptExprNoneNIL || b == OptExprNoneNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two TypeArray class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_TypeArray( const const_TypeArray a, const const_TypeArray b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == TypeArrayNIL || b == TypeArrayNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !isequal_int( a->sz, b->sz ) ){
	return TMFALSE;
    }
    if( !is_equivalent_type( a->elmtype, b->elmtype ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two TypeBase class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_TypeBase( const const_TypeBase a, const const_TypeBase b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == TypeBaseNIL || b == TypeBaseNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !isequal_BASETYPE( a->base, b->base ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two TypeFunction class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_TypeFunction( const const_TypeFunction a, const const_TypeFunction b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == TypeFunctionNIL || b == TypeFunctionNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !is_equivalent_type_list( a->formals, b->formals ) ){
	return TMFALSE;
    }
    if( !is_equivalent_type( a->rettype, b->rettype ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two TypeMap class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_TypeMap( const const_TypeMap a, const const_TypeMap b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == TypeMapNIL || b == TypeMapNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !is_equivalent_OwnerExpr( a->map, b->map ) ){
	return TMFALSE;
    }
    if( !is_equivalent_type( a->t, b->t ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two TypeNamedRecord class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_TypeNamedRecord( const const_TypeNamedRecord a, const const_TypeNamedRecord b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == TypeNamedRecordNIL || b == TypeNamedRecordNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !is_equivalent_origsymbol( a->name, b->name ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two TypeNeutralPointer class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_TypeNeutralPointer( const const_TypeNeutralPointer a, const const_TypeNeutralPointer b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == TypeNeutralPointerNIL || b == TypeNeutralPointerNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two TypePointer class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_TypePointer( const const_TypePointer a, const const_TypePointer b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == TypePointerNIL || b == TypePointerNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !is_equivalent_type( a->elmtype, b->elmtype ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two TypePragmas class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_TypePragmas( const const_TypePragmas a, const const_TypePragmas b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == TypePragmasNIL || b == TypePragmasNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !is_equivalent_Pragma_list( a->pragmas, b->pragmas ) ){
	return TMFALSE;
    }
    if( !is_equivalent_type( a->t, b->t ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two TypeProcedure class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_TypeProcedure( const const_TypeProcedure a, const const_TypeProcedure b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == TypeProcedureNIL || b == TypeProcedureNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !is_equivalent_type_list( a->formals, b->formals ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two TypeRecord class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_TypeRecord( const const_TypeRecord a, const const_TypeRecord b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == TypeRecordNIL || b == TypeRecordNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !is_equivalent_field_list( a->fields, b->fields ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two TypeShape class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_TypeShape( const const_TypeShape a, const const_TypeShape b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == TypeShapeNIL || b == TypeShapeNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !is_equivalent_size_list( a->sizes, b->sizes ) ){
	return TMFALSE;
    }
    if( !is_equivalent_type( a->elmtype, b->elmtype ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two TypeUnsizedArray class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_TypeUnsizedArray( const const_TypeUnsizedArray a, const const_TypeUnsizedArray b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == TypeUnsizedArrayNIL || b == TypeUnsizedArrayNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !is_equivalent_type( a->elmtype, b->elmtype ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two ExprAddress class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_ExprAddress( const const_ExprAddress a, const const_ExprAddress b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == ExprAddressNIL || b == ExprAddressNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !is_equivalent_location( a->adr, b->adr ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two ExprArray class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_ExprArray( const const_ExprArray a, const const_ExprArray b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == ExprArrayNIL || b == ExprArrayNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !is_equivalent_type( a->elmtype, b->elmtype ) ){
	return TMFALSE;
    }
    if( !is_equivalent_expression_list( a->elms, b->elms ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two ExprBinop class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_ExprBinop( const const_ExprBinop a, const const_ExprBinop b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == ExprBinopNIL || b == ExprBinopNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !is_equivalent_expression( a->left, b->left ) ){
	return TMFALSE;
    }
    if( !isequal_BINOP( a->optor, b->optor ) ){
	return TMFALSE;
    }
    if( !is_equivalent_expression( a->right, b->right ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two ExprBoolean class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_ExprBoolean( const const_ExprBoolean a, const const_ExprBoolean b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == ExprBooleanNIL || b == ExprBooleanNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !isequal_vnus_boolean( a->b, b->b ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two ExprByte class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_ExprByte( const const_ExprByte a, const const_ExprByte b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == ExprByteNIL || b == ExprByteNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !isequal_vnus_byte( a->v, b->v ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two ExprCast class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_ExprCast( const const_ExprCast a, const const_ExprCast b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == ExprCastNIL || b == ExprCastNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !is_equivalent_type( a->t, b->t ) ){
	return TMFALSE;
    }
    if( !is_equivalent_expression( a->x, b->x ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two ExprChar class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_ExprChar( const const_ExprChar a, const const_ExprChar b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == ExprCharNIL || b == ExprCharNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !isequal_vnus_char( a->c, b->c ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two ExprCheckedIndex class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_ExprCheckedIndex( const const_ExprCheckedIndex a, const const_ExprCheckedIndex b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == ExprCheckedIndexNIL || b == ExprCheckedIndexNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !is_equivalent_expression( a->val, b->val ) ){
	return TMFALSE;
    }
    if( !is_equivalent_expression( a->upperbound, b->upperbound ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two ExprComplex class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_ExprComplex( const const_ExprComplex a, const const_ExprComplex b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == ExprComplexNIL || b == ExprComplexNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !is_equivalent_expression( a->re, b->re ) ){
	return TMFALSE;
    }
    if( !is_equivalent_expression( a->im, b->im ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two ExprDeref class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_ExprDeref( const const_ExprDeref a, const const_ExprDeref b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == ExprDerefNIL || b == ExprDerefNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !is_equivalent_expression( a->ref, b->ref ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two ExprDouble class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_ExprDouble( const const_ExprDouble a, const const_ExprDouble b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == ExprDoubleNIL || b == ExprDoubleNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !isequal_vnus_double( a->v, b->v ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two ExprField class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_ExprField( const const_ExprField a, const const_ExprField b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == ExprFieldNIL || b == ExprFieldNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !is_equivalent_expression( a->rec, b->rec ) ){
	return TMFALSE;
    }
    if( !is_equivalent_origsymbol( a->field, b->field ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two ExprFieldNumber class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_ExprFieldNumber( const const_ExprFieldNumber a, const const_ExprFieldNumber b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == ExprFieldNumberNIL || b == ExprFieldNumberNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !is_equivalent_expression( a->rec, b->rec ) ){
	return TMFALSE;
    }
    if( !isequal_vnus_int( a->field, b->field ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two ExprFilledNew class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_ExprFilledNew( const const_ExprFilledNew a, const const_ExprFilledNew b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == ExprFilledNewNIL || b == ExprFilledNewNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !is_equivalent_type( a->t, b->t ) ){
	return TMFALSE;
    }
    if( !is_equivalent_expression( a->init, b->init ) ){
	return TMFALSE;
    }
    if( !is_equivalent_origsymbol( a->gcmarker, b->gcmarker ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two ExprFlatSelection class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_ExprFlatSelection( const const_ExprFlatSelection a, const const_ExprFlatSelection b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == ExprFlatSelectionNIL || b == ExprFlatSelectionNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !is_equivalent_expression( a->shape, b->shape ) ){
	return TMFALSE;
    }
    if( !is_equivalent_expression( a->index, b->index ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two ExprFloat class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_ExprFloat( const const_ExprFloat a, const const_ExprFloat b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == ExprFloatNIL || b == ExprFloatNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !isequal_vnus_float( a->v, b->v ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two ExprFunctionCall class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_ExprFunctionCall( const const_ExprFunctionCall a, const const_ExprFunctionCall b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == ExprFunctionCallNIL || b == ExprFunctionCallNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !is_equivalent_expression( a->function, b->function ) ){
	return TMFALSE;
    }
    if( !is_equivalent_expression_list( a->parameters, b->parameters ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two ExprGetBuf class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_ExprGetBuf( const const_ExprGetBuf a, const const_ExprGetBuf b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == ExprGetBufNIL || b == ExprGetBufNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !is_equivalent_expression( a->shape, b->shape ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two ExprGetLength class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_ExprGetLength( const const_ExprGetLength a, const const_ExprGetLength b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == ExprGetLengthNIL || b == ExprGetLengthNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !is_equivalent_expression( a->shape, b->shape ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two ExprGetNewMaster class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_ExprGetNewMaster( const const_ExprGetNewMaster a, const const_ExprGetNewMaster b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == ExprGetNewMasterNIL || b == ExprGetNewMasterNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !is_equivalent_OwnerExpr( a->on, b->on ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two ExprGetNewOwner class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_ExprGetNewOwner( const const_ExprGetNewOwner a, const const_ExprGetNewOwner b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == ExprGetNewOwnerNIL || b == ExprGetNewOwnerNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !is_equivalent_OwnerExpr( a->on, b->on ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two ExprGetSize class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_ExprGetSize( const const_ExprGetSize a, const const_ExprGetSize b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == ExprGetSizeNIL || b == ExprGetSizeNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !is_equivalent_expression( a->shape, b->shape ) ){
	return TMFALSE;
    }
    if( !is_equivalent_expression( a->dim, b->dim ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two ExprIf class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_ExprIf( const const_ExprIf a, const const_ExprIf b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == ExprIfNIL || b == ExprIfNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !is_equivalent_expression( a->cond, b->cond ) ){
	return TMFALSE;
    }
    if( !is_equivalent_expression( a->thenval, b->thenval ) ){
	return TMFALSE;
    }
    if( !is_equivalent_expression( a->elseval, b->elseval ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two ExprInt class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_ExprInt( const const_ExprInt a, const const_ExprInt b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == ExprIntNIL || b == ExprIntNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !isequal_vnus_int( a->v, b->v ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two ExprIsBoundViolated class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_ExprIsBoundViolated( const const_ExprIsBoundViolated a, const const_ExprIsBoundViolated b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == ExprIsBoundViolatedNIL || b == ExprIsBoundViolatedNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !is_equivalent_expression( a->val, b->val ) ){
	return TMFALSE;
    }
    if( !is_equivalent_expression( a->upperbound, b->upperbound ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two ExprIsLowerBoundViolated class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_ExprIsLowerBoundViolated( const const_ExprIsLowerBoundViolated a, const const_ExprIsLowerBoundViolated b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == ExprIsLowerBoundViolatedNIL || b == ExprIsLowerBoundViolatedNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !is_equivalent_expression( a->val, b->val ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two ExprIsNewMaster class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_ExprIsNewMaster( const const_ExprIsNewMaster a, const const_ExprIsNewMaster b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == ExprIsNewMasterNIL || b == ExprIsNewMasterNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !is_equivalent_OwnerExpr( a->on, b->on ) ){
	return TMFALSE;
    }
    if( !is_equivalent_expression_list( a->proc, b->proc ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two ExprIsNewOwner class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_ExprIsNewOwner( const const_ExprIsNewOwner a, const const_ExprIsNewOwner b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == ExprIsNewOwnerNIL || b == ExprIsNewOwnerNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !is_equivalent_OwnerExpr( a->on, b->on ) ){
	return TMFALSE;
    }
    if( !is_equivalent_expression_list( a->proc, b->proc ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two ExprIsRaised class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_ExprIsRaised( const const_ExprIsRaised a, const const_ExprIsRaised b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == ExprIsRaisedNIL || b == ExprIsRaisedNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !is_equivalent_expression( a->var, b->var ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two ExprIsUpperBoundViolated class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_ExprIsUpperBoundViolated( const const_ExprIsUpperBoundViolated a, const const_ExprIsUpperBoundViolated b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == ExprIsUpperBoundViolatedNIL || b == ExprIsUpperBoundViolatedNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !is_equivalent_expression( a->val, b->val ) ){
	return TMFALSE;
    }
    if( !is_equivalent_expression( a->upperbound, b->upperbound ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two ExprLong class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_ExprLong( const const_ExprLong a, const const_ExprLong b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == ExprLongNIL || b == ExprLongNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !isequal_vnus_long( a->v, b->v ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two ExprLowerCheckedIndex class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_ExprLowerCheckedIndex( const const_ExprLowerCheckedIndex a, const const_ExprLowerCheckedIndex b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == ExprLowerCheckedIndexNIL || b == ExprLowerCheckedIndexNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !is_equivalent_expression( a->val, b->val ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two ExprName class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_ExprName( const const_ExprName a, const const_ExprName b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == ExprNameNIL || b == ExprNameNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !is_equivalent_origsymbol( a->name, b->name ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two ExprNew class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_ExprNew( const const_ExprNew a, const const_ExprNew b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == ExprNewNIL || b == ExprNewNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !is_equivalent_type( a->t, b->t ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two ExprNewArray class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_ExprNewArray( const const_ExprNewArray a, const const_ExprNewArray b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == ExprNewArrayNIL || b == ExprNewArrayNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !is_equivalent_type( a->t, b->t ) ){
	return TMFALSE;
    }
    if( !is_equivalent_expression( a->length, b->length ) ){
	return TMFALSE;
    }
    if( !is_equivalent_optexpression( a->init, b->init ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two ExprNewRecord class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_ExprNewRecord( const const_ExprNewRecord a, const const_ExprNewRecord b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == ExprNewRecordNIL || b == ExprNewRecordNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !is_equivalent_type( a->t, b->t ) ){
	return TMFALSE;
    }
    if( !is_equivalent_expression_list( a->fields, b->fields ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two ExprNotNullAssert class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_ExprNotNullAssert( const const_ExprNotNullAssert a, const const_ExprNotNullAssert b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == ExprNotNullAssertNIL || b == ExprNotNullAssertNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !is_equivalent_expression( a->x, b->x ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two ExprNull class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_ExprNull( const const_ExprNull a, const const_ExprNull b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == ExprNullNIL || b == ExprNullNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two ExprNulledNew class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_ExprNulledNew( const const_ExprNulledNew a, const const_ExprNulledNew b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == ExprNulledNewNIL || b == ExprNulledNewNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !is_equivalent_type( a->t, b->t ) ){
	return TMFALSE;
    }
    if( !is_equivalent_origsymbol( a->gcmarker, b->gcmarker ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two ExprRecord class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_ExprRecord( const const_ExprRecord a, const const_ExprRecord b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == ExprRecordNIL || b == ExprRecordNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !is_equivalent_expression_list( a->fields, b->fields ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two ExprReduction class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_ExprReduction( const const_ExprReduction a, const const_ExprReduction b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == ExprReductionNIL || b == ExprReductionNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !isequal_BINOP( a->optor, b->optor ) ){
	return TMFALSE;
    }
    if( !is_equivalent_expression_list( a->operands, b->operands ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two ExprSelection class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_ExprSelection( const const_ExprSelection a, const const_ExprSelection b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == ExprSelectionNIL || b == ExprSelectionNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !is_equivalent_expression( a->shape, b->shape ) ){
	return TMFALSE;
    }
    if( !is_equivalent_expression_list( a->indices, b->indices ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two ExprShape class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_ExprShape( const const_ExprShape a, const const_ExprShape b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == ExprShapeNIL || b == ExprShapeNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !is_equivalent_size_list( a->sizes, b->sizes ) ){
	return TMFALSE;
    }
    if( !is_equivalent_type( a->elmtype, b->elmtype ) ){
	return TMFALSE;
    }
    if( !is_equivalent_expression( a->arr, b->arr ) ){
	return TMFALSE;
    }
    if( !isequal_int( a->nelms, b->nelms ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two ExprShort class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_ExprShort( const const_ExprShort a, const const_ExprShort b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == ExprShortNIL || b == ExprShortNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !isequal_vnus_short( a->v, b->v ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two ExprSizeof class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_ExprSizeof( const const_ExprSizeof a, const const_ExprSizeof b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == ExprSizeofNIL || b == ExprSizeofNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !is_equivalent_type( a->t, b->t ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two ExprString class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_ExprString( const const_ExprString a, const const_ExprString b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == ExprStringNIL || b == ExprStringNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !isequal_tmstring( a->s, b->s ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two ExprUnop class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_ExprUnop( const const_ExprUnop a, const const_ExprUnop b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == ExprUnopNIL || b == ExprUnopNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !isequal_UNOP( a->optor, b->optor ) ){
	return TMFALSE;
    }
    if( !is_equivalent_expression( a->operand, b->operand ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two ExprUpperCheckedIndex class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_ExprUpperCheckedIndex( const const_ExprUpperCheckedIndex a, const const_ExprUpperCheckedIndex b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == ExprUpperCheckedIndexNIL || b == ExprUpperCheckedIndexNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !is_equivalent_expression( a->val, b->val ) ){
	return TMFALSE;
    }
    if( !is_equivalent_expression( a->upperbound, b->upperbound ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two ExprWhere class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_ExprWhere( const const_ExprWhere a, const const_ExprWhere b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == ExprWhereNIL || b == ExprWhereNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !isequal_tmsymbol( a->scope, b->scope ) ){
	return TMFALSE;
    }
    if( !is_equivalent_expression( a->x, b->x ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two ExprWrapper class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_ExprWrapper( const const_ExprWrapper a, const const_ExprWrapper b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == ExprWrapperNIL || b == ExprWrapperNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !is_equivalent_Pragma_list( a->pragmas, b->pragmas ) ){
	return TMFALSE;
    }
    if( !is_equivalent_OwnerExpr_list( a->on, b->on ) ){
	return TMFALSE;
    }
    if( !is_equivalent_expression( a->x, b->x ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two NewDistAlign class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_NewDistAlign( const const_NewDistAlign a, const const_NewDistAlign b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == NewDistAlignNIL || b == NewDistAlignNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !is_equivalent_location( a->with, b->with ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two NewDistBC class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_NewDistBC( const const_NewDistBC a, const const_NewDistBC b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == NewDistBCNIL || b == NewDistBCNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !is_equivalent_expression( a->index, b->index ) ){
	return TMFALSE;
    }
    if( !is_equivalent_expression( a->size, b->size ) ){
	return TMFALSE;
    }
    if( !is_equivalent_expression( a->extent, b->extent ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two NewDistBlock class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_NewDistBlock( const const_NewDistBlock a, const const_NewDistBlock b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == NewDistBlockNIL || b == NewDistBlockNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !is_equivalent_expression( a->index, b->index ) ){
	return TMFALSE;
    }
    if( !is_equivalent_expression( a->size, b->size ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two NewDistCyclic class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_NewDistCyclic( const const_NewDistCyclic a, const const_NewDistCyclic b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == NewDistCyclicNIL || b == NewDistCyclicNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !is_equivalent_expression( a->index, b->index ) ){
	return TMFALSE;
    }
    if( !is_equivalent_expression( a->extent, b->extent ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two NewDistDontCare class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_NewDistDontCare( const const_NewDistDontCare a, const const_NewDistDontCare b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == NewDistDontCareNIL || b == NewDistDontCareNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two NewDistLocal class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_NewDistLocal( const const_NewDistLocal a, const const_NewDistLocal b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == NewDistLocalNIL || b == NewDistLocalNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    if( !is_equivalent_expression( a->index, b->index ) ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two NewDistReplicated class,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_NewDistReplicated( const const_NewDistReplicated a, const const_NewDistReplicated b )
{
    if( a == b ){
	return TMTRUE;
    }
    if( a == NewDistReplicatedNIL || b == NewDistReplicatedNIL ){
	return TMFALSE;
    }
    if( a->tag != b->tag ){
	return TMFALSE;
    }
    return TMTRUE;
}

/* Compare two OwnerExpr lists,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_OwnerExpr_list( const const_OwnerExpr_list a, const const_OwnerExpr_list b )
{
    unsigned int ix;

    if( a == b ){
	return TMTRUE;
    }
    if( a == OwnerExpr_listNIL || b == OwnerExpr_listNIL ){
	return TMFALSE;
    }
    if( a->sz != b->sz ){
	return TMFALSE;
    }
    for( ix=0; ix<a->sz; ix++ ){
	if( !is_equivalent_OwnerExpr( a->arr[ix], b->arr[ix] ) ){
	    return TMFALSE;
	}
    }
    return TMTRUE;
}

/* Compare two Pragma lists,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_Pragma_list( const const_Pragma_list a, const const_Pragma_list b )
{
    unsigned int ix;

    if( a == b ){
	return TMTRUE;
    }
    if( a == Pragma_listNIL || b == Pragma_listNIL ){
	return TMFALSE;
    }
    if( a->sz != b->sz ){
	return TMFALSE;
    }
    for( ix=0; ix<a->sz; ix++ ){
	if( !is_equivalent_Pragma( a->arr[ix], b->arr[ix] ) ){
	    return TMFALSE;
	}
    }
    return TMTRUE;
}

/* Compare two expression lists,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_expression_list( const const_expression_list a, const const_expression_list b )
{
    unsigned int ix;

    if( a == b ){
	return TMTRUE;
    }
    if( a == expression_listNIL || b == expression_listNIL ){
	return TMFALSE;
    }
    if( a->sz != b->sz ){
	return TMFALSE;
    }
    for( ix=0; ix<a->sz; ix++ ){
	if( !is_equivalent_expression( a->arr[ix], b->arr[ix] ) ){
	    return TMFALSE;
	}
    }
    return TMTRUE;
}

/* Compare two size lists,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_size_list( const const_size_list a, const const_size_list b )
{
    unsigned int ix;

    if( a == b ){
	return TMTRUE;
    }
    if( a == size_listNIL || b == size_listNIL ){
	return TMFALSE;
    }
    if( a->sz != b->sz ){
	return TMFALSE;
    }
    for( ix=0; ix<a->sz; ix++ ){
	if( !is_equivalent_size( a->arr[ix], b->arr[ix] ) ){
	    return TMFALSE;
	}
    }
    return TMTRUE;
}

/* Compare two PragmaExpression lists,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_PragmaExpression_list( const const_PragmaExpression_list a, const const_PragmaExpression_list b )
{
    unsigned int ix;

    if( a == b ){
	return TMTRUE;
    }
    if( a == PragmaExpression_listNIL || b == PragmaExpression_listNIL ){
	return TMFALSE;
    }
    if( a->sz != b->sz ){
	return TMFALSE;
    }
    for( ix=0; ix<a->sz; ix++ ){
	if( !is_equivalent_PragmaExpression( a->arr[ix], b->arr[ix] ) ){
	    return TMFALSE;
	}
    }
    return TMTRUE;
}

/* Compare two DistrExpr lists,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_DistrExpr_list( const const_DistrExpr_list a, const const_DistrExpr_list b )
{
    unsigned int ix;

    if( a == b ){
	return TMTRUE;
    }
    if( a == DistrExpr_listNIL || b == DistrExpr_listNIL ){
	return TMFALSE;
    }
    if( a->sz != b->sz ){
	return TMFALSE;
    }
    for( ix=0; ix<a->sz; ix++ ){
	if( !is_equivalent_DistrExpr( a->arr[ix], b->arr[ix] ) ){
	    return TMFALSE;
	}
    }
    return TMTRUE;
}

/* Compare two field lists,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_field_list( const const_field_list a, const const_field_list b )
{
    unsigned int ix;

    if( a == b ){
	return TMTRUE;
    }
    if( a == field_listNIL || b == field_listNIL ){
	return TMFALSE;
    }
    if( a->sz != b->sz ){
	return TMFALSE;
    }
    for( ix=0; ix<a->sz; ix++ ){
	if( !is_equivalent_field( a->arr[ix], b->arr[ix] ) ){
	    return TMFALSE;
	}
    }
    return TMTRUE;
}

/* Compare two origsymbol lists,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_origsymbol_list( const const_origsymbol_list a, const const_origsymbol_list b )
{
    unsigned int ix;

    if( a == b ){
	return TMTRUE;
    }
    if( a == origsymbol_listNIL || b == origsymbol_listNIL ){
	return TMFALSE;
    }
    if( a->sz != b->sz ){
	return TMFALSE;
    }
    for( ix=0; ix<a->sz; ix++ ){
	if( !is_equivalent_origsymbol( a->arr[ix], b->arr[ix] ) ){
	    return TMFALSE;
	}
    }
    return TMTRUE;
}

/* Compare two type lists,
 * and return TMTRUE iff they are equal.
 */
static tmbool is_equivalent_type_list( const const_type_list a, const const_type_list b )
{
    unsigned int ix;

    if( a == b ){
	return TMTRUE;
    }
    if( a == type_listNIL || b == type_listNIL ){
	return TMFALSE;
    }
    if( a->sz != b->sz ){
	return TMFALSE;
    }
    for( ix=0; ix<a->sz; ix++ ){
	if( !is_equivalent_type( a->arr[ix], b->arr[ix] ) ){
	    return TMFALSE;
	}
    }
    return TMTRUE;
}

