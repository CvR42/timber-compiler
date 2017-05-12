/* File: symbol_table.h
 *
 * Header file for symbol_table.c
 */

extern Entry_list add_symtab( Entry_list symtab, Entry e );
extern bool search_MethodMapping(
    const_MethodMapping_list trans,
    const_Signature s,
    unsigned int *pos
);
extern Entry search_Entry( const_Entry_list symtab, const tmsymbol name );
extern Entry search_Entry( const_Entry_list symtab, const_origsymbol name );
extern VariableEntry lookup_VariableEntry( const_Entry_list symtab, const tmsymbol name );
extern VariableEntry search_VariableEntry( const_Entry_list symtab, const tmsymbol name );
extern MethodEntry lookup_MethodEntry( const_Entry_list symtab, const tmsymbol name );
extern Entry lookup_Entry( const_Entry_list symtab, const tmsymbol name );
extern Entry lookup_Entry( const_Entry_list symtab, const_origsymbol name );

extern bool search_TypeEntry_pos( const_TypeEntry_list typelist, const tmsymbol name, unsigned int *pos );
extern TypeEntry search_TypeEntry( const_TypeEntry_list typelist, const tmsymbol name );

extern TypeEntry lookup_TypeEntry( const_TypeEntry_list typelist, const tmsymbol name );
extern TypeEntry lookup_TypeEntry( const_TypeEntry_list typelist, const_origsymbol name );
extern TypeEntry lookup_TypeEntry( const_TypeEntry_list typelist, const_type t );

extern TypeEntry_list register_subclass( TypeEntry_list typelist, const_origsymbol super, tmsymbol sub );

extern TypeEntry_list register_TypeEntry( TypeEntry_list typelist, TypeEntry e );
extern type lookup_vartype( const_Entry_list symtab, const const_origsymbol name );
#if 0
extern type lookup_vartype( const_Entry_list symtab, const tmsymbol name );
#endif
extern type lookup_vartype( const_Entry e );
extern bool is_valid_access(
 const_TypeEntry_list typelist,
 modflags flags,
 tmsymbol in_where,
 tmsymbol from_where
);

typedef enum en_want_static { NOT_STATIC, WANT_STATIC } want_static;

extern tmsymbol_list lookup_object_all_fields(
 const_TypeEntry_list typelist,
 tmsymbol obj,
 want_static st
);

extern VtableEntry_list construct_Class_vtable(
 const_ClassEntry e,
 const_TypeEntry_list types,
 const_Entry_list symtab
);

extern MethodInfo_list construct_MethodInfo_list(
 const_ClassEntry e,
 const_TypeEntry_list types,
 const_Entry_list symtab
);

extern MethodEntry search_Method(
 SparProgram *prog,
 const_Entry_list symtab,
 const_TypeEntry_list typelist,
 const tmsymbol where_class,		/* From which class is this invoked. */
 const tmsymbol invocation_class,	/* In which class is the method. */
 const_Signature s,			/* Desired signature of the method. */
 bool *is_static,
 bool *use_static_call,			// static call can/must be used?
 bool *rejected,
 const_origin org
);

extern const_FieldMapping bind_Field_to_Mapping(
 SparProgram *prog,
 const_Entry_list symtab,
 const_TypeEntry_list typelist,
 tmsymbol in_where,
 tmsymbol from_where,
 const_origsymbol field
);
extern VariableEntry bind_Field(
 SparProgram *prog,
 const_Entry_list symtab,
 const_TypeEntry_list typelist,
 tmsymbol in_where,
 tmsymbol from_where,
 const_origsymbol field
);

extern MethodEntry bind_Method(
 SparProgram *prog,
 const_Entry_list symtab,
 const_TypeEntry_list typelist,
 tmsymbol where_class,			/* From which class is this invoked. */
 tmsymbol invocation_class,		/* In which class is the method. */
 const_Signature s,			/* Desired signature of the method. */
 bool *is_static,
 bool *use_static_call,			// static call can/must be used?
 const_origin org
);

extern tmsymbol bind_Constructor(
 SparProgram *prog,
 const_Entry_list symtab,		/* The symbol table */
 const_TypeEntry_list typelist,		/* The list of known types */
 tmsymbol where_class,			/* From which class is this invoked. */
 tmsymbol invocation_class,		/* In which class is the method. */
 const_Signature s,			/* Desired signature of the method. */
 const_origin org
);

extern tmsymbol lookup_mangledname_type(
 const_TypeEntry_list typelist,
 tmsymbol t,
 const_Signature s
);

extern const_expression get_init_expression( const_Entry e );

extern VtableEntry_list construct_Interface_vtable(
    const_TypeEntry_list typelist,
    const_Entry_list symtab,
    tmsymbol cl
);

extern tmsymbol search_overridden( const_TypeEntry_list typelist, const_MethodEntry me );

extern FieldMapping_list build_Field_translation(
 FieldMapping_list mappings,
 tmsymbol owner,
 const_Field s
);
extern FieldMapping_list build_Field_translations(
 FieldMapping_list mappings,
 tmsymbol owner,
 const_Field_list fl
);

extern bool is_subclass( const_TypeEntry_list typelist, tmsymbol sub, tmsymbol super );
extern bool is_subclass( const_TypeEntry_list typelist, tmsymbol sub, const_TypeEntry super );
extern bool is_subclass( const_TypeEntry_list typelist, const_type sub, const_type super );
extern bool is_subclass( const_TypeEntry_list typelist, const_type sub, tmsymbol super );

extern bool implements_interface( const_TypeEntry_list typelist, origsymbol nm, const char *inf );
extern bool implements_interface( const_TypeEntry_list typelist, const_type t, const char *inf );
extern bool implements_interface( const_TypeEntry_list typelist, const_type_list types, const_origsymbol inf );

extern tmsymbol_list get_GC_links( const_Entry_list symtab, tmsymbol scope );
extern void null_GC_links( const_Entry_list symtab, tmsymbol scope );
extern tmsymbol_list get_global_GC_links( const_Entry_list symtab );

extern bool is_collectable_type( const_type t );
