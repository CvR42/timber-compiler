/* File: collect.c
 *
 * Routines that collect names from various parts of the vnus parse tree.
 */

#include <stdio.h>
#include <tmc.h>
#include <assert.h>

#include "defs.h"
#include "vnus_types.h"
#include "tmadmin.h"
#include "error.h"
#include "symbol_table.h"
#include "global.h"
#include "collect.h"

/* Given an formalParameter list, return the list of formalParameter names occuring
 * in that list.
 */
origsymbol_list collect_formalParameter_names( origsymbol_list ans, formalParameter_list l )
{
    unsigned int ix;
    origsymbol s;

    ans = setroom_origsymbol_list( ans, ans->sz+l->sz );
    for( ix=0; ix<l->sz; ix++ ){
	s = rdup_origsymbol( l->arr[ix]->name );
	ans = append_origsymbol_list( ans, s );
	assert( s->sym != tmsymbolNIL );
    }
    return ans;
}

/* Given a declaration, return the symbol that is declared here. */
static origsymbol get_decl_symbol( declaration d )
{
    origsymbol ans;

    ans = origsymbolNIL;
    switch( d->tag ){
	case TAGDeclFunction:
	    ans = rdup_origsymbol( to_DeclFunction(d)->name );
	    break;

	case TAGDeclProcedure:
	    ans = rdup_origsymbol( to_DeclProcedure(d)->name );
	    break;

        case TAGDeclVariable:
	    ans = rdup_origsymbol( to_DeclVariable(d)->name );
	    break;

	case TAGDeclExternalFunction:
	    ans = rdup_origsymbol( to_DeclExternalFunction(d)->name );
	    break;

	case TAGDeclExternalProcedure:
	    ans = rdup_origsymbol( to_DeclExternalProcedure(d)->name );
	    break;

        case TAGDeclExternalVariable:
	    ans = rdup_origsymbol( to_DeclExternalVariable(d)->name );
	    break;

    }
    return ans;
}

/* Given a list of declarations, collect the declared symbols. */
origsymbol_list collect_declaration_names( origsymbol_list nms, declaration_list l )
{
    unsigned int ix;
    origsymbol s;

    nms = setroom_origsymbol_list( nms, nms->sz+l->sz );
    for( ix=0; ix<l->sz; ix++ ){
	s = get_decl_symbol( l->arr[ix] );
	if( s != origsymbolNIL ){
	    nms = append_origsymbol_list( nms, s );
	}
    }
    return nms;
}

/* Given a cardinality list, return the symbols that are used in it. */
origsymbol_list collect_cardinality_symbols( cardinality_list l )
{
    origsymbol_list ans;
    unsigned int ix;
    origsymbol s;

    ans = new_origsymbol_list();
    ans = setroom_origsymbol_list( ans, l->sz );
    for( ix=0; ix<l->sz; ix++ ){
	s = rdup_origsymbol( l->arr[ix]->name );
	ans = append_origsymbol_list( ans, s );
	assert( s->sym != tmsymbolNIL );
    }
    return ans;
}

/* Given a Vnus program, return a list of all global symbols. */
void collect_global_symbols(
 origsymbol_list *globals,
 origsymbol_list *externals,
 vnusprog prog
)
{
    unsigned int ix;
    declaration_list l;
    declaration d;

    l = prog->decls;
    *globals = new_origsymbol_list();
    *externals = new_origsymbol_list();
    *globals = setroom_origsymbol_list( *globals, l->sz );
    *externals = setroom_origsymbol_list( *externals, l->sz );
    for( ix=0; ix<l->sz; ix++ ){
	d = l->arr[ix];
	switch( d->tag ){
	    case TAGDeclFunction:
		*globals = append_origsymbol_list(
		    *globals,
		    rdup_origsymbol( to_DeclFunction(d)->name )
		);
		break;

	    case TAGDeclProcedure:
		*globals = append_origsymbol_list(
		    *globals,
		    rdup_origsymbol( to_DeclProcedure(d)->name )
		);
		break;

	    case TAGDeclVariable:
		*globals = append_origsymbol_list(
		    *globals,
		    rdup_origsymbol( to_DeclVariable(d)->name )
		);
		break;

	    case TAGDeclExternalFunction:
		*externals = append_origsymbol_list(
		    *externals,
		    rdup_origsymbol( to_DeclExternalFunction(d)->name )
		);
		break;

	    case TAGDeclExternalProcedure:
		*externals = append_origsymbol_list(
		    *externals,
		    rdup_origsymbol( to_DeclExternalProcedure(d)->name )
		);
		break;

	    case TAGDeclExternalVariable:
		*externals = append_origsymbol_list(
		    *externals,
		    rdup_origsymbol( to_DeclExternalVariable(d)->name )
		);
		break;

	}
    }
}

/* Given a function definition, return all the local symbols in
 * that definition. We assume the declarations are correct.
 */
origsymbol_list collect_function_names(
 formalParameter_list args,
 origsymbol retval,
 declaration_list ldecl
)
{
    origsymbol_list locals;

    locals = new_origsymbol_list();
    if( args != formalParameter_listNIL ){
	locals = collect_formalParameter_names( locals, args );
    }
    if( ldecl != declaration_listNIL ){
	locals = collect_declaration_names( locals, ldecl );
    }
    if( retval != origsymbolNIL ){
	locals = append_origsymbol_list( locals, rdup_origsymbol( retval  ) );
    }
    return locals;
}

/* Given a function definition, return all the local symbols in
 * that definition. We assume the declarations are correct.
 */
origsymbol_list collect_procedure_names(
 formalParameter_list args,
 declaration_list ldecl
)
{
    origsymbol_list locals;

    locals = new_origsymbol_list();
    if( args != formalParameter_listNIL ){
	locals = collect_formalParameter_names( locals, args );
    }
    if( ldecl != declaration_listNIL ){
	locals = collect_declaration_names( locals, ldecl );
    }
    return locals;
}

/* Given a list of fields, return the names of these fields. */
origsymbol_list collect_field_names( const field_list l )
{
    origsymbol_list ans;
    unsigned int ix;

    ans = new_origsymbol_list();
    ans = setroom_origsymbol_list( ans, l->sz );
    for( ix=0; ix<l->sz; ix++ ){
	ans = append_origsymbol_list( ans, rdup_origsymbol( l->arr[ix]->name ) );
    }
    return ans;
}

/* Given a list of labeled statements, return a list of labels
 * of these statements.
 */
origsymbol_list collect_labels( const statement_list l )
{
    origsymbol_list ans = new_origsymbol_list();
    unsigned int ix;
    statement s;

    for( ix=0; ix<l->sz; ix++ ){
	s = l->arr[ix];
	if( s->label != origsymbolNIL ){
	    ans = append_origsymbol_list( ans, rdup_origsymbol( s->label ) );
	}
    }
    return ans;
}
