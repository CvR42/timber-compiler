// File: analysis.h

// Given an expression 'x', try to determine if the expression evaluates
// to zero/null/false. Possible answers are yes, no, or maybe.
extern AFLAG is_zero( const_expression x, const_ProgramState_list state );
extern AFLAG is_zero( const_expression x, const_declaration_list defs, const_ProgramState_list state );
extern AFLAG is_zero( const_location l, const_ProgramState_list state );

extern AFLAG is_zerotrip_cardinality_list( const_cardinality_list cards );

extern AbstractValue abstract_value( const_expression x, const_declaration_list defs, const_ProgramState_list state );
extern void dump_programstates( FILE *f, const_ProgramState_list states );
extern void abstract_either( AbstractValue a, AbstractValue b );
extern void abstract_thenelse(
 AbstractValue a,
 const_AbstractValue thenval,
 const_AbstractValue elseval
);
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
