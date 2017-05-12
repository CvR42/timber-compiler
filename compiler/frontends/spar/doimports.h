/* File: doimports.h */

extern tmsymbol canonify_type(
 SparProgram *prog,
 tmsymbol package,
 tmsymbol fromtype,
 const_origsymbol_list ondemands,
 TypeBinding_list *trans,
 const tmsymbol s,
 const_origin org
);
extern tmsymbol canonify_type(
 SparProgram *prog,
 tmsymbol package,
 tmsymbol fromtype,
 const_origsymbol_list ondemands,
 TypeBinding_list *trans,
 const_origsymbol s
);
extern tmsymbol search_TypeBinding( const_TypeBinding_list tl, tmsymbol s );
extern bool register_single_import_list(
 SparProgram *prog,
 TypeBinding_list *bindings,
 origsymbol_list tl
);
extern bool compile_type( SparProgram *prog, const tmsymbol t );
extern void check_findable_public( origsymbol t, const char *fnm );
extern bool java_lang_visible();
extern bool has_java_suffix( const char *fnm );
