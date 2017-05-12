/* File: service.h */

#ifndef INC_SERVICE_H
#define INC_SERVICE_H

extern bool search_tmsymbol_list( const_tmsymbol_list l, tmsymbol s, unsigned int *ixp );
extern bool member_origsymbol_list( const_origsymbol_list l, const_origsymbol s );
extern bool member_tmsymbol_list( const_tmsymbol_list l, tmsymbol s );
extern tmsymbol_list add_tmsymbol_list( tmsymbol_list l, tmsymbol s );
extern tmsymbol_list add_tmsymbol_list( tmsymbol_list l, const_tmsymbol_list lb );

extern origsymbol_list add_origsymbol_list( origsymbol_list l, const_origsymbol s );

extern origin gen_origin();
extern origsymbol gen_origsymbol( const char *prefix );
extern origsymbol add_origsymbol( const char *str );
extern origsymbol add_origsymbol( tmsymbol s);

extern tmsymbol gen_role_tmsymbol( tmsymbol typ, const char *role );
extern tmsymbol gen_role_tmsymbol( origsymbol typ, const char *role );

extern AssignStatement transmog_AssignmentExpression( AssignOpExpression x, const_origin org );

extern BINOP assignop_to_binop( const ASSIGN_OP op );

extern tmstring qualify_tmstring( const char *prestr, const char *suffstr );
extern tmsymbol qualify_tmsymbol( tmsymbol pre, tmsymbol suff );
extern tmsymbol qualify_tmsymbol( const char *prestr, tmsymbol suff );
extern bool is_qualified_prefix( tmsymbol pre, tmsymbol s );

extern tmstring printf_tmstring( const char *fmt, ... );

/* structure to describe debugging flags */
typedef struct str_db_flags {
    char flagchar;		/* char to switch the flag on */
    int *flagadr;		/* pointer to the flag */
    const char *flagdescr;	/* description string. */
} dbflag;

#define dbflagNIL (dbflag *)0

extern void setdbflags( const char *s, dbflag *flagtab, int val );
extern void helpdbflags( FILE *f, const dbflag *flagtab );

extern origsymbol create_variable( const char *str, origin org );
extern bool is_qualified_origsymbol( const_origsymbol nm );
extern bool is_qualified_tmsymbol( tmsymbol nm );
extern void break_qualified_name( const_origsymbol nm, origsymbol *first, origsymbol *last );
extern void break_qualified_tmsymbol( tmsymbol nm, tmsymbol *first, tmsymbol *last );
extern origsymbol firstpart_origsymbol( const_origsymbol s );
extern tmstring fullpath_file( const char *path, const char *file );

extern expression force_basetype( const_Entry_list symtab, expression x, BASETYPE t );
extern ActualDim_list force_int_vectors( const_Entry_list symtab, const ActualDim_list l );

extern bool is_void_type( const_type t );
extern bool is_tuple_type( const_type t );
extern expression force_type( const_Entry_list symtab, expression x, const_type t );
extern optexpression force_type( const_Entry_list symtab, optexpression x, const_type t );
extern expression_list force_basetype( const_Entry_list symtab, expression_list l, BASETYPE t );
extern expression_list force_type(
 const_Entry_list symtab,
 const_origsymbol fn,
 expression_list l,
 const_type_list fl
);

extern tmsymbol shortname( tmsymbol s );

extern bool file_exists( const char *fnm );

extern bool check_double_symbols( const char *desc, const_origsymbol_list l );
extern bool search_type_SparProgramUnit_list(
 const_SparProgramUnit_list l,
 tmsymbol t,
 unsigned int *pos
);

extern bool is_String_type( const_type t );
extern bool is_Object_type( const_type t );
extern bool is_vnusstring_type( const_type t );
extern bool is_boolean_type( const_type t );
extern type make_String_type();

extern bool is_labeled_statement( const_statement smt );

extern tmstring jni_mangle_string( const char *s );
#define jni_mangle_symbol(s) jni_mangle_string((s)->name)

extern OptExpr build_DefaultInit( type t );

extern bool extract_vector_power( const_origin org, const_expression x, unsigned int *v );
extern bool extract_tuple_length( const_origin org, const_type t, unsigned int *v );
extern bool extract_array_rank( const_origin org, const_expression x, unsigned int *v );

extern tmsymbol gen_fqname( tmsymbol fqname );

extern type clean_type( type t );
extern bool check_duplicate_FormalParameters( const_FormalParameter_list l );

extern TypeBinding_list register_TypeBinding(
 TypeBinding_list tl,
 tmsymbol file,
 TypeBinding b
);
extern tmsymbol translate_name( const_NameTranslation_list sl, tmsymbol s );

extern tmuint_list calculate_array_shape(
    origin org,
    expression_list elms,
    tmuint_list shape,
    unsigned int dim,
    unsigned int rank
);
extern ActualDim build_ActualDim( const_tmuint_list l );
extern bool is_true_constant( const_expression x );
extern bool is_false_constant( const_expression x );

extern void yup();

extern expression extract_trivial_inline_expression( Block blk, tmsymbol retvar );
extern bool check_IfExpression_types( const_Entry_list symtab, const_origin org, const_type t_then, const_type t_else );

extern bool are_equivalent_expressions( const_expression a, const_expression b );
extern bool occurs_expression_list( const_expression_list xl, const_expression x );
#endif
