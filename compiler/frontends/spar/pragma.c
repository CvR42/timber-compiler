// File: pragma.c

#include <tmc.h>

#include "defs.h"
#include "tmadmin.h"
#include "service.h"
#include "global.h"
#include "pragma.h"

// Given a  string describing the role of this construct, construct
// a 'role' pragma.
Pragma new_role_pragma( const char *role )
{
    return new_ValuePragma(
	add_origsymbol( "role" ),
	new_StringPragmaExpression( new_tmstring( role ) )
    );
}

// Given a pragma, return a pragma list with the pragma as single element.
Pragma_list build_Pragma_list( Pragma p )
{
    Pragma_list res = new_Pragma_list();
    if( p != PragmaNIL ){
	res = append_Pragma_list( res, p );
    }
    return res;
}

/* Given a pragma list 'l', the name of a pragma 'nm', and a pointer
 * to an index 'ixp', search for the pragma with the given name in the list.
 *
 * If found, assign to '*ixp' the index of the first pragma with the
 * given name, and return true.
 *
 * If not found, do not touch '*ixp', and return falsE.
 */
bool search_pragma_list( const_Pragma_list l, const char *nm, unsigned int *ixp )
{
    tmsymbol s = add_tmsymbol( nm );

    if( l == Pragma_listNIL ){
	return false;
    }
    // Search the pragma list back to front to give priority to the latest
    // pragma with a given name in the list. Yes this is pretty farfetched.
    unsigned int ix = l->sz;
    while( ix>0 ){
	ix--;
	if( l->arr[ix]->name->sym == s ){
	    *ixp = ix;
	    return true;
	}
    }
    return false;
}

/* Given a pragma list 'gl' and a pragma list 'l', the name of a pragma 'nm',
 * and a pointer to an index 'ixp', search for the pragma with the given name
 * in the list.
 *
 * If found, assign to '*pp' the pragma with the given name, and return true.
 * If the pragma occurs in both lists, prefer the result in 'l'.
 *
 * If not found, do not touch '*pp', and return FALSE.
 */
static bool search_pragma( const_Pragma_list gl, const_Pragma_list l, const char *nm, Pragma *pp )
{
    unsigned int ix;
    bool found = false;

    if( search_pragma_list( gl, nm, &ix ) ){
	// If found, record this as the result.
	*pp = gl->arr[ix];
	found = true;
    }

    if( search_pragma_list( l, nm, &ix ) ){
	// If found, record this as the result, perhaps overriding the 'gl'
	// result.
	*pp = l->arr[ix];
	found = true;
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

		    case TAGExternalNamePragmaExpression:
		    case TAGListPragmaExpression:
		    case TAGNamePragmaExpression:
		    case TAGNumberPragmaExpression:
		    case TAGStringPragmaExpression:
			break;
	        }
	    }
        }
    }
    return false;
}

const_tmstring get_string_pragma( const_Pragma_list gl, const_Pragma_list l, const char *nm )
{
    Pragma p;

    if( search_pragma( gl, l, nm, &p ) ){
        if( p == PragmaNIL ){
            return tmstringNIL;
        }
        switch( p->tag ){
            case TAGFlagPragma:
                return tmstringNIL;

	    case TAGValuePragma:
	    {
	        const_PragmaExpression x = to_const_ValuePragma(p)->x;

	        switch( x->tag ){
		    case TAGStringPragmaExpression:
			return to_const_StringPragmaExpression( x )->s;

		    case TAGBooleanPragmaExpression:
		    case TAGExternalNamePragmaExpression:
		    case TAGListPragmaExpression:
		    case TAGNamePragmaExpression:
		    case TAGNumberPragmaExpression:
			break;
	        }
	    }
        }
    }
    return tmstringNIL;
}

bool get_flag_pragma( const_Pragma_list gl, const_Pragma_list l, const char *nm )
{
    Pragma p;

    return search_pragma( gl, l, nm, &p );
}
