#ifndef __VNUSSTD_H__
#define __VNUSSTD_H__

// CvR: quick fix; the endian.h in /usr/local/pthreads/include gets
// upset about the most recent egcs version, since in that one
// the preprocessor variable 'i386' is no longer defined if the
// -ansi flag is given.
#if defined( __i386__ ) && !defined( i386 )
#define i386 __i386__
#endif

extern void runtime_error( const char *s );
extern void runtime_warning( const char *s );

#define NOT_EXECUTED -666

#endif
