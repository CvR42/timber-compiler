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
#include "parsepragmas.h"
#include "parseowners.h"
#include "applydiststrip.h"
#include "applydistcheck.h"
#include "applytag.h"
#include "applycheck.h"
#include "applyftf.h"
#include "applyftf2.h"
#include "applypre1.h"
#include "applypre2.h"
#include "applypre4.h"
#include "applybug.h"
#include "applyftn1.h"
#include "applyopasszap.h"
#include "applyexprredzap.h"
#include "applygetowner.h"

const char *application_name = "Automap Initial'er";

static const bool oldStyle = true;
static const bool newStyle = false;

vnusprog apply_transformation( vnusprog theProg )
{
    global_context_decl global_context;
    init_global_context(&global_context);
    global_context.prog = theProg;
    parse_global_pragmas(&global_context);
    theProg = parse_owners(theProg, &global_context);
    
    theProg = apply_dist_strip(theProg, &global_context);
    theProg = apply_dist_check(theProg, &global_context);
    
    theProg = apply_op_ass_zap(theProg, &global_context);
    theProg = apply_expr_red_zap(theProg, &global_context);
//    theProg = apply_tag(theProg, &global_context);
//    theProg = apply_check(theProg, &global_context);
//    theProg = apply_ftf(theProg, &global_context);
    theProg = apply_ftf2(theProg, &global_context);
    theProg = apply_pre1(theProg, &global_context);
    theProg = apply_pre2(theProg, &global_context);
    theProg = apply_pre4(theProg, &global_context);
//    theProg = apply_bug(theProg, &global_context);
//    theProg = apply_ftn1(theProg, &global_context); // Temporarily disabled wrt Will
    theProg = apply_get_owner(theProg, &global_context);
    destroy_global_context(&global_context);
    return theProg;
}

