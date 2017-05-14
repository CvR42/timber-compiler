/* File: vnusbasetypes.h
 *
 * Definitions for the basetypes of vnus.ds
 */

#ifndef INC_VNUSBASETYPES_H
#define INC_VNUSBASETYPES_H

typedef enum en_BASETYPE {
    BT_BYTE,
    BT_SHORT,
    BT_INT,
    BT_LONG,
    BT_FLOAT,
    BT_DOUBLE,
    BT_COMPLEX,
    BT_CHAR,
    BT_STRING,
    BT_BOOLEAN
} BASETYPE;

#define rfre_BASETYPE(e)
#define rdup_BASETYPE(e) (e)
#define cmp_BASETYPE(a,b) (((int)a)-((int)b))
#define isequal_BASETYPE(a,b) ((a)==(b))
#define BASETYPENIL ((BASETYPE) 0)
extern void print_BASETYPE( TMPRINTSTATE *st, BASETYPE t );
extern tmbool fscan_BASETYPE( FILE *f, BASETYPE *b );

typedef enum en_binop {
    BINOP_AND,
    BINOP_DIVIDE,
    BINOP_EQUAL,
    BINOP_GREATER,
    BINOP_GREATEREQUAL,
    BINOP_LESS,
    BINOP_LESSEQUAL,
    BINOP_MINUS,
    BINOP_MOD,
    BINOP_NOTEQUAL,
    BINOP_OR,
    BINOP_PLUS,
    BINOP_SHIFTLEFT,
    BINOP_SHIFTRIGHT,
    BINOP_SHORTAND,
    BINOP_SHORTOR,
    BINOP_TIMES,
    BINOP_USHIFTRIGHT,
    BINOP_XOR
} BINOP;

extern void print_BINOP( TMPRINTSTATE *st, BINOP t );
extern tmbool fscan_BINOP( FILE *f, BINOP *b );
#define isequal_BINOP(a,b) ((a)==(b))
#define BINOPNIL ((BINOP) 0)

#define is_equalitycompare_operator(op) (((op)==BINOP_EQUAL)||((op)==BINOP_NOTEQUAL))

#define rfre_BINOP(e)
#define rdup_BINOP(e) (e)
#define cmp_BINOP(a,b) (((int)a)-((int)b))

typedef enum en_unop {
    UNOP_NOT,
    UNOP_PLUS,
    UNOP_NEGATE
} UNOP;

#define rfre_UNOP(e)
#define rdup_UNOP(e) (e)
#define cmp_UNOP(a,b) (((int)a)-((int)b))
#define isequal_UNOP(a,b) ((a)==(b))
#define UNOPNIL ((UNOP) 0)
extern void print_UNOP( TMPRINTSTATE *st, UNOP t );
extern tmbool fscan_UNOP( FILE *f, UNOP *b );

/* Flags for declaration modifiers. */
#define MOD_FINAL	0x0001
#define MOD_READONLY	0x0002
#define MOD_LOCAL	0x0004
#define MOD_UNCHECKED	0x0008
#define MOD_VOLATILE	0x0010

// Internally used flags
#define MOD_ELIMINATED	0x4000	// This declaration is eliminated by an engine

typedef signed char vnus_byte;
#define rfre_vnus_byte(e)
#define rdup_vnus_byte(e) (e)
#define print_vnus_byte(st,e) print_tmshort(st,e)
#define isequal_vnus_byte(a,b) ((a)==(b))
#define VNUS_BYTE_MAX 127
#define VNUS_BYTE_MIN -128
#define vnus_byteNIL ((vnus_byte) 0)
extern tmbool fscan_vnus_byte( FILE *f, vnus_byte *b );

typedef tmshort vnus_short;
#define rfre_vnus_short(e)
#define rdup_vnus_short(e) (e)
#define print_vnus_short(st,e) print_int(st,e)
#define isequal_vnus_short(a,b) ((a)==(b))
#define VNUS_SHORT_MAX 32767
#define VNUS_SHORT_MIN -32768
#define vnus_shortNIL ((vnus_short) 0)
extern tmbool fscan_vnus_short( FILE *f, vnus_short *b );

typedef int vnus_char;
#define rfre_vnus_char(e) 
#define rdup_vnus_char(e) (e)
#define print_vnus_char(st,e) print_int(st,e)
#define fscan_vnus_char(f,e) fscan_int(f,e)
#define isequal_vnus_char(a,b) ((a)==(b))
#define vnus_charNIL ((vnus_char) 0)

#define rfre_vnus_long(e)
#define rdup_vnus_long(e) (e)
#define isequal_vnus_long(a,b) ((a)==(b))

#ifdef __GNUC__
__extension__ typedef long long int vnus_long;
#else
typedef long long int vnus_long;
#endif
extern void print_vnus_long( TMPRINTSTATE *st, const vnus_long e );
extern void fprint_vnus_long( FILE *f, const vnus_long e );
#define VNUS_LONG_MAX 9223372036854775807LL
#define VNUS_LONG_MIN (-9223372036854775807LL-1)
#define vnus_longNIL ((vnus_long) 0)

typedef bool vnus_boolean;
#define rfre_vnus_boolean(e)
#define rdup_vnus_boolean(b) (b)
#define cmp_vnus_boolean(a,b) (((int)a)-((int)b))
#define isequal_vnus_boolean(a,b) ((a)==(b))
#define vnus_booleanNIL false
extern void print_vnus_boolean( TMPRINTSTATE *st, vnus_boolean b );
extern tmbool fscan_vnus_boolean( FILE *f, vnus_boolean *b ); 

typedef int vnus_int;
#define rfre_vnus_int(e)
#define rdup_vnus_int(e) (e)
#define print_vnus_int(st,e) print_long(st,e)
#define isequal_vnus_int(a,b) ((a)==(b))
#define VNUS_INT_MAX 2147483647L
#define VNUS_INT_MIN (-2147483647L-1)
#define vnus_intNIL (0)
extern tmbool fscan_vnus_int( FILE *f, vnus_int *b ); 

typedef tmstring vnus_string;
#define rfre_vnus_string(e) rfre_tmstring(e)
#define rdup_vnus_string(e) rdup_tmstring(e)
#define print_vnus_string(st,e) print_tmstring(st,e)

extern vnus_long string_to_vnus_long( const char *p );
extern vnus_int string_to_vnus_int( const char *p );
extern tmbool fscan_vnus_long( FILE *f, vnus_long *b ); 

typedef float vnus_float;
extern tmbool fscan_vnus_float( FILE *f, vnus_float *v );
#define rfre_vnus_float(e)
#define rdup_vnus_float(e) (e)
extern void print_vnus_float( TMPRINTSTATE *st, vnus_float b );
#define vnus_floatNIL (0.0)
#define isequal_vnus_float(a,b) ((a)==(b))

typedef double vnus_double;
extern tmbool fscan_vnus_double( FILE *f, vnus_double *v );
#define rfre_vnus_double(e)
#define rdup_vnus_double(e) (e)
extern void print_vnus_double( TMPRINTSTATE *st, vnus_double b );
#define vnus_doubleNIL (0.0)
#define isequal_vnus_double(a,b) ((a)==(b))

typedef struct str_complex { double re; double im; } vnus_complex;
#define rfre_vnus_complex(e)
#define rdup_vnus_complex(e) (e)

extern vnus_int vnus_float_bits_to_int( vnus_float n );
extern vnus_long vnus_double_bits_to_long( vnus_double n );
extern vnus_float intbits_to_vnus_float( vnus_int n );
extern vnus_double longbits_to_vnus_double( vnus_long n );

#endif
