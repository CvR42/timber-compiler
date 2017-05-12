/* File: type.h */

extern bool is_complex_type( const_type t );
extern bool is_numeric_type( const_type t );
extern bool is_valid_cast( const_declaration_list decls, const_type to, const_type from );
extern bool is_equivalent_type( const_declaration_list decls, const_type a, const_type b );

// Function type registration.
extern void register_TypeFunction( declaration_list decls, type_list formals, type retval );
extern void register_TypeProcedure( declaration_list decls, type_list formals );
extern void generate_function_types( FILE *f, const_declaration_list decls );
extern void clear_function_types();

/* Type names. */
extern tmstring type_string( const_declaration_list decls, const_type t );
extern tmstring declaration_string( const_declaration_list decls, const_type t, int flags, const char *nm, bool formal );

#define is_record_type(t) (((t)->tag == TAGTypeRecord) || ((t)->tag == TAGTypeNamedRecord))
#define is_routine_type(t) (((t)->tag == TAGTypeFunction) || ((t)->tag == TAGTypeProcedure))
