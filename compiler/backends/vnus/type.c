/* File: type.c
 *
 * Type checking, classification and manipulation functions.
 */

#include <tmc.h>
#include <string.h>

#include "defs.h"
#include "tmadmin.h"
#include <vnusctl.h>
#include "type.h"
#include "symbol_table.h"
#include "symbol_support.h"
#include "error.h"
#include "service.h"

// The list of known function types.
static functionnode_list the_functions = functionnode_listNIL;

/* Forward declaration. */
static bool is_equivalent_type_list( const_declaration_list decls, const_type_list al, const_type_list bl );
static tmsymbol lookup_procedure_name( const_declaration_list decls, const_type_list formals );
static tmsymbol lookup_function_name( const_declaration_list decls, const_type rettype, const_type_list formals );
static tmstring type_list_string( const_declaration_list decls, const_type_list tl );

/* Given a basetype 't', return true if it is a numeric type. */
static bool is_numeric_type( const BASETYPE t )
{
    switch( t ){
	case BT_BYTE:
	case BT_SHORT:
	case BT_CHAR:
	case BT_INT:
	case BT_LONG:
	case BT_FLOAT:
	case BT_DOUBLE:
	case BT_COMPLEX:
	    return true;

	case BT_STRING:
	case BT_BOOLEAN:
	    break;
    }
    return false;
}

/* Given a type 't', return true if it is a numeric type. */
bool is_numeric_type( const_type t )
{
    if( t == typeNIL ){
	/* We're in trouble anyway, so do not be difficult. */
	return true;
    }
    switch( t->tag ){
	case TAGTypeBase:
	    return is_numeric_type( to_const_TypeBase(t)->base );

	case TAGTypeMap:
	    return is_numeric_type( to_const_TypeMap(t)->t );

	case TAGTypePragmas:
	    return is_numeric_type( to_const_TypePragmas(t)->t );

	case TAGTypeShape:
	case TAGTypePointer:
	case TAGTypeRecord:
	case TAGTypeNamedRecord:
	case TAGTypeUnsizedArray:
	case TAGTypeArray:
	case TAGTypeNeutralPointer:
	case TAGTypeFunction:
	case TAGTypeProcedure:
	    break;

    }
    return false;
}

/* Given a type 't', return true if it is the type complex. */
static inline bool is_complex_type( BASETYPE t )
{
    return t == BT_COMPLEX;
}

/* Given a type 't', return true if it is the type complex. */
bool is_complex_type( const_type t )
{
    if( t == typeNIL ){
	/* We're in trouble anyway, so do not be difficult. */
	return true;
    }
    switch( t->tag ){
	case TAGTypeBase:
	    return is_complex_type( to_const_TypeBase(t)->base );

	case TAGTypeMap:
	    return is_complex_type( to_const_TypeMap(t)->t );

	case TAGTypePragmas:
	    return is_complex_type( to_const_TypePragmas(t)->t );

	case TAGTypeShape:
	case TAGTypePointer:
	case TAGTypeRecord:
	case TAGTypeNamedRecord:
	case TAGTypeUnsizedArray:
	case TAGTypeArray:
	case TAGTypeNeutralPointer:
	case TAGTypeFunction:
	case TAGTypeProcedure:
	    break;

    }
    return false;
}

/* Given a list of fields 'pre' and a list of fields 'l', return true
 * iff 'pre' is a prefix of 'l'. That is, 'l' may be longer than 'pre',
 * but all the fields in 'pre' have a type that is equivalent with that
 * of the corresponding field in 'l'.
 */
static bool is_valid_cast_field_list(
 const_declaration_list decls,
 const_field_list pre,
 const_field_list l
)
{
    if( pre->sz>l->sz ){
	return false;
    }
    for( unsigned int ix=0; ix<pre->sz; ix++ ){
	if( !is_valid_cast( decls, pre->arr[ix]->elmtype, l->arr[ix]->elmtype ) ){
	    return false;
	}
    }
    return true;
}

/* Given a list of types 'to' and a list of types 'from', return true
 * iff all types in 'from' can be cast to their corresponding type in 'to'.
 */
static bool is_valid_cast(
 const_declaration_list decls,
 const_type_list to,
 const_type_list from
)
{
    if( to->sz != from->sz ){
	return false;
    }
    for( unsigned int ix=0; ix<to->sz; ix++ ){
	if( !is_valid_cast( decls, to->arr[ix], from->arr[ix] ) ){
	    return false;
	}
    }
    return true;
}

/* Given a type 'to' and a type 'from', return true iff it is allowed
 * to cast type 'from' to type 'to'.
 */
bool is_valid_cast(
 const_declaration_list decls,
 const_type to,
 const_type from
)
{
    if( to == typeNIL || from == typeNIL ){
	// Something was wrong, do not make it worse.
	return true;
    }
    if( is_equivalent_type( decls, from, to ) ){
	return true;
    }
    if( to->tag == TAGTypePragmas ){
	return is_valid_cast( decls, to_const_TypePragmas(to)->t, from );
    }
    if( from->tag == TAGTypePragmas ){
	return is_valid_cast( decls, to, to_const_TypePragmas(from)->t );
    }
    if( to->tag == TAGTypeMap ){
	return is_valid_cast( decls, to_const_TypeMap(to)->t, from );
    }
    if( from->tag == TAGTypeMap ){
	return is_valid_cast( decls, to, to_const_TypeMap(from)->t );
    }
    if( to->tag == TAGTypeUnsizedArray && from->tag == TAGTypeArray ){
	// We allow casts from sized array to unsized array of any type.
	return true;
    }
    if(
	(to->tag == TAGTypeUnsizedArray || to->tag == TAGTypePointer) &&
	from->tag == TAGTypeNeutralPointer
    ){
	return true;
    }
    if( to->tag != from->tag && (!is_record_type( from ) || !is_record_type( to )) ){
	return false;
    }
    switch( to->tag ){
	case TAGTypeMap:
	    internal_error( "mapping type should have been skipped" );
	    break;

	case TAGTypePragmas:
	    internal_error( "type pragmas should have been skipped" );
	    break;

	case TAGTypeBase:
	{
	    const BASETYPE bt_to = to_const_TypeBase(to)->base;
	    const BASETYPE bt_from = to_const_TypeBase(from)->base;

	    /* We only have to find converting casts that we accept. */
	    switch( bt_to ){
		case BT_BYTE:
		case BT_SHORT:
		case BT_INT:
		case BT_LONG:
		case BT_FLOAT:
		case BT_DOUBLE:
		case BT_CHAR:
		case BT_COMPLEX:
		    if( is_numeric_type( bt_from ) || bt_from == BT_CHAR ){
			return true;
		    }
		    break;

		case BT_BOOLEAN:
		case BT_STRING:
		    break;

	    }
	    break;
	}

	case TAGTypeUnsizedArray:
	    // We allow casts between unsized arrays of all element types.
	    return true;

	case TAGTypeShape:
	case TAGTypeArray:
	    break;

	case TAGTypeFunction:
	{
	    const_type_list from_formals = to_const_TypeFunction(from)->formals;
	    const_type_list to_formals = to_const_TypeFunction(to)->formals;
	    const_type from_rettype = to_const_TypeFunction(from)->rettype;
	    const_type to_rettype = to_const_TypeFunction(to)->rettype;

	    return
		is_valid_cast( decls, to_formals, from_formals ) &&
		is_valid_cast( decls, to_rettype, from_rettype );
	}

	case TAGTypeProcedure:
	{
	    const_type_list from_formals = to_const_TypeProcedure(from)->formals;
	    const_type_list to_formals = to_const_TypeProcedure(to)->formals;

	    return is_valid_cast( decls, to_formals, from_formals );
	}

	case TAGTypePointer:
	{
	    const_type toelm = to_const_TypePointer(to)->elmtype;
	    const_type fromelm = to_const_TypePointer(from)->elmtype;

	    return is_valid_cast( decls, toelm, fromelm ) ||
		(
		    is_record_type(toelm) && 
		    is_record_type(fromelm) &&
		    is_valid_cast( decls, fromelm, toelm )
		)
		||
		(
		    is_routine_type( toelm ) && 
		    is_routine_type( fromelm ) &&
		    is_valid_cast( decls, fromelm, toelm )
		)
		||
		// TODO: restrict this: it is only here to allow
		// arrays to be put in active reference blocks.
		(is_record_type( toelm ) && fromelm->tag == TAGTypeShape)
		;
	}

	case TAGTypeNamedRecord:
	{
	    const_field_list tofields = lookup_NamedRecord_fields(
		decls,
		to_const_TypeNamedRecord( to )->name
	    );
	    const_field_list fromfields;

	    if( from->tag == TAGTypeRecord ){
		fromfields = to_const_TypeRecord( from )->fields;
	    }
	    else {
		fromfields = lookup_NamedRecord_fields(
		    decls,
		    to_const_TypeNamedRecord( from )->name
		);
	    }
	    return is_valid_cast_field_list( decls, tofields, fromfields );
	}

	case TAGTypeRecord:
	{
	    const_field_list fromfields;

	    if( from->tag == TAGTypeRecord ){
		fromfields = to_const_TypeRecord(from)->fields;
	    }
	    else {
		fromfields = lookup_NamedRecord_fields( decls, to_const_TypeNamedRecord(from)->name );
	    }
	    return is_valid_cast_field_list( decls, to_const_TypeRecord(to)->fields, fromfields );
	}

	case TAGTypeNeutralPointer:
	    return true;

    }
    return false;
}

/* Given two fields 'a' and 'b', return true iff they are
 * equivalent.
 */
static bool is_equivalent_field( const_declaration_list decls, const_field a, const_field b )
{
    return is_equivalent_type( decls, a->elmtype, b->elmtype );
}

/* Given two field lists 'al' and 'bl', return true iff they are
 * equivalent.
 */
static bool is_equivalent_field_list( const_declaration_list decls, const_field_list al, const_field_list bl )
{
    if( al == bl ){
	return true;
    }
    if( al == field_listNIL || bl == field_listNIL ){
	return false;
    }
    if( al->sz != bl->sz ){
	return false;
    }
    for( unsigned int ix=0; ix<al->sz; ix++ ){
	if( !is_equivalent_field( decls, al->arr[ix], bl->arr[ix] ) ){
	    return false;
	}
    }
    return true;
}

/* Given a type 'a' and a type 'b', return true iff the types are
 * equivalent.
 */
bool is_equivalent_type( const_declaration_list decls, const_type a, const_type b )
{
    if( a == typeNIL || b == typeNIL ){
	return false;
    }
    if( a->tag == TAGTypePragmas ){
	return is_equivalent_type( decls, to_const_TypePragmas(a)->t, b );
    }
    if( b->tag == TAGTypePragmas ){
	return is_equivalent_type( decls, a, to_const_TypePragmas(b)->t );
    }
    if( a->tag == TAGTypeMap ){
	return is_equivalent_type( decls, to_const_TypeMap(a)->t, b );
    }
    if( b->tag == TAGTypeMap ){
	return is_equivalent_type( decls, a, to_const_TypeMap(b)->t );
    }
    if( a->tag == b->tag || (is_record_type(a) && is_record_type(b)) ){
	switch( a->tag ){
	    case TAGTypeMap:
		internal_error( "mapping type should have been skipped" );
		break;

	    case TAGTypePragmas:
		internal_error( "type pragmas should have been skipped" );
		break;

	    case TAGTypeBase:
		return (to_const_TypeBase(a)->base == to_const_TypeBase(b)->base);

	    case TAGTypeShape:
		return is_equivalent_type(
		    decls,
		    to_const_TypeShape(a)->elmtype,
		    to_const_TypeShape(b)->elmtype
		);

	    case TAGTypePointer:
		return is_equivalent_type(
		    decls,
		    to_const_TypePointer(a)->elmtype,
		    to_const_TypePointer(b)->elmtype
		);

	    case TAGTypeNamedRecord:
	    {
		const_field_list fla = lookup_NamedRecord_fields(
		    decls,
		    to_const_TypeNamedRecord( a )->name
		);
		const_field_list flb;
		if( b->tag == TAGTypeNamedRecord ){
		    if( to_const_TypeNamedRecord(a)->name->sym == to_const_TypeNamedRecord(b)->name->sym ){
			return true;
		    }
		    flb = lookup_NamedRecord_fields(
			decls,
			to_const_TypeNamedRecord( b )->name
		    );
		}
		else {
		    flb = to_const_TypeRecord(b)->fields;
		}
		return is_equivalent_field_list( decls, fla, flb );
	    }

	    case TAGTypeRecord:
	    {
		const_field_list fl;

		if( b->tag == TAGTypeNamedRecord ){
		    fl = lookup_NamedRecord_fields( decls, to_const_TypeNamedRecord( b )->name );
		}
		else {
		    fl = to_const_TypeRecord(b)->fields;
		}
		if( fl == field_listNIL ){
		    return true;
		}
		return is_equivalent_field_list(
		    decls,
		    to_const_TypeRecord(a)->fields,
		    fl
		);
	    }

	    case TAGTypeArray:
		if( to_const_TypeArray(a)->sz != to_const_TypeArray(b)->sz ){
		    return false;
		}
		return is_equivalent_type(
		    decls,
		    to_const_TypeArray(a)->elmtype,
		    to_const_TypeArray(b)->elmtype
		);

	    case TAGTypeUnsizedArray:
		return is_equivalent_type(
		    decls,
		    to_const_TypeUnsizedArray(a)->elmtype,
		    to_const_TypeUnsizedArray(b)->elmtype
		);

	    case TAGTypeNeutralPointer:
		return true;

	    case TAGTypeFunction:
		return is_equivalent_type_list(
			decls,
			to_const_TypeFunction(a)->formals,
			to_const_TypeFunction(b)->formals
		    ) &&
		    is_equivalent_type(
			decls,
			to_const_TypeFunction(a)->rettype,
			to_const_TypeFunction(b)->rettype
		    );

	    case TAGTypeProcedure:
		return is_equivalent_type_list(
		    decls,
		    to_const_TypeProcedure(a)->formals,
		    to_const_TypeProcedure(b)->formals
		);
	}
    }
    return false;
}

/* Given two type lists 'al' and 'bl', return true iff they are
 * equivalent.
 */
static bool is_equivalent_type_list( const_declaration_list decls, const_type_list al, const_type_list bl )
{
    if( al->sz != bl->sz ){
	return false;
    }
    for( unsigned int ix=0; ix<al->sz; ix++ ){
	if( !is_equivalent_type( decls, al->arr[ix], bl->arr[ix] ) ){
	    return false;
	}
    }
    return true;
}

/* A table of strings for the basetypes. */
static const char *basetype_strings[] =
{
    "VnusByte",		/* BT_BYTE */
    "VnusShort",	/* BT_SHORT */
    "VnusInt",		/* BT_INT */
    "VnusLong",		/* BT_LONG */
    "VnusFloat",	/* BT_FLOAT */
    "VnusDouble",	/* BT_DOUBLE */
    "VnusComplex",	/* BT_COMPLEX */
    "VnusChar",		/* BT_CHAR */
    "VnusString",	/* BT_STRING */
    "VnusBoolean",	/* BT_BOOLEAN */
    "Expr"		/* BT_ABSTRACTED */
};

/* Given a type 't', return a string containing the C type for it.  */
tmstring type_string( const_declaration_list decls, const_type t )
{
    char buf[10000];
    tmstring ans = tmstringNIL;

    if( t == typeNIL ){
	return tmstringNIL;
    }
    switch( t->tag ){
	case TAGTypeBase:
	    sprintf( buf, "%s", basetype_strings[(int) to_const_TypeBase(t)->base] );
	    ans = new_tmstring( buf );
	    break;

	case TAGTypeUnsizedArray:
	{
	    tmstring elmstr = type_string( decls, to_const_TypeUnsizedArray(t)->elmtype );
	    sprintf( buf, "%s *", elmstr );
	    rfre_tmstring( elmstr );
	    ans = new_tmstring( buf );
	    break;
	}

	case TAGTypeArray:
	{
	    tmstring elmstr = type_string( decls, to_const_TypeArray(t)->elmtype );
	    sprintf( buf, "%s[%u]", elmstr, to_const_TypeArray(t)->sz );
	    rfre_tmstring( elmstr );
	    ans = new_tmstring( buf );
	    break;
	}

        case TAGTypeShape:
	{
	    tmstring elmstr = type_string( decls, to_const_TypeShape(t)->elmtype );
	    unsigned int rank = to_const_TypeShape(t)->sizes->sz;
	    if( rank < 4 ){
		sprintf( buf, "ShapeD%u<%s>", rank, elmstr );
	    }
	    else {
		sprintf( buf, "ShapeDn<%s,%u>", elmstr, rank );
	    }
	    rfre_tmstring( elmstr );
	    ans = new_tmstring( buf );
	    break;
	}

	case TAGTypeNamedRecord:
	{
	    const_field_list fl = lookup_NamedRecord_fields( decls, to_const_TypeNamedRecord(t)->name );
	    tmsymbol recnm = get_record_name( decls, originNIL, fl );

	    ans = new_tmstring( recnm->name );
	    rfre_tmsymbol( recnm );
	    break;
	}

	case TAGTypeRecord:
	{
	    tmsymbol recnm = get_record_name( decls, originNIL, to_const_TypeRecord(t)->fields );

	    ans = new_tmstring( recnm->name );
	    rfre_tmsymbol( recnm );
	    break;
	}

	case TAGTypePointer:
	{
	    tmstring telm = type_string( decls, to_const_TypePointer(t)->elmtype );

	    sprintf( buf, "%s *", telm );
	    rfre_tmstring( telm );
	    ans = new_tmstring( buf );
	    break;
	}

	case TAGTypeNeutralPointer:
	    ans = new_tmstring( "void *" );
	    break;

	case TAGTypeProcedure:
	{
	    const_type_list formals = to_const_TypeProcedure(t)->formals;
	    tmsymbol nm = lookup_procedure_name( decls, formals );
	    if( nm == tmsymbolNIL ){
		tmstring parms = type_list_string( decls, formals );

		sprintf( buf, "(void *)%s", parms );
		rfre_tmstring( parms );
		ans = new_tmstring( buf );
	    }
	    else {
		ans = new_tmstring( nm->name );
	    }
	    break;
	}

	case TAGTypeFunction:
	{

	    const_type_list formals = to_const_TypeFunction(t)->formals;
	    const_type rettype = to_const_TypeFunction(t)->rettype;
	    tmsymbol nm = lookup_function_name( decls, rettype, formals );
	    if( nm == tmsymbolNIL ){
		tmstring parms = type_list_string( decls, formals );
		tmstring tnm = type_string( decls, rettype );

		sprintf( buf, "(%s)%s", tnm, parms );
		rfre_tmstring( parms );
		rfre_tmstring( tnm );
		ans = new_tmstring( buf );
	    }
	    else {
		ans = new_tmstring( nm->name );
	    }
	    break;
	}

	case TAGTypeMap:
	    ans = type_string( decls, to_const_TypeMap(t)->t );
	    break;

	case TAGTypePragmas:
	    ans = type_string( decls, to_const_TypePragmas(t)->t );
	    break;

    }
    return ans;
}

/* Given a list of types, return a string containing a bracketed
 * list of these types.
 */
static tmstring type_list_string( const_declaration_list decls, const_type_list tl )
{
    char buf[10000];
    unsigned int pos = 0;

    buf[pos++] = '(';
    if( tl->sz != 0 ){
	for( unsigned int ix=0; ix<tl->sz; ix++ ){
	    tmstring tnm = type_string( decls, tl->arr[ix] );

	    if( ix != 0 ){
		buf[pos++] = ',';
	    }
	    if( tnm != tmstringNIL ){
		strcpy( buf+pos, tnm );
		pos += strlen( tnm );
		rfre_tmstring( tnm );
	    }
	}
    }
    buf[pos++] = ')';
    buf[pos] = '\0';
    return new_tmstring( buf );
}

/* Given a type 't' and a name 'nm', return a string containing a
 * C declaration for 'nm'.
 */
tmstring declaration_string( const_declaration_list decls, const_type t, int flags, const char *nm, bool formal )
{
    char buf[10000];
    tmstring ans = tmstringNIL;

    switch( t->tag ){
	case TAGTypeBase:
	    if( has_any_flag( flags, MOD_FINAL ) ){
		sprintf(
		    buf,
		    "const %s %s",
		    basetype_strings[(int) to_const_TypeBase(t)->base],
		    nm
		);
	    }
	    else {
		sprintf(
		    buf,
		    "%s %s",
		    basetype_strings[(int) to_const_TypeBase(t)->base],
		    nm
		);
	    }
	    ans = new_tmstring( buf );
	    break;

	case TAGTypeUnsizedArray:
	{
	    if( has_any_flag( flags, MOD_FINAL ) ){
		sprintf( buf, "*const %s", nm );
	    }
	    else {
		sprintf( buf, "*%s", nm );
	    }
	    ans = declaration_string( decls, to_const_TypeUnsizedArray(t)->elmtype, 0, buf, formal );
	    break;
	}

	case TAGTypeArray:
	{
	    if( formal ){
		if( has_any_flag( flags, MOD_FINAL ) ){
		    sprintf( buf, "*const %s", nm );
		}
		else {
		    sprintf( buf, "*%s", nm );
		}
	    }
	    else {
		int sz = to_const_TypeArray(t)->sz;

		if( sz<1 ){
		    sz = 1;
		}
		sprintf( buf, "%s[%d]", nm, sz );
	    }
	    ans = declaration_string( decls, to_const_TypeArray(t)->elmtype, 0, buf, formal );
	    break;
	}

        case TAGTypeShape:
	{
	    tmstring elmstr = type_string( decls, to_const_TypeShape(t)->elmtype );
	    unsigned int rank = to_const_TypeShape(t)->sizes->sz;
	    const char *prefix = "";
	    if( has_any_flag( flags, MOD_FINAL ) ){
		prefix = "const ";
	    }
	    if( rank < 4 ){
		sprintf( buf, "%sShapeD%d<%s> %s", prefix, rank, elmstr, nm );
	    }
	    else {
		sprintf( buf, "%sShapeDn<%s,%d> %s", prefix, elmstr, rank, nm );
	    }
	    rfre_tmstring( elmstr );
	    ans = new_tmstring( buf );
	    break;
	}

	case TAGTypeNamedRecord:
	{
	    const_field_list fl = lookup_NamedRecord_fields(
		decls,
		to_const_TypeNamedRecord(t)->name
	    );

	    if( fl != field_listNIL ){
		tmsymbol recnm = get_record_name( decls, originNIL, fl );

		if( has_any_flag( flags, MOD_FINAL ) ){
		    sprintf( buf, "const %s %s", recnm->name, nm );
		}
		else {
		    sprintf( buf, "%s %s", recnm->name, nm );
		}
		ans = new_tmstring( buf );
		rfre_tmsymbol( recnm );
	    }
	    break;
	}

	case TAGTypeRecord:
	{
	    tmsymbol recnm = get_record_name( decls, originNIL, to_const_TypeRecord(t)->fields );
	    if( has_any_flag( flags, MOD_FINAL ) ){
		sprintf( buf, "const %s %s", recnm->name, nm );
	    }
	    else {
		sprintf( buf, "%s %s", recnm->name, nm );
	    }
	    ans = new_tmstring( buf );
	    rfre_tmsymbol( recnm );
	    break;
	}
	    
	case TAGTypePointer:
	    if( has_any_flag( flags, MOD_FINAL ) ){
		sprintf( buf, "*const %s", nm );
	    }
	    else {
		sprintf( buf, "*%s", nm );
	    }
	    ans = declaration_string( decls, to_const_TypePointer(t)->elmtype, 0, buf, formal );
	    break;

        case TAGTypeNeutralPointer:
	    if( has_any_flag( flags, MOD_FINAL ) ){
		sprintf( buf, "void *const %s", nm );
	    }
	    else {
		sprintf( buf, "void *%s", nm );
	    }
	    ans = new_tmstring( buf );
	    break;

	case TAGTypeProcedure:
	{
	    tmstring parms = type_list_string( decls, to_const_TypeProcedure(t)->formals );

	    sprintf( buf, "void (%s)%s", nm, parms );
	    rfre_tmstring( parms );
	    ans = new_tmstring( buf );
	    break;
	}

	case TAGTypeFunction:
	{
	    tmstring parms = type_list_string( decls, to_const_TypeFunction(t)->formals );
	    tmstring tnm = type_string( decls, to_const_TypeFunction(t)->rettype );

	    sprintf( buf, "%s(%s)%s", tnm, nm, parms );
	    rfre_tmstring( parms );
	    rfre_tmstring( tnm );
	    ans = new_tmstring( buf );
	    break;
	}

	case TAGTypePragmas:
	    ans = declaration_string( decls, to_const_TypePragmas(t)->t, flags, nm, formal );
	    break;

	case TAGTypeMap:
	    ans = declaration_string( decls, to_const_TypeMap(t)->t, flags, nm, formal );
	    break;

    }
    return ans;
}

// Given a list of functions, a return type and a list of formals,
// return 'true' if the list of functions contains the given function type.
// iff the function is found, fill '*ixp' with the index of the function.
static bool search_functionnode_list( 
    const_declaration_list decls,
    const_functionnode_list functions,
    const_type rettype,
    const_type_list formals,
    unsigned int *ixp
)
{
    for( unsigned int ix=0; ix<functions->sz; ix++ ){
	const_functionnode nd = the_functions->arr[ix];
	if(
	    ((rettype == nd->retval) || is_equivalent_type( decls, rettype, nd->retval )) &&
	    is_equivalent_type_list( decls, formals, nd->formals )
	)
	{
	    *ixp = ix;
	    return true;
	}
    }
    return false;
}

static tmsymbol lookup_function_name( const_declaration_list decls, const_type rettype, const_type_list formals )
{
    unsigned int ix;

    if( the_functions == NULL ){
	the_functions = new_functionnode_list();
    }
    if(
	search_functionnode_list( decls, the_functions, rettype, formals, &ix )
    )
    {
	return the_functions->arr[ix]->name;
    }
    //internal_error( "function not registered" );
    return tmsymbolNIL;
}

static tmsymbol lookup_procedure_name( const_declaration_list decls, const_type_list formals )
{
    unsigned int ix;

    if( the_functions == NULL ){
	the_functions = new_functionnode_list();
    }
    if(
	search_functionnode_list( decls, the_functions, typeNIL, formals, &ix )
    )
    {
	return the_functions->arr[ix]->name;
    }
    //internal_error( "procedure not registered" );
    return tmsymbolNIL;
}

void register_TypeFunction( declaration_list decls, type_list formals, type retval )
{
    unsigned int ix;

    if( the_functions == NULL ){
	the_functions = new_functionnode_list();
    }
    if(
	!search_functionnode_list( decls, the_functions, retval, formals, &ix )
    )
    {
	the_functions = append_functionnode_list(
	    the_functions,
	    new_functionnode(
		gen_tmsymbol( "fn" ),
		rdup_type( retval ),
		rdup_type_list( formals )
	    )
	);
    }
}

void register_TypeProcedure( declaration_list decls, type_list formals )
{
    unsigned int ix;

    if( the_functions == NULL ){
	the_functions = new_functionnode_list();
    }
    if(
	!search_functionnode_list( decls, the_functions, typeNIL, formals, &ix )
    )
    {
	the_functions = append_functionnode_list(
	    the_functions,
	    new_functionnode(
		gen_tmsymbol( "proc" ),
		typeNIL,
		rdup_type_list( formals )
	    )
	);
    }
}

// Given a file handle 'f', generate typedefs for all registered function
// types.
static void generate_function_type( FILE *f, const_declaration_list decls, const_functionnode nd )
{
    fputs( "typedef ", f );
    if( nd->retval == typeNIL ){
	fputs( "void ", f );
    }
    else {
	tmstring rts = type_string( decls, nd->retval );
	fputs( rts, f );
	fputc( ' ', f );
	rfre_tmstring( rts );
    }
    fputs( nd->name->name, f );
    tmstring fs = type_list_string( decls, nd->formals );
    fputs( fs, f );
    fputs( ";\n", f );
}

// Given a file handle 'f', generate typedefs for all registered function
// types.
void generate_function_types( FILE *f, const_declaration_list decls )
{
    if( the_functions == NULL ){
	return;
    }
    for( unsigned int ix=0; ix<the_functions->sz; ix++ ){
	generate_function_type( f, decls, the_functions->arr[ix] );
    }
}

void clear_function_types()
{
    if( the_functions != functionnode_listNIL ){
	rfre_functionnode_list( the_functions );
	the_functions = functionnode_listNIL;
    }
}

