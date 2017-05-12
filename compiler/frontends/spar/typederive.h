/* File: typederive.h */

extern type derive_type_name( const_Entry_list symtab, const_origsymbol nm );
extern type_list derive_type_FormalParameter_list( const_FormalParameter_list pl );
extern type derive_type_expression( const_Entry_list symtab, const_expression x );
extern type_list derive_type_expression_list( const_Entry_list symtab, const_expression_list x );

/* Type classification functions. */
extern type max_types( const_type ta, const_type tb );
extern BASETYPE get_BASETYPE( const_type t );
extern bool is_identity_conv_type( const_Entry_list symtab, const_type ta, const_type tb );

extern bool is_invocationequivalent_type_list( const_Entry_list symtab, const_TypeEntry_list typelist, const_type_list from, const_type_list to );
extern bool is_invocationequivalent_type( const_Entry_list symtab, const_TypeEntry_list typelist, const_type from, const_type to );
extern bool is_assignequivalent_type( const_Entry_list symtab, const_TypeEntry_list typelist, const_type from, const_type to );
extern bool is_equivalent_type( const_type a, const_type b );
extern bool is_numeric_type( const_Entry_list symtab, const_type t );
extern bool is_integral_type( const_Entry_list symtab, const_type t );
extern bool is_complex_type( const_Entry_list symtab, const_type t );
extern bool is_boolean_type( const_Entry_list symtab, const_type t );
extern bool is_valid_cast( const_Entry_list symtab, const_TypeEntry_list typelist, const_type from, const_type to );

extern bool is_interface_type( const_TypeEntry_list typelist, const_origsymbol name );

extern bool is_widening_prim_basetype( const BASETYPE from, const BASETYPE to );

extern type calculate_ifexpression_type( const_Entry_list symtab, const_origin org, const_expression thenval, const_expression elseval );
