/* File: vnus_types.c
 *
 * Routines to support the implementation of the vnus types.
 */

#include <tmc.h>
#include <stdlib.h>
#include <string.h>

#include "defs.h"
#include "vnus_types.h"
#include "tmadmin.h"
#include "error.h"
#include "ctype.h"
#include "service.h"

const int DOUBLE_NUM_MANTISSA_BITS = 52;
const vnus_long DOUBLE_EXPONENT_MASK = 0x7ff0000000000000LL;
const vnus_long DOUBLE_MANTISSA_MASK = 0x000fffffffffffffLL;
const int FLOAT_NUM_MANTISSA_BITS = 23;
const vnus_int FLOAT_EXPONENT_MASK = 0x7f800000L;
const vnus_int FLOAT_MANTISSA_MASK = 0x007fffffL;

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

vnus_long string_to_vnus_long( const char *p, const char **errmsg )
{
    vnus_long val = 0;
    int base = 10;

    *errmsg = (const char *) NULL;
    while( isspace( *p ) ){
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
	// TODO: make sure the literal isn't too large for a long.
	int code = decode_char( *p++ );
	if( code<0 ){
	    break;
	}
	if( code>=base ){
	    *errmsg = "illegal digit";
	}
	if( base == 10 ){
	    if( val>(VNUS_LONG_MAX/10) ){
		*errmsg = "long literal too large";
	    }
	}
	else {
	    if( val> (vnus_long) (VNUS_LONG_UNSIGNED_MAX/base) ){
		*errmsg = "long literal too large";
	    }
	}
	val *= base;
	val += code;
    }
    return val;
}

vnus_int string_to_vnus_int( const char *p, const char **errmsg )
{
    vnus_long val = string_to_vnus_long( p, errmsg );
    if( !*errmsg ){
	if( val<VNUS_INT_MIN ){
	    *errmsg = "int literal too small";
	}
	else if( val>VNUS_INT_UNSIGNED_MAX ){
	    // TODO: be more precise for decimal literals.
	    *errmsg = "int literal too large";
	}
    }
    return (vnus_int) val;
}

void print_vnus_long( TMPRINTSTATE *st, const vnus_long val )
{
#ifdef __GNUC__
    char buf[100];

    __extension__ sprintf( buf, "%lld", val );
    tm_printword( st, buf );
#else
    (void) st;
    (void) val;
    internal_error( "cannot print vnus_long" );
#endif
}

void fprint_vnus_long( FILE *f, const vnus_long val )
{
#ifdef __GNUC__
    __extension__ fprintf( f, "%lld", val );
#else
    (void) f;
    (void) val;
    internal_error( "cannot print vnus_long" );
#endif
}

void print_vnus_char( TMPRINTSTATE *st, const vnus_char c )
{
    char buf[100];

    if( c<0x80 && c>= ' ' ){
	// We can safely print this character.
	sprintf( buf, "'%c'", (char) c );
    }
    else {
	// Don't take any risks, print it as a \u escape sequence.
	sprintf( buf, "'\\u%04x'", (int) c );
	tm_printword( st, buf );
    }
}


static bool vnus_int_is_NaN( vnus_int n )
{
    return
	(n & FLOAT_EXPONENT_MASK) == FLOAT_EXPONENT_MASK &&
	(n & FLOAT_MANTISSA_MASK) != 0;
}

static bool vnus_long_is_NaN( vnus_long n )
{
    return
	(n & DOUBLE_EXPONENT_MASK) == DOUBLE_EXPONENT_MASK &&
	(n & DOUBLE_MANTISSA_MASK) != 0;
}

bool vnus_double_is_NaN( vnus_double n )
{
    return vnus_long_is_NaN( vnus_doublebits_to_long( n ) );
}

#define MAXNUMLEN 200

// Write a string representation of the given double into 'cbuf'.
// The string is terminated with a '\0' so that we can determine the length
// of the resulting string in Java. It works here since the real string
// cannot contain a '\0'.
static tmstring format_floatingpoint( double val, int precision ) 
{
    char buf[MAXNUMLEN];
    char *s = buf;
    int k;

    if( val<0.0 ){
	val = -val;
	*s++ = '-';
    }

    /* Print in normal or 'scientific' form according to value */
    if (val == 0.0 || (val >= 1.0e-3 && val < 1.0e7)) {
	static const double powTen[] = {
	  1.0e0, 1.0e1, 1.0e2, 1.0e3, 1.0e4, 1.0e5, 1.0e6
	};

	/* Account for precision digits ahead of the decimal point */
	for (k = 6; k >= 0 && val < powTen[k]; k--);
	precision -= k + 1;

	/* Print in decimal notation */
	sprintf(s, "%.*f", (int) precision, (double) val);

	/* Remove trailing zeroes after the decimal point */
	for (k = strlen(buf) - 1; buf[k] == '0' && buf[k - 1] != '.'; k--) {
	    buf[k] = '\0';
	}
    }
    else {
	char *t, *eptr, *eval;

	/* Print in exponential notation */
	sprintf(s, "%.*E", (int) precision - 1, (double) val);

	/* Find the exponent */
	eptr = strchr(buf, 'E');
	if( eptr == NULL ){
	    internal_error( "exponential notation number '%s' (0x%llx) does not contain exponent", buf, vnus_doublebits_to_long( val ) );
	}

	/* Remove a '+' sign, but leave a '-' sign */
	switch (eptr[1]) {
	    case '-':
		eval = &eptr[2];
		break;

	    case '+':			/* remove '+' */
		memmove(eptr + 1, eptr + 2, strlen(eptr + 1));
		/* fall through */

	    default:			/* shouldn't ever happen */
		eval = &eptr[1];
		break;
	}

	/* Remove leading zeroes in the exponent, if any */
	for( t = eval; t[0] == '0' && t[1] != '\0'; t++ );
	memmove( eval, t, strlen(t) + 1 );

	/* Remove trailing zeroes after the decimal point */
	for( t = eptr - 1; *t == '0' && t[-1] != '.'; t-- );
	memmove( t + 1, eptr, strlen(eptr) + 1 );
    }
    return new_tmstring( buf );
}

// This table that tells us how many decimal digits are needed to uniquely
// specify N binary bits, i.e.: bitsToDecimal[N-1] = Ceiling(N ln 2 / ln 10).
static const int bitsToDecimal[] = {
 1,  1,  1,  2,  2,  2,  3,  3,  3,  4,  4,  4,  4,  5,  5,  5,
 6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10, 10, 10,
10, 11, 11, 11, 12, 12, 12, 13, 13, 13, 13, 14, 14, 14, 15, 15,
15, 16, 16, 16
};

// TODO: be more conformant than this.
tmstring vnus_float_to_string( vnus_float f )
{
    int precision;

    // Determine number of digits of decimal precision to display
    vnus_int bits = vnus_floatbits_to_int( f );
    if( vnus_int_is_NaN( bits ) ){
	return new_tmstring( "NaN" );
    }
    if( bits == 0x7f800000 ){
	return new_tmstring( "Infinity" );
    }
    if( bits == (vnus_int) 0xff800000 ){
	return new_tmstring( "-Infinity" );
    }
    if( (bits & FLOAT_EXPONENT_MASK) == 0 ){
	    // denormalized value
	int bitIndex;
	for( bitIndex = FLOAT_NUM_MANTISSA_BITS - 1;
	    bitIndex > 0 && ((1L << bitIndex) & bits) == 0;
	    bitIndex--);
	precision = bitsToDecimal[bitIndex];
    }
    else {
	// normalized value
	precision = bitsToDecimal[FLOAT_NUM_MANTISSA_BITS - 1];
    }

    // Add an extra digit to handle rounding
    precision++;

    return format_floatingpoint( (double) f, precision );
}

vnus_float string_to_vnus_float( const char *s )
{
    return strtod( s, NULL );
}

// TODO: be more conformant than this.
tmstring vnus_double_to_string( vnus_double f )
{
    int precision;

    // Handle exceptional values
    vnus_long bits = vnus_doublebits_to_long( f );
    if( vnus_long_is_NaN( bits ) ){
	return new_tmstring( "NaN" );
    }
    if( bits == (vnus_long) 0x7ff0000000000000LL ){
	return new_tmstring( "Infinity" );
    }
    if( bits == (vnus_long) 0xfff0000000000000LL ){
	return new_tmstring( "-Infinity" );
    }

    // Determine number of digits of decimal precision to display
    if ((bits & DOUBLE_EXPONENT_MASK) == 0) {
	// denormalized value
	int bitIndex;
	for (bitIndex = DOUBLE_NUM_MANTISSA_BITS - 1;
		bitIndex > 0 && ((1 << bitIndex) & bits) == 0;
		bitIndex--);
	precision = bitsToDecimal[bitIndex];
    }
    else {
	// normalized value
	precision = bitsToDecimal[DOUBLE_NUM_MANTISSA_BITS - 1];
    }

    // Add an extra digit to handle rounding
    precision++;

    return format_floatingpoint( f, precision );
}

vnus_double string_to_vnus_double( const char *s )
{
    return strtod( s, NULL );
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

vnus_int vnus_floatbits_to_int( vnus_float n )
{
    intNfloat u;
    u.f = n;

    return u.i;
}

vnus_long vnus_doublebits_to_long( vnus_double n )
{
    longNdouble u;
    u.d = n;
    return u.l;
}

tmstring vnus_char_to_string( vnus_char c )
{
    if( c == '\n' ){
	return new_tmstring( "\\n" );
    }
    if( c == '\r' ){
	return new_tmstring( "\\r" );
    }
    if( c == '\t' ){
	return new_tmstring( "\\t" );
    }
    if( c>=0x20 && c<=0xff ){
	return printf_tmstring( "%c", (char) c );
    }
    return printf_tmstring( "\\u%04x", (unsigned int) c );
}
