/* File: symbol_table.c
 *
 * Handling of the tmsymbol table.
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <tmc.h>

#include "defs.h"
#include "tmadmin.h"
#include <vnusctl.h>
#include "global.h"
#include "error.h"
#include "generate.h"
#include "symbol_table.h"
#include "symbol_support.h"
#include "pragma.h"
#include "type.h"
#include "generate.h"
#include "typename.h"

static recordnode recordtree = recordnodeNIL;

/* Given a name 'name' that is supposed to be a formal parameter,
 * return true iff the parameter is annotated as 'readonly'.
 */
bool is_readonly( const_declaration_list decls, const_origsymbol name, const_Pragma_list gl )
{
    const_declaration e = search_entry( decls, name->sym );
    if( e == declarationNIL ){
	return false;
    }
    return get_flag_pragma( gl, e->pragmas, "readonly" );
}

/* Given a name 'name' that is supposed to be a formal parameter,
 * return true iff the parameter is annotated as 'location'.
 */
bool is_location( const_declaration_list decls, const_origsymbol name, const_Pragma_list gl )
{
    const_declaration e = search_entry( decls, name->sym );
    if( e == declarationNIL ){
	return false;
    }
    if( e->tag != TAGDeclFormalVariable ){
	return false;
    }
    return get_flag_pragma( gl, e->pragmas, "location" );
}


/* Given a list of record nodes 'l', a type 't' and a pointer to a position
 * 'pos', return false if no node in the list contains type 't'.
 * Return true if at least one node in the list contains type 't', and
 * assign the index in 'l' of that node to *pos.
 */
static bool search_recordnode(
 const_declaration_list decls,
 const_recordnode_list l,
 const_type t,
 unsigned int *pos
)
{
    for( unsigned int ix=0; ix<l->sz; ix++ ){
	if( is_equivalent_type( decls, t, l->arr[ix]->t ) ){
	    *pos = ix;
	    return true;
	}
    }
    return false;
}

/* Given a record tree 'tree', a list of fields 'fields', and an offset
 * in this list 'offset', return the name of the record with these fields.
 *
 * Upon entry and exit the tree contains information for the fields
 * starting at offset 'offset'. The fields before that presumably match
 * the fields of these nodes in the tree.
 */
static tmsymbol get_record_name_nodes(
    const_declaration_list decls,
    const_recordnode tree,
    const_origin org,
    const_field_list fields,
    const unsigned int offset
)
{
    unsigned int pos;

    if( offset>=fields->sz ){
	if( tree->used == 0 ){
	    tmstring nm = name_field_list( decls, fields );
	    sprintf( errarg, "record %s\n", nm );
	    rfre_tmstring( nm );
	    origin_internal_error( org, "Cannot get the name of an unregistered record (there is a longer record)" );
	}
	return tree->name;
    }
    if( !search_recordnode( decls, tree->children, fields->arr[offset]->elmtype, &pos ) ){
	tmstring nm = name_field_list( decls, fields );
	sprintf( errarg, "record %s\n", nm );
	rfre_tmstring( nm );
	origin_internal_error( org, "Cannot get the name of an unregistered record" );
    }
    return get_record_name_nodes( decls, tree->children->arr[pos], org, fields, offset+1 );
}

/* Given a list of record fields, return the name of a structure. */
tmsymbol get_record_name(
 const_declaration_list decls,
 const_origin org,
 const_field_list fields
)
{
    if( trace_struct_register ){
	tmstring nm = name_field_list( decls, fields );

	fprintf( stderr, "looking for the name of record %s\n", nm );
	rfre_tmstring( nm );
    }
    tmsymbol res = get_record_name_nodes( decls, recordtree, org, fields, 0 );
    if( res == recordtree->name ){
	res = add_tmsymbol( "VnusBase" );
    }
    if( trace_struct_register ){
	if( res == tmsymbolNIL ){
	    fprintf( stderr, "not found" );
	}
	else {
	    fprintf( stderr, "Name is %s\n", res->name );
	}
    }
    return res;
}

/* Given a record tree 'tree', a list of fields 'fields', and an offset
 * in this list 'offset', return a modified version of the tree that
 * has the given field registered.
 *
 * Upon entry and exit the tree contains information for the fields
 * starting at offset 'offset'. The fields before that presumably match
 * the fields of these nodes in the tree.
 */
static recordnode register_record_nodes(
    const_declaration_list decls,
    recordnode tree,
    const_field_list fields,
    const unsigned int offset
)
{
    unsigned int pos;

    if( offset>=fields->sz ){
	if( tree->used == 0 ){
	    tree->used = 1;
	    tree->name = gen_tmsymbol( "record" );
	}
	return tree;
    }
    if( !search_recordnode( decls, tree->children, fields->arr[offset]->elmtype, &pos ) ){
	pos = tree->children->sz;
	tree->children = append_recordnode_list(
	    tree->children,
	    new_recordnode(
		tmsymbolNIL,
		0,
		rdup_type( fields->arr[offset]->elmtype ),
		new_recordnode_list()
	    )
	);
    }
    assert( tree->children->arr[pos]->t != typeNIL );
    tree->children->arr[pos] = register_record_nodes(
	decls,
	tree->children->arr[pos],
	fields,
	offset+1
    );
    return tree;
}

/* Given a list of record fields, register the structure with these fields. */
void register_record( const_declaration_list decls, const_field_list fields )
{
    if( trace_struct_register ){
	tmstring nm = name_field_list( decls, fields );

	fprintf( stderr, "registering record %s\n", nm );
	rfre_tmstring( nm );
    }
    recordtree = register_record_nodes( decls, recordtree, fields, 0 );
}

/* Given a file handle 'f' and a tree of records 'tree', generate
 * definitions for all registered records.
 */
static void generate_forward_record_definitions_tree( FILE *f, const_declaration_list decls, const_recordnode tree )
{
    if( tree->used != 0 ){
	fprintf( f, "class %s;\n", tree->name->name );
    }
    for( unsigned int ix=0; ix<tree->children->sz; ix++ ){
	generate_forward_record_definitions_tree( f, decls, tree->children->arr[ix] );
    }
}

// Given a file handle, the list of declarations, a 'first' flag indicating
// wether this is the first initializer for this class, and a type,
// generate initializers for all types that need it.
static bool generate_initializer( FILE *f, const_declaration_list decls, bool first, unsigned int n, const_type t )
{
    switch( t->tag ){
	case TAGTypeBase:
	case TAGTypePointer:
	case TAGTypeRecord:
	case TAGTypeNamedRecord:
	case TAGTypeProcedure:
	case TAGTypeFunction:
	case TAGTypeArray:
	case TAGTypeUnsizedArray:
	case TAGTypeNeutralPointer:
	    break;

	case TAGTypePragmas:
	    return generate_initializer( f, decls, first, n, to_const_TypePragmas(t)->t );

	case TAGTypeMap:
	    return generate_initializer( f, decls, first, n, to_const_TypeMap(t)->t );

	case TAGTypeShape:
	{
	    const_TypeShape st = to_const_TypeShape( t );
	    if( first ){
		fputs( ": ", f );
		first = false;
	    }
	    else {
		fputs( ", ", f );
	    }
	    fprintf( f, "field%u", n );
	    generate_dimensionlist(
		f,
		decls,
		expressionNIL,
		0,
		st->sizes,
		origsymbolNIL
	    );
	    break;
	}

    }
    return first;
}

// Given a file handle, the list of declarations and a list of types in
// the current record, generate initializers for all types that need it.
static void generate_initializer_list( FILE *f, const_declaration_list decls, const_type_list subl )
{
    bool first = true;

    for( unsigned int ix=0; ix<subl->sz; ix++ ){
	first = generate_initializer( f, decls, first, ix, subl->arr[ix] );
    }
}

// Given a file handle 'f', a class name 'name', an indent 'indent' and
// a type list 'tl', generate a constructor to fill all the fields in the
// class in one go. Also generate a constructor that doesn't do any
// initialization.
static void generate_record_constructors( FILE *f, const_declaration_list decls, const char *name, const_type_list tl )
{
    // Generate the empty constructor, if necessary.
    if( tl->sz != 0 ){
	fprintf( f, "    inline %s()\n", name );
	fputs( "     ", f );
	generate_initializer_list( f, decls, tl );
	fputs( "\n    {}\n", f );
    }
    // Generate the constructor with parameter list.
    fprintf( f, "    inline %s( ", name );
    for( unsigned int ix=0; ix<tl->sz; ix++ ){
	char buf[20];

	sprintf( buf, "p_f%u", ix );
	tmstring decl = declaration_string( decls, tl->arr[ix], 0, buf, true );
	fputs( decl, f );
	rfre_tmstring( decl );
	if( ix+1 != tl->sz ){
	    fputs( ", ", f );
	}
    }
    fputs( " )", f );
    if( tl->sz != 0 ){
	fputc( ':', f );
    }
    fputc( '\n', f );
    if( tl->sz != 0 ){
	// Generate the initializers
	fputc( '\t', f );
	for( unsigned int ix=0; ix<tl->sz; ix++ ){
	    fprintf( f, "field%u(p_f%u)", ix, ix );
	    if( ix+1 != tl->sz ){
		fputs( ", ", f );
	    }
	}
	fputc( '\n', f );
    }
    // Generate the constructor body (always empty)
    fputs( "    {}\n", f );
}

/* Given a file handle 'f' and a tree of records 'tree', generate
 * definitions for all registered records.
 */
static void generate_record_definitions_tree( FILE *f, const_declaration_list decls, const char *supername, const_recordnode tree, const_type_list l )
{
    type_list subl = rdup_type_list( l );
    subl = append_type_list( subl, rdup_type( tree->t ) );
    if( tree->used != 0 ){
	fprintf( f, "class %s: public %s {\n", tree->name->name, supername );
	fputs( "public:\n", f );
	generate_record_constructors( f, decls, tree->name->name, subl );
	for( unsigned int ix=0; ix<subl->sz; ix++ ){
	    char buf[20];

	    sprintf( buf, "field%u", ix );
	    tmstring decl = declaration_string( decls, subl->arr[ix], 0, buf, true );
	    fprintf( f, "    %s;\n", decl );
	    rfre_tmstring( decl );
	}
	fputs( "};\n\n", f );
    }
    for( unsigned int ix=0; ix<tree->children->sz; ix++ ){
	generate_record_definitions_tree( f, decls, supername, tree->children->arr[ix], subl );
    }
    rfre_type_list( subl );
}

/* Generate typedefs for all registered record types. */
void generate_record_definitions( FILE *f, const_declaration_list decls )
{
    type_list l = new_type_list();
    if( recordtree->used == 0 ){
	recordtree->used = 1;
	recordtree->name = gen_tmsymbol( "record" );
    }
    for( unsigned int ix=0; ix<recordtree->children->sz; ix++ ){
	generate_forward_record_definitions_tree( f, decls, recordtree->children->arr[ix] );
    }
    fputs( "\n", f );
    const char *supername = "VnusBase";
    for( unsigned int ix=0; ix<recordtree->children->sz; ix++ ){
	generate_record_definitions_tree( f, decls, supername, recordtree->children->arr[ix], l );
    }
    rfre_type_list( l );
}


void init_symbol_support( void )
{
    if( recordtree != recordnodeNIL ){
	rfre_recordnode( recordtree );
    }
    recordtree = new_recordnode( tmsymbolNIL, 0, typeNIL, new_recordnode_list() );
}

void end_symbol_support( void )
{
    rfre_recordnode( recordtree );
    recordtree = recordnodeNIL;
}
