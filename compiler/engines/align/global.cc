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
int   trans_level = 5;
bool enableOwnerComputes = false;
/*
  Following is here because we need ../mapper/service.cc. sigh:-)
*/
bool enableSymbolicNumberOfProcessors = false;
WARNINGTYPE warning_level = WARN_ERROR;
