/* File: defs.h
 *
 * Global definitions.
 */

#define	DONT_CARE	0377

#define INDENTSTEP 4

#define FALSE 0
#define TRUE 1

#ifndef _cplusplus
#endif

typedef enum en_optbool { UNKNOWN=-1, NO = 0, YES = 1 } optbool;

#define rfre_optbool(e)
#define rdup_optbool(e) (e)

#define cmp_optbool(a,b) (((int) a)-((int) b))

/* The types are ordened so that 'smaller' types come first.
 * Types that do not fit in this ordering are placed first
 */
typedef enum en_BASETYPE {
    BT_BOOLEAN,
    BT_STRING,
    BT_COMPLEX,
    BT_BYTE,
    BT_SHORT,
    BT_CHAR,
    BT_INT,
    BT_LONG,
    BT_FLOAT,
    BT_DOUBLE
} BASETYPE;

#define rfre_BASETYPE(e)
#define rdup_BASETYPE(e) (e)

typedef enum en_binop {
    BINOP_AND,
    BINOP_OR,
    BINOP_MOD,
    BINOP_PLUS,
    BINOP_MINUS,
    BINOP_TIMES,
    BINOP_DIVIDE,
    BINOP_EQUAL,
    BINOP_NOTEQUAL,
    BINOP_LESS,
    BINOP_LESSEQUAL,
    BINOP_GREATER,
    BINOP_GREATEREQUAL,
    BINOP_XOR,
    BINOP_SHIFTLEFT,
    BINOP_SHIFTRIGHT,
    BINOP_USHIFTRIGHT
} BINOP;

#define rfre_BINOP(e)
#define rdup_BINOP(e) (e)

typedef enum en_unop {
    UNOP_NOT,
    UNOP_PLUS,
    UNOP_NEGATE
} UNOP;

#define rfre_UNOP(e)
#define rdup_UNOP(e) (e)
