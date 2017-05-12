/* File: typename.c
 *
 * Textual representation of types
 */

#include <tmc.h>
#include <string.h>

#include "defs.h"
#include "tmadmin.h"
#include "typename.h"
#include "exprname.h"
#include "service.h"
#include "typederive.h"
#include "error.h"

/* Given a base type 'bt', return a string containing the name of
 * that type.
 */
static tmstring name_basetype( BASETYPE bt )
{
    tmstring ans = tmstringNIL;

    switch( bt ){
        case BT_BOOLEAN:	ans = new_tmstring( "boolean" );	break;
        case BT_BYTE:        	ans = new_tmstring( "byte" );		break;
        case BT_SHORT:        	ans = new_tmstring( "short" );		break;
        case BT_INT:        	ans = new_tmstring( "int" );		break;
        case BT_LONG:        	ans = new_tmstring( "long" );		break;
        case BT_FLOAT:		ans = new_tmstring( "float" );		break;
        case BT_DOUBLE:		ans = new_tmstring( "double" );		break;
        case BT_COMPLEX:	ans = new_tmstring( "complex" );	break;
        case BT_STRING:		ans = new_tmstring( "__string" );	break;
        case BT_CHAR:		ans = new_tmstring( "char" );		break;
    }
    return ans;
}

/* Given a rank 'rank', return a list with that many '*' in it. */
static tmstring name_Size_list( const_expression rankexpr )
{
    char buf[5000];

    if( rankexpr->tag == TAGIntExpression ){
	vnus_int v = to_const_IntExpression(rankexpr)->v;

	if( v == 1 ){
	    return new_tmstring( "[]" );
	}
	if( v == 2 ){
	    return new_tmstring( "[*,*]" );
	}
	if( v == 3 ){
	    return new_tmstring( "[*,*,*]" );
	}
    }
    tmstring val = name_expression( rankexpr );
    sprintf( buf, "[*^%s]", val );
    rfre_tmstring( val );
    return new_tmstring( buf );
}

/* Given a list of types, return a string containing a bracketed
 * list of these types.
 */
static tmstring type_list_string( const type_list tl, const char openbrac, const char closebrac )
{
    char buf[10000];
    unsigned int pos = 0;

    buf[pos++] = openbrac;
    if( tl != type_listNIL && tl->sz != 0 ){
	for( unsigned int ix=0; ix<tl->sz; ix++ ){
	    tmstring tnm = typename_type( tl->arr[ix] );

	    if( ix != 0 ){
		buf[pos++] = ',';
	    }
	    strcpy( buf+pos, tnm );
	    pos += strlen( tnm );
	    rfre_tmstring( tnm );
	}
    }
    buf[pos++] = closebrac;
    buf[pos] = '\0';
    return new_tmstring( buf );
}

/* Given a type 't', return a string with the name of that type. */
tmstring typename_type( const_type t )
{
    tmstring res = tmstringNIL;

    if( t == typeNIL ){
	return new_tmstring( "(unknown)" );
    }
    switch( t->tag ){
        case TAGPrimitiveType:
            return name_basetype( to_const_PrimitiveType(t)->base );

        case TAGArrayType:
        {
            tmstring basenm = typename_type( to_const_ArrayType(t)->elmtype );
            tmstring sizestr = name_Size_list( to_const_ArrayType(t)->rank );

	    res = printf_tmstring( "%s%s", basenm, sizestr );
            rfre_tmstring( sizestr );
            rfre_tmstring( basenm );
            break;
        }

        case TAGPrimArrayType:
        {
            tmstring basenm = typename_type( to_const_PrimArrayType(t)->elmtype );

	    res = printf_tmstring( "__array(%s)", basenm );
            rfre_tmstring( basenm );
            break;
        }

	case TAGPragmaType:
	    return typename_type( to_const_PragmaType(t)->t );

	case TAGVoidType:
	    return new_tmstring( "void" );

	case TAGVectorType:
	{
            tmstring basenm = typename_type( to_const_VectorType(t)->elmtype );
	    tmstring val = name_expression( to_const_VectorType(t)->power );
	    res = printf_tmstring( "[%s^%s]", basenm, val );
            rfre_tmstring( basenm );
            rfre_tmstring( val );
	    break;
	}

	case TAGTupleType:
	    res = type_list_string( to_const_TupleType(t)->fields, '[', ']' );
	    break;

        case TAGGenericObjectType:
	{
	    tmstring tnm = rdup_tmstring( to_const_ObjectType(t)->name->sym->name );
	    res = printf_tmstring( "%s(<parameters>)", tnm );
	    rfre_tmstring( tnm );
	    break;
	}

        case TAGObjectType:
	{
	    const_ObjectType ot = to_const_ObjectType(t);

	    if( ot->name == origsymbolNIL ){
		return new_tmstring( "(unknown)" );
	    }
	    return rdup_tmstring( ot->name->sym->name );
	}

	case TAGTypeType:
	    return new_tmstring( "type" );

	case TAGNullType:
	    return new_tmstring( "null" );

	case TAGExceptionVariableType:
	    return new_tmstring( "exeptionvariabletype" );

	case TAGGCTopRefLinkType:
	    return new_tmstring( "gctopreflinktype" );

	case TAGGCRefLinkType:
	    return new_tmstring( "gcreflinktype" );

	case TAGFunctionType:
	{
	    tmstring parms = type_list_string( to_const_FunctionType(t)->formals, '(', ')' );
	    tmstring tnm = typename_type( to_const_FunctionType(t)->rettype );

	    res = printf_tmstring( "%s function%s", tnm, parms );
	    rfre_tmstring( parms );
	    rfre_tmstring( tnm );
	    break;
	}

        case TAGTypeOf:
	{
	    tmstring xnm = name_expression( to_const_TypeOf(t)->x );
	    res = printf_tmstring( "typeof(%s)", xnm );
	    rfre_tmstring( xnm );
	    break;
	}

        case TAGTypeOfIf:
	{
	    tmstring nmthen = name_expression( to_const_TypeOfIf(t)->thenval );
	    tmstring nmelse = name_expression( to_const_TypeOfIf(t)->elseval );
	    res = printf_tmstring( "typeofif(%s,%s)", nmthen, nmelse );
	    rfre_tmstring( nmthen );
	    rfre_tmstring( nmelse );
	    break;
	}

    }
    return res;
}

/* Given a signature, return a string describing this this signature. */
tmstring typename_Signature( const_Signature sg )
{
    tmstring res;
    tmstring parms = type_list_string( sg->parameters, '(', ')' );

    if( sg->name == tmsymbolNIL ){
	res = printf_tmstring( "(unknown method)%s", parms );
    }
    else {
	res = printf_tmstring( "%s%s", sg->name->name, parms );
    }
    rfre_tmstring( parms );
    return res;
}

tmstring typename_formals( const_FormalParameter_list formals )
{
    char buf[10000];
    unsigned int pos = 0;

    buf[pos++] = '(';
    if( formals->sz != 0 ){
	for( unsigned int ix=0; ix<formals->sz; ix++ ){
	    tmstring tnm = typename_type( formals->arr[ix]->t );

	    if( ix != 0 ){
		buf[pos++] = ',';
	    }
	    strcpy( buf+pos, tnm );
	    pos += strlen( tnm );
	    rfre_tmstring( tnm );
	}
    }
    buf[pos++] = ')';
    buf[pos] = '\0';
    return new_tmstring( buf );
}
