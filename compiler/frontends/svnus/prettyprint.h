/* File: prettyprint.h
 *
 * Header declarations for prettyprint.c.
 */

extern void pp_vardeclaration(
 FILE *f,
 unsigned int indent,
 tmsymbol nm,
 type t 
);
extern void pp_type( FILE *f, type t );
extern void pp_formalparameter_list( FILE *f, formalParameter_list xl );
extern void pp_vnusprog( FILE *f, vnusprog theprog );
