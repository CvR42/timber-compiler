/* File: constant.c
 *
 * Evaluate and check constant expressions.
 */

#include <tmc.h>

#include "defs.h"
#include "tmadmin.h"
#include "error.h"

vnus_long evaluate_integral_constant( const_expression x )
{
    switch( x->tag ){
	case TAGByteExpression:
	    return to_const_ByteExpression(x)->v;

	case TAGShortExpression:
	    return to_const_ShortExpression(x)->v;
	    
	case TAGIntExpression:
	    return to_const_IntExpression(x)->v;

	case TAGLongExpression:
	    return to_const_LongExpression(x)->v;

	case TAGCharExpression:
	    return to_const_CharExpression(x)->c;

	case TAGArrayInitExpression:
	case TAGAssignOpExpression:
	case TAGBooleanExpression:
	case TAGCastExpression:
	case TAGClassExpression:
	case TAGClassIdExpression:
	case TAGClassInstanceOfExpression:
	case TAGComplexExpression:
	case TAGDefaultValueExpression:
	case TAGDoubleExpression:
	case TAGFieldExpression:
	case TAGFieldInvocationExpression:
	case TAGFloatExpression:
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
	case TAGNotNullAssertExpression:
	case TAGNullExpression:
	case TAGOuterSuperFieldExpression:
	case TAGOuterSuperInvocationExpression:
	case TAGOuterThisExpression:
	case TAGPostDecrementExpression:
	case TAGPostIncrementExpression:
	case TAGPreDecrementExpression:
	case TAGPreIncrementExpression:
	case TAGSizeofExpression:
	case TAGStringExpression:
	case TAGSubscriptExpression:
	case TAGSuperFieldExpression:
	case TAGSuperInvocationExpression:
	case TAGTypeExpression:
	case TAGTypeFieldExpression:
	case TAGTypeInstanceOfExpression:
	case TAGTypeInvocationExpression:
	case TAGVariableNameExpression:
	case TAGVectorExpression:
	case TAGWhereExpression:
	case TAGShortopExpression:
	    internal_error( "Bad expression in integral constant" );
	    break;

	case TAGVectorSubscriptExpression:
	    // TODO: do somethings smarter than this.
	    internal_error( "Bad expression in integral constant" );
	    break;

	case TAGUnopExpression:
	{
	    const_UnopExpression ux = to_const_UnopExpression(x);
	    vnus_long v = evaluate_integral_constant( ux->operand );

	    switch( ux->optor ){
		case UNOP_NOT:
		    internal_error( "Bad operator in integral constant" );
		    break;

		case UNOP_INVERT:
		    return ~v;

		case UNOP_PLUS:
		    return v;

		case UNOP_NEGATE:
		    return -v;
	    }
	    break;
	}

	case TAGBinopExpression:
	{
	    const_BinopExpression bx = to_const_BinopExpression(x);
	    vnus_long a = evaluate_integral_constant( bx->left );
	    vnus_long b = evaluate_integral_constant( bx->right );

	    switch( bx->optor ){
		case BINOP_AND:
		    return a&b;

		case BINOP_OR:
		    return a|b;

		case BINOP_MOD:
		    if( b == 0 ){
			return 0;
		    }
		    return a%b;

		case BINOP_PLUS:
		    return a+b;

		case BINOP_MINUS:
		    return a-b;

		case BINOP_TIMES:
		    return a*b;

		case BINOP_DIVIDE:
		    if( b == 0 ){
			return 0;
		    }
		    return a/b;

		case BINOP_XOR:
		    return a^b;

		case BINOP_SHIFTLEFT:
		    return a<<b;

		case BINOP_SHIFTRIGHT:
		    return a>>b;

		case BINOP_USHIFTRIGHT:
		    return ((unsigned long) a)>>b;

		case BINOP_EQUAL:
		case BINOP_NOTEQUAL:
		case BINOP_LESS:
		case BINOP_LESSEQUAL:
		case BINOP_GREATER:
		case BINOP_GREATEREQUAL:
		    internal_error( "Bad operator in integral constant" );
		    break;
	    }
	    break;
	}

	case TAGAnnotationExpression:
	    return evaluate_integral_constant( to_const_AnnotationExpression(x)->x );

	case TAGBracketExpression:
	    return evaluate_integral_constant( to_const_BracketExpression(x)->x );
    }
    return 0;
}
