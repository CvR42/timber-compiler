// File: dump.h

extern void dump_Block( FILE *f, const_Block blk, const char *msg, ... );
extern void dump_Entry( FILE *f, const_Entry e, const char *msg, ... );
extern void dump_statement( FILE *f, const_statement smt, const char *msg, ... );
extern void dump_expression( FILE *f, const_expression x, const char *msg, ... );
extern void dump_TypeEntry_list( FILE *f, const_TypeEntry_list tl, const char *msg, ... );
extern void dump_TypeBinding_list( FILE *f, const_TypeBinding_list tl, const char *msg, ... );
