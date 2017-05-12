// File: markadmin.c
//
// Maintain a list of types that need an array marker function.

#include <stdio.h>
#include <tmc.h>

#include "defs.h"
#include "tmadmin.h"
#include "symbol_table.h"
#include "markadmin.h"

static arraymarker_list markers = arraymarker_listNIL;

static bool search_marker( const_type t, unsigned int *ixp )
{
    // TODO: don't get confused by annotations in types.
    for( unsigned int ix=0; ix<markers->sz; ix++ ){
	arraymarker m = markers->arr[ix];

	if( isequal_type( t, m->t ) ){
	    *ixp = ix;
	    return true;
	}
    }
    return false;
}

tmsymbol get_arraymarker_name( const_type t )
{
    unsigned int ix;

    if( markers == arraymarker_listNIL ){
	markers = new_arraymarker_list();
    }
    if( search_marker( t, &ix ) ){
	arraymarker m = markers->arr[ix];
	return m->fnname;
    }
    tmsymbol fnname = tmsymbolNIL;
    if( is_collectable_type( t ) ){
	fnname = gen_tmsymbol( "arraymarker" );
    }
    markers = append_arraymarker_list(
	markers,
	new_arraymarker( rdup_type( t ), fnname )
    );
    return fnname;
}

const_arraymarker_list get_arraymarkers()
{
    if( markers == arraymarker_listNIL ){
	markers = new_arraymarker_list();
    }
    return markers;
}

void clear_arraymarkers()
{
    if( markers != arraymarker_listNIL ){
	rfre_arraymarker_list( markers );
	markers = arraymarker_listNIL;
    }
}
