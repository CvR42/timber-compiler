.. File: gensupport.t
..
.. Generic support for treewalkers.
.. Provides things like push and pop actions
..
.macro conditional_init_prelst
    if (gc->prelst == NULL)
        gc->prelst = new_statement_list();
.endmacro
..
.macro conditional_init_postlst
    if (gc->postlst == NULL)
        gc->postlst = new_statement_list();
.endmacro
..
.macro push_prelst
    statement_list orig_prelst = gc->prelst;
    gc->prelst = NULL;
.endmacro
..
.macro pop_prelst
    gc->prelst = orig_prelst;
.endmacro
..
.macro push_postlst
    statement_list orig_postlst = gc->postlst;
    gc->postlst = NULL;
.endmacro
..
.macro pop_postlst
    gc->postlst = orig_postlst;
.endmacro
..
.macro init_prelst
    gc->prelst = statement_listNIL;
.endmacro
..
.macro init_postlst
    gc->postlst = statement_listNIL;
.endmacro
..
..
.macro init_scope
    gc->scope = tmsymbolNIL;
.endmacro
..
.macro conditional_init_scope
    if (gc->scope == NULL)
        gc->scope = gen_tmsymbol("__scope_");
.endmacro
..
.macro push_scope
    tmsymbol orig_scope = gc->scope;
    gc->scope = e->scope;
.endmacro
..
.macro conditional_set_scope scope
    if (gc->scope != NULL && $(scope) == NULL)
        $(scope) = gc->scope;
.endmacro
..
.macro pop_scope
    gc->scope = orig_scope;
.endmacro
..
..
.macro push_add_to_prelst
    int orig_add_to_prelst = gc->add_to_prelst;
    gc->add_to_prelst = false;
.endmacro
..
.macro pop_add_to_prelst
    gc->add_to_prelst = orig_add_to_prelst;
.endmacro
..
.macro init_add_to_prelst
    gc->add_to_prelst = false;
.endmacro
..
..
.macro push_do_statements
    int orig_do_statements = gc->do_statements;
    gc->do_statements = false;
.endmacro
..
.macro pop_do_statements
    gc->do_statements = orig_do_statements;
.endmacro
..
.macro init_do_statements
    gc->do_statements = false;
.endmacro
..
..
.macro push_do_transform
    int orig_do_transform = gc->do_transform;
    gc->do_transform = false;
.endmacro
..
.macro pop_do_transform
    gc->do_transform = orig_do_transform;
.endmacro
..
.macro init_do_transform
    gc->do_transform = false;
.endmacro
..
..
.macro push_is_indirection
    int orig_is_indirection = gc->is_indirection;
    gc->is_indirection = false;
.endmacro
..
.macro pop_is_indirection
    gc->is_indirection = orig_is_indirection;
.endmacro
..
.macro init_is_indirection
    gc->is_indirection = false;
.endmacro
..
..
.macro push_current_ass
    SmtAssign orig_current_ass = gc->current_ass;
    gc->current_ass = SmtAssignNIL;
.endmacro
..
.macro pop_current_ass
    gc->current_ass = orig_current_ass;
.endmacro
..
.macro init_current_ass
    gc->current_ass = SmtAssignNIL;
.endmacro
..
..
.macro push_current_owner
    OwnerExpr_list orig_current_owner = gc->current_owner;
    gc->current_owner = OwnerExpr_listNIL;
.endmacro
..
.macro pop_current_owner
    gc->current_owner = orig_current_owner;
.endmacro
..
.macro init_current_owner
    gc->current_owner = OwnerExpr_listNIL;
.endmacro
..
..
.macro push_current_varowner
    VariableOwner orig_current_varowner = gc->current_varowner;
//    gc->current_varowner = VariableOwnerNIL;
.endmacro
..
.macro pop_current_varowner
    gc->current_varowner = orig_current_varowner;
.endmacro
..
.macro init_current_varowner
    gc->current_varowner = VariableOwnerNIL;
.endmacro
..
..
.macro push_current_pragmas
    Pragma_list orig_current_pragmas = gc->current_pragmas;
    gc->current_pragmas = Pragma_listNIL;
.endmacro
..
.macro pop_current_pragmas
    gc->current_pragmas = orig_current_pragmas;
.endmacro
..
.macro init_current_pragmas
    gc->current_pragmas = Pragma_listNIL;
.endmacro
..
..
.macro push_current_parms
    origsymbol_list orig_current_parms = gc->current_parms;
    gc->current_parms = origsymbol_listNIL;
..    origsymbol orig_cbpar = gc->cbpar;
..    gc->cbpar = origsymbolNIL;
.endmacro
..
.macro pop_current_parms
    gc->current_parms = orig_current_parms;
..    gc->cbpar = orig_cbpar;
.endmacro
..
.macro init_current_parms
..    assert (gc->cbptr != NULL);
    gc->current_parms = origsymbol_listNIL;
..    gc->cbpar = gc->cbptr->name;
.endmacro
..
.macro set_current_parms
..    assert(e->parms->sz > 0);
    gc->current_parms = e->parms;
..    gc->cbpar = find_cbpar(gc->current_parms);
.endmacro
..
.macro push_findSym_l
    tmsymbol_list orig_findSym_l = gc->findSym_l;
    gc->findSym_l = tmsymbol_listNIL;
.endmacro
..
.macro pop_findSym_l
    gc->findSym_l = orig_findSym_l;
.endmacro
..
.macro push_replaceExpr_l
    expression_list orig_replaceExpr_l = gc->replaceExpr_l;
    gc->replaceExpr_l = expression_listNIL;
.endmacro
..
.macro pop_replaceExpr_l
    gc->replaceExpr_l = orig_replaceExpr_l;
.endmacro
..
.macro push_replaceLoc_l
    location_list orig_replaceLoc_l = gc->replaceLoc_l;
    gc->replaceLoc_l = location_listNIL;
.endmacro
..
.macro pop_replaceLoc_l
    gc->replaceLoc_l = orig_replaceLoc_l;
.endmacro
..
..
.macro generate_insert_prelst list
        if (gc->prelst != NULL)
        {
            unsigned sz = gc->prelst->sz;
            $(list) = insertlist_statement_list($(list), ix, gc->prelst);
            ix += sz;
            gc->prelst = NULL;
        }
.endmacro
..
.macro generate_insert_postlst list
        if (gc->postlst != NULL)
        {
            unsigned sz = gc->postlst->sz;
            $(list) = insertlist_statement_list($(list), ix+1, gc->postlst);
            ix += sz;
            gc->postlst = NULL;
        }
.endmacro
..
.macro traverse_statement_list_and_insert_prelist_and_postlist list
	for( unsigned ix=0; ix<$(list)->sz; ix++ ){
.call generate_descent_call "        " $(list)->arr[ix] statement statement
.call generate_insert_prelst $(list)
.call generate_insert_postlst $(list)
    }
.endmacro
..
.macro traverse_statement_list_and_insert_prelist list
	for( unsigned ix=0; ix<$(list)->sz; ix++ ){
.call generate_descent_call "        " $(list)->arr[ix] statement statement
.call generate_insert_prelst $(list)
    }
.endmacro
..
..
.. Generate signature for action
.. var : name of formal parameter representing syntax node
.. t   : type of syntax node
.macro generate_action_signature var t
static $t rewrite_$t_action(
  $t $(var), global_context gc )
.endmacro
..
.macro generate_action_signature_no_gc var t
static $t rewrite_$t_action(
  $t $(var), global_context /*gc*/ )
.endmacro
..
..
.. Generate declaration for action
.. var : name of formal parameter representing syntax node
.. t   : type of syntax node
.macro generate_action_declaration var t
static $t rewrite_$t_action(
  $t $(var), global_context gc );
.endmacro
..
..
.macro generate_action_forwards list
/* ----------- Generated forward declarations start here ----------- */
/* Forward action declarations. */
.foreach t $(list)
.call generate_action_declaration e $t
.endforeach
/* ----------- Generated forward declarations end here ----------- */
.endmacro
..
..
.. Generate code that cuts of a treewalk if a certain pragma is present
.macro generate_cutoff_pragma_stmt pragma
    if (pragma_list_contains_tmsymbol(
        e->pragmas,
        add_tmsymbol("$(pragma)"))){
//	fprintf(stderr,"Cutoff because of %s\n","$(pragma)");
        return e;
    }
.endmacro
..
.. Generate an action that aborts the treewalk if a certain pragma is present
.macro generate_cutoff_pragma_action node pragma
.call generate_action_signature e $(node)
{
.call generate_cutoff_pragma_stmt $(pragma)
.call generate_walker_call "    " e $(node) $(node)
    return e;
}
.endmacro
..
.. Generate a node that only administrates the current_parms and cbpar
.. fields in the global_context field
.. Should only be used for node = {DeclFunction,DeclProcedure}
.macro generate_admin_current_parms_action node
.call generate_action_signature e $(node)
{
.call push_current_parms
.call set_current_parms
.call generate_walker_call "    " e $(node) $(node)
.call pop_current_parms
    return e;
}
.endmacro
..
..
.. Generate node that administrates the scope.
.. Should probably only be used for block.
.macro generate_admin_scope_action node
/*
  Save the previous scope and replace with current scope.
  If current scope has no name and a variable is created, then the scope
  gets a name and this name has to be set.
  */
.call generate_action_signature e $(node)
{
    if( e == 0 ){
        return e;
    }
.call push_scope    
.call generate_walker_call "    " e $(node) $(node)
 // If something was done a scope was created, so scope has to be set.
.call conditional_set_scope e->scope
.call pop_scope
    return e;
}
.endmacro
..
..
.. Generate node that administrates the prelst.
.. Should probably only be used for statement_list.
.macro generate_admin_prelst_action node
/*
  Handle the prelst: insert it just before the statement that generated it.
  */
.call generate_action_signature e $(node)
{
    if( e == 0 )
        return e;
.call push_prelst
.call traverse_statement_list_and_insert_prelist e
.call pop_prelst
    return e;
}
.endmacro
..
.macro generate_admin_prelst_and_postlst_action node
/*
  Handle insertion of both prelists as well as postlists.
  */
.call generate_action_signature e $(node)
{
    if( e == 0 )
        return e;
.call push_prelst
.call push_postlst    
.call traverse_statement_list_and_insert_prelist_and_postlist e
.call pop_prelst
.call pop_postlst
    return e;
}
.endmacro
..
.. Generate a dummy action action that prevents a treewalk from going deeper
.macro generate_cutoff_dummy_action node
.call generate_action_signature_no_gc e $(node)
{
    return e;
}
.endmacro
..
