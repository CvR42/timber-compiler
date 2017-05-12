/* File: collect.h */

extern origsymbol_list collect_formalParameter_names( origsymbol_list in, formalParameter_list l );
extern origsymbol_list collect_declaration_names( origsymbol_list nms, declaration_list l );
extern origsymbol_list collect_cardinality_symbols( cardinality_list l );
extern void collect_global_symbols(
    origsymbol_list *globals,
    origsymbol_list *externals,
    vnusprog prog
);
extern origsymbol_list collect_function_names(
 formalParameter_list args,
 origsymbol retval,
 declaration_list ldecl
);
extern origsymbol_list collect_procedure_names(
 formalParameter_list args,
 declaration_list ldecl
);
extern origsymbol_list collect_field_names( const field_list l );
extern origsymbol_list collect_labels( const statement_list l );
