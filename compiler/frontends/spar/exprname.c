/* File: typename.c
 *
 * Textual representation of types
 */

#include <tmc.h>
#include <string.h>

#include "defs.h"
#include "tmadmin.h"
#include "exprname.h"
#include "typename.h"
#include "error.h"
#include "service.h"


// Given an expression 'x', return a string representation of
// the expression. The string is intended for error messages and
// logging messages, so clarity is more important than clarity.
tmstring name_expression( const_expression x )
{
    tmstring res = tmstringNIL;
    char buf[30];

    if( x == expressionNIL ){
	return new_tmstring( "(unknown)" );
    }
    switch( x->tag ){
	case TAGVariableNameExpression:
	    res = rdup_tmstring( to_const_VariableNameExpression(x)->name->sym->name );
	    break;

	case TAGByteExpression:
	    sprintf( buf, "%d", (int) to_const_ByteExpression(x)->v );
	    res = new_tmstring( buf );
	    break;

	case TAGShortExpression:
	    sprintf( buf, "%d", (int) to_const_ShortExpression(x)->v );
	    res = new_tmstring( buf );
	    break;

	case TAGIntExpression:
	    sprintf( buf, "%ld", (long int) to_const_IntExpression(x)->v );
	    res = new_tmstring( buf );
	    break;

	case TAGLongExpression:
	    __extension__ sprintf( buf, "%lld", (long long int) to_const_LongExpression(x)->v );
	    res = new_tmstring( buf );
	    break;

	case TAGFloatExpression:
	    sprintf( buf, "%g", to_const_FloatExpression(x)->v );
	    res = rdup_tmstring( buf );
	    break;

	case TAGDoubleExpression:
	    sprintf( buf, "%g", to_const_DoubleExpression(x)->v );
	    res = rdup_tmstring( buf );
	    break;

	case TAGCharExpression:
	{
	    vnus_char c = to_const_CharExpression(x)->c;
	    if( c<0x80 && c>=' ' ){
		// Innocent character, just print it.
		res = printf_tmstring( "'%c'", (char) c );
	    }
	    else {
		// Print as unicode escape sequence.
		res = printf_tmstring( "'\\u%04x'", (long int) c );
	    }
	    break;
	}

	case TAGBooleanExpression:
	    res = new_tmstring( to_const_BooleanExpression(x)->b?"true":"false" );
	    break;

	case TAGStringExpression:
	    res = printf_tmstring( "\"%s\"", to_const_StringExpression(x)->s );
	    break;

	case TAGNullExpression:
	    res = new_tmstring( "null" );
	    break;

 	case TAGSizeofExpression:
	{
	    tmstring tnm = typename_type( to_const_SizeofExpression(x)->t );
	    res = printf_tmstring( "__sizeof %s", tnm );
	    rfre_tmstring( tnm );
	    break;
	}

	case TAGOuterThisExpression:
	{
	    tmstring tnm = typename_type( to_const_OuterThisExpression(x)->t );
	    res = printf_tmstring( "%s.this", tnm );
	    rfre_tmstring( tnm );
	    break;
	}

 	case TAGTypeExpression:
	{
	    tmstring tnm = typename_type( to_const_TypeExpression(x)->t );
	    res = printf_tmstring( "type %s", tnm );
	    rfre_tmstring( tnm );
	    break;
	}

	case TAGInternalizeExpression:
	    res = name_expression( to_const_InternalizeExpression(x)->x );
	    break;

	case TAGAnnotationExpression:
	    res = name_expression( to_const_AnnotationExpression(x)->x );
	    break;

	case TAGBracketExpression:
	    res = name_expression( to_const_BracketExpression(x)->x );
	    break;

	case TAGDefaultValueExpression:
	{
	    type t = to_const_DefaultValueExpression(x)->t;
	    if( t->tag == TAGPrimitiveType ){
		BASETYPE bt = to_PrimitiveType(t)->base;
		if( bt == BT_INT || bt == BT_LONG || bt == BT_SHORT ){
		    res = new_tmstring( "0" );
		}
		else {
		    res = new_tmstring( "<expr>" );
		}
	    }
	    else {
		res = new_tmstring( "null" );
	    }
	    break;
	}

	case TAGArrayInitExpression:
	case TAGAssignOpExpression:
	case TAGBinopExpression:
	case TAGShortopExpression:
	case TAGCastExpression:
	case TAGClassExpression:
	case TAGClassIdExpression:
	case TAGClassInstanceOfExpression:
	case TAGComplexExpression:
	case TAGFieldExpression:
	case TAGFieldInvocationExpression:
	case TAGForcedCastExpression:
	case TAGGetLengthExpression:
	case TAGGetSizeExpression:
	case TAGGetBufExpression:
	case TAGIfExpression:
	case TAGOuterSuperInvocationExpression:
	case TAGInstanceOfExpression:
	case TAGTypeInstanceOfExpression:
	case TAGInterfaceInstanceOfExpression:
	case TAGMethodInvocationExpression:
	case TAGNewArrayExpression:
	case TAGNewClassExpression:
	case TAGNewInitArrayExpression:
	case TAGNewRecordExpression:
	case TAGNotNullAssertExpression:
	case TAGPostDecrementExpression:
	case TAGPostIncrementExpression:
	case TAGPreDecrementExpression:
	case TAGPreIncrementExpression:
	case TAGSubscriptExpression:
	case TAGOuterSuperFieldExpression:
	case TAGSuperFieldExpression:
	case TAGSuperInvocationExpression:
	case TAGTypeFieldExpression:
	case TAGTypeInvocationExpression:
	case TAGUnopExpression:
	case TAGVectorExpression:
	case TAGVectorSubscriptExpression:
	case TAGWhereExpression:
	    res = new_tmstring( "<expr>" );
	    break;

    }
    return res;
}

// Given an expression list 'xl', return a string representation of
// the expression list. The string is intended for error messages and
// logging messages, so clarity is more important than clarity.
tmstring name_expression_list( const_expression_list xl )
{
    char buf[10000];
    unsigned int pos = 0;

    buf[pos++] = '(';
    if( xl->sz != 0 ){
	for( unsigned int ix=0; ix<xl->sz; ix++ ){
	    tmstring xnm = name_expression( xl->arr[ix] );

	    if( ix != 0 ){
		buf[pos++] = ',';
	    }
	    strcpy( buf+pos, xnm );
	    pos += strlen( xnm );
	    rfre_tmstring( xnm );
	}
    }
    buf[pos++] = ')';
    buf[pos] = '\0';
    return new_tmstring( buf );
}
