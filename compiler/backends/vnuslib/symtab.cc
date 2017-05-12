/* File: symtab.cc
 *
 * Miscellaneous service routines.
 */

#include <tmc.h>
#include <ctype.h>
#include <assert.h>

#include "vnusbasetypes.h"
#include "tmadmin.h"
#include "vnusctl.h"

// Since 'search_entry' is by far the dominant time-consumer in the
// program, a cache of the most recently found entries is maintained.
// The replacement strategy is a simple mark/sweep algorithm.

#define CACHESZ 80
static tmsymbol cached_names[CACHESZ] = {0};
static declaration cached_decls[CACHESZ] = {0};
static const_declaration_list cached_dl[CACHESZ] = {0};
static bool cacheline_used[CACHESZ] = {false};
static unsigned int sweepix = 0;

static declaration get_cached_entry( const_declaration_list decls, tmsymbol name )
{
    for( unsigned int ix=0; ix<CACHESZ; ix++ ){
	if( cached_names[ix] == name && cached_dl[ix] == decls ){
	    cacheline_used[ix] = true;
	    return cached_decls[ix];
	}
    }
    return declarationNIL;
}

static void register_entry_cache( const_declaration_list decls, tmsymbol name, declaration d )
{
    while( cacheline_used[sweepix] ){
	cacheline_used[sweepix] = false;
	sweepix++;
	if( sweepix>=CACHESZ ){
	    sweepix = 0;
	}
    }
    cached_names[sweepix] = name;
    cached_decls[sweepix] = d;
    cached_dl[sweepix] = decls;
    cacheline_used[sweepix] = true;
}


/* Given a tmsymbol 'name', return the declaration for that name.
 * Since the tmsymbol table is supposed to describe every identifier
 * in the program, the function can (in principle) not fail.
 * If it does, it is treated as an internal error.
 */
declaration search_entry( const_declaration_list decls, const tmsymbol name )
{
    if( decls == declaration_listNIL ){
	return declarationNIL;
    }
    declaration e = get_cached_entry( decls, name );
    if( e != declarationNIL ){
	return e;
    }
    //fprintf( stderr, "Looking for '%s'\n", name->name );
    for( unsigned int ix = 0; ix<decls->sz; ix++ ){
	declaration e = decls->arr[ix];
	if( name == e->name->sym ){
	    register_entry_cache( decls, name, e );
	    return e;
	}
    }
    return declarationNIL;
}

/* Given a tmsymbol 'name', return the declaration for that name.
 * Since the tmsymbol table is supposed to describe every identifier
 * in the program, the function can (in principle) not fail.
 * If it does, it is treated as an internal error.
 */
declaration search_declaration( const_declaration_list decls, const tmsymbol name )
{
    return search_entry( decls, name );
}

/* Given an origsymbol 'name', return the declaration for that name.
 * Since the symbol table is supposed to describe every identifier
 * in the program, the function can (in principle) not fail.
 * If it does, it is treated as an error.
 */
declaration lookup_declaration( const_declaration_list decls, const_origsymbol name )
{
    tmsymbol s = name->sym;

    if( s == tmsymbolNIL ){
	origsymbol_error( name, "cannot lookup NIL symbol" );
	return declarationNIL;
    }
    declaration e = search_entry( decls, s );
    if( e == declarationNIL ){
	origsymbol_error( name, "not in symbol table" );
    }
    return e;
}

/* Given a tmsymbol 'name', return the declaration for that name.
 * Since the symbol table is supposed to describe every identifier
 * in the program, the function can (in principle) not fail.
 * If it does, it is treated as an error.
 */
declaration lookup_declaration( const_declaration_list decls, tmsymbol s )
{
    if( s == tmsymbolNIL ){
	error( "cannot lookup NIL symbol" );
	return declarationNIL;
    }
    declaration e = search_entry( decls, s );
    if( e == declarationNIL ){
	sprintf( errarg, "'%s'", s->name );
	error( "not in symbol table" );
    }
    return e;
}

/* Given a list of formal parameters, return a list of types. */
static type_list build_parm_type_list( const_declaration_list decls, const_origsymbol_list nl )
{
    type_list tl = setroom_type_list( new_type_list(), nl->sz );

    for( unsigned int ix=0; ix<nl->sz; ix++ ){
	tl = append_type_list(
	    tl,
	    lookup_vartype( decls, nl->arr[ix] )
	);
    }
    return tl;
}


/* Given a name 'name' that is supposed to be a variable,
 * return a copy of the type of that variable. 
 */
type lookup_vartype( const_declaration_list decls, const_origsymbol name )
{
    const_declaration d = lookup_declaration( decls, name );
    if( d == declarationNIL ){
	return typeNIL;
    }
    switch( d->tag ){
	case TAGDeclCardinalityVariable:
	    return new_TypeBase( BT_INT );

        case TAGDeclExternalVariable:
	    return rdup_type( to_const_DeclExternalVariable(d)->t );

        case TAGDeclGlobalVariable:
	    return rdup_type( to_const_DeclGlobalVariable(d)->t );

        case TAGDeclLocalVariable:
	    return rdup_type( to_const_DeclLocalVariable(d)->t );

        case TAGDeclFormalVariable:
	    return rdup_type( to_const_DeclFormalVariable(d)->t );

        case TAGDeclFunction:
	    return new_TypeFunction(
		build_parm_type_list( decls, to_const_DeclFunction(d)->parms ),
		rdup_type( to_const_DeclFunction(d)->rettype )
	    );

        case TAGDeclExternalFunction:
	    return new_TypeFunction(
		build_parm_type_list( decls, to_const_DeclExternalFunction(d)->parms ),
		rdup_type( to_const_DeclExternalFunction(d)->rettype )
	    );

        case TAGDeclProcedure:
	    return new_TypeProcedure(
		build_parm_type_list( decls, to_const_DeclProcedure(d)->parms )
	    );

        case TAGDeclExternalProcedure:
	    return new_TypeProcedure(
		build_parm_type_list( decls, to_const_DeclExternalProcedure(d)->parms )
	    );

	case TAGDeclRecord:
	    internal_error( "Cannot get the type of a record declaration" );
	    break;
    }
    return typeNIL;
}

/* Given a name 'name' that is supposed to be a formal parameter,
 * return the type of that formal. The returned type
 * expression is still owned by the symbol table.
 */
const_type lookup_argtype( const_declaration_list decls, const_origsymbol name )
{
    declaration d = lookup_declaration( decls, name );

    if( d == declarationNIL ){
	return typeNIL;
    }
    if( d->tag != TAGDeclFormalVariable ){
	origsymbol_error( name, "not a formal parameter" );
	return typeNIL;
    }
    return to_const_DeclFormalVariable(d)->t;
}

const_field_list lookup_NamedRecord_fields( const_declaration_list decls, const_origsymbol name )
{
    declaration d = lookup_declaration( decls, name );
    if( d == declarationNIL ){
	return field_listNIL;
    }
    switch( d->tag ){
        case TAGDeclRecord:
	    return to_DeclRecord(d)->fields;

	default:
	    origsymbol_error( name, "not a record" );
	    break;
    }
    return field_listNIL;
}

/* Given a name 'name' that is supposed to be of a function,
 * return the return type of that function. The returned type
 * expression is still owned by the symbol table.
 */
type lookup_fntype( const_declaration_list decls, const_origsymbol name )
{
    declaration d = lookup_declaration( decls, name );

    if( d == declarationNIL ){
	return typeNIL;
    }
    switch( d->tag ){
        case TAGDeclFunction:
	    return to_DeclFunction(d)->rettype;

        case TAGDeclExternalFunction:
	    return to_DeclExternalFunction(d)->rettype;

	default:
	    origsymbol_error( name, "not a function" );
	    break;
    }
    return typeNIL;
}

