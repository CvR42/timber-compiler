/* Applytp2 test engine

   Do Andrei's stuff.
   Make sure the task and priority arrays are registered with
   the runtime system.
   
   */

#include <assert.h>
#include <tmc.h>

#include "defs.h"
#include "tmadmin.h"
#include "vnusctl.h"
#include "global.h"
#include "error.h"
#include "service.h"
#include "parsepragmas.h"
#include "applytp2.h"
#include "symtable.h"

static void add_proc_decl(global_context gc, origsymbol name)
{
    int pos = declaration_list_find_pos(gc->prog->declarations, name);
    if (pos != -1)
    { 
        rfre_origsymbol(name);
        return;
    }
    size_list sizes = new_size_list();
    sizes = append_size_list(
	sizes,
	new_SizeDontcare(
	    gen_origin()));
    DeclFormalVariable parm = new_DeclFormalVariable(
	gen_origsymbol("l"),
        0,
        Pragma_listNIL,
	rdup_tmsymbol(name->sym),
	new_TypePointer(
	    new_TypeShape(
		sizes,
		new_TypeBase(BT_INT)/*,
				      typeNIL*/)));
    
    origsymbol_list parms = new_origsymbol_list();
    parms = append_origsymbol_list(parms, rdup_origsymbol(parm->name));
    DeclExternalProcedure proc_decl = new_DeclExternalProcedure(
        name,
        0,
        Pragma_listNIL,
        parms);
    gc->prog->declarations = append_declaration_list(gc->prog->declarations, proc_decl);
    gc->prog->declarations = append_declaration_list(gc->prog->declarations, parm);
}

static void add_external_decls(global_context gc)
{
    add_proc_decl(gc, add_origsymbol("register_processor_list"));
    add_proc_decl(gc, add_origsymbol("register_priority_list"));
}

static block add_register_calls(block b, global_context gc)
{
    declaration proc_l = declaration_list_find_pragma(
	gc->prog->declarations,
	add_tmsymbol("task_proc"));
    assert (proc_l != declarationNIL && proc_l->tag == TAGDeclGlobalVariable);
    declaration prio_l = declaration_list_find_pragma(
	gc->prog->declarations,
	add_tmsymbol("task_prio"));
    assert (prio_l != declarationNIL && prio_l->tag == TAGDeclGlobalVariable);
    fprintf(stderr,"TP2: processor and priority lists found\n");
    expression_list proc_par_l = new_expression_list();
    proc_par_l = append_expression_list(
	proc_par_l,
	new_ExprAddress(
	    new_LocName(
		rdup_origsymbol(proc_l->name))));
    expression_list prio_par_l = new_expression_list();
    prio_par_l = append_expression_list(
	prio_par_l,
	new_ExprAddress(
	    new_LocName(
		rdup_origsymbol(prio_l->name))));
    SmtProcedureCall proc_call = snew_SmtProcedureCall(
	new_ExprName(add_origsymbol("register_processor_list")),
	proc_par_l);
    SmtProcedureCall prio_call = snew_SmtProcedureCall(
	new_ExprName(add_origsymbol("register_priority_list")),
	prio_par_l);
    if (b->statements == statement_listNIL)
	b->statements = new_statement_list();
    b->statements = insert_statement_list(b->statements, 0, proc_call);
    b->statements = insert_statement_list(b->statements, 1, prio_call);
    return b;
}


/* Given a vnusprog, rewrite it to the final form for output. */
vnusprog apply_tp2( vnusprog elm, global_context gc )
{
    add_external_decls(gc);
    elm->statements = add_register_calls(elm->statements, gc);
    return elm;
}

