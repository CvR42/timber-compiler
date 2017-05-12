// File: inliner.c
//
// Do inlining of a given method.

#include <tmc.h>
#include <assert.h>

#include "defs.h"
#include "tmadmin.h"
#include "global.h"
#include "service.h"
#include "error.h"
#include "inliner.h"
#include "analyze.h"
#include "isconstant.h"
#include "substitute.h"
#include "symbol_table.h"
#include "getscopes.h"
#include "dump.h"
#include "noreturn.h"


// Given a tmsymbol representing a scope name, return a 
// scope translation to map the scope name to an other, newly
// generate, name.
static NameTranslation build_NameTranslation( tmsymbol s, const char *role )
{
    tmsymbol nws = gen_tmsymbol( role );

    if( trace_inlining ){
	fprintf( stderr, "Added scope translation %s->%s\n", s->name, nws->name );
    }
    return new_NameTranslation( s, nws );
}

// Given a list of tmsymbols representing scope names, return a list of 
// scope translations to map these scope names to other, newly
// generated, names.
static NameTranslation_list build_NameTranslation_list(
 tmsymbol_list l,
 const char *role
)
{
    NameTranslation_list res = setroom_NameTranslation_list(
	new_NameTranslation_list(),
	l->sz
    );
    for( unsigned int ix=0; ix<l->sz; ix++ ){
	res = append_NameTranslation_list(
	    res,
	    build_NameTranslation( l->arr[ix], role )
	);
    }
    return res;
}

// Given an expression 'x', return true iff the expression is better propagated
// instead of abstracted.
// If we are considering an expression that will end up as a this parameter
// expression, we permit a bit more, since that way we are likely to avoid null
// pointer checks and GC administration.
static bool is_inlinable_parameter( const_Entry_list symtab, const_expression x )
{
    if( x == expressionNIL ){
	return false;
    }
    if( is_constant( x ) ){
	return true;
    }
    if( x->tag == TAGVariableNameExpression ){
	if( has_any_flag( to_const_VariableNameExpression(x)->flags, VAR_THIS ) ){
	    return true;
	}
	origsymbol nm = to_const_VariableNameExpression(x)->name;

	VariableEntry e = lookup_VariableEntry( symtab, nm->sym );
	return (e->tag != TAGGlobalVariableEntry) || 
	    has_any_flag( e->flags, ACC_FINAL|ACC_DEDUCED_FINAL|ACC_PRIVATE );
    }
    else if( x->tag == TAGNotNullAssertExpression ){
	return is_inlinable_parameter( symtab, to_const_NotNullAssertExpression(x)->x );
    }
    else if( x->tag == TAGVectorExpression ){
	expression_list xl = to_const_VectorExpression(x)->fields;

	for( unsigned int ix=0; ix<xl->sz; ix++ ){
	    if( !is_inlinable_parameter( symtab, xl->arr[ix] ) ){
		return false;
	    }
	}
	return true;
    }
    // TODO: find out why this can end up as an assignment lhs, when
    // the formal parameter it substitutes is final or deduced final.
    // Unfortunately, this expression could end up as a assignment lhs,
    // and we can't have a cast there.
    else if( x->tag == TAGCastExpression ){
	return is_inlinable_parameter( symtab, to_const_CastExpression(x)->x );
    }
    return false;
}

// Given a formal parameter and an actual parameter, generate a
// new name for the formal parameter, construct a declaration that
// declares the renamed formal parameter as a variable that is initialized
// with the actual parameter, and add the substitution to the list
// of abstractions.
//
// Type parameters and final parameters with trivial actuals do not cause
// a local declaration, but an immediate substitution.
static void build_parameter_declaration(
 Entry_list symtab,
 const_expression actual,
 const_FormalParameter formal,
 tmsymbol *scopenm,
 ValueAbstraction_list vl,
 NameTranslation_list var_trans,
 statement_list decls
)
{
    ValueAbstraction a;

    assert( formal->t->tag != TAGTypeType );
    if(
	has_any_flag( formal->flags, ACC_FINAL|ACC_DEDUCED_FINAL|ACC_PRIVATE ) &&
	is_inlinable_parameter( symtab, actual )
    ){
	// We might as well propagate this actual value directly,
	// since it is constant and won't be overwritten.
	if( trace_inlining ){
	    fprintf( stderr, "Inlined parameter '%s' is bound to inlinable actual\n", formal->name->sym->name );
	}
	a = new_ValueAbstraction(
	    rdup_tmsymbol( formal->name->sym ),
	    rdup_expression( actual )
	);
	vl = append_ValueAbstraction_list( vl, a );
    }
    else {
	tmsymbol subsname = gen_tmsymbol( formal->name->sym->name );
	if( trace_inlining ){
	    fprintf(
		stderr,
		"Inlined parameter '%s' is translated to local var '%s'\n",
		formal->name->sym->name,
		subsname->name
	    );
	}
	var_trans = append_NameTranslation_list(
	    var_trans,
	    new_NameTranslation( formal->name->sym, subsname )
	);
	FieldDeclaration dcl = new_FieldDeclaration(
	    originNIL,
	    Pragma_listNIL,
	    origsymbol_listNIL,		// Labels
	    ACC_FINAL,			// Flags
	    true,			// used
	    add_origsymbol( subsname ),
	    rdup_type( formal->t ),
	    new_OptExpr( rdup_expression( actual ) )
	);
	decls = append_statement_list( decls, dcl );
	if( *scopenm == tmsymbolNIL ){
	    *scopenm = gen_tmsymbol( "scope" );
	}
	LocalVariableEntry fd = new_LocalVariableEntry(
	    subsname,
	    Pragma_listNIL,
	    formal->flags,
	    tmsymbolNIL,
	    tmsymbolNIL,
	    1L,			// usecount
	    rdup_origsymbol( formal->name ),
	    *scopenm,
	    rdup_type( formal->t ),
	    new_OptExpr( rdup_expression( actual ) )
	);
	symtab = add_symtab( symtab, fd );
    }
}

// Given a formal parameter list  and an actual parameter list, generate a
// new name for the formal parameter, construct a declaration that
// declares the renamed formal parameter as a variable that is initialized
// with the actual parameter, and add the substitution to the list
// of abstractions.
//
// Type parameters and final parameters with trivial actuals do not cause
// a local declaration, but an immediate substitution.
static void build_parameter_declarations(
 Entry_list symtab,
 tmsymbol nm,
 const_expression_list actuals,
 const_FormalParameter_list formals,
 tmsymbol *scopenm,
 ValueAbstraction_list vl,
 NameTranslation_list var_trans,
 statement_list decls
)
{
    if( formals->sz != actuals->sz ){
	internal_error(
	    "Formal and actual parameter list of inlined fn '%s' differ in length (%u vs. %u)",
	    nm->name,
	    formals->sz,
	    actuals->sz
	);
    }
    for( unsigned int ix=0; ix<formals->sz; ix++ ){
	build_parameter_declaration(
	    symtab,
	    actuals->arr[ix],
	    formals->arr[ix],
	    scopenm,
	    vl,
	    var_trans,
	    decls
	);
    }
}

// Given an entry in the symbol table, if it is a local symbol in one of
// the scopes listed in the scope translation list 'sl', add a
// copy to the symbol table. The copy is mutated to use
// the translated scope name, and a new variable name is also introduced.
// A translation from the old name to the new name is added to the
// variable translation list 'var_trans'.
static void build_local_mapping(
 const_Entry e, 
 Entry_list symtab, 
 const_ValueAbstraction_list vl,
 const_NameTranslation_list sl,
 NameTranslation_list var_trans
)
{
    switch( e->tag ){
	case TAGFunctionEntry:
	case TAGExternalFunctionEntry:
	case TAGAbstractFunctionEntry:
	case TAGForwardFunctionEntry:
	case TAGFieldEntry:
	case TAGForwardFieldEntry:
	case TAGGlobalVariableEntry:
	case TAGGCLinkEntry:
	case TAGGCTopLinkEntry:
	    break;

	case TAGFormalParameterEntry:
	{
	    tmsymbol scope = to_const_FormalParameterEntry(e)->scope;
	    tmsymbol s1 = translate_name( sl, scope );
	    if( s1 != tmsymbolNIL ){
		// This entry was in a scope of the code we are cloning.
		// Make a new symbol table entry in the translated scope,
		// think up a new name for the cloned entry, and add a
		// translation rule to translate from the old name to
		// the new one.
		Entry nw_e = rdup_Entry( e );
		FormalParameterEntry fe = to_FormalParameterEntry(nw_e);

		fe->scope = s1;
		fe->name = gen_tmsymbol( fe->name->name );
		symtab = append_Entry_list( symtab, fe );
		var_trans = append_NameTranslation_list(
		    var_trans,
		    new_NameTranslation( e->name, fe->name )
		);
		if( trace_inlining ){
		    fprintf(
			stderr,
			"Duplicated formal variable %s in %s to %s in %s\n",
			e->name->name,
			scope->name,
			fe->name->name,
			s1->name
		    );
		}
	    }
	    break;
	}

	case TAGCardinalityVariableEntry:
	    // TODO: we should do something, but we cannot know wethere
	    // the variable is in one of the blocks we are treating.
	    break;

	case TAGLocalVariableEntry:
	{
	    tmsymbol scope = to_const_LocalVariableEntry(e)->scope;
	    tmsymbol s1 = translate_name( sl, scope );
	    if( s1 != tmsymbolNIL ){
		// This entry was in a scope of the code we are cloning.
		// Make a new symbol table entry in the translated scope,
		// think up a new name for the cloned entry, and add a
		// translation rule to translate from the old name to
		// the new one.
		Entry nw_e = rdup_Entry( e );
		LocalVariableEntry fe = to_LocalVariableEntry(nw_e);
		fe->init = substitute_optexpression( fe->init, vl, sl, var_trans );

		fe->scope = s1;
		fe->name = gen_tmsymbol( fe->name->name );
		symtab = append_Entry_list( symtab, fe );
		var_trans = append_NameTranslation_list(
		    var_trans,
		    new_NameTranslation( e->name, fe->name )
		);
		if( trace_inlining ){
		    fprintf(
			stderr,
			"Duplicated local variable %s in %s to %s in %s\n",
			e->name->name,
			scope->name,
			fe->name->name,
			s1->name
		    );
		}
	    }
	    break;
	}

    }
}

// Given a symbol table, copy all entries that are local symbols in one of
// the scopes listed in the scope translation list 'sl', add the
// copies to the symbol table. The copies are mutated to use
// the translated scope names, and new variable names are also introduced.
// Translations from the old variable names to the new names are added to the
// value abstraction list 'vl'.
static void build_local_mappings(
 Entry_list symtab, 
 ValueAbstraction_list vl,
 const_NameTranslation_list sl,
 NameTranslation_list var_trans
)
{
    const unsigned int sz = symtab->sz;

    for( unsigned int ix=0; ix<sz; ix++ ){
	build_local_mapping( symtab->arr[ix], symtab, vl, sl, var_trans );
    }
}

// Given a method entry 'e', a symbol table and a list of formal and
// actual parameters, return a block that represents the inlined
// version of the method.
Block inline_method(
 Entry_list symtab,
 const_MethodEntry e,
 const_origin org,
 const_expression_list this_parms,
 const_expression_list parms,
 tmsymbol retvar
)
{
    if( e->tag != TAGFunctionEntry ){
	internal_error( "Only a FunctionEntry can be inlined" );
    }
    Block res = rdup_Block( to_const_FunctionEntry(e)->body );
    tmsymbol_list scopes = new_tmsymbol_list();
    tmsymbol_list labels = new_tmsymbol_list();
    getscopes_Block( res, scopes, labels );
    ValueAbstraction_list vl = new_ValueAbstraction_list();
    NameTranslation_list sl = build_NameTranslation_list( scopes, "scope" );
    NameTranslation_list ll = build_NameTranslation_list( labels, "label" );
    NameTranslation_list var_trans = new_NameTranslation_list();
    statement_list decls = new_statement_list();
    res->scope = translate_name( sl, res->scope );
    build_parameter_declarations(
	symtab,
	e->name,
	parms,
	e->parameters,
	&res->scope,
	vl,
	var_trans,
	decls
    );
    if( this_parms == expression_listNIL ){
	assert( e->thisparameters->sz == 0 );
    }
    else {
	build_parameter_declarations(
	    symtab,
	    e->name,
	    this_parms,
	    e->thisparameters,
	    &res->scope,
	    vl,
	    var_trans,
	    decls
	);
    }

    build_local_mappings( symtab, vl, sl, var_trans );
    rfre_tmsymbol_list( scopes );
    rfre_tmsymbol_list( labels );
    res = substitute_Block( org, res, TypeAbstraction_listNIL, ll, vl, sl, var_trans );
    rfre_ValueAbstraction_list( vl );
    rfre_NameTranslation_list( sl );
    rfre_NameTranslation_list( ll );
    rfre_NameTranslation_list( var_trans );
    res->statements = insertlist_statement_list(
	res->statements,
	0,
	decls
    );
    res = noreturn_Block( org, res, retvar );
    if( trace_inlining ){
	dump_Block( stderr, res, "result of inline_method on %s", e->name->name  );
    }
    return res;
}
