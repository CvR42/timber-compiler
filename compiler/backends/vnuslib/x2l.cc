/* File: x2l.cc
 *
 * expression to location conversion.
 */

#include <tmc.h>
#include <ctype.h>
#include <assert.h>

#include "vnusbasetypes.h"
#include "tmadmin.h"
#include "vnusctl.h"

// Given an expression, return a location that is equivalent to it.
// For expressions where this is not possible, return locationNIL.
location expression_to_location( const_expression x )
{
    location res = locationNIL;

    switch( x->tag ){
	case TAGExprAddress:
	case TAGExprArray:
	case TAGExprBinop:
	case TAGExprBoolean:
	case TAGExprByte:
	case TAGExprCast:
	case TAGExprChar:
	case TAGExprCheckedIndex:
	case TAGExprComplex:
	case TAGExprDouble:
	case TAGExprFilledNew:
	case TAGExprNulledNew:
	case TAGExprFloat:
	case TAGExprFunctionCall:
	case TAGExprGetBuf:
	case TAGExprGetLength:
	case TAGExprGetNewMaster:
	case TAGExprGetNewOwner:
	case TAGExprGetSize:
	case TAGExprIf:
	case TAGExprInt:
	case TAGExprIsBoundViolated:
	case TAGExprIsLowerBoundViolated:
	case TAGExprIsNewMaster:
	case TAGExprIsNewOwner:
	case TAGExprIsRaised:
	case TAGExprIsUpperBoundViolated:
	case TAGExprLong:
	case TAGExprLowerCheckedIndex:
	case TAGExprNew:
	case TAGExprNewArray:
	case TAGExprNewRecord:
	case TAGExprNotNullAssert:
	case TAGExprNull:
	case TAGExprRecord:
	case TAGExprReduction:
	case TAGExprShape:
	case TAGExprShort:
	case TAGExprSizeof:
	case TAGExprString:
	case TAGExprUnop:
	case TAGExprUpperCheckedIndex:
	case TAGExprWhere:
	    res = locationNIL;
	    break;

	case TAGExprName:
	    res = new_LocName( rdup_origsymbol( to_const_ExprName(x)->name ) );
	    break;

	case TAGExprWrapper:
	{
	    const_ExprWrapper xw = to_const_ExprWrapper(x);

	    location sub = expression_to_location( xw->x );
	    if( sub != locationNIL ){
		res = new_LocWrapper(
		    rdup_Pragma_list( xw->pragmas ),
		    rdup_OwnerExpr_list( xw->on ),
		    sub
		);
	    }
	    break;
	}

	case TAGExprSelection:
	{
	    const_ExprSelection xsel = to_const_ExprSelection(x);

	    res = new_LocSelection(
		rdup_expression( xsel->shape ),
		rdup_expression_list( xsel->indices )
	    );
	    break;
	}

	case TAGExprFlatSelection:
	{
	    const_ExprFlatSelection xsel = to_const_ExprFlatSelection(x);

	    res = new_LocFlatSelection(
		rdup_expression( xsel->shape ),
		rdup_expression( xsel->index )
	    );
	    break;
	}

	case TAGExprField:
	{
	    const_ExprField xf = to_const_ExprField(x);

	    res = new_LocField(
		rdup_expression( xf->rec ),
		rdup_origsymbol( xf->field )
	    );
	    break;
	}

	case TAGExprFieldNumber:
	{
	    const_ExprFieldNumber xf = to_const_ExprFieldNumber(x);

	    res = new_LocFieldNumber(
		rdup_expression( xf->rec ),
		xf->field
	    );
	    break;
	}

	case TAGExprDeref:
	    res = new_LocDeref( rdup_expression( to_const_ExprDeref(x)->ref ) );
	    break;
    }
    return res;
}
