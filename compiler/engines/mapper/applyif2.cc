/* Applyif2

   Add external declaration for numberOfProcessors.
   Put forkall loop around statements in global statement block.
   Loop has one iteration for each processor.
   At start of loopbody assign loop iterator to commonblock processor number.
   Last item is not done anymore because common block is disabled.
   */
   
#include <assert.h>
#include <tmc.h>

#include "defs.h"
#include "tmadmin.h"
#include "vnusctl.h"
#include "global.h"
#include "error.h"
#include "service.h"
#include "applyif2.h"

/* ----------- Generated forward declarations end here ----------- */

/* ----------- Actors ----------- */

/* Rewrite action for 'vnusprog':
   */
static vnusprog rewrite_vnusprog_action(
    vnusprog elm, global_context gc )
{
    DeclExternalVariable nopDecl = new_DeclExternalVariable(
        add_origsymbol("numberOfProcessors"),
        0,
        new_Pragma_list(),
        new_TypeBase(BT_INT));
    DeclExternalFunction thisProcDecl = new_DeclExternalFunction(
	add_origsymbol("thisProcessor"),
	0,
	new_Pragma_list(),
	new_origsymbol_list(),
	new_TypeBase(BT_INT));
    DeclCardinalityVariable cDecl = new_DeclCardinalityVariable(
        gen_origsymbol("__procnr"),
        0,
        new_Pragma_list());
    gc->prog->declarations = append_declaration_list(
        gc->prog->declarations, nopDecl);
    gc->prog->declarations = append_declaration_list(
        gc->prog->declarations, thisProcDecl);
    gc->prog->declarations = append_declaration_list(
        gc->prog->declarations, cDecl);

    statement_list nsl = new_statement_list();
    SmtForkall nfstmt = new_SmtForkall(
        0,
        gen_origin(),
        new_Pragma_list(),
        new_OwnerExpr_list(),
        new_cardinality_list(),
        new_OwnerExpr_list(),
        new_block(
            add_tmsymbol("forkall_scope"),
            gen_origin(),
            new_Pragma_list(),
            new_OwnerExpr_list(),
            elm->statements->statements));
    cardinality nc = new_cardinality(
        rdup_origsymbol(cDecl->name),
        new_ExprInt(0),
        new_ExprName(rdup_origsymbol(nopDecl->name)),
        new_ExprInt(1),
        new_secondary_list());
    nfstmt->cards = append_cardinality_list(nfstmt->cards, nc);
    nsl = append_statement_list(nsl, nfstmt);
    elm->statements->statements = nsl;
    
    return elm;
}


/* ----------- Generated code starts here ----------- */
/* ----------- Generated code ends here ----------- */

/* Given a vnusprog, rewrite it to the final form for output. */
vnusprog apply_if2(
    vnusprog elm, global_context gc )
{
    elm = (vnusprog) rewrite_vnusprog_action( elm, gc );
    return elm;
}

