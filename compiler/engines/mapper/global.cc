/* File: global.cc
 *
 * Global variables.
 * Obviously, there should be as few global variables as possible.
 * Main candidates are debugging and tracing switches.
 */

#include <stdlib.h>
#include <stdio.h>
#include <tmc.h>

#include "defs.h"
#include "tmadmin.h"
#include "global.h"

bool givestat = false;
//bool generateThreadedCode = false;
bool enableSidneyAggregation = false;
bool disableCommunicationAggregation = false;
bool disableReduction = false;
bool disableOwnerAbsorbtion = false;
bool disableConditionLifting = false;
bool enableLeoBypass = false;
bool enableArjanBypass = false;
bool enableOwnerComputes = false;
bool enableSymbolicNumberOfProcessors = false;
WARNINGTYPE warning_level = WARN_ERROR;
