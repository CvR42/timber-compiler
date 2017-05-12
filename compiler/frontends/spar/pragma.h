// File: pragma.h

extern bool search_pragma_list( const_Pragma_list l, const char *nm, unsigned int *ixp );
extern bool get_boolean_pragma( const_Pragma_list gl, const_Pragma_list l, const char *nm, bool *val );
extern bool get_flag_pragma( const_Pragma_list gl, const_Pragma_list l, const char *nm );
extern const char *get_string_pragma( const_Pragma_list gl, const_Pragma_list l, const char *nm );

// Construction helper functions.
extern Pragma new_role_pragma( const char *role );
extern Pragma_list build_Pragma_list( Pragma p );
