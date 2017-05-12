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
#include "vnus_types.h"
#include "tmadmin.h"
#include "global.h"

int lextr = FALSE;
int givestat = FALSE;
int goterr = FALSE;
int dumptree = FALSE;
int dumpmangledtree = FALSE;
int showmangling = FALSE;
int tracesymtab = FALSE;
