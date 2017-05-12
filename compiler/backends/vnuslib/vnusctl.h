/* File: vnusctl.h
 *
 * Functions on Vnus parse trees that can be shared
 * by the vnus engines.
 */


#ifndef INC_VNUSCTL_H
#define INC_VNUSCTL_H

// parse tree manipulations
extern expression location_to_expression( const_location l );
extern location expression_to_location( const_expression x );

// Membership test functions
extern bool member_origsymbol_list( const_origsymbol_list l, const_origsymbol s );
extern bool member_tmsymbol_list( const_tmsymbol_list l, const tmsymbol s );
extern tmsymbol_list add_tmsymbol_list( tmsymbol_list l, tmsymbol s );

// Construction functions
#define gen_origin() real_gen_origin(__FILE__,__LINE__)
#define gen_origsymbol(s) real_gen_origsymbol(__FILE__,__LINE__,(s))
#define add_origsymbol(s) real_add_origsymbol(__FILE__,__LINE__,(s))
extern origin real_gen_origin(const char *f, const int l);
extern origsymbol real_gen_origsymbol(const char *f, const int l, const char *name);
extern origsymbol real_add_origsymbol(const char *f, const int l, const char *name);

// Lookup functions
extern declaration search_entry( const_declaration_list decls, const tmsymbol name );
extern declaration search_declaration( const_declaration_list decls, const tmsymbol name );
extern declaration lookup_declaration( const_declaration_list decls, const_origsymbol name );
extern declaration lookup_declaration( const_declaration_list decls, tmsymbol name );
extern type lookup_vartype( const_declaration_list decls, const_origsymbol name );
extern const_type lookup_argtype( const_declaration_list decls, const_origsymbol name );
extern const_field_list lookup_NamedRecord_fields( const_declaration_list decls, const_origsymbol name );
extern int find_field_fields( const_field_list fields, const_origsymbol field );
extern type lookup_fntype( const_declaration_list decls, const_origsymbol name );

// Error handling
#define ERRARGLEN 80
extern char errpos[];
extern char errarg[];

extern void error( const char *msg );
extern void sys_error( int no );
extern void internal_error( const char *msg );
extern void origin_error( const_origin org, const char *msg );
extern void origsymbol_error( const_origsymbol s, const char *msg );
extern void errcheck( void );
extern void origin_internal_error( const_origin org, const char *msg );
extern void origsymbol_internal_error( const_origsymbol s, const char *msg );

// Type derivation function
extern type derive_type_optexpression(
 const_declaration_list decls,
 const_origin org,
 const_optexpression x
);
extern type derive_type_expression( const_declaration_list decls, const_origin org, const_expression x );
extern type derive_type_location( const_declaration_list decls, const_origin org, const_location loc );
extern type derive_type_formalParameter( const_declaration_list decls, const_origsymbol arg );
extern type_list derive_type_expression_list( const_declaration_list decls, const_origin org, const_expression_list xl );

// Analysis functions.
extern tmbool is_equivalent_expression( const_expression x, const_expression y );
extern tmbool is_equivalent_DistrExpr( const_DistrExpr x, const_DistrExpr y );
extern bool is_associative_operator( BINOP op );
extern bool is_constant_expression( const_expression x );
extern bool maybe_has_sideeffect( const_size_list x, int flags );
extern bool maybe_has_sideeffect( const_expression x, int flags );
extern bool maybe_has_sideeffect( const_optexpression x, int flags );
extern bool maybe_has_sideeffect( const_location x, int flags );
#define NO_SE_NEW		0x0001	// Doing new is not a side-effect
#define NO_SE_EXCEPTION		0x0002	// Exception is not a side-effect

// Evaluation and constant folding functions
extern vnus_char evaluate_char_constant( const char *s );
extern expression constant_fold_expression( expression x );
extern size_list constant_fold_size_list( size_list x );
extern expression_list constant_fold_expression_list( expression_list x );
extern vnusprog constant_fold_vnusprog( vnusprog x );

// Flag testing
#define has_any_flag(val,bits) (((val)&(bits))!=0)
#define has_all_flags(val,bits) (((val)&(bits))==(bits))

#endif

