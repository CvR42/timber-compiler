/* File: symbol_table.h
 *
 * Header file for symbol_table.c
 */

/* The global variables below describe the structures of the
 * tmsymbol table.
 */

extern vnusdeclarationdata search_vnusdeclaration( const tmsymbol name );
extern vnusdeclarationdata lookup_vnusdeclaration( const tmsymbol name );
extern const formalParameter_list lookup_formalParameter_list( const origsymbol name );
extern type lookup_parmtype( const origsymbol name );
extern type lookup_fntype( const origsymbol name );
extern type lookup_vartype( const origsymbol name );

extern void set_symbol_table( vnusdeclaration_list symtab );
extern vnusdeclaration_list get_symbol_table( void );
extern void init_symbol_table( void );
extern void end_symbol_table( void );

extern void add_symtab_returnvar( origsymbol nm, tmsymbol scope, pragma_list pragmas, type t );
extern void add_symtab_globalvar(
 const origsymbol nm,
 const pragma_list pragmas,
 const type t,
 const optexpression init
);
extern void add_symtab_externalvar( origsymbol nm, pragma_list pragmas, type t );
extern void add_symtab_localvar(
 const origsymbol nm,
 const tmsymbol scope,
 const pragma_list pragmas,
 const type t,
 const optexpression init
);
extern void add_symtab_formal(
 origsymbol nm,
 tmsymbol scope,
 pragma_list pragmas,
 type t
);
extern void add_symtab_cardvar( origsymbol nm, pragma_list pragmas );
extern void add_symtab_function(
 const pragma_list pragmas,
 const origsymbol nm,
 const formalParameter_list args,
 const origsymbol valnm,
 const type valtype,
 const block body
);
extern void add_symtab_procedure( pragma_list pragmas, origsymbol nm, formalParameter_list args, block body );
extern void add_symtab_external_function(
 const pragma_list pragmas,
 const origsymbol nm,
 const formalParameter_list args,
 const type valtype
);
extern void add_symtab_external_procedure( pragma_list pragmas, origsymbol nm, formalParameter_list args );
