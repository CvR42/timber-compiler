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

struct sctnode *toktree;

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
    nw->valid   = FALSE;
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
	tp->valid = TRUE;
	tp->tokval= val;
	tp->toknm = nm;
    }
    else{
	tp->sub = addtok( tp->sub, &str[1], val, nm );
    }
    return tree;
}

/******************************************************
 *            TOKEN AND RESERVED WORD TABLES          *
 ******************************************************/

/* A structure to describe tokens and reserved words */
struct tok {
    const char *tokstr;		/* the tmstring to match. */
    int tokval;			/* associated token value for yacc */
    const char *toknm;		/* name for debugging */
};

#define TOKNIL (struct tok *) 0;

/* A table of tokens. Is terminated by an entry with empty tmstring. */
static struct tok toktab[] =
{
    { "$>", OP_PRAGMAEND, "OP_PRAGMAEND" },
    { "*", OP_TIMES, "OP_TIMES" },
    { "+", OP_PLUS, "OP_PLUS" },
    { "-", OP_MINUS, "OP_MINUS" },
    { "/", OP_DIVIDE, "OP_DIVIDE" },
    { "::", OP_LABEL, "OP_LABEL" },
    { ":=", OP_ASSIGNMENT, "OP_ASSIGNMENT" },
    { "<", OP_LESS, "OP_LESS" },
    { "<$", OP_PRAGMASTART, "OP_PRAGMASTART" },
    { "<<", OP_SHIFTLEFT, "OP_SHIFTLEFT" },
    { "<=", OP_LESSEQUAL, "OP_LESSEQUAL" },
    { "<>", OP_NOTEQUAL, "OP_NOTEQUAL" },
    { "=", OP_EQUAL, "OP_EQUAL" },
    { ">", OP_GREATER, "OP_GREATER" },
    { ">=", OP_GREATEREQUAL, "OP_GREATEREQUAL" },
    { ">>", OP_SHIFTRIGHT, "OP_SHIFTRIGHT" },
    { ">>>", OP_USHIFTRIGHT, "OP_USHIFTRIGHT" },
};

/* A table of reserved words: symbols that have a special meaning.
   Is terminated by an entry with NULL tmstring.
 */

struct tok rwtab[] =
{
    { "and", KEY_AND, "KEY_AND" },
    { "barrier", KEY_BARRIER, "KEY_BARRIER" },
    { "block", KEY_BLOCK, "KEY_BLOCK" },
    { "boolean", KEY_BOOLEAN, "KEY_BOOLEAN" },
    { "byte", KEY_BYTE, "KEY_BYTE" },
    { "char", KEY_CHAR, "KEY_CHAR" },
    { "complex", KEY_COMPLEX, "KEY_COMPLEX" },
    { "cyclic", KEY_CYCLIC, "KEY_CYCLIC" },
    { "default", KEY_DEFAULT, "KEY_DEFAULT" },
    { "delete", KEY_DELETE, "KEY_DELETE" },
    { "double", KEY_DOUBLE, "KEY_DOUBLE" },
    { "each", KEY_EACH, "KEY_EACH" },
    { "else", KEY_ELSE, "KEY_ELSE" },
    { "external", KEY_EXTERNAL, "KEY_EXTERNAL" },
    { "false", KEY_FALSE, "KEY_FALSE" },
    { "fitroom", KEY_FITROOM, "KEY_FITROOM" },
    { "float", KEY_FLOAT, "KEY_FLOAT" },
    { "for", KEY_FOR, "KEY_FOR" },
    { "forall", KEY_FORALL, "KEY_FORALL" },
    { "foreach", KEY_FOREACH, "KEY_FOREACH" },
    { "forkall", KEY_FORKALL, "KEY_FORKALL" },
    { "function", KEY_FUNCTION, "KEY_FUNCTION" },
    { "garbagecollect", KEY_GARBAGECOLLECT, "KEY_GARBAGECOLLECT" },
    { "getblocksize", KEY_GETBLOCKSIZE, "KEY_GETBLOCKSIZE" },
    { "getroom", KEY_GETROOM, "KEY_GETROOM" },
    { "getsize", KEY_GETSIZE, "KEY_GETSIZE" },
    { "globalpragmas", KEY_GLOBALPRAGMAS, "KEY_GLOBALPRAGMAS" },
    { "goto", KEY_GOTO, "KEY_GOTO" },
    { "if", KEY_IF, "KEY_IF" },
    { "int", KEY_INT, "KEY_INT" },
    { "ismultidim", KEY_ISMULTIDIM, "KEY_ISMULTIDIM" },
    { "isowner", KEY_ISOWNER, "KEY_ISOWNER" },
    { "long", KEY_LONG, "KEY_LONG" },
    { "mod", KEY_MOD, "KEY_MOD" },
    { "new", KEY_NEW, "KEY_NEW" },
    { "not", KEY_NOT, "KEY_NOT" },
    { "null", KEY_NULL, "KEY_NULL" },
    { "of", KEY_OF, "KEY_OF" },
    { "or", KEY_OR, "KEY_OR" },
    { "owner", KEY_OWNER, "KEY_OWNER" },
    { "pointer", KEY_POINTER, "KEY_POINTER" },
    { "print", KEY_PRINT, "KEY_PRINT" },
    { "println", KEY_PRINTLN, "KEY_PRINTLN" },
    { "procedure", KEY_PROCEDURE, "KEY_PROCEDURE" },
    { "receive", KEY_RECEIVE, "KEY_RECEIVE" },
    { "record", KEY_RECORD, "KEY_RECORD" },
    { "return", KEY_RETURN, "KEY_RETURN" },
    { "returns", KEY_RETURNS, "KEY_RETURNS" },
    { "send", KEY_SEND, "KEY_SEND" },
    { "sender", KEY_SENDER, "KEY_SENDER" },
    { "setroom", KEY_SETROOM, "KEY_SETROOM" },
    { "setsize", KEY_SETSIZE, "KEY_SETSIZE" },
    { "shape", KEY_SHAPE, "KEY_SHAPE" },
    { "short", KEY_SHORT, "KEY_SHORT" },
    { "string", KEY_STRING, "KEY_STRING" },
    { "switch", KEY_SWITCH, "KEY_SWITCH" },
    { "to", KEY_TO, "KEY_TO" },
    { "true", KEY_TRUE, "KEY_TRUE" },
    { "view", KEY_VIEW, "KEY_VIEW" },
    { "while", KEY_WHILE, "KEY_WHILE" },
    { "xor", KEY_XOR, "KEY_XOR" },
    { NULL, KEY_NOT, "" }
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
	c = getc( lexfile );
    }
    return c;
}

/* Return a new origin descriptor. */
origin make_origin( void )
{
    return new_origin( add_tmsymbol( lexfilename ), lexlineno );
}

/* Given a symbol, return a new originated symbol. */
origsymbol make_origsymbol( const tmsymbol s )
{
    return new_origsymbol( s, make_origin() );
}

void show_parse_context( FILE *f )
{
    unsigned int ix;
    bool shown = FALSE;

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
        shown = TRUE;
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
        return FALSE;
    }
    *bufp++ = c;
    if( c == '\\' ){
        c = lexgetc();
        if( c == EOF ){
	    *bufp = '\0';
            return TRUE;
        }
	*bufp++ = c;
	if( isdigit( c ) ){
	    c = lexgetc();
	    if( isdigit( c ) ){
		*bufp++ = c;

		c = lexgetc();
		if( isdigit( c ) ){
		    *bufp++ = c;
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
    return TRUE;
}

/* Try to read a string. Return TRUE if this is successful, and set '*s'
 * to point to that string, else return FALSE. A string may contain escape
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
	return FALSE;
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
    return TRUE;
}

/* Try to read a character constant. Return TRUE if this is successful,
 * and set '*s' to point to a string that contains the character constant,
 * else return FALSE. A character constant may contain escape sequences.
 * The '\'' around the string are stripped.
 */
static bool scanchar( tmstring *s )
{
    int c;
    char buf[MAXCHARESCAPE+1];

    c = lexgetc();
    if( c != '\'' ){
	lexungetc( c );
	return FALSE;
    }
    if( !scanescapedchar( buf ) ){
	parserror( "end of file in string" );
	buf[0] = '\0';
    }
    *s = new_tmstring( buf );
    c = lexgetc();
    if( c != '\'' ){
	parserror( "unterminated character constatn" );
    }
    return TRUE;
}

/* Try to read characters from 'lexgetc()' to match one of the tokens
   from the table 'toktab' in the tmstring 'buf'. The token characters
   to match are given by the scan tree 'tree'.  Fill '*tokval' with the
   token value, and '*toknm' with the name of the token. Return TRUE if
   this is successful, else return FALSE.
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
	return FALSE;
    }
    for( tp=tree; tp!=SCTNIL; tp=tp->next ){
	if( tp->sctchar == c ) break;
    }
    if( tp == SCTNIL ){
	lexungetc( c );
	return FALSE;
    }
    buf[0] = c;
    if( scantoken( tp->sub, &buf[1], tokval, toknm ) ){
	return TRUE;
    }
    if( tp->valid ){
	*tokval = tp->tokval;
	*toknm = tp->toknm;
	buf[1] = '\0';
	return TRUE;
    }
    lexungetc( c );
    return FALSE;
}

/* Try to read a symbol in the tmstring 'buf' using lexgetc(). Return TRUE if
   this is successful, else return FALSE.
   A symbol is of the form [a-zA-Z_][a-zA-Z0-9_]*.
 */
static bool scansymbol( char *buf )
{
    int c;

    c = lexgetc();
    if( !isalpha( c ) && c != '_' ){
	lexungetc( c );
	return( FALSE );
    }
    do{
	*buf++ = c;
	c = lexgetc();
    } while( isalnum( c ) || c == '_' );
    *buf = '\0';
    lexungetc( c );
    return TRUE;
}

/* Read an unsigned numerical literal in the string 'yytext' using lexgetc().
 * A sign in front of the numerical literal is allowed, but if it
 * isn't followed by digits, it is rejected.
 */
static bool scan_numerical_literal()
{
    char *p = yytext;
    int c;

    c = lexgetc();
#if 0
    if( c == '+' || c == '-' ){
	int c1 = lexgetc();

	lexungetc( c1 );
	if( !isdigit( c1 ) && c1 != '.' ){
	    lexungetc( c );
	    return FALSE;
	}
    }
    else if( c == '.' ){
#endif
    if( c == '.' ){
	int c1 = lexgetc();

	lexungetc( c1 );
	if( !isdigit( c1 ) ){
	    lexungetc( c );
	    return FALSE;
	}
    }
    else if( !isdigit( c ) ){
	lexungetc( c );
	return FALSE;
    }
    do{
	*p++ = c;
	c = lexgetc();
    } while( isdigit( c ) );
    if( c == '.' ){
	do{
	    *p++ = c;
	    c = lexgetc();
	} while( isdigit( c ) );
    }
    if( c == 'e' || c == 'E' ){
	*p++ = c;
	c = lexgetc();
	if( c == '-' || c == '+' ){
	    *p++ = c;
	    c = lexgetc();
	}
	while( isdigit( c ) ){
	    *p++ = c;
	    c = lexgetc();
	}
    }
    if( strchr( "ilsbfd", tolower( c ) ) != NULL ){
	*p++ = c;
    }
    else {
	lexungetc( c );
    }
    *p = '\0';
    return TRUE;
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
    if( scanchar( &yylval._vnus_char ) ){
	return CHAR_LITERAL;
    }
    if( scanstring( &yylval._vnus_string ) ){
	return STRING_LITERAL;
    }
    if( scansymbol( yytext ) ){
	struct tok *rwp;
	tmstring low;
	char *p;

	low = new_tmstring( yytext );
	p = low;
	while( *p != '\0' ){
	    *p = tolower( *p );
	    p++;
	}
	for( rwp = rwtab; rwp->tokstr != NULL; rwp++ ){
	    if( strcmp( rwp->tokstr, low ) == 0 ){
		lexshow(rwp->tokval,rwp->toknm);
		rfre_tmstring( low );
		return( rwp->tokval );
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
	    case 'i':
		lexshow( INT_LITERAL, "INT_LITERAL" );
		yytext[pos] = '\0';
		yylval._vnus_int = new_tmstring( yytext );
		return INT_LITERAL;

	    case 'l':
		lexshow( LONG_LITERAL, "LONG_LITERAL" );
		yytext[pos] = '\0';
		yylval._vnus_long = new_tmstring( yytext );
		return LONG_LITERAL;

	    case 's':
		lexshow( SHORT_LITERAL, "SHORT_LITERAL" );
		yytext[pos] = '\0';
		yylval._vnus_short = new_tmstring( yytext );
		return SHORT_LITERAL;

	    case 'b':
		lexshow( BYTE_LITERAL, "BYTE_LITERAL" );
		yytext[pos] = '\0';
		yylval._vnus_byte = new_tmstring( yytext );
		return BYTE_LITERAL;

	    case 'f':
		lexshow( FLOAT_LITERAL, "FLOAT_LITERAL" );
		yytext[pos] = '\0';
		yylval._vnus_float = new_tmstring( yytext );
		return FLOAT_LITERAL;

	    case 'd':
		lexshow( DOUBLE_LITERAL, "DOUBLE_LITERAL" );
		yytext[pos] = '\0';
		yylval._vnus_double = new_tmstring( yytext );
		return DOUBLE_LITERAL;

	}
        p = yytext;
        isint = TRUE;
        while( *p != '\0' ){
            if( *p == '.' || *p == 'e' || *p == 'E' ){
		isint = FALSE;
		break;
	    }
            p++;
        }                  
        if( isint ){
            lexshow( INT_LITERAL, "INT_LITERAL" );
	    yylval._vnus_int = new_tmstring( yytext );
            return INT_LITERAL;
        }                     
        lexshow( FLOAT_LITERAL, "FLOAT_LITERAL" );
	yylval._vnus_float = new_tmstring( yytext );
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
