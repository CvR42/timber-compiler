/* File: service.c
 *
 * Miscellaneous service routines.
 */

#include <tmc.h>

#include "defs.h"
#include "vnus_types.h"
#include "tmadmin.h"
#include "global.h"
#include "service.h"
#include "error.h"

int find_field( const type rectype, const origsymbol field )
{
    unsigned int ix;
    field_list fields;

    if( rectype->tag != TAGTypeRecord ){
	internal_botch( "can only find the field of a record type" );
	return 0;
    }
    fields = to_TypeRecord(rectype)->fields;
    for( ix=0; ix<fields->sz; ix++ ){
	if( fields->arr[ix]->name->sym == field->sym ){
	    return ix;
	}
    }
    return -1;
}

bool member_origsymbol_list( const origsymbol_list l, const origsymbol s )
{
    unsigned int ix;

    for( ix=0; ix<l->sz; ix++ ){
	if( l->arr[ix]->sym == s->sym ){
	    return TRUE;
	}
    }
    return FALSE;
}
