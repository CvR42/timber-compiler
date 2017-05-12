/* File: parser.h
 *
 * Headers for globals in parser.y.
 */

extern int yydebug;	/* Flag for parser tracing. */
extern vnusprog parse( FILE *infile, tmstring infilename );
