/* File: error.h */

#ifndef INC_ERROR_H
#define INC_ERROR_H

#include <stdarg.h>

#define ERRARGLEN 800
extern char errpos[];
extern char errarg[];

extern void verror( const char *msg, va_list args );
extern void sys_error( int no );
extern void errcheck( void );
extern void origin_warning( const_origin org, const char *msg, ... );
extern void origin_error( const_origin org, const char *msg, ... );
extern void origsymbol_error( const_origsymbol s, const char *msg, ... );
extern void origsymbol_warning( const_origsymbol s, const char *msg, ... );
extern void internal_error( const char *msg, ... );
extern void origin_internal_error( const_origin org, const char *msg, ... );
extern void origsymbol_internal_error( const_origsymbol s, const char *msg, ... );
extern bool is_compilation_failure();
extern void error( const char *msg, ... );
extern void warning( const char *msg, ... );
extern void complain_modifiers( const_origin org, const modflags flags, const char *where );

#define badtag(t) tm_badtag(__FILE__,__LINE__,t)

#endif
