/* File: service.c
 *
 * Miscellaneous service routines.
 */

#include <tmc.h>
#include <ctype.h>

#include "defs.h"
#include "tmadmin.h"
#include <vnusctl.h>
#include "global.h"
#include "service.h"
#include "error.h"
#include "symbol_table.h"

/* Given a pointer to a string, interpret it as a number of processors. */
void scan_number_of_processors( const char *str )
{

    if( processor_array == tmuint_listNIL ){
        processor_array = new_tmuint_list();
    }
    else {
        processor_array->sz = 0;
    }
    for(;;){
	while( *str != '\0' && !isdigit( *str ) ){
	    str++;
	}
	if( *str == '\0' ){
	    break;
	}
	unsigned int nproc = 0;
	while( isdigit( *str ) ){
	    nproc = nproc*10+(unsigned int) (*str - '0');
	    str++;
	}
	processor_array = append_tmuint_list( processor_array, nproc );
    }
    numberOfProcessors = 1;
    for( unsigned int ix=0; ix<processor_array->sz; ix++ ){
	numberOfProcessors *= processor_array->arr[ix];
    }
}

int find_field( const_declaration_list decls, const_type rectype, const_origsymbol fld )
{
    const_field_list fields;
    int res = 0;

    if( fld == origsymbolNIL ){
	internal_error( "Null field" );
    }
    switch( rectype->tag ){
	case TAGTypeNamedRecord:
	    fields = lookup_NamedRecord_fields(
		decls,
		to_const_TypeNamedRecord(rectype)->name
	    );
	    res = find_field_fields( fields, fld );
	    break;

	case TAGTypeRecord:
	    fields = to_const_TypeRecord(rectype)->fields;
	    res = find_field_fields( fields, fld );
	    break;

	case TAGTypePragmas:
	    res = find_field( decls, to_const_TypePragmas(rectype)->t, fld );
	    break;

	case TAGTypeMap:
	    res = find_field( decls, to_const_TypeMap(rectype)->t, fld );
	    break;

	case TAGTypeShape:
	case TAGTypeBase:
	case TAGTypePointer:
	case TAGTypeProcedure:
	case TAGTypeFunction:
	case TAGTypeNeutralPointer:
	case TAGTypeUnsizedArray:
	case TAGTypeArray:
	    internal_error( "can only find the field of a record type" );
    }
    return res;
}

// Given a list of declarations 'defs' and a type 't', return a copy
// of the list of fields of the type.
field_list get_record_fields( const_declaration_list defs, const_type t )
{
    switch( t->tag ){
	case TAGTypeRecord:
	    return rdup_field_list( to_const_TypeRecord( t )->fields );

	case TAGTypeNamedRecord:
	    return rdup_field_list(
		lookup_NamedRecord_fields( defs, to_const_TypeNamedRecord(t)->name )
	    );

	case TAGTypeBase:
	case TAGTypeShape:
	case TAGTypePointer:
	case TAGTypeProcedure:
	case TAGTypeFunction:
	case TAGTypeUnsizedArray:
	case TAGTypeArray:
	case TAGTypeNeutralPointer:
	    break;

	case TAGTypeMap:
	    return get_record_fields( defs, to_const_TypeMap(t)->t );

	case TAGTypePragmas:
	    return get_record_fields( defs, to_const_TypePragmas(t)->t );

    }
    return field_listNIL;
}

void dump_expression( FILE *f, const_expression x, const char *desc )
{
    TMPRINTSTATE *st = tm_setprint( f, 1, 75, 8, 0 );
    fputs( desc, f );
    fputs( ": ", f );
    print_expression( st, x );
    fputs( "\n", f );
    int level = tm_endprint( st );
    if( level != 0 ){
	fprintf( stderr, "Internal botch: bracket level = %d\n", level );
    }
    (void) fflush( f );
}

// Given an expression, return true iff it represents the constant '1'.
bool is_constant_value( const_expression x, int n )
{
    if( x->tag == TAGExprInt ){
	return to_const_ExprInt(x)->v == n;
    }
    return false;
}

// Handy function to put a breakpoint on.
void yup()
{
    fprintf( stderr, "Yup\n" );
}

/* Given a type 't' and a basetype 'bt', return true if the types are
 * the same.
 */
bool is_this_basetype( const type t, const BASETYPE bt )
{
    if( t == typeNIL ){
	/* We're in trouble anyway, so don't be difficult. */
	return true;
    }
    switch( t->tag ){
	case TAGTypeBase:
	    return (to_TypeBase(t)->base == bt);

	case TAGTypeMap:
	    return is_this_basetype( to_TypeMap(t)->t, bt );

	case TAGTypePragmas:
	    return is_this_basetype( to_TypePragmas(t)->t, bt );

	case TAGTypeShape:
	case TAGTypePointer:
	case TAGTypeRecord:
	case TAGTypeNamedRecord:
	case TAGTypeArray:
	case TAGTypeUnsizedArray:
	case TAGTypeNeutralPointer:
	case TAGTypeFunction:
	case TAGTypeProcedure:
	    break;
    }
    return false;
}

// Given a list of expressions and an expression 'x', return true
// iff the list contains an expression that is equivalent to 'x'.
bool occurs_expression_list( const_expression_list xl, const_expression x )
{
    for( unsigned int ix=0; ix<xl->sz; ix++ ){
	if( is_equivalent_expression( xl->arr[ix], x ) ){
	    return true;
	}
    }
    return false;
}
