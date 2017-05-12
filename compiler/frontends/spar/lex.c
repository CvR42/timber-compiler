/* File: lex.c
 * Lexical analyzer for Spar.
 */

#include <ctype.h>
#include <string.h>
#include <tmc.h>
#include <errno.h>
#include <stdarg.h>

#include "defs.h"
#include "tmadmin.h"
#include "error.h"
#include "global.h"
#include "lex.h"
#include "parser-tokens.h"

#if 1
#define lexshow(tok,nm) if(lextr) fprintf(stderr,"token: %s(%d) yytext=\"%s\"\n",nm,tok,yytext)
#else
#define lexshow(tok,nm)
#endif

static size_t yytext_len = 0;
static size_t yytext_pos;
static char *yytext = (char *) NULL;

static char *linebuf = (char *) NULL;
static int linebufsz = 0;
static int linesz = 0;
static int lineix = 0;
static int prevlineix = -1;
static int oldlineix = 0;
static int lexlineno = 0;
static bool at_eof = false;
static tmstring lexfilename = tmstringNIL;
static FILE *lexfile;	/* file to read from. */
static bool in_pragma_state = false;

// The length of an unicode escape string '\u1234'
#define UNICODE_ESCAPE_LENGTH 6

#define EOF_CHAR 0x1A

/******************************************************
 *            SCANNING TREES                          *
 ******************************************************/

/* Scanning trees are used to recogize separator tokens.
   They form a linked list of possible acceptable characters.
   Each node may have a sub-list of possible extensions
   and the token value for the current match.
 */

struct sctnode {
    struct sctnode *next;	/* next possibility in list */
    int sctchar;		/* char to match            */
    struct sctnode *sub;	/* subtree to use on match. */
    bool valid;			/* is acceptable token?     */
    int tokval;			/* token value for yacc     */
    const char *toknm;		/* token name for debugging */
    unsigned int feature;	/* Feature this token belongs to. */
};

#define SCTNIL (struct sctnode *)0

static long newsctnodecnt;
static long fresctnodecnt;

static struct sctnode *toktree;

// Given a number of characters to write, make sure that yytext can
// accomotate at least that many characters.
static void reserve_yytext( size_t n )
{
    if( yytext_pos+n>yytext_len ){
	size_t new_len = 2*(1+yytext_len+n);

	yytext = TM_REALLOC( char *, yytext, (size_t) new_len );
	yytext_len = new_len;
    }
}

// Given a character, write it to yytext.
static void put_yytext( char c )
{
    reserve_yytext( 1 );
    yytext[yytext_pos++] = c;
}

static void clear_yytext()
{
    yytext_pos = 0;
    if( yytext_len == 0 ){
	reserve_yytext( 1 );
    }
    yytext[0] = '\0';
}

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
    nw->feature = 0;
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

/* Add to the scan tree 'tree' a new token with string 'str',
   YACC value 'val' and (debugging) name 'nm'.
   Return a pointer to the modified tree.
 */
static struct sctnode *addtok(
    struct sctnode *tree,
    const char *str,
    int val,
    const char *nm,
    unsigned int feat
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
	tp->feature = feat;
    }
    else{
	tp->sub = addtok( tp->sub, &str[1], val, nm, feat );
    }
    return tree;
}

/******************************************************
 *            TOKEN AND RESERVED WORD TABLES          *
 ******************************************************/

/* A structure to describe tokens and reserved words */
struct tok {
    const char *tokstr;		/* the string to match. */
    int tokval;			/* associated token value for yacc */
    const char *toknm;		/* name for debugging */
    unsigned int feature;	/* In which language extension is it defined. */
};

/* A table of tokens. Is terminated by an entry with empty string. */
static struct tok toktab[] =
{
    { "!", OP_NOT, "OP_NOT", FEAT_JAVA },
    { "!=", OP_NOTEQUAL, "OP_NOTEQUAL", FEAT_JAVA },
    { "$>", OP_PRAGMAEND, "OP_PRAGMAEND", FEAT_PRAGMA },
    { "%", OP_MOD, "OP_MOD", FEAT_JAVA },
    { "%=", OP_ASSIGNMOD, "OP_ASSIGNMOD", FEAT_JAVA },
    { "&", OP_AND, "OP_AND", FEAT_JAVA },
    { "&&", OP_SHORTAND, "OP_SHORTAND", FEAT_JAVA },
    { "&=", OP_ASSIGNAND, "OP_ASSIGNAND", FEAT_JAVA },
    { "(|", OP_GENERIC_OPEN, "OP_GENERIC_OPEN", FEAT_JAVA },
    { "*", OP_TIMES, "OP_TIMES", FEAT_JAVA },
    { "*=", OP_ASSIGNTIMES, "OP_ASSIGNTIMES", FEAT_JAVA },
    { "+", OP_PLUS, "OP_PLUS", FEAT_JAVA },
    { "++", OP_INCREMENT, "OP_INCREMENT", FEAT_JAVA },
    { "+=", OP_ASSIGNPLUS, "OP_ASSIGNPLUS", FEAT_JAVA },
    { "-", OP_MINUS, "OP_MINUS", FEAT_JAVA },
    { "--", OP_DECREMENT, "OP_DECREMENT", FEAT_JAVA },
    { "-=", OP_ASSIGNMINUS, "OP_ASSIGNMINUS", FEAT_JAVA },
    { "->", OP_FLOWTO, "OP_FLOWTO", FEAT_CSP },
    { "/", OP_DIVIDE, "OP_DIVIDE", FEAT_JAVA },
    { "/=", OP_ASSIGNDIVIDE, "OP_ASSIGNDIVIDE", FEAT_JAVA },
    { ":-", OP_ITERATES, "OP_ITERATES", FEAT_CARDS },
    { "<", OP_LESS, "OP_LESS", FEAT_JAVA },
    { "<$", OP_PRAGMASTART, "OP_PRAGMASTART", FEAT_PRAGMA },
    { "<<", OP_SHIFTLEFT, "OP_SHIFTLEFT", FEAT_JAVA },
    { "<<=", OP_ASSIGNSHIFTLEFT, "OP_ASSIGNSHIFTLEFT", FEAT_JAVA },
    { "<=", OP_LESSEQUAL, "OP_LESSEQUAL", FEAT_JAVA },
    { "=", OP_ASSIGNMENT, "OP_ASSIGNMENT", FEAT_JAVA },
    { "==", OP_EQUAL, "OP_EQUAL", FEAT_JAVA },
    { ">", OP_GREATER, "OP_GREATER", FEAT_JAVA },
    { ">=", OP_GREATEREQUAL, "OP_GREATEREQUAL", FEAT_JAVA },
    { ">>", OP_SHIFTRIGHT, "OP_SHIFTRIGHT", FEAT_JAVA },
    { ">>=", OP_ASSIGNSHIFTRIGHT, "OP_ASSIGNSHIFTRIGHT", FEAT_JAVA },
    { ">>>", OP_USHIFTRIGHT, "OP_USHIFTRIGHT", FEAT_JAVA },
    { ">>>=", OP_ASSIGNUSHIFTRIGHT, "OP_ASSIGNUSHIFTRIGHT", FEAT_JAVA },
    { "\\", OP_DIY_INFIX, "OP_DIY_INFIX", FEAT_JAVA },
    { "^", OP_XOR, "OP_XOR", FEAT_JAVA },
    { "^=", OP_ASSIGNXOR, "OP_ASSIGNXOR", FEAT_JAVA },
    { "|", OP_OR, "OP_OR", FEAT_JAVA },
    { "|)", OP_GENERIC_CLOSE, "OP_GENERIC_CLOSE", FEAT_JAVA },
    { "|=", OP_ASSIGNOR, "OP_ASSIGNOR", FEAT_JAVA },
    { "||", OP_SHORTOR, "OP_SHORTOR", FEAT_JAVA },
    { "~", OP_INVERT, "OP_INVERT", FEAT_JAVA },
    { 0, 0, 0, 0 }
};

/* A table of reserved words: symbols that have a special meaning.
 * Is terminated by an entry with NULL string.
 */

static struct tok rwtab[] =
{
    { "__array", KEY_ARRAY, "KEY_ARRAY", FEAT_PRIMARRAY },
    { "__delete", KEY_DELETE, "KEY_DELETE", FEAT_DELETE },
    { "__doublebits", KEY_DOUBLEBITS, "KEY_DOUBLEBITS", FEAT_JAVA },
    { "__floatbits", KEY_FLOATBITS, "KEY_FLOATBITS", FEAT_JAVA },
    { "__getbuf", KEY_GETBUF, "KEY_GETBUF", FEAT_PRIMARRAY },
    { "__print", KEY_PRINT, "KEY_PRINT", FEAT_PRINT },
    { "__println", KEY_PRINTLN, "KEY_PRINTLN", FEAT_PRINT },
    { "__sizeof", KEY_SIZEOF, "KEY_SIZEOF", FEAT_PRIMARRAY },
    { "__string", KEY_STRING, "KEY_STRING", FEAT_STRING },
    { "__wait", KEY_WAIT, "KEY_WAIT", FEAT_CSP },
    { "abstract", KEY_ABSTRACT, "KEY_ABSTRACT", FEAT_JAVA },
    { "assert", KEY_ASSERT, "KEY_ASSERT", FEAT_JAVA },
    { "boolean", KEY_BOOLEAN, "KEY_BOOLEAN", FEAT_JAVA },
    { "break", KEY_BREAK, "KEY_BREAK", FEAT_JAVA },
    { "byte", KEY_BYTE, "KEY_BYTE", FEAT_JAVA },
    { "case", KEY_CASE, "KEY_CASE", FEAT_JAVA },
    { "catch", KEY_CATCH, "KEY_CATCH", FEAT_JAVA },
    { "char", KEY_CHAR, "KEY_CHAR", FEAT_JAVA },
    { "class", KEY_CLASS, "KEY_CLASS", FEAT_JAVA },
    { "complex", KEY_COMPLEX, "KEY_COMPLEX", FEAT_COMPLEX },
    { "const", KEY_CONST, "KEY_CONST", FEAT_JAVA },
    { "continue", KEY_CONTINUE, "KEY_CONTINUE", FEAT_JAVA },
    { "default", KEY_DEFAULT, "KEY_DEFAULT", FEAT_JAVA },
    { "do", KEY_DO, "KEY_DO", FEAT_JAVA },
    { "double", KEY_DOUBLE, "KEY_DOUBLE", FEAT_JAVA },
    { "each", KEY_EACH, "KEY_EACH", FEAT_PARALLEL },
    { "else", KEY_ELSE, "KEY_ELSE", FEAT_JAVA },
    { "extends", KEY_EXTENDS, "KEY_EXTENDS", FEAT_JAVA },
    { "false", KEY_FALSE, "KEY_FALSE", FEAT_JAVA|FEAT_INPRAGMA },
    { "final", KEY_FINAL, "KEY_FINAL", FEAT_JAVA },
    { "finally", KEY_FINALLY, "KEY_FINALLY", FEAT_JAVA },
    { "float", KEY_FLOAT, "KEY_FLOAT", FEAT_JAVA },
    { "for", KEY_FOR, "KEY_FOR", FEAT_JAVA },
    { "foreach", KEY_FOREACH, "KEY_FOREACH", FEAT_PARALLEL },
    { "globalpragmas", KEY_GLOBALPRAGMAS, "KEY_GLOBALPRAGMAS", FEAT_PRAGMA },
    { "goto", KEY_GOTO, "KEY_GOTO", FEAT_JAVA },
    { "if", KEY_IF, "KEY_IF", FEAT_JAVA },
    { "implements", KEY_IMPLEMENTS, "KEY_IMPLEMENTS", FEAT_JAVA },
    { "import", KEY_IMPORT, "KEY_IMPORT", FEAT_JAVA },
    { "inline", KEY_INLINE, "KEY_INLINE", FEAT_INLINE },
    { "instanceof", KEY_INSTANCEOF, "KEY_INSTANCEOF", FEAT_JAVA },
    { "int", KEY_INT, "KEY_INT", FEAT_JAVA },
    { "interface", KEY_INTERFACE, "KEY_INTERFACE", FEAT_JAVA },
    { "long", KEY_LONG, "KEY_LONG", FEAT_JAVA },
    { "native", KEY_NATIVE, "KEY_NATIVE", FEAT_JAVA },
    { "new", KEY_NEW, "KEY_NEW", FEAT_JAVA },
    { "null", KEY_NULL, "KEY_NULL", FEAT_JAVA },
    { "package", KEY_PACKAGE, "KEY_PACKAGE", FEAT_JAVA },
    { "private", KEY_PRIVATE, "KEY_PRIVATE", FEAT_JAVA },
    { "protected", KEY_PROTECTED, "KEY_PROTECTED", FEAT_JAVA },
    { "public", KEY_PUBLIC, "KEY_PUBLIC", FEAT_JAVA },
    { "return", KEY_RETURN, "KEY_RETURN", FEAT_JAVA },
    { "short", KEY_SHORT, "KEY_SHORT", FEAT_JAVA },
    { "static", KEY_STATIC, "KEY_STATIC", FEAT_JAVA },
    { "strictfp", KEY_STRICTFP, "KEY_STRICTFP", FEAT_JAVA },
    { "super", KEY_SUPER, "KEY_SUPER", FEAT_JAVA },
    { "switch", KEY_SWITCH, "KEY_SWITCH", FEAT_JAVA },
    { "synchronized", KEY_SYNCHRONIZED, "KEY_SYNCHRONIZED", FEAT_JAVA },
    { "this", KEY_THIS, "KEY_THIS", FEAT_JAVA },
    { "throw", KEY_THROW, "KEY_THROW", FEAT_JAVA },
    { "throws", KEY_THROWS, "KEY_THROWS", FEAT_JAVA },
    { "timeout", KEY_TIMEOUT, "KEY_TIMEOU", FEAT_CSP },
    { "transient", KEY_TRANSIENT, "KEY_TRANSIENT", FEAT_JAVA },
    { "true", KEY_TRUE, "KEY_TRUE", FEAT_JAVA|FEAT_INPRAGMA },
    { "try", KEY_TRY, "KEY_TRY", FEAT_JAVA },
    { "type", KEY_TYPE, "KEY_TYPE", FEAT_INLINE },
    { "void", KEY_VOID, "KEY_VOID", FEAT_JAVA },
    { "volatile", KEY_VOLATILE, "KEY_VOLATILE", FEAT_JAVA },
    { "while", KEY_WHILE, "KEY_WHILE", FEAT_JAVA },
    { (char *) NULL, KEY_IF, "", 0 }
};


/******************************************************
 *            Parser states                           *
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
   Enlarge the queue if necessary.
 */
static void lexungetc( int c )
{
    if( c == '\n' || c == '\r' ){
	lexlineno--;
    }
    if( prevlineix>=0 ){
	lineix = prevlineix;
	prevlineix = -1;
	return;
    }
    if( ungetbufix >= ungetbuflen ){
	ungetbuflen+=UNGETBUFLENSTEP;
	ungetbuf = TM_REALLOC( int *, ungetbuf, ungetbuflen*sizeof(int) );
    }
    ungetbuf[ungetbufix++] = c;
}

// Given a character return its value interpreted as a hex character.
static int hex_char_val( char c )
{
    if( !isxdigit( c ) ){
	parserror( "illegal unicode sequence" );
	return 0;
    }
    if( c<='9' ){
	return (int) (c-'0');
    }
    if( c>='a' && c<='f' ){
	return (int) (10+c-'a');
    }
    return (int) (10+c-'A');
}

// Fill the line buffer with the next line.
static void lexfillbuf()
{
    int ix = 0;
    for(;;){
	int c = fgetc( lexfile );
	if( c == EOF ){
	    if( ix>0 && linebuf[ix-1] == EOF_CHAR ){
		// Ignore an EOF_CHAR at the end of the file.
		ix--;
	    }
	    at_eof = true;
	    break;
	}
	if( ix>=linebufsz ){
	    int new_linebufsz = 2*(linebufsz+2);
	    linebuf = TM_REALLOC( char *, linebuf, (size_t) new_linebufsz );
	    linebufsz = new_linebufsz;
	}
	linebuf[ix++] = (char) c;
	if( c == '\n' || c == '\r' ){
	    break;
	}
    }
    linesz = ix;
}

// Get a character from input stream or pushback queue.
// The input is supposed to be unicode, but for the moment we mostly
// treat it as an ascii stream. The only concession we make is that
// we translate unicode characters with values below 256 to their
// ASCII equivalent.
static int lexgetc( void )
{
    if( ungetbufix != 0 ){
	return( ungetbuf[--ungetbufix] );
    }
    if( lineix>=linesz ){
	if( at_eof ){
	    return EOF;
	}
	lexfillbuf();
        lineix = 0;
        oldlineix = 0;
        prevlineix = -1;
    }
    prevlineix = lineix;
    // Translate unicode escapes below 256 to their ascii equivalent.
    // First, see if there is a match.
    if( linebuf[lineix] == '\\' && linebuf[lineix+1] == 'u' ){
	int pos = lineix+2;

	while( linebuf[pos] == 'u' ){
	    pos++;
	}
	if( linebuf[pos] == '0' && linebuf[pos+1] == '0' ){
	    // This is a unicode we can translate.
	    lineix = pos+4;
	    int c = 16*hex_char_val( linebuf[pos+2] )+hex_char_val( linebuf[pos+3] );
	    if( c == EOF_CHAR && lineix>=linesz && at_eof ){
		// Ignore EOF_CHAR at the end of the file.
		return EOF;
	    }
	    return c;
	}
    }
    if( linebuf[lineix] == '\n' || linebuf[lineix] == '\r' ){
	lexlineno++;
    }
    return linebuf[lineix++];
}

/* Return a new origin descriptor. */
origin make_origin( void )
{
    origin org = new_origin( add_tmsymbol( lexfilename ), lexlineno );
    return org;
}

/* Given a symbol, return a new originated symbol. */
origsymbol make_origsymbol( const tmsymbol s )
{
    return new_origsymbol( s, make_origin() );
}

static void show_parse_context( FILE *f )
{
    int ix;
    bool shown = false;
    bool did_newline = false;

    for( ix=0; ix<linesz; ix++ ){
	fputc( linebuf[ix], f );
	if( linebuf[ix] == '\n' ){
	    did_newline = true;
	}
    }
    if( !did_newline ){
	fputc( '\n', f );
    }
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
    fputc( '\n', f );
}

void parserror( const char *message, ... )
{
    va_list args;

    va_start( args, message );
    sprintf( errpos, "%s:%d", lexfilename, lexlineno );
    verror( message, args );
    show_parse_context( stderr );
}

void parsewarning( const char *message )
{
    sprintf( errpos, "%s:%d", lexfilename, lexlineno );
    warning( message );
    show_parse_context( stderr );
}

/* Given a string, see if it is an octal constant, and if so,
 * make sure that it only contains valid octal digits.
 */
static void check_octal_digits( const char *p )
{
    if( *p != '0' ){
	return;
    }
    while( isdigit( *p ) ){
	if( *p == '8' || *p == '9' ){
	    parserror( "invalid octal digit" );
	    return;
	}
	p++;
    }
}

/* Current maximum is '\u1234'. */
#define MAXCHARESCAPE 6

/* Try to read a (possibly escaped) character from the file 'f'. Give
 * an error message if this is not successful. The string of characters
 * forming the escaped character are put into 'buf'.
 *
 * Since at most MAXCHARESCAPE characters fit in an escape sequence,
 * a fixed buffer of at least that size *plus one* is assumed.
 */
static bool scanescapedchar( unsigned int *valp )
{
    int c;
    unsigned int val;

    c = lexgetc();
    if( c == EOF ){
        return false;
    }
    if( c == '\\' ){
        c = lexgetc();
	switch( c ){
	    case EOF:
		parserror( "End of file in string" );
		return false;

	    case EOF_CHAR:
		val = (unsigned int) c;
		break;

	    case 'b':
		val = 0x08;
		break;

	    case 't':
		val = 0x09;
		break;

	    case 'n':
		val = 0x0a;
		break;

	    case 'f':
		val = 0x0c;
		break;

	    case 'r':
		val = 0x0d;
		break;

	    case '"':
		val = '"';
		break;

	    case '\'':
		val = '\'';
		break;

	    case '\\':
		val = '\\';
		break;

	    case '0':
	    case '1':
	    case '2':
	    case '3':
	    case '4':
	    case '5':
	    case '6':
	    case '7':
		{
		    val = (unsigned int) c-'0';
		    c = lexgetc();
		    if( isdigit( c ) ){
			val = 8*val+(c-'0');

			if( val<=037 ){
			    c = lexgetc();
			    if( isdigit( c ) ){
				val = 8*val+(c-'0');
			    }
			    else {
				lexungetc( c );
			    }
			}
		    }
		    else {
			lexungetc( c );
		    }
		    break;
		}

	    case 'u':
	    {
		val = 0;

		while( c == 'u' ){
		    c = lexgetc();
		}
		lexungetc( c );
		for( int n=0; n<4; n++ ){
		    c = lexgetc();
		    if( !isxdigit( c ) ){
			lexungetc( c );
			parserror( "illegal unicode sequence" );
			break;
		    }
		    val *= 16;
		    c = toupper( c );
		    if( c<='9' ){
			val += (unsigned long) (c-'0');
		    }
		    else {
			val += (unsigned long) (10+c-'A');
		    }
		}
		break;
	    }

	    default:
		parserror( "Illegal character escape sequence" );
		val = '?';
		break;
	}
    }
    else {
	if( c == '\r' || c == '\n' ){
	    parserror( "end of line in string" );
	}
	val = (unsigned int) c;
    }
    *valp = val;
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

    c = lexgetc();
    if( c != '"' ){
	lexungetc( c );
	return false;
    }
    ix = 0u;
    sz = 5u;	/* Initial guess for a reasonable string size. */
    bufp = create_tmstring( sz );
    bufp[0] = '\0';
    for(;;){
	unsigned int val;

	c = lexgetc();
	if( c == '"' ){
	    break;
	}
	if( c == EOF ){
	    parserror( "end of file in string" );
	    break;
	}
	lexungetc( c );
	if( !scanescapedchar( &val ) ){
	    parserror( "end of file in string" );
	    break;
	}
	// Upper bound for the room we will need now.
	size_t csz = UNICODE_ESCAPE_LENGTH;
	if( ix+csz+1>sz ){
	    // We must resize the string.
	    sz += 1+csz+sz;
	    bufp = realloc_tmstring( bufp, sz );
	}
	if( val>=20 && val<=255 ){
	    // Printable character.
	    bufp[ix++] = (char) val;
	} 
	else if( val == '\t' ){
	    bufp[ix++] = '\\';
	    bufp[ix++] = 't';
	}
	else if( val == '\r' ){
	    bufp[ix++] = '\\';
	    bufp[ix++] = 'r';
	}
	else if( val == '\n' ){
	    bufp[ix++] = '\\';
	    bufp[ix++] = 'n';
	}
	else {
	    // We cannot store this directly in the string, so store
	    // it as an escape sequence.
	    sprintf( bufp+ix, "\\u%04x", (int) val );
	    ix += UNICODE_ESCAPE_LENGTH;
	}
    }
    bufp[ix] = '\0';
    *s = bufp;
    return true;
}

/* Try to read a character constant. Return true if this is successful,
 * and set '*s' to point to a string that contains the character constant,
 * else return false. A character constant may contain escape sequences.
 * The '\'' around the string are stripped.
 */
static bool scanchar( vnus_char *cp )
{
    int c;
    char buf[MAXCHARESCAPE+1];

    c = lexgetc();
    if( c != '\'' ){
	lexungetc( c );
	return false;
    }
    char *bufp = buf;

    c = lexgetc();
    if( c == EOF ){
	parserror( "end of file in character literal" );
	*cp = '?';
        return true;
    }
    if( c != '\\' ){
	if( c == '\'' ){
	    parserror( "single quote character in character literal should be escaped" );
	}
	if( c == '\r' || c == '\n' ){
	    parserror( "line terminator in character literal" );
	}
	// This is just an ordinary quoted character.
	*cp = (vnus_char) c;
	c = lexgetc();
	if( c != '\'' ){
	    parserror( "unterminated character constant" );
	}
	return true;
    }
    c = lexgetc();
    // We now have found an escape sequence, so decode it according to
    // JLS2 3.10.6.
    switch( c ){
	case EOF:
	case EOF_CHAR:
	    parserror( "End of file in escaped character constant" );
	    *cp = '?';
	    break;

	case 'b':
	    *cp = 0x08;
	    break;

	case 't':
	    *cp = 0x09;
	    break;

	case 'n':
	    *cp = 0x0a;
	    break;

	case 'f':
	    *cp = 0x0c;
	    break;

	case 'r':
	    *cp = 0x0d;
	    break;

	case '"':
	    *cp = '"';
	    break;

	case '\'':
	    *cp = '\'';
	    break;

	case '\\':
	    *cp = '\\';
	    break;

	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	    {
		vnus_char val = c-'0';
		c = lexgetc();
		if( isdigit( c ) ){
		    val = 8*val+(c-'0');

		    c = lexgetc();
		    if( isdigit( c ) ){
			if( val>037 ){
			    parserror( "Octal escape value too large for a character" );
			}
			val = 8*val+(c-'0');
		    }
		    else
			lexungetc( c );
		}
		else {
		    lexungetc( c );
		}
		*cp = val;
		break;
	    }

	case 'u':
	{
	    vnus_char val = 0;
	    *bufp++ = (char) c;
	    while( c == 'u' ){
		c = lexgetc();
	    }
	    lexungetc( c );
	    for( int n=0; n<4; n++ ){
		c = lexgetc();
		if( !isxdigit( c ) ){
		    lexungetc( c );
		    parserror( "illegal unicode sequence" );
		    *cp = val;
		    return true;
		}
		val *= 16;
		c = toupper( c );
		if( c<='9' ){
		    val += (vnus_char) (c-'0');
		}
		else {
		    val += (vnus_char) (10+c-'A');
		}
	    }
	    *cp = val;
	    break;
	}

	default:
	    parserror( "Illegal character escape sequence" );
	    *cp = '?';
	    return true;
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
    put_yytext( (char) c );
    if( scantoken( tp->sub, tokval, toknm ) ){
	return true;
    }
    if( tp->valid ){
	if( tp->feature != 0 && (tp->feature & features) == 0 ){
	    return false;
	}
	*tokval = tp->tokval;
	*toknm = tp->toknm;
	put_yytext( '\0' );
	return true;
    }
    lexungetc( c );
    return false;
}

/* Try to read a symbol in the string 'buf' using lexgetc(). Return true if
   this is successful, else return false.
   A symbol is of the form [a-zA-Z_][a-zA-Z0-9_]*.
 */
static bool scansymbol()
{
    int c;

    clear_yytext();
    c = lexgetc();
    if( !isalpha( c ) && c != '_' ){
	lexungetc( c );
	return false;
    }
    do{
	put_yytext( (char) c );
	c = lexgetc();
    } while( isalnum( c ) || c == '_' );
    put_yytext( '\0' );
    lexungetc( c );
    return true;
}

/* Read an unsigned hexadecimal integer literal in the string 'yytext' using
 * lexgetc(). yytext will contain the decimal equivalent of the number.
 */
static bool scan_hex_literal()
{
    int c = lexgetc();
    clear_yytext();
    if( c != '0' ){
	lexungetc( c );
	return false;
    }
    put_yytext( '0' );
    c = lexgetc();
    if( tolower( c ) != 'x' ){
	lexungetc( c );
	lexungetc( '0' );
	return false;
    }
    int c1 = lexgetc();
    if( !isxdigit( c1 ) ){
	lexungetc( c1 );
	lexungetc( c );
	lexungetc( '0' );
	return false;
    }
    put_yytext( (char) c );
    lexungetc( c1 );
    // At this point we are committed to a hexadecimal literal.
    for(;;){
	c = lexgetc();
	if( !isxdigit( c ) ){
	    break;
	}
	put_yytext( (char) c );
    }
    if( tolower( c ) == 'l' ){
	put_yytext( (char) c );
    }
    else {
	lexungetc( c );
    }
    put_yytext( '\0' );
    return true;
}

/* Read an unsigned numerical literal in the string 'yytext' using lexgetc().
 */
static bool scan_numerical_literal()
{
    int c;

    clear_yytext();
    c = lexgetc();
    if( c == '.' ){
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
	put_yytext( (char) c );
	c = lexgetc();
    } while( isdigit( c ) );
    if( c == '.' ){
	do{
	    put_yytext( (char) c );
	    c = lexgetc();
	} while( isdigit( c ) );
    }
    if( c == 'e' || c == 'E' ){
	put_yytext( (char) c );
	c = lexgetc();
	if( c == '-' || c == '+' ){
	    put_yytext( (char) c );
	    c = lexgetc();
	}
	while( isdigit( c ) ){
	    put_yytext( (char) c );
	    c = lexgetc();
	}
    }
    const char *suffixes;
    if( features & FEAT_COMPLEX ){
	suffixes = "lfdi";
    }
    else {
	suffixes = "lfd";
    }
    if( strchr( suffixes, tolower( c ) ) != NULL ){
	put_yytext( (char) c );
    }
    else {
	lexungetc( c );
    }
    put_yytext( '\0' );
    return true;
}

/* "//" encountered, skip characters until end of line. */
static void skiplinecomment()
{
    int c = lexgetc();

    while( c != '\n' && c != '\r' ){
	if( c == EOF ){
	    parserror( "line comment should be terminated with new line" );
	    return;
	}
	c = lexgetc();
    }
}

/* '/' and '*' encountered, skip characters until end of comment. */
static void skipcomment( void )                        
{
    int c = lexgetc();                 

    do {
	while( c != '*' ){
	    c = lexgetc();
	    if( c == EOF ){
		parserror( "unterminated /* comment at end of file" );
		return;
	    }
	}
	c = lexgetc();
    } while( c != '/' );
}

// Given a string that is known to represent a floating point, make
// sure it is in fact a valid floating point.
static void check_float_format( const char *s )
{
    const char *p = s;

    if( *p == '-' || *p == '+' ){
	p++;
	if( *p == '-' || *p == '+' ){
	    parserror( "floating point literal has than one sign" );
	    p++;
	}
    }
    while( isdigit( *p ) ){
	p++;
    }
    if( *p == '.' ){
	p++;
	// the number has a fractional part. Eat all digits.
	while( isdigit( *p ) ){
	    p++;
	}
    }
    if( tolower( *p ) == 'e' ){
	p++;
	// There is an exponent.
	if( *p == '-' || *p == '+' ){
	    p++;
	}
	if( !isdigit( *p ) ){
	    parserror( "malformed exponent" );
	}
	// Eat all digits of the exponent.
	while( isdigit( *p ) ){
	    p++;
	}
    }
    if( *p != '\0' ){
	parserror( "unexplained characters after floating point literal: '%s'", p );
    }
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
	c = '/';
    }
    // Vertical tab is not whitespace according to Java.
    if( isspace( c ) && c != '\v' ){
	goto again;
    }
    if( c == EOF ){
	clear_yytext();
	oldlineix = lineix;
	lexshow( EOF, "EOF" );
	return 0;
    }
    lexungetc( c );
    oldlineix = lineix;
    if( scanchar( &yylval._vnus_char ) ){
	if( lextr ){
	    if( isprint( yylval._vnus_char ) ){
		fprintf( stderr, "CHAR_LITERAL 0x%04x ('%c')\n", yylval._vnus_char, (char) yylval._vnus_char );
	    }
	    else {
		fprintf( stderr, "CHAR_LITERAL 0x%04x\n", yylval._vnus_char );
	    }
	}
	return CHAR_LITERAL;
    }
    if( scanstring( &yylval._vnus_string ) ){
	if( lextr ){
	    fputs( "STRING_LITERAL\n", stderr );
	}
	return STRING_LITERAL;
    }
    if( scansymbol() ){
	struct tok *rwp;

	for( rwp = rwtab; rwp->tokstr != NULL; rwp++ ){
	    if(
		strcmp( rwp->tokstr, yytext ) == 0 &&
		(!in_pragma_state || (rwp->feature & FEAT_INPRAGMA)) &&
		( (rwp->feature & features) != 0u )
	    ){
		lexshow( rwp->tokval, rwp->toknm );
		return( rwp->tokval );
	    }
	}
	yylval._identifier = add_tmsymbol( yytext );
	lexshow( IDENTIFIER, "IDENTIFIER" );
	return IDENTIFIER;
    }
    if( scan_hex_literal() ){
	size_t pos = strlen( yytext )-1;
	const char *errmsg = (const char *) NULL;
	if( tolower( yytext[pos] ) == 'l' ){
	    vnus_long val = string_to_vnus_long( yytext, &errmsg );
	    if( errmsg != NULL ){
		parserror( errmsg );
	    }
	    if( errno == ERANGE ){
		parserror( "Numeric overflow" );
	    }
	    lexshow( LONG_LITERAL, "LONG_LITERAL" );
	    yylval._vnus_long = val;
	    return LONG_LITERAL;
	}
	else {
	    vnus_long val = string_to_vnus_int( yytext, &errmsg );
	    if( errno == ERANGE ){
		parserror( "Numeric overflow" );
	    }
	    if( errmsg != NULL ){
		parserror( errmsg );
	    }
            lexshow( INT_LITERAL, "INT_LITERAL" );
	    yylval._vnus_int = (vnus_int) val;
            return INT_LITERAL;
	}
    }
    if( scan_numerical_literal() ){
        const char *p;                                                  
	const char *errmsg = (const char *) NULL;
        int isint;
	size_t pos;

	/* This assumes yytext is at least 1 long. Given the context,
	 * this is a valid assumption.
	 */
	pos = strlen( yytext )-1;
	switch( tolower( yytext[pos] ) ){
	    case 'l':
	    {
		vnus_long val = string_to_vnus_long( yytext, &errmsg );

		if( errmsg != NULL ){
		    parserror( errmsg );
		}
		lexshow( LONG_LITERAL, "LONG_LITERAL" );
		yytext[pos] = '\0';
		check_octal_digits( yytext );
		yylval._vnus_long = val;
		return LONG_LITERAL;
	    }

	    case 'f':
		lexshow( FLOAT_LITERAL, "FLOAT_LITERAL" );
		yytext[pos] = '\0';
		check_float_format( yytext );
		yylval._vnus_float = string_to_vnus_float( yytext );
		return FLOAT_LITERAL;

	    case 'd':
		lexshow( DOUBLE_LITERAL, "DOUBLE_LITERAL" );
		yytext[pos] = '\0';
		check_float_format( yytext );
		yylval._vnus_double = string_to_vnus_double( yytext );
		return DOUBLE_LITERAL;

	    case 'i':
		lexshow( IMAGINARY_LITERAL, "IMAGINARY_LITERAL" );
		yytext[pos] = '\0';
		check_float_format( yytext );
		yylval._vnus_double = string_to_vnus_double( yytext );
		return IMAGINARY_LITERAL;

	    default:
		break;

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
	    vnus_int val = string_to_vnus_int( yytext, &errmsg );
	    if( errmsg != NULL ){
		parserror( errmsg );
	    }
            lexshow( INT_LITERAL, "INT_LITERAL" );
	    check_octal_digits( yytext );
	    yylval._vnus_int = val;
            return INT_LITERAL;
        }                     
	check_float_format( yytext );
        lexshow( DOUBLE_LITERAL, "DOUBLE_LITERAL" );
	yylval._vnus_double = string_to_vnus_double( yytext );
        return DOUBLE_LITERAL;
    }
    clear_yytext();
    if( scantoken( toktree, &tokval, &toknm ) ){
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

    newsctnodecnt = 0;
    fresctnodecnt = 0;
    ungetbuflen = 2;	/* Don't make this 0, some mallocs don't like it */
    ungetbuf = TM_MALLOC( int *, ungetbuflen*sizeof(int) );
    ungetbufix = 0;

    // Just an initial guess at the required buffer size.
    size_t new_linebufsz = 100;
    linebuf = TM_REALLOC( char *, linebuf, new_linebufsz );
    linebufsz = (int) new_linebufsz;

    toktree = SCTNIL;
    for( ttp = toktab; ttp->tokstr != NULL; ttp++ ){
	toktree = addtok(
	    toktree,
	    ttp->tokstr,
	    ttp->tokval,
	    ttp->toknm,
	    ttp->feature
	);
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
    linesz = 0;
    lexlineno = 1;          
    at_eof = false;
}

/* Terminate lexical analysis routines. Free all allocated memory */
void end_lex( void )
{
    rfre_sctnode( toktree );
    toktree = SCTNIL;
    TM_FREE( ungetbuf );
    ungetbuflen = 0;
    ungetbufix = 0;
    ungetbuf = (int *) NULL;

    TM_FREE( linebuf );
    linebufsz = 0;
    linebuf = (char *) NULL;

    if( lexfilename != tmstringNIL ){
        rfre_tmstring( lexfilename );
    }
    lexfilename = tmstringNIL;
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
