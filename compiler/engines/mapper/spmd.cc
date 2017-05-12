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
#include "apply2owned.h"
#include "applybreakout.h"
#include "applyfillowner.h"
#include "applyremrepl.h"
#include "applyresident.h"
#include "applytaskpar.h"
#include "applyif2.h"
#include "applyif5.h"
#include "applyif6.h"
#include "applyif7.h"
#include "applymergeloop.h"
#include "applyit02.h"
#include "applyit1.h"
#include "applyit2.h"
#include "applypcp0.h"
#include "applypcp3.h"
#include "applyred1.h"
#include "applyca1.h"
#include "applyca2.h"
#include "applymas.h"
#include "applyca3.h"
#include "applyot.h"
#include "applyot2.h"
#include "applyopt.h"
#include "applyel1.h"
#include "applyoa5.h"
#include "applyio3.h"
#include "applyclean.h"
#include "applyclean2.h"
#include "applyclean3.h"
#include "applyclean4.h"
#include "applyclean5.h"
#include "applyaggr.h"
#include "applyaggr2.h"
#include "applyliftcond.h"
#include "applymergecond.h"
#include "applyclear_on.h"
#include "applyselect_on.h"

const char *application_name = "Automap SPMD'er";

vnusprog apply_transformation( vnusprog theProg )
{
    global_context_decl global_context;
    init_global_context(&global_context);
    global_context.prog = theProg;
    parse_global_pragmas(&global_context);
    if (!enableOwnerComputes)
    {
	theProg = apply_select_on(theProg, &global_context);
    }
    theProg = apply_clear_on(theProg, &global_context);
    if (enableLeoBypass)
	goto LeoBypass;
    theProg = apply_fillowner(theProg, &global_context);
    theProg = apply_rem_repl(theProg, &global_context);
    theProg = apply_2owned(theProg, &global_context);
    theProg = apply_clean5(theProg, &global_context);
    if (!disableReduction)
    {
	theProg = apply_red1(theProg, &global_context);
    }
    theProg = apply_mergeloop(theProg, &global_context);
    theProg = apply_breakout(theProg, &global_context);
    if (enableArjanBypass)
	goto ArjanBypass;
    theProg = apply_resident(theProg, &global_context);
    theProg = apply_if2(theProg, &global_context);
    theProg = apply_if5(theProg, &global_context); // Probably unused
    theProg = apply_mergeloop(theProg, &global_context);
//    theProg = apply_taskpar(theProg, &global_context); // Temporarily disabled wrt Will
//    theProg = apply_it02(theProg, &global_context); // Old comp.vs.comm
//    theProg = apply_it1(theProg, &global_context); // Old style align
//    theProg = apply_it2(theProg, &global_context); // Old style align
//    theProg = apply_pcp0(theProg, &global_context); // Old style align
//    theProg = apply_pcp3(theProg, &global_context); // Old style align
    if (!disableCommunicationAggregation)
    {
	theProg = apply_ca1(theProg, &global_context);
	theProg = apply_ca2(theProg, &global_context);
//	theProg = apply_mas(theProg, &global_context); // Failed attempt at something smart
	if (enableSidneyAggregation)
	{
		theProg = apply_aggr(theProg, &global_context);
	}
	else
	{
	    theProg = apply_ca3(theProg, &global_context);
	}
    }
    theProg = apply_if6(theProg, &global_context);
//    theProg = apply_if7(theProg, &global_context); // Not necessary anymore
//    theProg = apply_ot(theProg, &global_context); // old style stuff
    theProg = apply_ot2(theProg, &global_context);
    if (!disableConditionLifting)
    {
	do 
	{
	    theProg = apply_liftcond(theProg, &global_context);
	    theProg = apply_clean2(theProg, &global_context);
	    theProg = apply_clean3(theProg, &global_context);
	    theProg = apply_mergecond(theProg, &global_context);
	}
	while (hasFired_liftcond() || hasFired_mergecond());
    }
  LeoBypass:
//    theProg = apply_opt(theProg, &global_context); // old style stuff
    if (!disableOwnerAbsorbtion)
    {
	theProg = apply_el1(theProg, &global_context);
	theProg = apply_oa5(theProg, &global_context);
    }
    if (!disableCommunicationAggregation)
    {
	if (enableSidneyAggregation)
	{
	    theProg = apply_clean2(theProg, &global_context); // hack, duplicated from elsewhere
	    theProg = apply_clean3(theProg, &global_context); // hack, duplicated from elsewhere
	    theProg = apply_mergeloop(theProg, &global_context); // hack, duplicated from elsewhere
	    theProg = apply_aggr2(theProg, &global_context);
	    theProg = apply_if6(theProg, &global_context); // hack, duplicated from elsewhere
	}
    }
    theProg = apply_io3(theProg, &global_context); // New style distributions
    theProg = apply_clean(theProg, &global_context);
    theProg = apply_clean2(theProg, &global_context);
    theProg = apply_clean3(theProg, &global_context);
    theProg = apply_clean4(theProg, &global_context);
    theProg = apply_clean5(theProg, &global_context);
    //  theEnd:
  ArjanBypass:
    destroy_global_context(&global_context);
    return theProg;
}

