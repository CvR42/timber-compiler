/* File: defs.h */

#ifndef _a_VNUS_DEFS_H_
#define _a_VNUS_DEFS_H_

#include <tmc.h>
#include "vnusbasetypes.h"

#define INDENTSTEP 4

typedef enum en_optbool { UNKNOWN=-1, NO = 0, YES = 1 } optbool;

#define rfre_optbool(e)
#define rdup_optbool(e) (e)

typedef enum en_PROGTYPE {
    PROG_SEQUENTIAL,
    PROG_DISTRIBUTED
} PROGTYPE;

#define PATHSEP "/"

#define badtag(t) tm_badtag(__FILE__,__LINE__,t)

#endif
