// File: analyze.c

#include <tmc.h>

#include "vnusbasetypes.h"
#include "tmadmin.h"
#include "vnusctl.h"

static bool is_constant_expression( const_expression_list xl );

bool is_constant_expression( const_expression x )
{
    switch( x->tag ){
	case TAGExprBoolean:
	case TAGExprByte:
	case TAGExprChar:
	case TAGExprDouble:
	case TAGExprFloat:
	case TAGExprInt:
	case TAGExprLong:
	case TAGExprNull:
	case TAGExprShort:
	case TAGExprSizeof:
	case TAGExprString:
	    return true;

	case TAGExprAddress:
	case TAGExprArray:
	case TAGExprDeref:
	case TAGExprField:
	case TAGExprFieldNumber:
	case TAGExprFilledNew:
	case TAGExprNulledNew:
	case TAGExprFunctionCall:
	case TAGExprGetBuf:
	case TAGExprGetLength:
	case TAGExprGetNewMaster:
	case TAGExprGetNewOwner:
	case TAGExprGetSize:
	case TAGExprIsNewMaster:
	case TAGExprIsNewOwner:
	case TAGExprIsRaised:
	case TAGExprName:
	case TAGExprNew:
	case TAGExprNewArray:
	case TAGExprNewRecord:
	case TAGExprRecord:
	case TAGExprSelection:
	case TAGExprFlatSelection:
	case TAGExprShape:
	    return false;

	case TAGExprComplex:
	{
	    const_ExprComplex cx = to_const_ExprComplex(x);
	    return is_constant_expression(cx->re) && is_constant_expression(cx->im);
	}

	case TAGExprIsBoundViolated:
	{
	    const_ExprIsBoundViolated cx = to_const_ExprIsBoundViolated(x);
	    return is_constant_expression(cx->val) &&
		is_constant_expression(cx->upperbound);
	}

	case TAGExprIsUpperBoundViolated:
	{
	    const_ExprIsUpperBoundViolated cx = to_const_ExprIsUpperBoundViolated(x);
	    return is_constant_expression(cx->val) &&
		is_constant_expression(cx->upperbound);
	}

	case TAGExprIsLowerBoundViolated:
	{
	    const_ExprIsLowerBoundViolated cx = to_const_ExprIsLowerBoundViolated(x);
	    return is_constant_expression(cx->val);
	}

	case TAGExprCheckedIndex:
	{
	    const_ExprCheckedIndex cx = to_const_ExprCheckedIndex(x);
	    return is_constant_expression(cx->val) &&
		is_constant_expression(cx->upperbound);
	}

	case TAGExprUpperCheckedIndex:
	{
	    const_ExprUpperCheckedIndex cx = to_const_ExprUpperCheckedIndex(x);
	    return is_constant_expression(cx->val) &&
		is_constant_expression(cx->upperbound);
	}

	case TAGExprLowerCheckedIndex:
	{
	    const_ExprLowerCheckedIndex cx = to_const_ExprLowerCheckedIndex(x);
	    return is_constant_expression(cx->val);
	}

	case TAGExprBinop:
	{
	    const_ExprBinop cx = to_const_ExprBinop(x);
	    return is_constant_expression(cx->left) && is_constant_expression(cx->right);
	}

	case TAGExprReduction:
	{
	    const_ExprReduction cx = to_const_ExprReduction(x);
	    return is_constant_expression( cx->operands );
	}

	case TAGExprIf:
	{
	    const_ExprIf cx = to_const_ExprIf(x);
	    return is_constant_expression( cx->cond ) &&
		 is_constant_expression( cx->thenval ) &&
		 is_constant_expression( cx->elseval );
	}

	case TAGExprCast:
	    return is_constant_expression( to_const_ExprCast(x)->x );

	case TAGExprWhere:
	    return is_constant_expression( to_const_ExprWhere(x)->x );

	case TAGExprUnop:
	    return is_constant_expression( to_const_ExprUnop(x)->operand );

	case TAGExprNotNullAssert:
	    return is_constant_expression( to_const_ExprNotNullAssert(x)->x );

	case TAGExprWrapper:
	    return is_constant_expression( to_const_ExprWrapper(x)->x );

    }
    return false;
}

static bool is_constant_expression( const_expression_list xl )
{
    if( xl == expression_listNIL ){
	return false;
    }
    for( unsigned int ix=0; ix<xl->sz; ix++ ){
	if( is_constant_expression( xl->arr[ix] ) ){
	    return true;
	}
    }
    return true;
}

static bool maybe_has_sideeffect( const_expression_list xl, int flags );

bool maybe_has_sideeffect( const_location x, int flags )
{
    switch( x->tag ){
	case TAGLocName:
	    return false;

	case TAGLocField:
	    return maybe_has_sideeffect( to_const_LocField(x)->rec, flags );

	case TAGLocFieldNumber:
	    return maybe_has_sideeffect( to_const_LocFieldNumber(x)->rec, flags );

	case TAGLocSelection:	// Bounds violation.
	case TAGLocNotNullAssert:
	    return true;

	case TAGLocFlatSelection:	// Bounds violation.
	    return maybe_has_sideeffect( to_const_LocFlatSelection(x)->shape, flags ) ||
		maybe_has_sideeffect( to_const_LocFlatSelection(x)->index, flags );

	case TAGLocDeref:
	    return maybe_has_sideeffect( to_const_LocDeref(x)->ref, flags );

	case TAGLocWhere:
	    // TODO: have a better look.
	    return true;

	case TAGLocWrapper:
	    return maybe_has_sideeffect( to_const_LocWrapper(x)->l, flags );

    }
    return false;
}

bool maybe_has_sideeffect( const_expression x, int flags )
{
    switch( x->tag ){
	case TAGExprBoolean:
	case TAGExprByte:
	case TAGExprChar:
	case TAGExprDouble:
	case TAGExprFloat:
	case TAGExprInt:
	case TAGExprLong:
	case TAGExprNull:
	case TAGExprShort:
	case TAGExprSizeof:
	case TAGExprString:
	case TAGExprName:
	    return false;

	case TAGExprFunctionCall:
	case TAGExprIsNewOwner:
	case TAGExprIsNewMaster:
	case TAGExprGetNewOwner:
	case TAGExprGetNewMaster:
	case TAGExprIsRaised:
	    // TODO: have a better look
	    return true;

	case TAGExprShape:
	    return maybe_has_sideeffect( to_const_ExprShape(x)->sizes, flags ) ||
		maybe_has_sideeffect( to_const_ExprShape(x)->arr, flags );

	case TAGExprArray:
	    return maybe_has_sideeffect( to_const_ExprArray(x)->elms, flags );

	case TAGExprSelection:
	    if( !has_any_flag( flags, NO_SE_EXCEPTION ) ){
		return true;
	    }
	    return maybe_has_sideeffect( to_const_ExprSelection(x)->shape, flags ) ||
		maybe_has_sideeffect( to_const_ExprSelection(x)->indices, flags );

	case TAGExprIsBoundViolated:
	    if( !has_any_flag( flags, NO_SE_EXCEPTION ) ){
		return true;
	    }
	    return maybe_has_sideeffect( to_const_ExprIsBoundViolated(x)->val, flags ) ||
		maybe_has_sideeffect( to_const_ExprIsBoundViolated(x)->upperbound, flags );

	case TAGExprIsUpperBoundViolated:
	    if( !has_any_flag( flags, NO_SE_EXCEPTION ) ){
		return true;
	    }
	    return maybe_has_sideeffect( to_const_ExprIsUpperBoundViolated(x)->val, flags ) ||
		maybe_has_sideeffect( to_const_ExprIsUpperBoundViolated(x)->upperbound, flags );

	case TAGExprIsLowerBoundViolated:
	    if( !has_any_flag( flags, NO_SE_EXCEPTION ) ){
		return true;
	    }
	    return maybe_has_sideeffect( to_const_ExprIsLowerBoundViolated(x)->val, flags );

	case TAGExprCheckedIndex:
	    return maybe_has_sideeffect( to_const_ExprCheckedIndex(x)->val, flags ) ||
		maybe_has_sideeffect( to_const_ExprCheckedIndex(x)->upperbound, flags );

	case TAGExprUpperCheckedIndex:
	    return maybe_has_sideeffect( to_const_ExprUpperCheckedIndex(x)->val, flags ) ||
		maybe_has_sideeffect( to_const_ExprUpperCheckedIndex(x)->upperbound, flags );

	case TAGExprLowerCheckedIndex:
	    return maybe_has_sideeffect( to_const_ExprLowerCheckedIndex(x)->val, flags );

	case TAGExprFlatSelection:
	    return maybe_has_sideeffect( to_const_ExprFlatSelection(x)->shape, flags ) ||
		maybe_has_sideeffect( to_const_ExprFlatSelection(x)->index, flags );

	case TAGExprGetBuf:
	    return maybe_has_sideeffect( to_const_ExprGetBuf(x)->shape, flags );

	case TAGExprGetLength:
	    return maybe_has_sideeffect( to_const_ExprGetLength(x)->shape, flags );

	case TAGExprGetSize:
	    return maybe_has_sideeffect( to_const_ExprGetSize(x)->shape, flags ) ||
		maybe_has_sideeffect( to_const_ExprGetSize(x)->dim, flags );

	case TAGExprAddress:
	    return maybe_has_sideeffect( to_const_ExprAddress(x)->adr, flags );

	case TAGExprDeref:
	    return maybe_has_sideeffect( to_const_ExprDeref(x)->ref, flags );

	case TAGExprField:
	    return maybe_has_sideeffect( to_const_ExprField(x)->rec, flags );

	case TAGExprFieldNumber:
	    return maybe_has_sideeffect( to_const_ExprFieldNumber(x)->rec, flags );

	case TAGExprRecord:
	    return maybe_has_sideeffect( to_const_ExprRecord(x)->fields, flags );

	case TAGExprNewRecord:
	    return maybe_has_sideeffect( to_const_ExprNewRecord(x)->fields, flags );

	case TAGExprNew:
	{
	    if( flags & NO_SE_NEW ){
		return false;
	    }
	    return true;
	}

	case TAGExprFilledNew:
	{
	    if( flags & NO_SE_NEW ){
		const_ExprFilledNew nx = to_const_ExprFilledNew(x);
		// TODO: A SizeExpression may have a side-effect
		return maybe_has_sideeffect( nx->init, flags );
	    }
	    return true;
	}

	case TAGExprNulledNew:
	{
	    if( flags & NO_SE_NEW ){
		//const_ExprNulledNew nx = to_const_ExprNulledNew(x);
		// TODO: A SizeExpression may have a side-effect
		return true;
	    }
	    return true;
	}

	case TAGExprNewArray:
	{
	    if( flags & NO_SE_NEW ){
		const_ExprNewArray nx = to_const_ExprNewArray(x);
		return maybe_has_sideeffect( nx->init, flags ) ||
		    maybe_has_sideeffect( nx->length, flags );
	    }
	    return true;
	}

	case TAGExprComplex:
	{
	    const_ExprComplex cx = to_const_ExprComplex(x);
	    return maybe_has_sideeffect( cx->re, flags ) ||
		maybe_has_sideeffect( cx->im, flags );
	}

	case TAGExprBinop:
	{
	    const_ExprBinop cx = to_const_ExprBinop(x);
	    return maybe_has_sideeffect( cx->left, flags ) ||
		maybe_has_sideeffect( cx->right, flags );
	}

	case TAGExprReduction:
	{
	    const_ExprReduction cx = to_const_ExprReduction(x);
	    return maybe_has_sideeffect( cx->operands, flags );
	}

	case TAGExprIf:
	{
	    const_ExprIf cx = to_const_ExprIf(x);
	    return maybe_has_sideeffect(cx->cond, flags ) ||
		 maybe_has_sideeffect(cx->thenval, flags ) ||
		 maybe_has_sideeffect(cx->elseval, flags );
	}

	case TAGExprCast:
	    return maybe_has_sideeffect( to_const_ExprCast(x)->x, flags );

	case TAGExprWhere:
	    return maybe_has_sideeffect( to_const_ExprWhere(x)->x, flags );

	case TAGExprUnop:
	    return maybe_has_sideeffect( to_const_ExprUnop(x)->operand, flags );

	case TAGExprNotNullAssert:
	    if( !has_any_flag( flags, NO_SE_EXCEPTION ) ){
		return true;
	    }
	    return maybe_has_sideeffect( to_const_ExprNotNullAssert(x)->x, flags );

	case TAGExprWrapper:
	    return maybe_has_sideeffect( to_const_ExprWrapper(x)->x, flags );

    }
    return false;
}

bool maybe_has_sideeffect( const_optexpression x, int flags )
{
    bool res = false;

    switch( x->tag ){
	case TAGOptExprNone:
	    break;

	case TAGOptExpr:
	    res = maybe_has_sideeffect( to_const_OptExpr(x)->x, flags );
	    break;
    }
    return res;
}

static bool maybe_has_sideeffect( const_size x, int flags )
{
    bool res = false;

    switch( x->tag ){
	case TAGSizeDontcare:
	    break;

	case TAGSizeExpression:
	    res = maybe_has_sideeffect( to_const_SizeExpression(x)->x, flags );
	    break;
    }
    return res;
}

bool maybe_has_sideeffect( const_size_list xl, int flags )
{
    if( xl == size_listNIL ){
	return false;
    }
    for( unsigned int ix=0; ix<xl->sz; ix++ ){
	if( maybe_has_sideeffect( xl->arr[ix], flags ) ){
	    return true;
	}
    }
    return true;
}

static bool maybe_has_sideeffect( const_expression_list xl, int flags )
{
    if( xl == expression_listNIL ){
	return false;
    }
    for( unsigned int ix=0; ix<xl->sz; ix++ ){
	if( maybe_has_sideeffect( xl->arr[ix], flags ) ){
	    return true;
	}
    }
    return true;
}
