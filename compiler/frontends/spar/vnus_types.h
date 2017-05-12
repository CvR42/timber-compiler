/* file: vnus_types.h
 *
 * Representation of the Vnus types in the Spar frontend.
 *
 * For the moment this is unconditionally coded for GNU C++ on
 * Intel processor running Linux.
 */

#ifndef INC_VNUS_TYPES_H
#define INC_VNUS_TYPES_H

typedef struct str_complex { double re; double im; } vnus_complex;


typedef signed char vnus_byte;
#define rfre_vnus_byte(e)
#define rdup_vnus_byte(e) (e)
#define print_vnus_byte(st,e) print_tmshort(st,e)
#define isequal_vnus_byte(a,b) ((a)==(b))
#define VNUS_BYTE_MAX 127
#define VNUS_BYTE_MIN (-128)

typedef tmshort vnus_short;
#define rfre_vnus_short(e)
#define rdup_vnus_short(e) (e)
#define print_vnus_short(st,e) print_tmshort(st,e)
#define isequal_vnus_short(a,b) ((a)==(b))
#define VNUS_SHORT_MAX 32767
#define VNUS_SHORT_MIN (-32768)

typedef tmushort vnus_char;
#define rfre_vnus_char(e) 
#define rdup_vnus_char(e) (e)
#define isequal_vnus_char(a,b) ((a)==(b))
#define VNUS_CHAR_MIN 0
#define VNUS_CHAR_MAX (65535)
extern void print_vnus_char( TMPRINTSTATE *st, const vnus_char c );
extern tmstring vnus_char_to_string( vnus_char c );

#define rfre_vnus_long(e)
#define rdup_vnus_long(e) (e)
#define isequal_vnus_long(a,b) ((a)==(b))

#ifdef __GNUC__
__extension__ typedef long long int vnus_long;
#else
typedef long long int vnus_long;
#endif
extern void print_vnus_long( TMPRINTSTATE *st, const vnus_long val );
extern void fprint_vnus_long( FILE *f, const vnus_long val );
#define VNUS_LONG_UNSIGNED_MAX 0xFFFFFFFFFFFFFFFFULL
#define VNUS_LONG_MAX 9223372036854775807LL
#define VNUS_LONG_MIN (-9223372036854775807LL-1)

typedef bool vnus_boolean;
#define rfre_vnus_boolean(e)
#define rdup_vnus_boolean(e) (e)

typedef int vnus_int;
#define rfre_vnus_int(e)
#define rdup_vnus_int(e) (e)
#define print_vnus_int(st,e) print_int(st,e)
#define isequal_vnus_int(a,b) ((a)==(b))
#define VNUS_INT_UNSIGNED_MAX 0xFFFFFFFFUL
#define VNUS_INT_MAX 2147483647L
#define VNUS_INT_MIN (-2147483647L-1)

typedef float vnus_float;
#define rfre_vnus_float(e)
#define rdup_vnus_float(e) (e)
#define print_vnus_float(st,e) print_double(st,e)
#define isequal_vnus_float(a,b) ((a)==(b))
extern tmstring vnus_float_to_string( vnus_float f );
extern vnus_float string_to_vnus_float( const char *s );
extern vnus_float intbits_to_vnus_float( vnus_int n );
extern vnus_int vnus_floatbits_to_int( vnus_float n );

typedef double vnus_double;
#define rfre_vnus_double(e)
#define rdup_vnus_double(e) (e)
#define print_vnus_double(st,e) print_double(st,e)
#define isequal_vnus_double(a,b) ((a)==(b))
extern tmstring vnus_double_to_string( vnus_double f );
extern vnus_double string_to_vnus_double( const char *s );
extern vnus_double longbits_to_vnus_double( vnus_long n );
extern vnus_long vnus_doublebits_to_long( vnus_double n );
extern bool vnus_double_is_NaN( vnus_double n );

#define rfre_vnus_complex(e)
#define rdup_vnus_complex(e) (e)

typedef tmstring vnus_string;
#define rfre_vnus_string(e) rfre_tmstring(e)
#define rdup_vnus_string(e) rdup_tmstring(e)
#define print_vnus_string(st,e) print_tmstring(st,e)

extern vnus_long string_to_vnus_long( const char *p, const char **error );
extern vnus_int string_to_vnus_int( const char *p, const char **error );
#endif
