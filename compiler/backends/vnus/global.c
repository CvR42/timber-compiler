/* File: global.c
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

bool trace_struct_register = false;
bool trace_analysis = false;
bool trace_scope_admin = false;
bool dontflattenselection = false;
bool lextr = false;
bool givestat = false;
bool dumptree = false;
bool generateTimingCode = false;
bool longAsClass = false;
unsigned int numberOfProcessors = 0;
PROGTYPE program_type = PROG_SEQUENTIAL;
tmuint_list processor_array;
