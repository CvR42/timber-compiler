/* file: main.c
 *
 * main(), initialization and termination.
 */

/* standard UNIX libaries */
#include <stdio.h>
#include <ctype.h>
#include <errno.h>

/* the tm support library */
#include <tmc.h>

#include "defs.h"
#include "tmadmin.h"
#include "global.h"
#include "service.h"
#include "vnusctl.h"
#include "engine.h"
#include "applyftf2.h"
#include "applypre1.h"
#include "applypre2.h"
#include "applypre4.h"
#include "applyif2.h"
#include "applytp0.h"
#include "applytp1.h"
#include "applytp2.h"

const char *application_name = "Automap Task Parallellissimo";

vnusprog apply_transformation( vnusprog theProg )
{
    global_context_decl global_context;
    init_global_context(&global_context);
    global_context.prog = theProg;

    theProg = apply_ftf2(theProg, &global_context);
    theProg = apply_pre1(theProg, &global_context);
    theProg = apply_pre2(theProg, &global_context);
    theProg = apply_pre4(theProg, &global_context);
    
    theProg = apply_tp0(theProg, &global_context);
    theProg = apply_tp1(theProg, &global_context);
    theProg = apply_tp2(theProg, &global_context);

    theProg = apply_if2(theProg, &global_context);
    
    destroy_global_context(&global_context);
    return theProg;
}

