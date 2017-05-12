/* File: global.h
 *
 * Header file for global.c.
 */
#ifndef __ALIGN_GLOBAL_H__
#define __ALIGN_GLOBAL_H__

typedef enum en_WARNINGTYPE 
{
    WARN_QUIET,
    WARN_ERROR,
    WARN_WARNING,
    WARN_RULE,
    WARN_DEBUG
} WARNINGTYPE;

extern bool givestat;
extern int trans_level;
extern bool enableOwnerComputes;
extern WARNINGTYPE warning_level;
extern bool enableSymbolicNumberOfProcessors;

#endif
