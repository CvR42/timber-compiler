// File: vnusbasetypes.cc
//
// Some handling functions for types defined in vnusbasetype.h

#include <stdio.h>
#include <tmc.h>
#include <ctype.h>
#include <string.h>

#include "vnusbasetypes.h"

// A union to easily convert between vnus_int and vnus_float and v/v
union intNfloat {
    vnus_int i;
    vnus_float f;
};

// A union to easily convert between vnus_long and vnus_double and v/v
union longNdouble {
    vnus_long l;
    vnus_double d;
};

void print_vnus_boolean( TMPRINTSTATE *st, vnus_boolean b )
{
    tm_printword( st, (b ? "TRUE" : "FALSE") );
}

void print_vnus_long( TMPRINTSTATE *st, const vnus_long e )
{
    char buf[sizeof(long long)*8];
    
    sprintf( buf, "%lld", e );
    tm_printword( st, buf );
}

void print_vnus_double( TMPRINTSTATE *st, const vnus_double e )
{
    char buf[sizeof(long long)*8];
    
    sprintf( buf, "%lld", vnus_double_bits_to_long( e ) );
    tm_printword( st, buf );
}

void print_vnus_float( TMPRINTSTATE *st, const vnus_float e )
{
    print_long( st, vnus_float_bits_to_int( e ) );
}

tmbool fscan_vnus_boolean( FILE *f, vnus_boolean *b )
{
    unsigned int n;
    char word[10];

    n = tm_fscanopenbrac( f );
    if( tm_fscanspace( f ) ){
	return TMTRUE;
    }
    if( tm_fscancons( f, word, 10 ) ){
	return TMTRUE;
    }
    if( 0 == strcmp( word, "TRUE" ) ){
	*b = true;
    }
    else if( 0 == strcmp( word, "FALSE" ) ){
	*b = false;
    }
    else {
	(void) sprintf( tm_errmsg, "Not a boolean: %s", word );
	return TMTRUE;
    }
    return tm_fscanclosebrac( f, n );
}

void print_BASETYPE( TMPRINTSTATE *st, BASETYPE t )
{
    switch( t ){
        case BT_BOOLEAN:	tm_printword( st, "BOOLEAN" );	break;
        case BT_BYTE:		tm_printword( st, "BYTE" );	break;
        case BT_CHAR:		tm_printword( st, "CHAR" );	break;
        case BT_COMPLEX:	tm_printword( st, "COMPLEX" );	break;
        case BT_DOUBLE:		tm_printword( st, "DOUBLE" );	break;
        case BT_FLOAT:		tm_printword( st, "FLOAT" );	break;
        case BT_INT:		tm_printword( st, "INT" );	break;
        case BT_LONG:		tm_printword( st, "LONG" );	break;
        case BT_SHORT:		tm_printword( st, "SHORT" );	break;
        case BT_STRING:		tm_printword( st, "STRING" );	break;
    }
}

tmbool fscan_BASETYPE( FILE *f, BASETYPE *b )
{
    unsigned int n;
    char word[10];

    n = tm_fscanopenbrac( f );
    if( tm_fscanspace( f ) ){
	return TMTRUE;
    }
    if( tm_fscancons( f, word, 10 ) ){
	return TMTRUE;
    }
    if( 0 == strcmp( word, "BOOLEAN" ) ){
	*b = BT_BOOLEAN;
    }
    else if( 0 == strcmp( word, "BYTE" ) ){
	*b = BT_BYTE;
    }
    else if( 0 == strcmp( word, "CHAR" ) ){
	*b = BT_CHAR;
    }
    else if( 0 == strcmp( word, "COMPLEX" ) ){
	*b = BT_COMPLEX;
    }
    else if( 0 == strcmp( word, "DOUBLE" ) ){
	*b = BT_DOUBLE;
    }
    else if( 0 == strcmp( word, "FLOAT" ) ){
	*b = BT_FLOAT;
    }
    else if( 0 == strcmp( word, "INT" ) ){
	*b = BT_INT;
    }
    else if( 0 == strcmp( word, "LONG" ) ){
	*b = BT_LONG;
    }
    else if( 0 == strcmp( word, "SHORT" ) ){
	*b = BT_SHORT;
    }
    else if( 0 == strcmp( word, "STRING" ) ){
	*b = BT_STRING;
    }
    else {
	(void) sprintf( tm_errmsg, "Not a BASETYPE: %s", word );
	return TMTRUE;
    }
    return tm_fscanclosebrac( f, n );
}

void print_BINOP( TMPRINTSTATE *st, BINOP t )
{
    switch( t )
    {
        case BINOP_AND:			tm_printword( st, "AND" );	break;
        case BINOP_DIVIDE:		tm_printword( st, "DIVIDE" );	break;
        case BINOP_EQUAL:		tm_printword( st, "EQUAL" );	break;
        case BINOP_GREATER:		tm_printword( st, "GREATER" );	break;
        case BINOP_GREATEREQUAL:	tm_printword( st, "GREATEREQUAL" ); break;
        case BINOP_LESS:		tm_printword( st, "LESS" );	break;
        case BINOP_LESSEQUAL:		tm_printword( st, "LESSEQUAL" );	break;
        case BINOP_MINUS:		tm_printword( st, "MINUS" );	break;
        case BINOP_MOD:			tm_printword( st, "MOD" );	break;
        case BINOP_NOTEQUAL:		tm_printword( st, "NOTEQUAL" );	break;
        case BINOP_OR:			tm_printword( st, "OR" );	break;
        case BINOP_PLUS:		tm_printword( st, "PLUS" );	break;
        case BINOP_SHIFTLEFT:		tm_printword( st, "SHIFTLEFT" ); break;
        case BINOP_SHIFTRIGHT:		tm_printword( st, "SHIFTRIGHT" ); break;
        case BINOP_SHORTAND:		tm_printword( st, "SHORTAND" );	break;
        case BINOP_SHORTOR:		tm_printword( st, "SHORTOR" );	break;
        case BINOP_TIMES:		tm_printword( st, "TIMES" );	break;
        case BINOP_USHIFTRIGHT:		tm_printword( st, "USHIFTRIGHT" ); break;
        case BINOP_XOR:			tm_printword( st, "XOR" ); break;
    }
}

tmbool fscan_BINOP( FILE *f, BINOP *b )
{
    unsigned int n;
    char word[20];

    n = tm_fscanopenbrac( f );
    if( tm_fscanspace( f ) ){
	return TMTRUE;
    }
    if( tm_fscancons( f, word, 20 ) ){
	return TMTRUE;
    }
    if( 0 == strcmp( word, "AND" ) ){
	*b = BINOP_AND;
    }
    else if( 0 == strcmp( word, "SHORTAND" ) ){
	*b = BINOP_SHORTAND;
    }
    else if( 0 == strcmp( word, "SHORTOR" ) ){
	*b = BINOP_SHORTOR;
    }
    else if( 0 == strcmp( word, "OR" ) ){
	*b = BINOP_OR;
    }
    else if( 0 == strcmp( word, "MOD" ) ){
	*b = BINOP_MOD;
    }
    else if( 0 == strcmp( word, "PLUS" ) ){
	*b = BINOP_PLUS;
    }
    else if( 0 == strcmp( word, "MINUS" ) ){
	*b = BINOP_MINUS;
    }
    else if( 0 == strcmp( word, "TIMES" ) ){
	*b = BINOP_TIMES;
    }
    else if( 0 == strcmp( word, "DIVIDE" ) ){
	*b = BINOP_DIVIDE;
    }
    else if( 0 == strcmp( word, "EQUAL" ) ){
	*b = BINOP_EQUAL;
    }
    else if( 0 == strcmp( word, "NOTEQUAL" ) ){
	*b = BINOP_NOTEQUAL;
    }
    else if( 0 == strcmp( word, "LESS" ) ){
	*b = BINOP_LESS;
    }
    else if( 0 == strcmp( word, "LESSEQUAL" ) ){
	*b = BINOP_LESSEQUAL;
    }
    else if( 0 == strcmp( word, "GREATER" ) ){
	*b = BINOP_GREATER;
    }
    else if( 0 == strcmp( word, "GREATEREQUAL" ) ){
	*b = BINOP_GREATEREQUAL;
    }
    else if( 0 == strcmp( word, "XOR" ) ){
	*b = BINOP_XOR;
    }
    else if( 0 == strcmp( word, "SHIFTLEFT" ) ){
	*b = BINOP_SHIFTLEFT;
    }
    else if( 0 == strcmp( word, "SHIFTRIGHT" ) ){
	*b = BINOP_SHIFTRIGHT;
    }
    else if( 0 == strcmp( word, "USHIFTRIGHT" ) ){
	*b = BINOP_USHIFTRIGHT;
    }
    else {
	(void) sprintf( tm_errmsg, "Not a BINOP: %s", word );
	return TMTRUE;
    }
    return tm_fscanclosebrac( f, n );
}

void print_UNOP( TMPRINTSTATE *st, UNOP t )
{
    switch( t )
    {
	case UNOP_NOT:		tm_printword( st, "NOT" );	break;
	case UNOP_PLUS:		tm_printword( st, "PLUS" );	break;
	case UNOP_NEGATE:	tm_printword( st, "NEGATE" );	break;
    }
}

tmbool fscan_UNOP( FILE *f, UNOP *b )
{
    unsigned int n;
    char word[20];

    n = tm_fscanopenbrac( f );
    if( tm_fscanspace( f ) ){
	return TMTRUE;
    }
    if( tm_fscancons( f, word, 20 ) ){
	return TMTRUE;
    }
    if( 0 == strcmp( word, "NOT" ) ){
	*b = UNOP_NOT;
    }
    else if( 0 == strcmp( word, "PLUS" ) ){
	*b = UNOP_PLUS;
    }
    else if( 0 == strcmp( word, "NEGATE" ) ){
	*b = UNOP_NEGATE;
    }
    else {
	(void) sprintf( tm_errmsg, "Not an UNOP: %s", word );
	return TMTRUE;
    }
    return tm_fscanclosebrac( f, n );
}

tmbool fscan_vnus_byte( FILE *f, vnus_byte *b )
{
    tmshort n;
    tmbool res = fscan_tmshort( f, &n );
    *b = n;
    return res;
}

tmbool fscan_vnus_short( FILE *f, vnus_short *b )
{
    int n;
    tmbool res = fscan_int( f, &n );
    *b = n;
    return res;
}

tmbool fscan_vnus_int( FILE *f, vnus_int *b )
{
    long n;
    tmbool res = fscan_long( f, &n );
    *b = n;
    return res;
}

tmbool fscan_vnus_float( FILE *f, vnus_float *b )
{
    long n;
    tmbool res = fscan_long( f, &n );
    *b = intbits_to_vnus_float( n );
    return res;
}

#define MAX_LONG_DIGITS 200

tmbool fscan_vnus_long( FILE *f, vnus_long *v )
{
    unsigned int brac = tm_fscanopenbrac( f );
    char buf[MAX_LONG_DIGITS+2];
    char *bufp = buf;

    int c = fgetc( f );
    if( c == '-' || c == '+' ){
	*bufp++ = c;
	c = fgetc( f );
    } 
    while( isdigit( c ) ){
	*bufp++ = c;
	c = fgetc( f );
	if( bufp>=buf+MAX_LONG_DIGITS ){
	    strcpy( tm_errmsg, "vnus_long has too many digits" );
	    return TMTRUE;
	}
    }
    ungetc( c, f );
    if( bufp == buf ){
	strcpy( tm_errmsg, "vnus_long expected" );
	return TMTRUE;
    }
    *bufp = '\0';
    *v = string_to_vnus_long( buf );
    return tm_fscanclosebrac( f, brac );
}

tmbool fscan_vnus_double( FILE *f, vnus_double *v )
{
    unsigned int brac = tm_fscanopenbrac( f );
    char buf[MAX_LONG_DIGITS+2];
    char *bufp = buf;

    int c = fgetc( f );
    if( c == '-' || c == '+' ){
	*bufp++ = c;
	c = fgetc( f );
    } 
    while( isdigit( c ) ){
	*bufp++ = c;
	c = fgetc( f );
	if( bufp>=buf+MAX_LONG_DIGITS ){
	    strcpy( tm_errmsg, "vnus_double has too many digits" );
	    return TMTRUE;
	}
    }
    ungetc( c, f );
    if( bufp == buf ){
	strcpy( tm_errmsg, "vnus_double expected" );
	return TMTRUE;
    }
    *bufp = '\0';
    vnus_long n = string_to_vnus_long( buf );
    *v = longbits_to_vnus_double( n );
    return tm_fscanclosebrac( f, brac );
}

vnus_int vnus_float_bits_to_int( vnus_float n )
{
    intNfloat u;
    u.f = n;
    return u.i;
}

vnus_long vnus_double_bits_to_long( vnus_double n )
{
    longNdouble u;
    u.d = n;
    return u.l;
}

vnus_float intbits_to_vnus_float( vnus_int n )
{
    intNfloat u;
    u.i = n;

    return u.f;
}

vnus_double longbits_to_vnus_double( vnus_long n )
{
    longNdouble u;
    u.l = n;
    return u.d;
}
