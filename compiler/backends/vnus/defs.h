/* File: defs.h */

#ifndef __VNUS_DEFS_H__
#define __VNUS_DEFS_H__

#include <vnusbasetypes.h>

#define INDENTSTEP 4

typedef enum en_PROGTYPE {
    PROG_SEQUENTIAL,
    PROG_DISTRIBUTED
} PROGTYPE;

typedef enum en_aflag {
    AF_NO = 0,
    AF_MAYBE = 1,
    AF_YES = 2
} AFLAG;

#define rfre_AFLAG(e)
#define rdup_AFLAG(e) (e)

#define PATHSEP "/"

#define badtag(t) tm_badtag(__FILE__,__LINE__,t)

#endif
