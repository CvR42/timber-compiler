/* File: collect.h */

extern origsymbol_list collect_declaration_names( const declaration_list l );
extern origsymbol_list collect_cardinality_symbols( const cardinality_list l );
extern origsymbol_list collect_secondary_symbols( const secondary_list l );
extern origsymbol_list collect_function_names(
 const origsymbol_list parms,
 const origsymbol retval,
 const declaration_list ldecl
);
extern origsymbol_list collect_field_names( const field_list l );
extern origsymbol_list collect_labels( const statement_list l );
