/* Tm - an interface code generator.
 * Author: C. van Reeuwijk.
 *
 * All rights reserved.
 */

/* File: lex.c
 * Lexical analyzer for Tm datastructure definitions.
 */

#include <ctype.h>
#include <string.h>
#include <tmc.h>

#include "defs.h"
#include "tmadmin.h"
#include <vnusctl.h>
#include "error.h"
#include "global.h"
#include "lex.h"
#include "tokens.h"

#if 1
#define lexshow(tok,nm) if(lextr) fprintf(stderr,"token: %s(%d) yytext=\"%s\"\n",nm,tok,yytext)
#else
#define lexshow(tok,nm)
#endif

static char yytext[256];
static char linebuf[LINESIZE] = "";
static unsigned int lineix = 0;
static unsigned int oldlineix = 0;
static int lexlineno = 0;
static tmstring lexfilename = tmstringNIL;
static FILE *lexfile;	/* file to read from. */

// This is a lexer state flag: if true, we are parsing a pragma,
// which means that all keywords except 'true' and 'false' are not recognized.
static bool in_pragma_state = false;

/******************************************************
 *            SCANNING TREES                          *
 ******************************************************/

/* Scanning trees are used to recogize separator tokens.
   They form a linked list of possible acceptable characters.
   Each node may have a sub-list of possible extensions
   and an the token value for the current match.
 */

struct sctnode {
    struct sctnode *next;	/* next possibility in list */
    int sctchar;		/* char to match            */
    struct sctnode *sub;	/* subtree to use on match. */
    bool valid;			/* is acceptable token?     */
    int tokval;			/* token value for yacc     */
    const char *toknm;		/* token name for debugging */
};

#define SCTNIL (struct sctnode *)0

static long newsctnodecnt;
static long fresctnodecnt;

static struct sctnode *toktree;

/* Create a new scan tree node to match character 'c'. The next character
   to be considered is described by node 'nxt'. The subtree is set empty,
   and token is set invalid.
 */
static struct sctnode *newsctnode( struct sctnode *nxt, int c )
{
    struct sctnode *nw;

    nw = TM_MALLOC( struct sctnode *, sizeof( struct sctnode ) );
    newsctnodecnt++;
    nw->next    = nxt;
    nw->sctchar = c;
    nw->sub     = SCTNIL;
    nw->valid   = false;
    return nw;
}

/* Recursively free scan tree node 'n'. */
static void rfre_sctnode( struct sctnode *n )
{
    if( n == SCTNIL ){
	return;
    }
    rfre_sctnode( n->next );
    rfre_sctnode( n->sub );
    TM_FREE( n );
    fresctnodecnt++;
}

/* Add to the scan tree 'tree' a new token with tmstring 'str',
   YACC value 'val' and (debugging) name 'nm'.
   Return a pointer to the modified tree.
 */
static struct sctnode *addtok(
    struct sctnode *tree,
    const char *str,
    int val,
    const char *nm
)
{
    struct sctnode *tp;

    for( tp=tree; tp!=SCTNIL; tp=tp->next ){
	if( tp->sctchar == str[0] ) break;
    }
    if( tp == SCTNIL ){
	tree = newsctnode( tree, str[0] );
	tp = tree;
    }
    if( str[1] == '\0' ){
	tp->valid = true;
	tp->tokval= val;
	tp->toknm = nm;
    }
    else{
	tp->sub = addtok( tp->sub, &str[1], val, nm );
    }
    return tree;
}

/******************************************************
 *            Lexer state                             *
 ******************************************************/

void enter_pragma_state()
{
    in_pragma_state = true;
}

void leave_pragma_state()
{
    in_pragma_state = false;
}

/******************************************************
 *            TOKEN AND RESERVED WORD TABLES          *
 ******************************************************/

/* A structure to describe tokens and reserved words */
struct tok {
    const char *tokstr;		/* the tmstring to match. */
    int tokval;			/* associated token value for yacc */
    const char *toknm;		/* name for debugging */
    unsigned int flags;
};

// The possible flag in a token table.
#define TOKFLAG_PRAGMA 0x0001

/* A table of tokens. Is terminated by an entry with empty tmstring. */
static struct tok toktab[] =
{
    { "&", OP_ADDRESS, "OP_ADDRESS", 0 },
    { "*", OP_TIMES, "OP_TIMES", 0 },
    { "/", OP_DIVIDE, "OP_DIVIDE", 0 },
    { "+", OP_PLUS, "OP_PLUS", 0 },
    { "-", OP_MINUS, "OP_MINUS", 0 },
    { "<=", OP_LESSEQUAL, "OP_LESSEQUAL", 0 },
    { "<>", OP_NOTEQUAL, "OP_NOTEQUAL", 0 },
    { "<", OP_LESS, "OP_LESS", 0 },
    { ">=", OP_GREATEREQUAL, "OP_GREATEREQUAL", 0 },
    { ">>", OP_SHIFTRIGHT, "OP_SHIFTRIGHT", 0 },
    { ">>>", OP_USHIFTRIGHT, "OP_USHIFTRIGHT", 0 },
    { "<<", OP_SHIFTLEFT, "OP_SHIFTLEFT", 0 },
    { ">", OP_GREATER, "OP_GREATER", 0 },
    { "=", OP_EQUAL, "OP_EQUAL", 0 },
    { "~", OP_NEGATE, "OP_NEGATE", 0 },
    { (const char *) NULL, 0, "", 0 }
};

/* A table of reserved words: symbols that have a special meaning.
   Is terminated by an entry with NULL tmstring.
 */

static struct tok rwtab[] =
{
    { "addtask", KEY_ADDTASK, "KEY_ADDTASK", 0 },
    { "and", KEY_AND, "KEY_AND", 0 },
    { "array", KEY_ARRAY, "KEY_ARRAY", 0 },
    { "assign", KEY_ASSIGN, "KEY_ASSIGN", 0 },
    { "assignop", KEY_ASSIGNOP, "KEY_ASSIGNOP", 0 },
    { "barrier", KEY_BARRIER, "KEY_BARRIER", 0 },
    { "blockreceive", KEY_BLOCKRECEIVE, "KEY_BLOCKRECEIVE", 0 },
    { "blocksend", KEY_BLOCKSEND, "KEY_BLOCKSEND", 0 },
    { "boolean", KEY_BOOLEAN, "KEY_BOOLEAN", 0 },
    { "byte", KEY_BYTE, "KEY_BYTE", 0 },
    { "cardinalityvariable", KEY_CARDINALITYVARIABLE, "KEY_CARDINALITYVARIABLE", 0 },
    { "cast", KEY_CAST, "KEY_CAST", 0 },
    { "catch", KEY_CATCH, "KEY_CATCH", 0 },
    { "char", KEY_CHAR, "KEY_CHAR", 0 },
    { "checkedindex", KEY_CHECKEDINDEX, "KEY_CHECKEDINDEX", 0 },
    { "complex", KEY_COMPLEX, "KEY_COMPLEX", 0 },
    { "declarations", KEY_DECLARATIONS, "KEY_DECLARATIONS", 0 },
    { "default", KEY_DEFAULT, "KEY_DEFAULT", 0 },
    { "delete", KEY_DELETE, "KEY_DELETE", 0 },
    { "dontcare", KEY_DONTCARE, "KEY_DONTCARE", 0 },
    { "double", KEY_DOUBLE, "KEY_DOUBLE", 0 },
    { "dowhile", KEY_DOWHILE, "KEY_DOWHILE", 0 },
    { "each", KEY_EACH, "KEY_EACH", 0 },
    { "empty", KEY_EMPTY, "KEY_EMPTY", 0 },
    { "executetasks", KEY_EXECUTETASKS, "KEY_EXECUTETASKS", 0},
    { "expression", KEY_EXPRESSION, "KEY_EXPRESSION", 0 },
    { "expressionpragma", KEY_EXPRESSIONPRAGMA, "KEY_EXPRESSIONPRAGMA", 0 },
    { "externalfunction", KEY_EXTERNALFUNCTION, "KEY_EXTERNALFUNCTION", 0 },
    { "externalprocedure", KEY_EXTERNALPROCEDURE, "KEY_EXTERNALPROCEDURE", 0 },
    { "externalvariable", KEY_EXTERNALVARIABLE, "KEY_EXTERNALVARIABLE", 0 },
    { "false", KEY_FALSE, "KEY_FALSE", TOKFLAG_PRAGMA },
    { "field", KEY_FIELD, "KEY_FIELD", 0 },
    { "fillednew", KEY_FILLEDNEW, "KEY_FILLEDNEW", 0 },
    { "final", KEY_FINAL, "KEY_FINAL", 0 },
    { "float", KEY_FLOAT, "KEY_FLOAT", 0 },
    { "for", KEY_FOR, "KEY_FOR", 0 },
    { "forall", KEY_FORALL, "KEY_FORALL", 0 },
    { "foreach", KEY_FOREACH, "KEY_FOREACH", 0 },
    { "fork", KEY_FORK, "KEY_FORK", 0 },
    { "forkall", KEY_FORKALL, "KEY_FORKALL", 0 },
    { "formalvariable", KEY_FORMALVARIABLE, "KEY_FORMALVARIABLE", 0 },
    { "function", KEY_FUNCTION, "KEY_FUNCTION", 0 },
    { "functioncall", KEY_FUNCTIONCALL, "KEY_FUNCTIONCALL", 0 },
    { "garbagecollect", KEY_GARBAGECOLLECT, "KEY_GARBAGECOLLECT", 0 },
    { "getbuf", KEY_GETBUF, "KEY_GETBUF", 0 },
    { "getlength", KEY_GETLENGTH, "KEY_GETLENGTH", 0 },
    { "getsize", KEY_GETSIZE, "KEY_GETSIZE", 0 },
    { "globalvariable", KEY_GLOBALVARIABLE, "KEY_GLOBALVARIABLE", 0 },
    { "goto", KEY_GOTO, "KEY_GOTO", 0 },
    { "if", KEY_IF, "KEY_IF", 0 },
    { "int", KEY_INT, "KEY_INT", 0 },
    { "israised", KEY_ISRAISED, "KEY_ISRAISED", 0 },
    { "local", KEY_LOCAL, "KEY_LOCAL", 0 },
    { "localvariable", KEY_LOCALVARIABLE, "KEY_LOCALVARIABLE", 0 },
    { "long", KEY_LONG, "KEY_LONG", 0 },
    { "mod", KEY_MOD, "KEY_MOD", 0 },
    { "new", KEY_NEW, "KEY_NEW", 0 },
    { "newarray", KEY_NEWARRAY, "KEY_NEWARRAY", 0 },
    { "newfilledarray", KEY_NEWFILLEDARRAY, "KEY_NEWFILLEDARRAY", 0 },
    { "newrecord", KEY_NEWRECORD, "KEY_NEWRECORD", 0 },
    { "not", KEY_NOT, "KEY_NOT", 0 },
    { "notnullassert", KEY_NOTNULLASSERT, "KEY_NOTNULLASSERT", 0 },
    { "null", KEY_NULL, "KEY_NULL", 0 },
    { "nullednew", KEY_NULLEDNEW, "KEY_NULLEDNEW", 0 },
    { "or", KEY_OR, "KEY_OR", 0 },
    { "pointer", KEY_POINTER, "KEY_POINTER", 0 },
    { "pragma", KEY_PRAGMA, "KEY_PRAGMA", 0 },
    { "print", KEY_PRINT, "KEY_PRINT", 0 },
    { "println", KEY_PRINTLN, "KEY_PRINTLN", 0 },
    { "procedure", KEY_PROCEDURE, "KEY_PROCEDURE", 0 },
    { "procedurecall", KEY_PROCEDURECALL, "KEY_PROCEDURECALL", 0 },
    { "program", KEY_PROGRAM, "KEY_PROGRAM", 0 },
    { "readonly", KEY_READONLY, "KEY_READONLY", 0 },
    { "receive", KEY_RECEIVE, "KEY_RECEIVE", 0 },
    { "record", KEY_RECORD, "KEY_RECORD", 0 },
    { "reduction", KEY_REDUCTION, "KEY_REDUCTION", 0 },
    { "registertask", KEY_REGISTERTASK, "KEY_REGISTERTASK", 0},
    { "rethrow", KEY_RETHROW, "KEY_RETHROW", 0 },
    { "return", KEY_RETURN, "KEY_RETURN", 0 },
    { "send", KEY_SEND, "KEY_SEND", 0 },
    { "shape", KEY_SHAPE, "KEY_SHAPE", 0 },
    { "short", KEY_SHORT, "KEY_SHORT", 0 },
    { "shortand", KEY_SHORTAND, "KEY_SHORTAND", 0 },
    { "shortor", KEY_SHORTOR, "KEY_SHORTOR", 0 },
    { "sizeof", KEY_SIZEOF, "KEY_SIZEOF", 0 },
    { "statements", KEY_STATEMENTS, "KEY_STATEMENTS", 0 },
    { "string", KEY_STRING, "KEY_STRING", 0 },
    { "switch", KEY_SWITCH, "KEY_SWITCH", 0 },
    { "throw", KEY_THROW, "KEY_THROW", 0 },
    { "timeout", KEY_TIMEOUT, "KEY_TIMEOUT", 0 },
    { "true", KEY_TRUE, "KEY_TRUE", TOKFLAG_PRAGMA },
    { "unchecked", KEY_UNCHECKED, "KEY_UNCHECKED", 0 },
    { "value", KEY_VALUE, "KEY_VALUE", 0 },
    { "volatile", KEY_VOLATILE, "KEY_VOLATILE", 0 },
    { "wait", KEY_WAIT, "KEY_WAIT", 0 },
    { "waitpending", KEY_WAITPENDING, "KEY_WAITPENDING", 0 },
    { "where", KEY_WHERE, "KEY_WHERE", 0 },
    { "while", KEY_WHILE, "KEY_WHILE", 0 },
    { "xor", KEY_XOR, "KEY_XOR", 0 },
    { (const char *) NULL, 0, "", 0 }
};

/******************************************************
 *            File management                         *
 ******************************************************/

#define UNGETBUFLENSTEP 10

/* variables for a dynamic unget buffer:
 * length, pointer to buffer and index of next char to un-get.
 */
static unsigned int ungetbuflen;
static int *ungetbuf;
static unsigned int ungetbufix;

/* Push back character 'c' in local pushback queue.
 * Enlarge queue if necessary.
 */
static void lexungetc( int c )
{
    if( lineix>0 ){
	lineix--;
	return;
    }
    if( ungetbufix >= ungetbuflen ){
	ungetbuflen+=UNGETBUFLENSTEP;
	ungetbuf = TM_REALLOC( int *, ungetbuf, ungetbuflen*sizeof(int) );
    }
    ungetbuf[ungetbufix++] = c;
}

/* Get a character from input stream or pushback queue. */
static int lexgetc( void )
{
    int c;

    if( ungetbufix != 0 ){
	c = ungetbuf[--ungetbufix];
    }
    else {
	if( linebuf[lineix] == '\0' ){
	    if( fgets( linebuf, LINESIZE, lexfile ) == NULL ){
		return EOF;
	    }
	    lineix = 0;
	    oldlineix = 0;
	}
	return linebuf[lineix++];
    }
    return c;
}

/* Return a new origin descriptor. */
origin make_origin( void )
{
    return new_origin( add_tmsymbol( lexfilename ), lexlineno );
}

static void show_parse_context( FILE *f )
{
    unsigned int ix;
    bool shown = false;

    fputs( linebuf, f );
    for( ix=0; ix<oldlineix; ix++ ){
	if( linebuf[ix] == '\t' ){
	    fputc( '\t', f );
	}
	else {
	    fputc( ' ', f );
	}
    }
    for( ix=ix; ix<lineix; ix++ ){
        fputc( '^', f );
        shown = true;
    }
    if( !shown ){       
        fputc( '^', f );
    }                                                                    
    fputs( "\n", f );
}

void parserror( const char *message )
{
    sprintf( errpos, "%s(%d)", lexfilename, lexlineno );
    error( message );
    show_parse_context( stderr );
}

/* Current maximum is '\000'. */
#define MAXCHARESCAPE 4

/* Try to read a (possibly escaped) character from the file 'f'. Give
 * an error message if this is not successful. The string of characters
 * forming the escaped character are put into 'buf'.
 *
 * Since at most MAXCHARESCAPE characters fit in an escape sequence,
 * a fixed buffer of at least that size *plus one* is assumed.
 */
static bool scanescapedchar( char *buf )
{
    int c;
    char *bufp = buf;

    c = lexgetc();
    if( c == EOF ){
        return false;
    }
    *bufp++ = (char) c;
    if( c == '\\' ){
        c = lexgetc();
        if( c == EOF ){
	    *bufp = '\0';
            return true;
        }
	*bufp++ = (char) c;
	if( isdigit( c ) ){
	    c = lexgetc();
	    if( isdigit( c ) ){
		*bufp++ = (char) c;

		c = lexgetc();
		if( isdigit( c ) ){
		    *bufp++ = (char) c;
		}
		else
		    lexungetc( c );
	    }
	    else {
		lexungetc( c );
	    }
	}
	/* TODO: handle unicode escape sequences. */
    }
    *bufp = '\0';
    return true;
}

/* Try to read a string. Return true if this is successful, and set '*s'
 * to point to that string, else return false. A string may contain escape
 * sequences with a '\\', but no newlines. The '"' around
 * the string are stripped.
 */
static bool scanstring( tmstring *s )
{
    int c;
    tmstring bufp;
    unsigned int ix;
    unsigned int sz;
    char buf[MAXCHARESCAPE+1];

    c = lexgetc();
    if( c != '"' ){
	lexungetc( c );
	return false;
    }
    ix = 0;
    sz = 5;	/* Initial guess for a reasonable string size. */
    bufp = create_tmstring( sz );
    bufp[0] = '\0';
    for(;;){
	size_t csz;

	if( !scanescapedchar( buf ) ){
	    parserror( "end of file in string" );
	    break;
	}
	if( buf[0] == '"' ){
	    break;
	}
	if( buf[0] == '\n' ){
	    parserror( "end of line in string" );
	    break;
	}
	csz = strlen( buf );
	if( ix+csz+1>sz ){
	    sz += 1+csz+sz;
	    bufp = realloc_tmstring( bufp, sz );
	}
	strcpy( bufp+ix, buf );
	ix += csz;
    }
    *s = bufp;
    return true;
}

/* Try to read a character constant. Return true if this is successful,
 * and set '*s' to point to a string that contains the character constant,
 * else return false. A character constant may contain escape sequences.
 * The '\'' around the string are stripped.
 */
static bool scanchar( char *s )
{
    int c;

    c = lexgetc();
    if( c != '\'' ){
	lexungetc( c );
	return false;
    }
    if( !scanescapedchar( s ) ){
	parserror( "end of file in string" );
	s[0] = '\0';
    }
    c = lexgetc();
    if( c != '\'' ){
	parserror( "unterminated character constant" );
    }
    return true;
}

/* Try to read characters from 'lexgetc()' to match one of the tokens
   from the table 'toktab' in the tmstring 'buf'. The token characters
   to match are given by the scan tree 'tree'.  Fill '*tokval' with the
   token value, and '*toknm' with the name of the token. Return true if
   this is successful, else return false.
 */
static bool scantoken(
    const struct sctnode *tree,
    char *buf,
    int *tokval,
    const char **toknm
)
{
    int c;
    const struct sctnode *tp;

    c = lexgetc();
    if( c == EOF ){
	return false;
    }
    for( tp=tree; tp!=SCTNIL; tp=tp->next ){
	if( tp->sctchar == c ) break;
    }
    if( tp == SCTNIL ){
	lexungetc( c );
	return false;
    }
    buf[0] = (char) c;
    if( scantoken( tp->sub, &buf[1], tokval, toknm ) ){
	return true;
    }
    if( tp->valid ){
	*tokval = tp->tokval;
	*toknm = tp->toknm;
	buf[1] = '\0';
	return true;
    }
    lexungetc( c );
    return false;
}

/* Try to read a symbol in the tmstring 'buf' using lexgetc(). Return true if
   this is successful, else return false.
   A symbol is of the form [a-zA-Z_][a-zA-Z0-9_]*.
 */
static bool scansymbol( char *buf )
{
    int c = lexgetc();
    if( !isalpha( c ) && c != '_' ){
	lexungetc( c );
	return false;
    }
    do{
	*buf++ = (char) c;
	c = lexgetc();
    } while( isalnum( c ) || c == '_' );
    *buf = '\0';
    lexungetc( c );
    return true;
}

/* Read a numerical literal in the string 'yytext' using lexgetc().
 * A sign in front of the numerical literal is allowed, but if it
 * isn't followed by digits, it is rejected.
 */
static bool scan_numerical_literal()
{
    char *p = yytext;
    int c;

    c = lexgetc();
    if( c == '+' || c == '-' ){
	int c1 = lexgetc();

	lexungetc( c1 );
	if( !isdigit( c1 ) && c1 != '.' ){
	    lexungetc( c );
	    return false;
	}
    }
    else if( c == '.' ){
	int c1 = lexgetc();

	lexungetc( c1 );
	if( !isdigit( c1 ) ){
	    lexungetc( c );
	    return false;
	}
    }
    else if( !isdigit( c ) ){
	lexungetc( c );
	return false;
    }
    do{
	*p++ = (char) c;
	c = lexgetc();
    } while( isdigit( c ) );
    if( c == '.' ){
	do{
	    *p++ = (char) c;
	    c = lexgetc();
	} while( isdigit( c ) );
    }
    if( c == 'e' || c == 'E' ){
	*p++ = (char) c;
	c = lexgetc();
	if( c == '-' || c == '+' ){
	    *p++ = (char) c;
	    c = lexgetc();
	}
	while( isdigit( c ) ){
	    *p++ = (char) c;
	    c = lexgetc();
	}
    }
    if( strchr( "ilsbfdtu", tolower( c ) ) != NULL ){
	*p++ = (char) c;
    }
    else {
	lexungetc( c );
    }
    *p = '\0';
    return true;
}

/* "//" encountered, skip characters until end of line. */
static void skiplinecomment( void )
{
    int c;

    c = lexgetc();
    while( c != '\n' && c != EOF ){
	c = lexgetc();
    }
    lexlineno++;
}

/* '/' and '*' encountered, skip characters until end of comment. */
static void skipcomment( void )                        
{
    int c;                                              

    c = lexgetc();                 
    do{                                                    
	while( c != '*' ){
	    if( c == '\n' ){
		lexlineno++;
	    }
	    c = lexgetc();               
	    if( c == EOF ){
		parserror( "EOF in comment" );
	    }
	}   
	c = lexgetc();
    } while( c != '/' );
}

/* Return next token from lex input file. Set 'yytext' to the characters
 * of the next token, and 'yylval' to the associated value of the token.
 */
int yylex( void )
{
    int c;
    const char *toknm;
    int tokval;

again:
    c = lexgetc();
    if( c == '/' ){
	c = lexgetc();
	if( c == '/' ){
	    skiplinecomment();
	    goto again;
	}
	if( c == '*' ){
	    skipcomment();
	    goto again;
	}
	lexungetc( c );
	c = '|';
    }
    if( c == '\n' ){
	lexlineno++;
	goto again;
    }
    if( isspace( c ) ) goto again;
    if( c == EOF ){
	yytext[0] = '\0';
	oldlineix = lineix;
	lexshow(EOF,"EOF");
	return 0;
    }
    lexungetc( c );
    oldlineix = lineix;
    if( scanchar( yytext ) ){
	yylval._vnus_char = evaluate_char_constant( yytext );
	return CHAR_LITERAL;
    }
    if( scanstring( &yylval._vnus_string ) ){
	return STRING_LITERAL;
    }
    if( scansymbol( yytext ) ){
	struct tok *rwp;
	tmstring low = new_tmstring( yytext );
	char *p = low;

	while( *p != '\0' ){
	    *p = (char) tolower( *p );
	    p++;
	}
	for( rwp = rwtab; rwp->tokstr != NULL; rwp++ ){
	    if( strcmp( rwp->tokstr, low ) == 0 ){
		if( !in_pragma_state || (rwp->flags & TOKFLAG_PRAGMA) ){
		    lexshow(rwp->tokval,rwp->toknm);
		    rfre_tmstring( low );
		    return( rwp->tokval );
		}
	    }
	}
	rfre_tmstring( low );
	yylval._identifier = add_tmsymbol( yytext );
	lexshow( IDENTIFIER, "IDENTIFIER" );
	return IDENTIFIER;
    }
    if( scan_numerical_literal() ){
        const char *p;                                                  
        int isint;
	size_t pos;

	/* This assumes yytext is at least 1 long. Given the context,
	 * this is a valid assumption.
	 */
	pos = strlen( yytext )-1;
	switch( tolower( yytext[pos] ) ){
	    case 'b':
		lexshow( BYTE_LITERAL, "BYTE_LITERAL" );
		yytext[pos] = '\0';
		// TODO: generate an error if there is an overflow.
		yylval._vnus_byte = (vnus_byte) atoi( yytext );
		return BYTE_LITERAL;

	    case 'd':
		lexshow( DOUBLE_LITERAL, "DOUBLE_LITERAL" );
		yytext[pos] = '\0';
		yylval._vnus_double = strtod( yytext, (char **) NULL );
		return DOUBLE_LITERAL;

	    case 'f':
		lexshow( FLOAT_LITERAL, "FLOAT_LITERAL" );
		yytext[pos] = '\0';
		yylval._vnus_float = strtod( yytext, (char **)NULL  );
		return FLOAT_LITERAL;

	    case 'i':
		lexshow( INT_LITERAL, "INT_LITERAL" );
		yytext[pos] = '\0';
		yylval._vnus_int = (vnus_int) atol( yytext );
		return INT_LITERAL;

	    case 'l':
		lexshow( LONG_LITERAL, "LONG_LITERAL" );
		yytext[pos] = '\0';
		yylval._vnus_long = string_to_vnus_long( yytext );
		return LONG_LITERAL;

	    case 's':
		lexshow( SHORT_LITERAL, "SHORT_LITERAL" );
		yytext[pos] = '\0';
		yylval._vnus_short = (vnus_short) atoi( yytext );
		return SHORT_LITERAL;

	    case 't':
		lexshow( FLOAT_LITERAL, "FLOAT_LITERAL" );
		yytext[pos] = '\0';
		yylval._vnus_float = intbits_to_vnus_float( (vnus_int) atol( yytext ) );
		return FLOAT_LITERAL;

	    case 'u':
		lexshow( DOUBLE_LITERAL, "DOUBLE_LITERAL" );
		yytext[pos] = '\0';
		yylval._vnus_double = longbits_to_vnus_double( string_to_vnus_long( yytext ) );
		return DOUBLE_LITERAL;

	}
        p = yytext;
        isint = true;
        while( *p != '\0' ){
            if( *p == '.' || *p == 'e' || *p == 'E' ){
		isint = false;
		break;
	    }
            p++;
        }                  
        if( isint ){
            lexshow( INT_LITERAL, "INT_LITERAL" );
	    yylval._vnus_int = (vnus_int) atol( yytext );
            return INT_LITERAL;
        }                     
        lexshow( FLOAT_LITERAL, "FLOAT_LITERAL" );
	yylval._vnus_float = strtod( yytext, (char **) NULL );
        return FLOAT_LITERAL;
    }
    if( scantoken( toktree, yytext, &tokval, &toknm ) ){
	lexshow( tokval, toknm );
	return tokval;
    }
    c = lexgetc();
    if( lextr ){
	if( isprint( c ) ){
	    fprintf( stderr, "character: '%c' (0x%02x)\n", c, c );
	}
	else {
	    fprintf( stderr, "character: 0x%02x\n", c );
	}
    }
    return c;
}

/* Initialize lexical analysis routines. */
void init_lex( void )
{
    struct tok *ttp;

    newsctnodecnt=0;
    fresctnodecnt=0;
    ungetbuflen = 2;	/* Don't make this 0, some mallocs don't like it */
    ungetbuf = TM_MALLOC( int *, ungetbuflen*sizeof(int) );
    ungetbufix = 0;
    toktree = SCTNIL;
    for( ttp = toktab; ttp->tokstr != NULL; ttp++ ){
	toktree = addtok( toktree, ttp->tokstr, ttp->tokval, ttp->toknm );
    }
}

/* Specify that lex routines should read from file 'f'. This file
 * has name 'nm'.
 */                            
void setlexfile( FILE *f, const char *nm )
{
    lexfile = f;
    if( lexfilename != tmstringNIL ){
        rfre_tmstring( lexfilename );
    }
    lexfilename = new_tmstring( nm );
    lexlineno = 1;          
}

/* Terminate lexcial analysis routines. Free all allocated memory */
void end_lex( void )
{
    rfre_sctnode( toktree );
    toktree = SCTNIL;
    TM_FREE( ungetbuf );
    ungetbuflen = 0;
    ungetbufix = 0;
}

/* Give allocation statistics of lex routines. */
void stat_lex( FILE *f )
{
#ifdef STAT
    fprintf( f, "ungetbuflen=%d\n", ungetbuflen );
    fprintf(
	f,
	"%-20s: %6ld allocated, %6ld freed.%s\n",
	"sctnode",
	newsctnodecnt,
	newsctnodecnt,
	((newsctnodecnt==fresctnodecnt)? "": "<-")
    );
#else
    (void) f; /* to stop 'f unused' */
#endif
}
