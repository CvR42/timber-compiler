/* File: defs.h
 *
 * Global definitions.
 */

#define INDENTSTEP 4

#define FALSE (0)
#define TRUE (1)

#define PATHSEP '/'

#define rfre_bool(e)
#define rdup_bool(e) (e)

typedef enum en_context {
    CTX_GLOBAL, CTX_OBJECT,
    CTX_STATICROUTINE, CTX_DYNAMICROUTINE,
    CTX_STATICINIT, CTX_DYNAMICINIT,
    CTX_CONSTRUCTOR
} context;

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

typedef enum en_shortop {
    SHORTOP_AND,
    SHORTOP_OR
} SHORTOP;

#define rfre_SHORTOP(e)
#define rdup_SHORTOP(e) (e)

typedef enum en_assignop {
    ASSIGN,
    ASSIGN_PLUS,
    ASSIGN_MINUS,
    ASSIGN_TIMES,
    ASSIGN_DIVIDE,
    ASSIGN_MOD,
    ASSIGN_XOR,
    ASSIGN_AND,
    ASSIGN_OR,
    ASSIGN_SHIFTLEFT,
    ASSIGN_SHIFTRIGHT,
    ASSIGN_USHIFTRIGHT
} ASSIGN_OP;

#define rfre_ASSIGN_OP(e)
#define rdup_ASSIGN_OP(e) (e)

typedef enum en_unop {
    UNOP_NOT,
    UNOP_INVERT,
    UNOP_PLUS,
    UNOP_NEGATE
} UNOP;

#define rfre_UNOP(e)
#define rdup_UNOP(e) (e)

typedef enum en_aflag {
    AF_NO = 0,
    AF_MAYBE = 1,
    AF_YES = 2,
    AF_UNKNOWN = 3
} AFLAG;

#define rfre_AFLAG(e)
#define rdup_AFLAG(e) (e)

/* Representation of modifier flags. They can be or-ed together. */

typedef unsigned long int modflags;

/* When defined there, these bits follow the coding of JVM,
 * see JVMS2 4.1, 4.5, 4.6 and 4.7
 */
#define ACC_PUBLIC		0x00000001u
#define ACC_PRIVATE		0x00000002u
#define ACC_PROTECTED		0x00000004u
#define ACC_STATIC		0x00000008u
#define ACC_FINAL		0x00000010u
#define ACC_SYNCHRONIZED	0x00000020u
#define ACC_VOLATILE		0x00000040u
#define ACC_TRANSIENT		0x00000080u
#define ACC_NATIVE		0x00000100u
#define ACC_INTERFACE		0x00000200u
#define ACC_ABSTRACT		0x00000400u
#define ACC_STRICTFP		0x00000800u

// These bits are specific to Spar, and/or are used internally.
// ACC_COMPILECONSTANT is a 'compile-time constant' as defined in JLS
// JLS 12.4.1
// ACC_DEDUCED_FINAL is an indication that analysis has shown that a
// variable is final, but this was not indicated by the user. Constant
// propagation is not done if `pref_strictanalysis' is set, since it
// violates Java semantics...
#define ACC_FORKABLE		0x00800000u	// Is a forkable class
#define ACC_INNER_CLASS		0x01000000u	// Is an inner class
#define ACC_LOCAL_OBJECT	0x02000000u	// Is a local class/interf.
#define ACC_MEMBER_OBJECT	0x04000000u	// Is a member class/interf.
#define ACC_DEDUCED_FINAL	0x08000000u
#define ACC_BEING_INLINED	0x10000000u	// Method is now inlined.
#define ACC_COMPILECONSTANT	0x20000000u
#define ACC_INLINE		0x40000000u	// Method/constr can be inlined
#define ACC_INIT		0x80000000u	// This is an init fn/field

#define has_flags(all,req) (((all)&(req))==(req))
#define has_any_flag(all,req) (((all)&(req))!=0u)

/* The allowed modifier sets for various language elements. */
#define MODS_TOP_CLASS		(ACC_PUBLIC|ACC_ABSTRACT|ACC_FINAL|ACC_STRICTFP)
#define MODS_NESTED_CLASS	(ACC_PROTECTED|ACC_PRIVATE|ACC_ABSTRACT|ACC_STATIC|ACC_FINAL|ACC_STRICTFP)
#define MODS_MEMBER_CLASS	(ACC_PUBLIC|ACC_PROTECTED|ACC_PRIVATE|ACC_ABSTRACT|ACC_STATIC|ACC_FINAL|ACC_STRICTFP)
#define MODS_LOCAL_CLASS	(ACC_PUBLIC|ACC_ABSTRACT|ACC_FINAL|ACC_STRICTFP)
#define MODS_CLASS		(MODS_TOP_CLASS|MODS_MEMBER_CLASS|MODS_LOCAL_CLASS)
#define MODS_CONSTRUCTOR	(ACC_PUBLIC|ACC_PROTECTED|ACC_PRIVATE|ACC_INLINE)
#define MODS_FIELD		(ACC_PUBLIC|ACC_PROTECTED|ACC_PRIVATE|ACC_FINAL|ACC_STATIC|ACC_TRANSIENT|ACC_VOLATILE)
#define MODS_TOP_INTERFACE	(ACC_PUBLIC|ACC_ABSTRACT|ACC_STRICTFP|ACC_INTERFACE)
#define MODS_MEMBER_INTERFACE	(ACC_PUBLIC|ACC_PROTECTED|ACC_PRIVATE|ACC_ABSTRACT|ACC_STATIC|ACC_STRICTFP|ACC_INTERFACE)
#define MODS_LOCAL_INTERFACE	(ACC_PUBLIC|ACC_ABSTRACT|ACC_STRICTFP|ACC_INTERFACE)
#define MODS_CONSTANT		(ACC_PUBLIC|ACC_STATIC|ACC_FINAL)
#define MODS_INTERFACE		(MODS_TOP_INTERFACE|MODS_LOCAL_INTERFACE|MODS_MEMBER_INTERFACE)
#define MODS_METHOD		(ACC_PUBLIC|ACC_PROTECTED|ACC_PRIVATE|ACC_ABSTRACT|ACC_STATIC|ACC_FINAL|ACC_SYNCHRONIZED|ACC_NATIVE|ACC_STRICTFP)
#define MODS_INLINE		(ACC_PUBLIC|ACC_PROTECTED|ACC_PRIVATE|ACC_STATIC|ACC_FINAL|ACC_SYNCHRONIZED|ACC_INLINE|ACC_STRICTFP)
#define MODS_INTERFACE_METHOD	(ACC_PUBLIC|ACC_ABSTRACT)
#define MODS_INTERFACE_FIELD	(ACC_PUBLIC|ACC_STATIC|ACC_FINAL)

// Representation of flags that can be attached to a variable name.

typedef unsigned int varflags;

#define VAR_FINAL	0x0001u	// Initialized var, will not change
#define VAR_THIS	0x0002u	// a 'this' var.
#define VAR_CLASS	0x0004u	// a 'class' var.
#define VAR_GENERATED	0x0008u	// var was generated by the compiler ('tempo').
#define VAR_LOCAL	0x0010u	// var is a local variable or cardinality
#define VAR_FORMAL	0x0020u	// var is a formal variable
#define VAR_DYNFIELD	0x0040u	// var is a dynamic field
#define VAR_STATFIELD	0x0080u	// var is a static field
#define VAR_GLOBAL	0x0100u	// var is a 'global' (only generated vars)

// Flags to represent special properties of the invoked function/procedure
typedef unsigned int invocationflags;

#define CALL_CONSTRUCTOR	0x0001u	// Invocation calls a constructor
#define CALL_STATICINIT		0x0002u	// Invocation calls static initializer
#define CALL_DYNAMICINIT	0x0004u	// Invocation calls dynam. initializer

#define rfre_invocationflags(e)
#define rdup_invocationflags(e) (e)
#define print_invocationflags print_tmuint
#define isequal_invocationflags(a,b) (true)

/* Language extension ('feature') flags. */
#define FEAT_JAVA	0x0001u	// Base language (should always be set)
#define FEAT_COMPLEX	0x0002u
#define FEAT_PRAGMA	0x0004u
#define FEAT_DELETE	0x0008u
#define FEAT_ARRAY	0x0010u
#define FEAT_PARALLEL	0x0020u
#define FEAT_PRINT	0x0040u
#define FEAT_INLINE	0x0080u
#define FEAT_CARDS	0x0100u
#define FEAT_STRING	0x0200u
#define FEAT_PRIMARRAY	0x0400u
#define FEAT_CSP	0x0800u

// Not a real feature flag, but if set, this is also a keyword in the
// context of a pragma.
#define FEAT_INPRAGMA	0x8000u

#define rfre_modflags(e)
#define rdup_modflags(e) (e)
#define print_modflags print_tmuint
#define isequal_modflags(a,b) (true)

#define rfre_varflags(e)
#define rdup_varflags(e) (e)
#define print_varflags print_tmuint
#define isequal_varflags(a,b) (true)

#define EXCEPTION_HANDLER_NAME "spar.compiler.DefaultExceptionHandler.handler"
