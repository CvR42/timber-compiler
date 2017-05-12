/* File: service.h */

#ifndef INC_SERVICE_H
#define INC_SERVICE_H

extern void scan_number_of_processors( const char *str );
extern int find_field( const_declaration_list decls, const_type rectype, const_origsymbol field );
extern field_list get_record_fields( const_declaration_list defs, const_type t );
extern void dump_expression( FILE *f, const_expression x, const char *desc );

extern bool is_constant_value( const_expression x, int n );
extern void yup();

extern bool is_this_basetype( const type t, const BASETYPE bt );

extern bool occurs_expression_list( const_expression_list xl, const_expression x );
#endif
