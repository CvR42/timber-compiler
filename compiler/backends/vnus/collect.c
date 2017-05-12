/* File: collect.c
 *
 * Routines that collect names from various parts of the vnus parse tree.
 */

#include <stdio.h>
#include <tmc.h>

#include "defs.h"
#include "tmadmin.h"
#include "error.h"
#include "symbol_table.h"
#include "global.h"
#include "collect.h"

/* Given a list of declarations, collect the symbols if the declared. */
origsymbol_list collect_declaration_names( const declaration_list l )
{
    origsymbol_list ans = new_origsymbol_list();

    ans = setroom_origsymbol_list( ans, l->sz );
    for( unsigned int ix=0; ix<l->sz; ix++ ){
	const origsymbol s = rdup_origsymbol( l->arr[ix]->name );

	ans = append_origsymbol_list( ans, s );
    }
    return ans;
}

/* Given a cardinality list, return the symbols that are used in it. */
origsymbol_list collect_cardinality_symbols( cardinality_list l )
{
    origsymbol_list ans = new_origsymbol_list();

    ans = setroom_origsymbol_list( ans, l->sz );
    for( unsigned int ix=0; ix<l->sz; ix++ ){
	const origsymbol s = rdup_origsymbol( l->arr[ix]->name );

	ans = append_origsymbol_list( ans, s );
    }
    return ans;
}

/* Given a secondary list, return the symbols that are used in it. */
origsymbol_list collect_secondary_symbols( secondary_list l )
{
    origsymbol_list ans = new_origsymbol_list();

    if (l == secondary_listNIL)
	return ans;
    
    ans = setroom_origsymbol_list( ans, l->sz );
    for( unsigned int ix=0; ix<l->sz; ix++ ){
	const origsymbol s = rdup_origsymbol( l->arr[ix]->name );

	ans = append_origsymbol_list( ans, s );
    }
    return ans;
}

/* Given a function definition, return all the local symbols in
 * that definition. We assume the declarations are correct.
 */
origsymbol_list collect_function_names(
 origsymbol_list parms,
 origsymbol retval,
 declaration_list ldecl
)
{
    origsymbol_list argnms;
    origsymbol_list localnms;
    origsymbol_list locals;

    argnms = rdup_origsymbol_list( parms );
    localnms = collect_declaration_names( ldecl );
    locals = concat_origsymbol_list( argnms, localnms );
    locals = append_origsymbol_list( locals, rdup_origsymbol( retval ) );
    return locals;
}

/* Given a list of fields, return the names of these fields. */
origsymbol_list collect_field_names( const field_list l )
{
    origsymbol_list ans = new_origsymbol_list();

    ans = setroom_origsymbol_list( ans, l->sz );
    for( unsigned int ix=0; ix<l->sz; ix++ ){
	if( l->arr[ix]->name != origsymbolNIL ){
	    ans = append_origsymbol_list( ans, rdup_origsymbol( l->arr[ix]->name ) );
	}
    }
    return ans;
}

/* Given a list of labeled statements, return a list of labels
 * of these statements.
 */
origsymbol_list collect_labels( const statement_list l )
{
    origsymbol_list ans = new_origsymbol_list();

    for( unsigned int ix=0; ix<l->sz; ix++ ){
	statement s = l->arr[ix];
	if( s->label != origsymbolNIL ){
	    ans = append_origsymbol_list( ans, rdup_origsymbol( s->label ) );
	}
    }
    return ans;
}
