/* File: global.h
 *
 * Header file for global.c.
 */
#ifndef __MAPPER_GLOBAL_H__
#define __MAPPER_GLOBAL_H__

typedef enum en_WARNINGTYPE 
{
    WARN_QUIET,
    WARN_ERROR,
    WARN_WARNING,
    WARN_RULE,
    WARN_DEBUG
} WARNINGTYPE;

extern bool givestat;
//extern bool generateThreadedCode;
extern bool enableSidneyAggregation;
extern bool disableCommunicationAggregation;
extern bool disableReduction;
extern bool disableOwnerAbsorbtion;
extern bool disableConditionLifting;
extern bool enableLeoBypass;
extern bool enableArjanBypass;
extern bool enableOwnerComputes;
extern bool enableSymbolicNumberOfProcessors;
extern WARNINGTYPE warning_level;

#endif
