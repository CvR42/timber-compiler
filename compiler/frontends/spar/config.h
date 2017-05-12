/* config.h.  Generated automatically by configure.  */
/* file: config.h
 *
 * Compiler configuration
 */

#ifndef INC_CONFIG_H
#define INC_CONFIG_H

// The paths to the spar library to use.
//
// Note that first the DEVEL_SPARLIB_PATH is used, and then the
// INSTALL_SPARLIB_PATH. In an installed version DEVEL_SPARLIB_PATH
// is not (supposed to be) a valid path.

// The path during development.
#define DEVEL_SPARLIB_PATH "/Users/reeuwijk/timber/compiler/frontends/spar/../sparlib"

// The path after installation
#define INSTALL_SPARLIB_PATH "/Users/reeuwijk/lab/lib/sparlib"

#define HAVE_STRERROR 1

// The class we probe for to check wether the installation environement
// is sane.
#define SANITYCHECK_CLASS "java.lang.Exception"

// Maximal level of recursive inlining. It is used to cut off infinite
// recursion. the INFERRED one is for methods that the compiler decides
// can be inlined.
#define MAX_INLINE_DEPTH 100
#define MAX_INFERRED_INLINE_DEPTH 8

// Maximal code size, in abstract instructions, of a function that is
// declared 'inline' by the compiler.
#define INLINABLE_CODESIZE 30

#endif
