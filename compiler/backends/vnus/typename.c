// File: typename.c

#include <tmc.h>
#include <string.h>

#include "defs.h"
#include "tmadmin.h"
#include "typename.h"
#include "type.h"

/* Given a base type 'bt', return a string containing the name of
 * that type.
 */
static tmstring name_basetype( BASETYPE bt )
{
    tmstring ans = tmstringNIL;

    switch( bt ){
	case BT_STRING: 	ans = new_tmstring( "string" ); 	break;
	case BT_BOOLEAN:	ans = new_tmstring( "boolean" );	break;
	case BT_BYTE:		ans = new_tmstring( "byte" );		break;
	case BT_SHORT:		ans = new_tmstring( "short" );		break;
	case BT_LONG:		ans = new_tmstring( "long" );		break;
	case BT_INT:		ans = new_tmstring( "int" );		break;
	case BT_FLOAT:		ans = new_tmstring( "float" );		break;
	case BT_DOUBLE:		ans = new_tmstring( "double" );		break;
	case BT_CHAR:		ans = new_tmstring( "char" );		break;
	case BT_COMPLEX:	ans = new_tmstring( "complex" );	break;
    }
    return ans;
}

/* Given an expression 'x', return a string describing this expression.
 * This is only for error messages, so don't go into too much detail.
 */
static tmstring name_expression( const_expression x )
{
    char buf[1000];

    switch( x->tag ){
	case TAGExprByte:
	    sprintf( buf, "%d", (int) to_const_ExprByte(x)->v );
	    return new_tmstring( buf );

	case TAGExprShort:
	    sprintf( buf, "%d", (int) to_const_ExprShort(x)->v );
	    return new_tmstring( buf );

	case TAGExprInt:
	    sprintf( buf, "%ld", (long int) to_const_ExprInt(x)->v );
	    return new_tmstring( buf );

	case TAGExprLong:
	    sprintf( buf, "%lld", to_const_ExprLong(x)->v );
	    return new_tmstring( buf );

	case TAGExprWrapper:	return name_expression( to_const_ExprWrapper(x)->x );
	case TAGExprNull:	return new_tmstring( "null" );

	case TAGExprSizeof:	return new_tmstring( "sizeof(<type>)" );

	case TAGExprAddress:
	case TAGExprArray:
	case TAGExprBinop:
	case TAGExprBoolean:
	case TAGExprCast:
	case TAGExprChar:
	case TAGExprCheckedIndex:
	case TAGExprComplex:
	case TAGExprDeref:
	case TAGExprDouble:
	case TAGExprField:
	case TAGExprFieldNumber:
	case TAGExprFilledNew:
	case TAGExprNulledNew:
	case TAGExprFlatSelection:
	case TAGExprFloat:
	case TAGExprFunctionCall:
	case TAGExprGetBuf:
	case TAGExprGetLength:
	case TAGExprGetNewMaster:
	case TAGExprGetNewOwner:
	case TAGExprGetSize:
	case TAGExprIf:
	case TAGExprIsBoundViolated:
	case TAGExprIsLowerBoundViolated:
	case TAGExprIsNewMaster:
	case TAGExprIsNewOwner:
	case TAGExprIsRaised:
	case TAGExprIsUpperBoundViolated:
	case TAGExprLowerCheckedIndex:
	case TAGExprNew:
	case TAGExprNewArray:
	case TAGExprNewRecord:
	case TAGExprRecord:
	case TAGExprReduction:
	case TAGExprSelection:
	case TAGExprShape:
	case TAGExprString:
	case TAGExprUnop:
	case TAGExprUpperCheckedIndex:
	case TAGExprWhere:
	    break;

	case TAGExprNotNullAssert:
	    return name_expression( to_const_ExprNotNullAssert(x)->x );

	case TAGExprName:
	    return rdup_tmstring( to_const_ExprName(x)->name->sym->name );
	    
    }
    return new_tmstring( "<expr>" );
}

/* Given a size 'sz', return a string with this size. */
static tmstring name_size( const size sz )
{
    char buf[5000];

    buf[0] = '\0';
    switch( sz->tag ){
	case TAGSizeDontcare:
	    strcpy( buf, "*" );
	    break;

	case TAGSizeExpression:
	    return name_expression( to_SizeExpression(sz)->x );

    }
    return new_tmstring( buf );
}

/* Given a list of sizes 'fl', return a string with these sizes. */
static tmstring name_size_list( const_size_list fl )
{
    char buf[5000];
    tmstring sizestr;

    strcpy( buf, "[" );
    for( unsigned int ix=0; ix<fl->sz; ix++ ){
	if( ix != 0 ){
	    strcat( buf, "," );
	}
	sizestr = name_size( fl->arr[ix] );
	strcat( buf, sizestr );
	rfre_tmstring( sizestr );
    }
    strcat( buf, "]" );
    return new_tmstring( buf );
}

// Given a field declaration, return a string representing it
static tmstring name_field( const_declaration_list decls, const_field f )
{
    char buf[5000];

    tmstring tnm = name_type( decls, f->elmtype );
    if( f->name == origsymbolNIL ){
	sprintf( buf, "%s", tnm );
    }
    else {
	sprintf( buf, "%s:%s", f->name->sym->name, tnm );
    }
    rfre_tmstring( tnm );
    return new_tmstring( buf );
}

/* Given a list of types, return a string containing a bracketed
 * list of these types.
 */
tmstring name_field_list( const_declaration_list decls, const_field_list tl )
{
    char buf[10000];
    unsigned int pos = 0;

    buf[pos++] = '[';
    if( tl->sz != 0 ){
	for( unsigned int ix=0; ix<tl->sz; ix++ ){
	    tmstring tnm = name_field( decls, tl->arr[ix] );

	    if( ix != 0 ){
		buf[pos++] = ',';
	    }
	    strcpy( buf+pos, tnm );
	    pos += strlen( tnm );
	    rfre_tmstring( tnm );
	}
    }
    buf[pos++] = ']';
    buf[pos] = '\0';
    return new_tmstring( buf );
}

// Generate a string for a parameter list.
tmstring name_type_list( const_declaration_list decls, const_type_list tl )
{
    char buf[10000];
    unsigned int pos = 0;

    buf[pos++] = '[';
    if( tl->sz != 0 ){
	for( unsigned int ix=0; ix<tl->sz; ix++ ){
	    tmstring tnm = name_type( decls, tl->arr[ix] );

	    if( ix != 0 ){
		buf[pos++] = ',';
	    }
	    if( tnm != tmstringNIL ){
		strcpy( buf+pos, tnm );
		pos += strlen( tnm );
		rfre_tmstring( tnm );
	    }
	}
    }
    buf[pos++] = ']';
    buf[pos] = '\0';
    return new_tmstring( buf );
}

// Given a type 't', return a string with the name of that type.
tmstring name_type( const_declaration_list decls, const_type t )
{
    char buf[5000];

    if( t == typeNIL ){
	return new_tmstring( "unknown type" );
    }
    switch( t->tag ){
	case TAGTypeBase:
	    return name_basetype( to_const_TypeBase(t)->base );

	case TAGTypeShape:
	{
	    tmstring basenm = name_type( decls, to_const_TypeShape(t)->elmtype );
	    tmstring sizestr = name_size_list( to_const_TypeShape(t)->sizes );

	    strcpy( buf, "shape " );
	    strcat( buf, sizestr );
	    strcat( buf, " of " );
	    strcat( buf, basenm );
	    rfre_tmstring( sizestr );
	    rfre_tmstring( basenm );
	    return new_tmstring( buf );
	}

	case TAGTypePointer:
	{
	    tmstring basenm = name_type( decls, to_const_TypePointer(t)->elmtype );

	    sprintf( buf, "pointer %s", basenm );
	    rfre_tmstring( basenm );
	    return new_tmstring( buf );
	}

	case TAGTypeNamedRecord:
	    sprintf( buf, "record %s", to_const_TypeNamedRecord(t)->name->sym->name );
	    return new_tmstring( buf );

	case TAGTypeRecord:
	{
	    tmstring fields = name_field_list( decls, to_const_TypeRecord(t)->fields );
	    sprintf( buf, "record %s", fields );
	    rfre_tmstring( fields );
	    return new_tmstring( buf );
	}

	case TAGTypeUnsizedArray:
	{
	    tmstring basenm = name_type( decls, to_const_TypeUnsizedArray(t)->elmtype );

	    sprintf( buf, "array of %s", basenm );
	    rfre_tmstring( basenm );
	    return new_tmstring( buf );
	}

	case TAGTypeArray:
	{
	    tmstring basenm = name_type( decls, to_const_TypeArray(t)->elmtype );

	    sprintf( buf, "%s[%u]", basenm, to_const_TypeArray(t)->sz );
	    rfre_tmstring( basenm );
	    return new_tmstring( buf );
	}

	case TAGTypeNeutralPointer:
	    return new_tmstring( "neutral pointer" );

	case TAGTypeProcedure:
	{
	    tmstring parms = name_type_list( decls, to_const_TypeProcedure(t)->formals );

	    sprintf( buf, "procedure%s", parms );
	    rfre_tmstring( parms );
	    return new_tmstring( buf );
	}

	case TAGTypeFunction:
	{
	    tmstring parms = name_type_list( decls, to_const_TypeFunction(t)->formals );
	    tmstring tnm = name_type( decls, to_const_TypeFunction(t)->rettype );

	    sprintf( buf, "function%s %s", parms, tnm );
	    rfre_tmstring( parms );
	    rfre_tmstring( tnm );
	    return new_tmstring( buf );
	}

	case TAGTypePragmas:
	    return name_type( decls, to_const_TypePragmas(t)->t );

	case TAGTypeMap:
	    return name_type( decls, to_const_TypeMap(t)->t );

    }
    return new_tmstring( "??" );
}
