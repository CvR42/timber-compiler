/* File: collect.h */

extern origsymbol_list collect_vardeclaration_names(
 origsymbol_list nms,
 const_statement_list l
);
extern origsymbol_list collect_Cardinality_symbols( const_Cardinality_list l );
extern origsymbol_list collect_function_names(
 const_FormalParameter_list args,
 const_statement_list ldecl
);

#define FORCE_STATIC true
#define NO_FORCE_STATIC false

extern origsymbol_list collect_labels( const_statement_list l );
extern Field_list collect_type_fields( const_statement_list sl, bool force_static );
extern Signature_list collect_type_methods( const_statement_list sl );
extern Signature_list collect_class_constructors( const_statement_list sl );
extern tmsymbol_list collect_interfaces( const_TypeEntry_list types, origsymbol_list callstack, tmsymbol_list all, TypeEntry e );

extern Signature construct_Method_Signature( const_MethodDeclaration m );
