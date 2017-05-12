/* File: lex.h
   Description of exported objects of lex.c
 */

extern int yylex( void );
extern void init_lex( void );
extern void end_lex( void );
extern void stat_lex( FILE *f );
extern void parserror( const char *message, ... );
extern void parsewarning( const char *message );
extern origin make_origin( void );
extern origsymbol make_origsymbol( const tmsymbol s );
extern void setlexfile( FILE *f, const char *nm );

extern void enter_pragma_state();
extern void leave_pragma_state();
