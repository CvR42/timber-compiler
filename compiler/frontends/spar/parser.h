/* File: parser.h
 *
 * Headers for globals in parser.y.
 */

extern int yydebug;
extern SparProgramUnit parse( FILE *infile, const_tmstring infilename, bool strict_java );
