/* File: symbol_table.c
 *
 * Handling of the symbol table.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <tmc.h>

#include "defs.h"
#include "vnus_types.h"
#include "tmadmin.h"
#include "global.h"
#include "error.h"
#include "prettyprint.h"
#include "symbol_table.h"
#include "typederive.h"

static vnusdeclaration_list symboltable = vnusdeclaration_listNIL;

/* Given a tmsymbol 'name', return the vnusdeclaration for that name.
 * Since the symbol table is supposed to describe every identifier
 * in the program, the function can (in principle) not fail.
 * If it does, it is treated as an internal error.
 */
vnusdeclarationdata search_vnusdeclaration( const tmsymbol name )
{
    unsigned int ix;
    vnusdeclaration e;

    if( symboltable == vnusdeclaration_listNIL ){
	return vnusdeclarationdataNIL;
    }
    for( ix = 0; ix<symboltable->sz; ix++ ){
	e = symboltable->arr[ix];
	if( name == e->name->sym ){
	    return e->data;
	}
    }
    return vnusdeclarationdataNIL;
}

/* Given a tmsymbol 'name', return the vnusdeclaration for that name.
 * Since the symbol table is supposed to describe every identifier
 * in the program, the function can (in principle) not fail.
 * If it does, it is treated as an internal error.
 */
vnusdeclarationdata lookup_vnusdeclaration( const tmsymbol name )
{
    vnusdeclarationdata e;

    e = search_vnusdeclaration( name );
    if( e == vnusdeclarationdataNIL ){
	sprintf( errarg, "identifier '%s'", name->name );
	internal_botch( "Not in symbol table" );
    }
    return e;
}

/* Given a name 'name' that is supposed to be of a function,
 * return the return type of that function. The returned type
 * expression is still owned by the symbol table.
 */
type lookup_fntype( const origsymbol name )
{
    vnusdeclarationdata d; 

    d = lookup_vnusdeclaration( name->sym );
    if( d == vnusdeclarationdataNIL ){
	return typeNIL;
    }
    switch( d->tag ){
        case TAGVDeclFunction:
	    return to_VDeclFunction(d)->t;

        case TAGVDeclExternalFunction:
	    return to_VDeclExternalFunction(d)->t;

	default:
	    break;
    }
    return typeNIL;
}

/* Given a name 'name' that is supposed to be a formal parameter,
 * return the type of that formal. The returned type
 * expression is still owned by the symbol table.
 */
type lookup_parmtype( const origsymbol name )
{
    vnusdeclarationdata d; 

    d = lookup_vnusdeclaration( name->sym );
    if( d == vnusdeclarationdataNIL ){
	return typeNIL;
    }
    if( d->tag != TAGVDeclFormalVariable ){
	origin_error( name, "not a formal parameter" );
	return typeNIL;
    }
    return to_VDeclFormalVariable(d)->t;
}

/* Given a name 'name' that is supposed to be a variable,
 * return a copy of the type of that variable.
 */
type lookup_vartype( const origsymbol name )
{
    vnusdeclarationdata d;

    d = lookup_vnusdeclaration( name->sym );
    switch( d->tag ){
	case TAGVDeclCardinalityVariable:
	    return new_TypeBase( BT_INT );

        case TAGVDeclReturnVariable:
	    return rdup_type( to_VDeclReturnVariable(d)->t );

        case TAGVDeclGlobalVariable:
	    return rdup_type( to_VDeclGlobalVariable(d)->t );

        case TAGVDeclExternalVariable:
	    return rdup_type( to_VDeclExternalVariable(d)->t );

        case TAGVDeclLocalVariable:
	    return rdup_type( to_VDeclLocalVariable(d)->t );

        case TAGVDeclFormalVariable:
	    return rdup_type( to_VDeclFormalVariable(d)->t );

        case TAGVDeclFunction:
	    return new_TypeFunction(
		derive_type_formalParameter_list( to_VDeclFunction(d)->parms ),
		rdup_type( to_VDeclFunction(d)->t )
	    );

        case TAGVDeclProcedure:
	    return new_TypeProcedure(
		derive_type_formalParameter_list( to_VDeclProcedure(d)->parms )
	    );

        case TAGVDeclExternalFunction:
	    return new_TypeFunction(
		derive_type_formalParameter_list( to_VDeclExternalFunction(d)->parms ),
		rdup_type( to_VDeclExternalFunction(d)->t )
	    );

        case TAGVDeclExternalProcedure:
	    return new_TypeProcedure(
		derive_type_formalParameter_list( to_VDeclExternalProcedure(d)->parms )
	    );
    }
    return typeNIL;
}

/* Given a symbol 's', assume that it is the name of a function of
 * procedure, and return the list of formal parameters of it.
 */
const formalParameter_list lookup_formalParameter_list( const origsymbol s )
{
    formalParameter_list formals;
    vnusdeclarationdata d;

    d = search_vnusdeclaration( s->sym );
    if( d == vnusdeclarationdataNIL ){
        origin_error( s, "unknown symbol" );
	return formalParameter_listNIL;
    }
    formals = formalParameter_listNIL;
    switch( d->tag ){
	case TAGVDeclReturnVariable:
	case TAGVDeclGlobalVariable:
	case TAGVDeclExternalVariable:
	case TAGVDeclFormalVariable:
	case TAGVDeclLocalVariable:
	case TAGVDeclCardinalityVariable:
	    origin_error( s, "not a function or procedure" );
	    break;

	case TAGVDeclFunction:
	    formals = to_VDeclFunction(d)->parms;
	    break;

	case TAGVDeclProcedure:
	    formals = to_VDeclProcedure(d)->parms;
	    break;

	case TAGVDeclExternalFunction:
	    formals = to_VDeclExternalFunction(d)->parms;
	    break;

	case TAGVDeclExternalProcedure:
	    formals = to_VDeclExternalProcedure(d)->parms;
	    break;

    }
    return formals;
}

void add_symtab_returnvar(
 const origsymbol nm,
 const tmsymbol scope,
 const pragma_list pragmas,
 const type t
)
{
    vnusdeclarationdata d;
    vnusdeclaration e;

    if( tracesymtab ){
        fprintf(
            stderr,
            "Add to symtab: return variable '%s' (%s)\n",
            nm->sym->name,
            scope->name 
        );
    }
    d = new_VDeclReturnVariable( rdup_tmsymbol( scope ), rdup_type( t ) );
    e = new_vnusdeclaration( rdup_origsymbol( nm ), rdup_pragma_list( pragmas ), d );
    symboltable = append_vnusdeclaration_list( symboltable, e );
}

void add_symtab_externalvar( origsymbol nm, pragma_list pragmas, type t )
{
    vnusdeclarationdata d;
    vnusdeclaration e;

    if( tracesymtab ){
        fprintf(
            stderr,
            "Add to symtab: external variable '%s'\n",
            nm->sym->name
        );
    }
    d = new_VDeclExternalVariable( rdup_type( t ) );
    e = new_vnusdeclaration( rdup_origsymbol( nm ), rdup_pragma_list( pragmas ), d );
    symboltable = append_vnusdeclaration_list( symboltable, e );
}

void add_symtab_globalvar(
 const origsymbol nm,
 const pragma_list pragmas,
 const type t,
 const optexpression init
)
{
    vnusdeclarationdata d;
    vnusdeclaration e;

    if( tracesymtab ){
        fprintf(
            stderr,
            "Add to symtab: global variable '%s'\n",
            nm->sym->name
        );
    }
    d = new_VDeclGlobalVariable( rdup_type( t ), rdup_optexpression( init ) );
    e = new_vnusdeclaration( rdup_origsymbol( nm ), rdup_pragma_list( pragmas ), d );
    symboltable = append_vnusdeclaration_list( symboltable, e );
}

void add_symtab_localvar(
 const origsymbol nm,
 const tmsymbol scope,
 const pragma_list pragmas,
 const type t,
 const optexpression init
)
{
    vnusdeclarationdata d;
    vnusdeclaration e;

    if( tracesymtab ){
        fprintf(
            stderr,
            "Add to symtab: local variable '%s' (%s)\n",
            nm->sym->name,
            scope->name 
        );
    }
    d = new_VDeclLocalVariable( rdup_tmsymbol( scope ), rdup_type( t ), rdup_optexpression( init ) );
    e = new_vnusdeclaration( rdup_origsymbol( nm ), rdup_pragma_list( pragmas ), d );
    symboltable = append_vnusdeclaration_list( symboltable, e );
}

void add_symtab_formal(
 origsymbol nm,
 tmsymbol scope,
 pragma_list pragmas,
 type t
)
{
    vnusdeclarationdata d;
    vnusdeclaration e;

    if( tracesymtab ){
        fprintf(
            stderr,
            "Add to symtab: formal parameter '%s' (%s)\n",
            nm->sym->name,
            scope->name 
        );
    }
    d = new_VDeclFormalVariable( rdup_tmsymbol( scope ), rdup_type( t ) );
    e = new_vnusdeclaration( rdup_origsymbol( nm ), rdup_pragma_list( pragmas ), d );
    symboltable = append_vnusdeclaration_list( symboltable, e );
}

void add_symtab_cardvar( origsymbol nm, pragma_list pragmas )
{
    vnusdeclarationdata d;
    vnusdeclaration e;

    if( tracesymtab ){
        fprintf(
            stderr,
            "Add to symtab: cardinality variable '%s'\n",
            nm->sym->name
        );
    }
    d = new_VDeclCardinalityVariable();
    e = new_vnusdeclaration( rdup_origsymbol( nm ), rdup_pragma_list( pragmas ), d );
    symboltable = append_vnusdeclaration_list( symboltable, e );
}

void add_symtab_function(
 const pragma_list pragmas,
 const origsymbol nm,
 const formalParameter_list args,
 const origsymbol valnm,
 const type valtype,
 const block body
)
{
    vnusdeclarationdata d;
    vnusdeclaration e;

    if( tracesymtab ){
        fprintf(
            stderr,
            "Add to symtab: function '%s'\n",
            nm->sym->name
        );
    }
    d = new_VDeclFunction(
	rdup_formalParameter_list( args ),
	rdup_origsymbol( valnm ),
        rdup_type( valtype ),
	rdup_block( body )
    );
    e = new_vnusdeclaration( rdup_origsymbol( nm ), rdup_pragma_list( pragmas ), d );
    symboltable = append_vnusdeclaration_list( symboltable, e );
}

void add_symtab_procedure(
 const pragma_list pragmas,
 const origsymbol nm,
 const formalParameter_list args,
 const block body
)
{
    vnusdeclarationdata d;
    vnusdeclaration e;

    if( tracesymtab ){
        fprintf(
            stderr,
            "Add to symtab: procedure '%s'\n",
            nm->sym->name
        );
    }
    d = new_VDeclProcedure( rdup_formalParameter_list( args ), rdup_block( body ) );
    e = new_vnusdeclaration( rdup_origsymbol( nm ), rdup_pragma_list( pragmas ), d );
    symboltable = append_vnusdeclaration_list( symboltable, e );
}

void add_symtab_external_function(
 pragma_list pragmas,
 origsymbol nm,
 formalParameter_list args,
 type valtype
)
{
    vnusdeclarationdata d;
    vnusdeclaration e;

    if( tracesymtab ){
        fprintf(
            stderr,
            "Add to symtab: external function '%s'\n",
            nm->sym->name
        );
    }
    d = new_VDeclExternalFunction(
	rdup_formalParameter_list( args ),
        rdup_type( valtype )
    );
    e = new_vnusdeclaration( rdup_origsymbol( nm ), rdup_pragma_list( pragmas ), d );
    symboltable = append_vnusdeclaration_list( symboltable, e );
}

void add_symtab_external_procedure(
 pragma_list pragmas,
 origsymbol nm, 
 formalParameter_list args
)
{
    vnusdeclarationdata d;
    vnusdeclaration e;

    if( tracesymtab ){
        fprintf(
            stderr,
            "Add to symtab: external procedure '%s'\n",
            nm->sym->name
        );
    }
    d = new_VDeclExternalProcedure( rdup_formalParameter_list( args ) );
    e = new_vnusdeclaration( rdup_origsymbol( nm ), rdup_pragma_list( pragmas ), d );
    symboltable = append_vnusdeclaration_list( symboltable, e );
}

/* Given a list of entries, copy it to make it the new symbol table. */
void set_symbol_table( vnusdeclaration_list symtab )
{
    if( symboltable != vnusdeclaration_listNIL ){
	rfre_vnusdeclaration_list( symboltable );
    }
    symboltable = rdup_vnusdeclaration_list( symtab );
}

/* Return a copy of the symbol table. */
vnusdeclaration_list get_symbol_table( void )
{
    return rdup_vnusdeclaration_list( symboltable );
}

void init_symbol_table( void )
{
    if( symboltable != vnusdeclaration_listNIL ){
	rfre_vnusdeclaration_list( symboltable );
    }
    symboltable = new_vnusdeclaration_list();
}

void end_symbol_table( void )
{
    rfre_vnusdeclaration_list( symboltable );
    symboltable = vnusdeclaration_listNIL;
}
