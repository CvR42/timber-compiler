/* File: pragma.c */

#include <tmc.h>

#include <assert.h>
#include <string.h>

#include "defs.h"
#include "tmadmin.h"
#include "global.h"
#include "service.h"
#include <vnusctl.h>
#include "pragma.h"

/* Given a pragma list 'l', the name of a pragma 'nm', and a pointer
 * to an index 'ixp', search for the pragma with the given name in the list.
 *
 * If found, assign to '*pp' the last pragma with the
 * given name, and return true.
 *
 * If not found, do not touch '*pp', and return false.
 */
static bool search_pragma( const_Pragma_list gl, const_Pragma_list l, const char *nm, Pragma *pp )
{
    bool found = false;
    tmsymbol s = add_tmsymbol( nm );

    if( gl != Pragma_listNIL ){
	for( unsigned int ix=0; ix<gl->sz; ix++ ){
	    if( gl->arr[ix]->name->sym == s ){
		*pp = gl->arr[ix];
		found = true;
	    }
	}
    }
    if( l != Pragma_listNIL ){
	for( unsigned int ix=0; ix<l->sz; ix++ ){
	    if( l->arr[ix]->name->sym == s ){
		*pp = l->arr[ix];
		found = true;
	    }
	}
    }
    return found;
}

bool get_boolean_pragma( const_Pragma_list gl, const_Pragma_list l, const char *nm, bool *val )
{
    Pragma p;

    if( search_pragma( gl, l, nm, &p ) ){
        if( p == PragmaNIL ){
            return false;
        }
        switch( p->tag ){
            case TAGFlagPragma:
                *val = true;
                return true;

	    case TAGValuePragma:
	    {
	        PragmaExpression x = to_ValuePragma(p)->x;

	        switch( x->tag ){
		    case TAGBooleanPragmaExpression:
			*val = (bool) to_BooleanPragmaExpression(x)->b;
			return true;

		    case TAGStringPragmaExpression:
		    case TAGNumberPragmaExpression:
		    case TAGListPragmaExpression:
		    case TAGNamePragmaExpression:
		    case TAGExternalNamePragmaExpression:
			break;
	        }
	    }
        }
    }
    return false;
}

const char *get_string_pragma( const_Pragma_list gl, const_Pragma_list l, const char *nm )
{
    Pragma p;

    if( search_pragma( gl, l, nm, &p ) ){
        if( p == PragmaNIL ){
            return false;
        }
        switch( p->tag ){
            case TAGFlagPragma:
                return NULL;

	    case TAGValuePragma:
	    {
	        PragmaExpression x = to_ValuePragma(p)->x;

	        switch( x->tag ){
		    case TAGStringPragmaExpression:
			return to_StringPragmaExpression(x)->s;

		    case TAGBooleanPragmaExpression:
		    case TAGNumberPragmaExpression:
		    case TAGListPragmaExpression:
		    case TAGNamePragmaExpression:
		    case TAGExternalNamePragmaExpression:
			break;
	        }
	    }
        }
    }
    return NULL;
}

bool get_flag_pragma( const_Pragma_list gl, const_Pragma_list l, const char *nm )
{
    Pragma p;

    return search_pragma( gl, l, nm, &p );
}

/* Given a Vnus program, see if you can find a 'processors' pragma,
 * and fill the global variable 'processors' with its value.
 */
void derive_processor_array( const vnusprog prog )
{
    Pragma p;

    if( !search_pragma( prog->pragmas, Pragma_listNIL, "processors", &p ) ){
        /* No 'processors' pragma. */
        return;
    }
    if( p == PragmaNIL ){
        /* Paranoia. */
        return;
    }
    if( p->tag != TAGValuePragma ){
        return;
    }
    PragmaExpression x = to_ValuePragma(p)->x;
    if( x->tag != TAGStringPragmaExpression ){
        return;
    }
    tmstring s = to_StringPragmaExpression(x)->s;
    scan_number_of_processors( s );
}

/* Given a Vnus program, see if you can find a 'processors' pragma.
 * If it's not present, generate it by taking the values from the
 * global variable processor_array.
 */
void derive_processors_pragma( const vnusprog prog )
{
    Pragma p = PragmaNIL;

    if( search_pragma( prog->pragmas, Pragma_listNIL, "processors", &p ) ){
	    /* Found 'processors' pragma. */
        return;
    }
    assert( p == PragmaNIL );
    if( processor_array->sz == 0 )
	    /* Apparently sequential */
	return;
    
    char s[256];
    sprintf( s, "%d", processor_array->arr[0]);
    for (unsigned i=1; i<processor_array->sz; i++){
	sprintf( &s[strlen(s)],",%d", processor_array->arr[i]);
    }
    assert( strlen(s) < 256 );
    ValuePragma vp = new_ValuePragma(
	add_origsymbol("processors"),
	new_StringPragmaExpression(new_tmstring(s)));
    prog->pragmas = append_Pragma_list( prog->pragmas, vp );
}
