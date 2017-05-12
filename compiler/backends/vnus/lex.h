/* Tm - an interface code generator.
 * Author: C. van Reeuwijk.
 *
 * All rights reserved.
 */

/* File: lex.h
   Description of exported objects of lex.c
 */

#define LINESIZE 300

extern int yylex( void );
extern void init_lex( void );
extern void end_lex( void );
extern void stat_lex( FILE *f );
extern void parserror( const char *message );
extern origin make_origin( void );
extern void setlexfile( FILE *f, const char *nm );

extern void enter_pragma_state();
extern void leave_pragma_state();

