/* File: collect.c
 *
 * Routines that collect names from various parts of the vnus parse tree.
 */

#include <stdio.h>
#include <tmc.h>
#include <assert.h>

#include "defs.h"
#include "tmadmin.h"
#include "symbol_table.h"
#include "collect.h"
#include "typederive.h"
#include "service.h"
#include "error.h"

/* Given an FormalParameter list, return the list of FormalParameter names occuring
 * in that list.
 */
static origsymbol_list collect_FormalParameter_names( origsymbol_list ans, const_FormalParameter_list l )
{

    ans = setroom_origsymbol_list( ans, ans->sz+l->sz );
    for( unsigned int ix=0; ix<l->sz; ix++ ){
	origsymbol s = rdup_origsymbol( l->arr[ix]->name );
	ans = append_origsymbol_list( ans, s );
	assert( s->sym != tmsymbolNIL );
    }
    return ans;
}

// Given a list of declarations, collect the symbols symbols of declared
// variables.
origsymbol_list collect_vardeclaration_names( origsymbol_list nms, const_statement_list l )
{
    if( l == statement_listNIL ){
	return nms;
    }
    for( unsigned int ix=0; ix<l->sz; ix++ ){
	statement smt = l->arr[ix];

	if( smt != statementNIL && is_VariableDeclaration( smt ) ){
	    nms = append_origsymbol_list(
		nms,
		rdup_origsymbol( to_VariableDeclaration( smt )->name )
	    );
	}
    }
    return nms;
}

/* Given a Cardinality list, return the symbols that are used in it. */
origsymbol_list collect_Cardinality_symbols( const_Cardinality_list l )
{
    origsymbol_list ans = new_origsymbol_list();

    ans = setroom_origsymbol_list( ans, l->sz );
    for( unsigned int ix=0; ix<l->sz; ix++ ){
	origsymbol s = rdup_origsymbol( l->arr[ix]->name );
	ans = append_origsymbol_list( ans, s );
	assert( s->sym != tmsymbolNIL );
    }
    return ans;
}

/* Given a function definition, return all the local symbols in
 * that definition. We assume the declarations are correct.
 */
origsymbol_list collect_function_names(
 const_FormalParameter_list args,
 const_statement_list ldecl
)
{
    origsymbol_list locals;

    locals = new_origsymbol_list();
    if( args != FormalParameter_listNIL ){
	locals = collect_FormalParameter_names( locals, args );
    }
    if( ldecl != statement_listNIL ){
	locals = collect_vardeclaration_names( locals, ldecl );
    }
    return locals;
}

/* Given a list of labeled statements, return a list of labels
 * of these statements.
 */
origsymbol_list collect_labels( const_statement_list l )
{
    origsymbol_list ans = new_origsymbol_list();

    for( unsigned int ix=0; ix<l->sz; ix++ ){
	const_statement s = l->arr[ix];

	if( s != statementNIL ){
	    origsymbol_list labels = s->labels;

	    if( labels != origsymbol_listNIL ){
		ans = concat_origsymbol_list(
		    ans,
		    rdup_origsymbol_list( labels )
		);
	    }
	}
    }
    return ans;
}

/* Collect class or interface fields, in the order they appear. */
Field_list collect_type_fields( const_statement_list sl, bool force_static )
{
    Field_list res;
    unsigned int ix;
    modflags mask = 0;

    if( force_static ){
	mask = ACC_STATIC;
    }
    res = setroom_Field_list( new_Field_list(), sl->sz );
    for( ix=0; ix<sl->sz; ix++ ){
	statement s = sl->arr[ix];

	if( is_VariableDeclaration( s ) ){
	    res = append_Field_list(
		res,
		new_Field(
		    mask | to_declaration(s)->flags,
		    to_VariableDeclaration( s )->name->sym
		)
	    );
	}
    }
    return res;
}

// Given a MethodDeclaration, construct a Signature for this
// method.
Signature construct_Method_Signature( const_MethodDeclaration m )
{
    type_list formals = derive_type_FormalParameter_list(
	m->parameters
    );
    modflags flags = m->flags;

    return new_Signature( flags, rdup_tmsymbol( m->name->sym ), formals );
}

/* Collect class methods, in the order they appear. If force_static
 * is true, set the ACC_STATIC flag in all collected signatures.
 */
Signature_list collect_type_methods( const_statement_list sl )
{
    Signature_list res = setroom_Signature_list( new_Signature_list(), sl->sz );
    for( unsigned int ix=0; ix<sl->sz; ix++ ){
	const_statement smt = sl->arr[ix];

	if( is_MethodDeclaration( smt ) && smt->tag != TAGConstructorDeclaration ){
	    Signature s = construct_Method_Signature( to_const_MethodDeclaration( smt ) );

	    res = append_Signature_list( res, s );
	}
    }
    return res;
}

/* Collect class constructors. */
Signature_list collect_class_constructors( const_statement_list sl )
{
    Signature_list res = new_Signature_list();
    for( unsigned ix=0; ix<sl->sz; ix++ ){
	const_statement s = sl->arr[ix];

	if( s->tag == TAGConstructorDeclaration ){
	    Signature sig = construct_Method_Signature( to_const_ConstructorDeclaration(s) );

	    res = append_Signature_list( res, sig );
	}
    }
    return res;
}

// Given a list of types and a type name, return all the
// interfaces of this class in depth-first order. That is, a super-interface
// is listed before the class or interface that refer to it.
static inline tmsymbol_list collect_interfaces(
 const_TypeEntry_list types,
 origsymbol_list callstack,
 tmsymbol_list all,
 tmsymbol nm
){
    if( nm == tmsymbolNIL ){
	return all;
    }
    TypeEntry e = lookup_TypeEntry( types, nm );
    return collect_interfaces( types, callstack, all, e );
}

// Given a list of types and a type name, return all the
// interfaces of this class in depth-first order. That is, a super-interface
// is listed before the class or interface that refer to it.
static inline tmsymbol_list collect_interfaces(
 const_TypeEntry_list types,
 origsymbol_list callstack,
 tmsymbol_list all,
 const_origsymbol nm
){
    if( nm == origsymbolNIL ){
	return all;
    }
    return collect_interfaces( types, callstack, all, nm->sym );
}

// Given a list of types and a type name, return all the
// interfaces of this class in depth-first order. That is, a super-interface
// is listed before the class or interface that refer to it.
static inline tmsymbol_list collect_interfaces(
 const_TypeEntry_list types,
 origsymbol_list callstack,
 tmsymbol_list all,
 const_type t
)
{
    if( t == typeNIL ){
	return all;
    }
    assert( t->tag == TAGObjectType );
    return collect_interfaces( types, callstack, all, to_const_ObjectType(t)->name );
}

// Given a list of types and an entry in this list, return all the
// interfaces of this class in depth-first order. That is, a super-interface
// is listed before the class or interface that refer to it.
tmsymbol_list collect_interfaces(
 const_TypeEntry_list types,
 origsymbol_list callstack,
 tmsymbol_list all,
 TypeEntry e
)
{

    origsymbol_list my_callstack;
    
    if( callstack == origsymbol_listNIL ){
	my_callstack = new_origsymbol_list();
    }
    else {
	my_callstack = rdup_origsymbol_list( callstack );

	if( member_origsymbol_list( callstack, e->name ) ){
	    if( e->tag == TAGClassEntry ){
		origsymbol_error( e->name, "Class is its own superclass" );
		rfre_type( to_ClassEntry( e )->super );
		to_ClassEntry( e )->super = typeNIL;
	    }
	    else {
		origsymbol_error( e->name, "Interface is its own superinterface" );
		rfre_type_list( to_InterfaceEntry( e )->interfaces );
		to_InterfaceEntry( e )->interfaces = type_listNIL;
	    }
	    return all;
	}
    }
    my_callstack = append_origsymbol_list(
	my_callstack,
	rdup_origsymbol( e->name )
    );
    switch( e->tag ){
	case TAGClassEntry:
	{
	    const_type_list l = to_const_ClassEntry( e )->interfaces;

	    const_type tsuper = to_const_ClassEntry( e )->super;
	    all = collect_interfaces( types, my_callstack, all, tsuper );
	    for( unsigned int ix=0; ix<l->sz; ix++ ){
		const_type t = l->arr[ix];
		if( t != typeNIL && t->tag == TAGObjectType ){
		    tmsymbol s = to_const_ObjectType(t)->name->sym;
		    if( !member_tmsymbol_list( all, s ) ){
			all = collect_interfaces( types, my_callstack, all, s );
			all = append_tmsymbol_list( all, rdup_tmsymbol( s ) );
		    }
		}
	    }
	    break;
	}

	case TAGInterfaceEntry:
	{
	    const_type_list l = to_const_InterfaceEntry( e )->interfaces;

	    for( unsigned int ix=0; ix<l->sz; ix++ ){
		const_type t = l->arr[ix];
		if( t->tag == TAGObjectType ){
		    tmsymbol s = to_const_ObjectType(t)->name->sym;
		    if( !member_tmsymbol_list( all, s ) ){
			all = collect_interfaces( types, my_callstack, all, s );
			all = append_tmsymbol_list( all, rdup_tmsymbol( s ) );
		    }
		}
	    }
	    break;
	}

	case TAGPackageEntry:
	case TAGForwardObjectEntry:
	    break;
    }
    rfre_origsymbol_list( my_callstack );
    return all;
}
