/* File: typederive.h */

extern bool derive_natural_constant( const expression x, long int *val );
extern type derive_type_name( const origsymbol nm );
extern type derive_type_formalParameter( const formalParameter arg );
extern type_list derive_type_formalParameter_list( const formalParameter_list pl );
extern type derive_type_expression( const expression x );
extern type derive_type_optexpression( const optexpression x );

/* Type classification functions. */
extern type max_types( const type ta, const type tb );
extern bool is_scalar_type( const type t );
extern bool is_equivalent_type( const type ta, const type tb );
extern bool is_equal_type( const type ta, const type tb );
