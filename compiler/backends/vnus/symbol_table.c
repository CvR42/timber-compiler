/* File: symbol_table.c
 *
 * Handling of the tmsymbol table.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <tmc.h>

#include "defs.h"
#include "tmadmin.h"
#include <vnusctl.h>
#include "symbol_table.h"
#include "global.h"

static scopeinfo_list scope_database = scopeinfo_listNIL;

/* See if any of the scopes is not defined. */
bool check_known_scopes( void )
{
    bool ok = true;

    for( unsigned int ix=0; ix<scope_database->sz; ix++ ){
	scopeinfo scope = scope_database->arr[ix];
	if( scope->known == 0 ){
	    tmsymbol nm = scope->name;
	    sprintf( errarg, "'%s'", nm==NULL?"<null>":nm->name );
	    error( "scope does not exist" );
	    origsymbol_list l = scope->locals;
	    if( l->sz == 0 ){
		fprintf( stderr, "There are no variables in this scope.\n" );
	    }
	    else {
		fprintf( stderr, "Variables in this scope are:\n" );
		for( unsigned int ixl=0; ixl<l->sz; ixl++ ){
		    origsymbol s = l->arr[ixl];
		    fprintf(
			stderr,
			"  %s:%d: %s\n",
			s->orig->file->name,
			s->orig->line,
			s->sym->name
		    );
		}
	    }
	    ok = false;
	}
    }
    return ok;
}

/* Given a scope name 'scope', search for it in the scope database
 * return false if it cannot be found, or return true if it can
 * be found, and assign the index in the database to '*pos'.
 */
static bool search_scope( const tmsymbol scope, unsigned int *pos )
{
    for( unsigned int ix=0; ix<scope_database->sz; ix++ ){
	if( scope_database->arr[ix]->name == scope ){
	    *pos = ix;
	    return true;
	}
    }
    return false;
}

/* Given a symbol 's' and its scope 'scope', add it to the
 * scope database.
 */
static void register_local_identifier(
    const origsymbol s,
    const tmsymbol scope
)
{
    unsigned int pos;

    if( trace_scope_admin ){
	fprintf( stderr, "Registered identifier %s in scope %s\n", s->sym->name, scope->name );
    }
    if( search_scope( scope, &pos ) ){
	scope_database->arr[pos]->locals = append_origsymbol_list(
	    scope_database->arr[pos]->locals,
	    rdup_origsymbol( s )
	);
    }
    else {
	scope_database = append_scopeinfo_list(
	    scope_database,
	    new_scopeinfo(
		rdup_tmsymbol( scope ),
		0,
		append_origsymbol_list(
		    new_origsymbol_list(),
		    rdup_origsymbol( s )
		)
	    )
	);
    }
}

/* Given a scope 'scope', register it as known in the scope database.  */
void register_scope( const tmsymbol scope )
{
    unsigned int pos;

    if( scope == tmsymbolNIL ){
	return;
    }
    if( trace_scope_admin ){
	fprintf( stderr, "Registered scope %s\n", scope->name );
    }
    if( search_scope( scope, &pos ) ){
	if( scope_database->arr[pos]->known ){
	    sprintf( errarg, "'%s'", scope->name );
	    error( "scope name already used" );
	}
	scope_database->arr[pos]->known = true;
    }
    else {
	scope_database = append_scopeinfo_list(
	    scope_database,
	    new_scopeinfo( rdup_tmsymbol( scope ), true, new_origsymbol_list() )
	);
    }
}

/* Given a scope 'scope', remove it from the scope database.  */
void unregister_scope( const tmsymbol scope )
{
    unsigned int pos;

    if( scope == tmsymbolNIL ){
	return;
    }
    if( trace_scope_admin ){
	fprintf( stderr, "Unregistered scope %s\n", scope->name );
    }
    if( search_scope( scope, &pos ) ){
	scope_database = delete_scopeinfo_list( scope_database, pos );
    }
    else {
	sprintf( errarg, "'%s'", scope->name );
	internal_error( "cannot delete unknown scope" );
    }
}

/* Given a scope name 'scope', return a list containing the symbols that are
 * declared in that scope, or tmsymbol_listNIL if no such scope is known.
 */
origsymbol_list get_scope_locals( const tmsymbol scope )
{
    unsigned int pos;

    if( scope == tmsymbolNIL ){
	return origsymbol_listNIL;
    }
    if( search_scope( scope, &pos ) ){
	return rdup_origsymbol_list( scope_database->arr[pos]->locals );
    }
    fprintf( stderr, "Scope not found: %s\n", scope->name );
    return origsymbol_listNIL;
}

/* Given a scope name 'scope', return  the number of symbols that are
 * declared in that scope, or 0 if no such scope is known.
 */
unsigned int count_scope_locals( const tmsymbol scope )
{
    unsigned int pos;

    if( scope == tmsymbolNIL ){
	return 0;
    }
    if( search_scope( scope, &pos ) ){
	unsigned int res = scope_database->arr[pos]->locals->sz;
	if( trace_scope_admin ){
	    fprintf( stderr, "Scope %s has %u local variables\n", scope->name, res );
	}
	return res;
    }
    fprintf( stderr, "Scope not found: %s\n", scope->name );
    return 0;
}

/* Flush the scope database. */
void clear_scopes_register( void )
{
    if( scope_database != scopeinfo_listNIL ){
	rfre_scopeinfo_list( scope_database );
    }
    scope_database = new_scopeinfo_list();
}

/* Given a declaration 'e', register the possible local identifier
 * in the scope info database.
 */
static void register_variable_scope( const declaration e )
{
    origsymbol name = e->name;

    switch( e->tag ){
        case TAGDeclGlobalVariable:
	    break;

        case TAGDeclExternalVariable:
	    break;

        case TAGDeclLocalVariable:
	    register_local_identifier( name, to_DeclLocalVariable(e)->scope );
	    break;

        case TAGDeclFormalVariable:
	    register_local_identifier( name, to_DeclFormalVariable(e)->scope );
	    break;

        case TAGDeclCardinalityVariable:
	    break;

        case TAGDeclRecord:
	    break;

        case TAGDeclFunction:
	    break;

        case TAGDeclExternalFunction:
	    break;

        case TAGDeclProcedure:
	    break;

        case TAGDeclExternalProcedure:
	    break;
    }
}

/* Given a list of entries, register all local identifiers in the
 * scope info database.
 */
void register_variable_scopes( const declaration_list l )
{
    for( unsigned int ix=0; ix<l->sz; ix++ ){
	register_variable_scope( l->arr[ix] );
    }
}

void init_symbol_table( void )
{
    clear_scopes_register();
}

void end_symbol_table( void )
{
    rfre_scopeinfo_list( scope_database );
    scope_database = scopeinfo_listNIL;
}
