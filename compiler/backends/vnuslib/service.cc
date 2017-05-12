/* File: service.cc
 *
 * Miscellaneous service routines.
 */

#include <tmc.h>
#include <ctype.h>
#include <assert.h>

#include "vnusbasetypes.h"
#include "tmadmin.h"
#include "vnusctl.h"

/* Return a fake origin for a generated construct. */
origin real_gen_origin( const char *f, const int l )
{
    static int gencount = 0;
    char buf[256];

    sprintf( buf, "%s::%d", f, l );
    return new_origin( add_tmsymbol( buf ), gencount++ );
}

origsymbol real_gen_origsymbol( const char *f, const int l, const char *name )
{
    return new_origsymbol( gen_tmsymbol(name), real_gen_origin( f, l ) );
}

origsymbol real_add_origsymbol( const char *f, const int l, const char *name )
{
    return new_origsymbol( add_tmsymbol(name), real_gen_origin( f, l ) );
}

bool member_origsymbol_list( const_origsymbol_list l, const_origsymbol s )
{
    for( unsigned int ix=0; ix<l->sz; ix++ ){
	if( l->arr[ix]->sym == s->sym ){
	    return true;
	}
    }
    return false;
}

bool member_tmsymbol_list( const_tmsymbol_list l, const tmsymbol s )
{
    for( unsigned int ix=0; ix<l->sz; ix++ ){
	if( l->arr[ix] == s ){
	    return true;
	}
    }
    return false;
}

tmsymbol_list add_tmsymbol_list( tmsymbol_list l, tmsymbol s )
{
    if( !member_tmsymbol_list( l, s ) ){
	l = append_tmsymbol_list( l, s );
    }
    return l;
}

// Given a location, return the equivalent expression
expression location_to_expression( const_location l )
{
    expression res = NULL;

    switch( l->tag ){
	case TAGLocName:
	    res = new_ExprName( rdup_origsymbol( to_const_LocName(l)->name ) );
	    break;

	case TAGLocField:
	    res = new_ExprField(
		rdup_expression( to_const_LocField(l)->rec ),
		rdup_origsymbol( to_const_LocField(l)->field )
	    );
	    break;

	case TAGLocFieldNumber:
	    res = new_ExprFieldNumber(
		rdup_expression( to_const_LocFieldNumber(l)->rec ),
		to_const_LocFieldNumber(l)->field
	    );
	    break;

	case TAGLocSelection:
	    res = new_ExprSelection(
		rdup_expression( to_const_LocSelection(l)->shape ),
		rdup_expression_list( to_const_LocSelection(l)->indices )
	    );
	    break;

	case TAGLocFlatSelection:
	    res = new_ExprFlatSelection(
		rdup_expression( to_const_LocFlatSelection(l)->shape ),
		rdup_expression( to_const_LocFlatSelection(l)->index )
	    );
	    break;

	case TAGLocWrapper:
	    res = new_ExprWrapper(
		rdup_Pragma_list( to_const_LocWrapper(l)->pragmas ),
		rdup_OwnerExpr_list( to_const_LocWrapper(l)->on ),
		location_to_expression( to_const_LocWrapper(l)->l )
	    );
	    break;

	case TAGLocDeref:
	    res = new_ExprDeref( rdup_expression( to_const_LocDeref(l)->ref ) );
	    break;

	case TAGLocWhere:
	    res = new_ExprWhere(
		rdup_tmsymbol( to_const_LocWhere(l)->scope ),
		location_to_expression( to_const_LocWhere(l)->l )
	    );
	    break;

	case TAGLocNotNullAssert:
	    res = new_ExprNotNullAssert(
		location_to_expression( to_const_LocNotNullAssert(l)->l )
	    );
	    break;
    }
    return res;
}

int find_field_fields( const_field_list fields, const_origsymbol fld )
{
    assert(fld != NULL);
    
    for( unsigned int ix=0; ix<fields->sz; ix++ ){
	field f = fields->arr[ix];
	if( f->name != origsymbolNIL && f->name->sym == fld->sym ){
	    return ix;
	}
    }
    return -1;
}

/* Given a type 't' and a basetype 'bt', return true if the types are
 * the same.
 */
bool is_this_basetype( const_type t, const BASETYPE bt )
{
    if( t == typeNIL ){
	/* We're in trouble anyway, so don't be difficult. */
	return true;
    }
    switch( t->tag ){
	case TAGTypeBase:
	    return (to_const_TypeBase(t)->base == bt);

	case TAGTypeMap:
	    return is_this_basetype( to_const_TypeMap(t)->t, bt );

	case TAGTypePragmas:
	    return is_this_basetype( to_const_TypePragmas(t)->t, bt );

	case TAGTypeShape:
	case TAGTypePointer:
	case TAGTypeRecord:
	case TAGTypeNamedRecord:
	case TAGTypeArray:
	case TAGTypeUnsizedArray:
	case TAGTypeNeutralPointer:
	case TAGTypeFunction:
	case TAGTypeProcedure:
	    break;
    }
    return false;
}

/* Given a basetype 't', return true if it is a integral type. */
static bool is_integral_basetype( const BASETYPE t )
{
    if( t == BT_BYTE ) return true;
    if( t == BT_SHORT ) return true;
    if( t == BT_INT ) return true;
    if( t == BT_LONG ) return true;
    return false;
}

/* Given a type 't', return true if it is a integral type. */
bool is_integral_type( const_type t )
{
    if( t == typeNIL ){
	/* We're in trouble anyway, so don't be difficult. */
	return true;
    }
    switch( t->tag ){
	case TAGTypeBase:
	    return is_integral_basetype( to_const_TypeBase(t)->base );

	case TAGTypeMap:
	    return is_integral_type( to_const_TypeMap(t)->t );

	case TAGTypePragmas:
	    return is_integral_type( to_const_TypePragmas(t)->t );

	case TAGTypeShape:
	case TAGTypePointer:
	case TAGTypeRecord:
	case TAGTypeNamedRecord:
	case TAGTypeArray:
	case TAGTypeUnsizedArray:
	case TAGTypeNeutralPointer:
	case TAGTypeFunction:
	case TAGTypeProcedure:
	    break;

    }
    return false;
}

vnus_char evaluate_char_constant( const char *s )
{
    vnus_char v = s[0];

    switch( s[0] ){
	case '\\':
	    // See JLS2 3.10.6 for the table I got this from.
	    switch( s[1] ){
		case 'b':	v = 0x08;	break;
		case 't':	v = 0x09;	break;
		case 'n':	v = 0x0a;	break;
		case 'f':	v = 0x0c;	break;
		case 'r':	v = 0x0d;	break;
		case '"':	v = 0x22;	break;
		case '\'':	v = 0x27;	break;
		case '\\':	v = 0x5c;	break;

		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		    v = s[1]-'0';
		    if( s[2]>='0' && s[2]<='7' ){
			v *= 8;
			v += s[2]-'0';
			if( s[3]>='0' && s[3]<='7' ){
			    v *= 8;
			    v += s[3]-'0';
			}
		    }
		    break;

		case 'u':
		{
		    const char *p = s+1;
		    while( *p == 'u' ){
			p++;
		    }
		    v = strtol( p, NULL, 16 );
		    break;
		}

		default:
		    v = s[1];
	    }
	    break;

	default:
	    v = s[0];
	    break;
    }
    return v;
}

static int decode_char( const char c )
{
    if( isdigit( c ) ){
	return c-'0';
    }
    if( c>='a' && c<= 'f' ){
	return 0xa+c-'a';
    }
    if( c>='A' && c<= 'F' ){
	return 0xa+c-'A';
    }
    return -1;
}

vnus_long string_to_vnus_long( const char *p )
{
    vnus_long val = 0;
    int base = 10;
    bool neg = false;

    while( isspace( *p ) ){
	p++;
    }
    if( *p == '+' ){
	p++;
    }
    if( *p == '-' ){
	neg = true;
	p++;
    }
    if( *p == '0' ){
	base = 8;
	p++;
	if( *p == 'x' || *p == 'X' ){
	    base = 16;
	    p++;
	}
    }
    for(;;){
	int code = decode_char( *p++ );
	if( code<0 ){
	    break;
	}
	val *= base;
	val += code;
    }
    return neg?-val:val;
}

// Return true iff the operator is associative.
bool is_associative_operator( BINOP op )
{
    return (
	op == BINOP_AND ||
	op == BINOP_OR ||
	op == BINOP_PLUS ||
	op == BINOP_SHORTAND ||
	op == BINOP_SHORTOR ||
	op == BINOP_TIMES ||
	op == BINOP_XOR
    );
}

