/* file: main.c
 *
 * main(), initialization and termination.
 */

/* standard UNIX libaries */
#include <stdio.h>
#include <ctype.h>

/* the tm support library */
#include <tmc.h>

#include "defs.h"
#include "tmadmin.h"
// Put includes for rules here:
#include "global.h"
#include "hoist_on.h"

const char *application_name = "Automap Align'er";

vnusprog apply_transformation( vnusprog theProg )
{
// Put calls to rules here like:
    if (trans_level > 1 && !enableOwnerComputes)
	theProg = apply_hoist_on (theProg);
    return theProg;
}
