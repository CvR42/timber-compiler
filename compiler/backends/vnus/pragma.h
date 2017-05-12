/* File: pragma.h */

extern bool get_boolean_pragma( const_Pragma_list gl, const_Pragma_list l, const char *nm, bool *val );
extern bool get_flag_pragma( const_Pragma_list gl, const_Pragma_list l, const char *nm );
extern const char *get_string_pragma( const_Pragma_list gl, const_Pragma_list l, const char *nm );
extern void derive_processor_array( const vnusprog prog );
extern void derive_processors_pragma( const vnusprog prog );
