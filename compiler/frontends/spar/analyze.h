// File: analyze.h

#ifndef INC_ANALYZE_H
#define INC_ANALYZE_H

extern AFLAG aflag_either( AFLAG a, AFLAG b );
extern AFLAG aflag_both( AFLAG a, AFLAG b );

extern void abstract_either( AbstractValue a, AbstractValue b );
extern void abstract_thenelse( AbstractValue a, const_AbstractValue thenval, const_AbstractValue elseval );

extern bool search_var_programstate(
 const_VarState_list vl,
 tmsymbol nm,
 unsigned int *pos
);
extern bool search_var_programstates(
 const_ProgramState_list pl,
 unsigned int toplvl,
 tmsymbol nm,
 unsigned int *lvl,
 unsigned int *pos
);

extern bool is_compiletime_constant( const_optexpression x, origsymbol_list vars );
extern bool is_compiletime_constant( const_expression x, origsymbol_list vars );
extern bool is_compiletime_constant_operator( BINOP op );
extern bool is_compiletime_constant_type( const_type t );

extern bool is_trivial_initialization( const_expression x );
extern bool is_trivial_initialization( const_optexpression x );

// Additional flags that indicate what is *not* considered a side-effect.
// NO_SE_NEW implies NO_SE_ARRAY_NEW
// NO_SE_EXCEPTION implies NO_SE_NULLPOINTER
#define NO_SE_EXCEPTION		0x0001	// Throwing exception is not a side-effect
#define NO_SE_NULLPOINTER	0x0002	// Throwing NullPointerException not a s.e.
#define NO_SE_NEW		0x0004	// Doing 'new' is not a side-effect
#define NO_SE_ARRAY_NEW		0x0008	// Doing array 'new' is not a side-effect
#define NO_SE_CLASSINIT		0x0010	// Class init is not a side-effect

extern bool maybe_has_sideeffect( const_optexpression x, int flags );
extern bool maybe_has_sideeffect( const_expression x, int flags );
extern bool maybe_has_sideeffect( const_expression_list xl, int flags );

extern AbstractValue abstract_value( const_expression x, const_ProgramState_list state, const_Entry_list symtab );

extern void dump_programstates( FILE *f, const_ProgramState_list states );
extern void dump_programstate( FILE *f, unsigned int lvl, const_ProgramState state );

extern bool is_GCsetting_statement( const_Entry_list symtab, const_statement smt );


extern AFLAG is_zerotrip_Cardinality_list( const_Cardinality_list cards );

#endif
