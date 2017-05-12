/* File: generate.h
 *
 * Header declarations for generate.c.
 */

extern void generate_vnusprog( FILE *f, const_tmstring_list includes, const_vnusprog theprog, bool dump_gc_stats );
extern void generate_dimensionlist(
 FILE *f,
 const_declaration_list decls,
 const_expression init,
 const tmuint nelms,
 const_size_list fl,
 const_origsymbol markfn
);
