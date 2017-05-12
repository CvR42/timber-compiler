/* File: parser.y */

%{
#include <stdlib.h>
#include <stdio.h>
#include <tmc.h>
#include <assert.h>
#include <string.h>

#include "defs.h"
#include "tmadmin.h"
#include "error.h"
#include "lex.h"
#include "parser.h"
#include "check.h"
#include "global.h"
#include "symbol_table.h"
#include "prettyprint.h"
#include "service.h"

/* Always allow parse trace, since it is switchable from the
 * command line.
 */
#define YYDEBUG 1

static SparProgramUnit result;

// A stack with the class modifiers of all pending classes and interfaces.
// For the moment this stack is only used to help in the check that inlined
// classes are static or final.
static modflags_list class_modifiers_stack = modflags_listNIL;

/* The required error handler for yacc. */
static void yyerror( const char *s )
{
    if( strcmp( s, "parse error" ) == 0 ){
	parserror( "syntax error" );
    }
    else {
	parserror( s );
    }
}

static void push_class_modifiers( const modflags flags )
{
    class_modifiers_stack = append_modflags_list( class_modifiers_stack, flags );
}

static void pop_class_modifiers()
{
    assert( class_modifiers_stack->sz != 0 );
    class_modifiers_stack->sz--;
}

static modflags get_top_class_modifiers()
{
    assert( class_modifiers_stack->sz != 0 );
    return class_modifiers_stack->arr[class_modifiers_stack->sz-1];
}

/* Given a group of modifiers, complain if more than one of the modifiers
 * public, protected and private are set.
 */
static modflags check_access_modifiers( origin org, modflags flags )
{
    const modflags mask = ACC_PUBLIC|ACC_PRIVATE|ACC_PROTECTED;
    modflags sel;

    sel = flags & mask;
    if( sel == 0 || sel == ACC_PUBLIC || sel == ACC_PRIVATE || sel == ACC_PROTECTED ){
	return flags;
    }
    if( org == originNIL ){
	parserror( "only one of the modifiers `public', `protected', and `private' should be given" );
    }
    else {
	origin_error( org, "only one of the modifiers `public', `protected', and `private' should be given" );
    }
    /* Force the access modifiers to `public'. */
    flags &= ~mask;
    flags |= ACC_PUBLIC;
    return flags;
}

/* Given a list of Imports, create two lists, one with direct imports,
 * one with on demand imports.
 *
 * Always add the on-demand package `java.lang', and remove duplicates
 * from both lists without complaint.
 */
static void separate_imports( Import_list imps, origsymbol_list *directs, origsymbol_list *ondemands )
{
    *directs = new_origsymbol_list();
    *ondemands = new_origsymbol_list();
    *ondemands = append_origsymbol_list(
	*ondemands,
	new_origsymbol(
	    add_tmsymbol( "java.lang" ),
	    gen_origin()
	)
    );
    for( unsigned int ix=0; ix<imps->sz; ix++ ){
	Import imp = imps->arr[ix];

	if( imp != ImportNIL ){
	    switch( imp->tag ){
		case TAGDirectImport:
		    if( !member_origsymbol_list( *directs, imp->name ) ){
			*directs = append_origsymbol_list(
			    *directs,
			    rdup_origsymbol( imp->name )
			);
		    }
		    break;

		case TAGOnDemandImport:
		    if( !member_origsymbol_list( *ondemands, imp->name ) ){
			*ondemands = append_origsymbol_list( *ondemands, rdup_origsymbol( imp->name ) );
		    }
		    break;
	    }
	}
    }
}

// Given a type `t' and a list of pragmas `pragmas', wrap the type
// with a PragmaType if this is useful.
static type wrap_pragmas_type( type t, Pragma_list pragmas )
{
    type res;

    if( pragmas == Pragma_listNIL || pragmas->sz == 0 ){
	 res = t;
	 if( pragmas != Pragma_listNIL ){
	     rfre_Pragma_list( pragmas );
	 }
    }
    else {
	res = new_PragmaType( pragmas, t );
    }
    return res;
}

// Given an element type and a Dim structure, unwrap it.
static type unwrap_array_FormalDim(
    type elmtype,
    FormalDim dim
)
{
    type res = new_ArrayType( elmtype, dim->rnk );
    res = wrap_pragmas_type( res, dim->pragmas );
    fre_FormalDim( dim );
    return res;
}

/* Given a type and a list of ranks, return a series of nested array type
 * representing this. Upon return, the ownership of both parameters is
 * lost to the caller.
 */
static type wrap_ranked_type( type t, FormalDim_list ranks )
{
    unsigned int ix = ranks->sz;

    while( ix != 0 ){
	ix--;
	t = unwrap_array_FormalDim( t, rdup_FormalDim( ranks->arr[ix] ) );
    }
    rfre_FormalDim_list( ranks );
    return t;
}

/* Given a type `tin' and a varid `id', convert all size lists in `id' to
 * wrapped types of `tin'. Assign the naked variable name to `*s', and
 * the wrapped type to `tout'.
 */
static void unwrap_varid( type tin, varid id, type *tout, origsymbol *s, optexpression *init )
{
    *tout = wrap_ranked_type( tin, id->wrap );
    *s = id->name;
    *init = id->init;
    fre_varid( id );
}

/* Given a type `tin' and a list of varid `ids', convert all size lists in
 * `ids' to wrapped types of `tin'.
 */
static statement_list unwrap_variableDeclarators( modflags flags, type tin, varid_list ids )
{
    statement_list res = setroom_statement_list( new_statement_list(), ids->sz );

    while( ids->sz>0 ){
	varid id;
	int ok;
	type t = rdup_type( tin );
	ids = extract_varid_list( ids, 0, &id, &ok );
	origsymbol nm;
	optexpression init;

	assert( ok );
	unwrap_varid( t, id, &t, &nm, &init );
	if( init == 0 ){
	    init = new_OptExprNone();
	}
	res = append_statement_list(
	    res,
	    new_FieldDeclaration(
		rdup_origin( nm->org ),		/* origin */
		Pragma_listNIL,			/* pragmas */
		origsymbol_listNIL,		// Labels
		flags,
		false,
		nm,
		t,
		init
	    )
	);
    }
    return res;
}


/* Given a declaration, and a list of pragmas, return a new declaration
 * that contains the pragmas.
 */
static declaration add_declaration_pragmas( declaration d, Pragma_list pl )
{
    if( d == declarationNIL ){
	return d;
    }
    if( d->pragmas == Pragma_listNIL ){
	d->pragmas = pl;
    }
    else {
	d->pragmas = concat_Pragma_list( d->pragmas, pl );
    }
    return d;
}

/* Given a declaration, and a list of pragmas, return a new declaration
 * that contains the pragmas.
 */
static statement add_statement_pragmas( statement d, Pragma_list pl )
{
    if( d == statementNIL ){
	return d;
    }
    if( d->pragmas == Pragma_listNIL ){
	d->pragmas = pl;
    }
    else {
	d->pragmas = concat_Pragma_list( d->pragmas, pl );
    }
    return d;
}

/* Given a list of declarations, and a list of pragmas, return a modified
 * list of declaration that contains the pragmas.
 */
static statement_list add_statement_list_pragmas( statement_list l, Pragma_list pl )
{
    unsigned int ix;

    for( ix=0; ix<l->sz; ix++ ){
	l->arr[ix] = add_statement_pragmas( l->arr[ix], rdup_Pragma_list( pl ) );
    }
    rfre_Pragma_list( pl );
    return l;
}

/* Given a statement and a label, add the label to the list of this 
 * statement.
 */
static statement add_statement_label( statement s, origsymbol l )
{
    if( s->labels == NULL ){
	s->labels = new_origsymbol_list();
    }
    // By appending the labels we reverse the order w.r.t. the source
    // text, but this does not make any difference, we use the slightly
    // cheaper append.
    s->labels = append_origsymbol_list( s->labels, l );
    return s;
}

/* Given a MethodHeader and a method body, sort out which kind of
 * declaration this is, and return the appropriate one.
 *
 * Parts of the input structures are used in the return value, the
 * remainder is freed.
 */
static declaration unwrap_method_declaration( MethodHeader hdr, Block body )
{
    declaration res;

    hdr->t = wrap_ranked_type( hdr->t, hdr->wrap );
    if( hdr->modifiers & ACC_ABSTRACT ){
	if( body != BlockNIL ){
	    parserror( "Abstract methods cannot have a body" );
	    rfre_Block( body );
	    body = BlockNIL;
	}
	res = new_AbstractFunctionDeclaration(
	    rdup_origin( hdr->name->org ),
	    Pragma_listNIL,	// Pragmas
	    origsymbol_listNIL,	// Labels
	    hdr->modifiers,
	    false,
	    hdr->name,
	    hdr->formals,
	    hdr->throws,
	    hdr->t
	);
    }
    else if( hdr->modifiers & ACC_NATIVE ){
	if( body != BlockNIL ){
	    parserror( "Native methods cannot have a body" );
	    rfre_Block( body );
	    body = BlockNIL;
	}
	res = new_NativeFunctionDeclaration(
	    rdup_origin( hdr->name->org ),
	    Pragma_listNIL,		// Pragmas
	    origsymbol_listNIL,		// Labels
	    hdr->modifiers,
	    false,
	    hdr->name,
	    hdr->formals,
	    hdr->throws,
	    hdr->t
	);
    }
    else {
	if( body == BlockNIL ){
	    parserror( "This method requires a body; otherwise declare it as abstract" );
	}
	res = new_FunctionDeclaration(
	    rdup_origin( hdr->name->org ),
	    Pragma_listNIL,
	    origsymbol_listNIL,		// Labels
	    hdr->modifiers,
	    false,
	    hdr->name,
	    hdr->formals,
	    hdr->throws,
	    hdr->t,
	    body
	);
    }
    fre_MethodHeader( hdr );
    return res;
}

/* Given a statement, make it a Block.
 * If the statement is a BlockStatement, be smart.
 */
static Block make_statement_Block( statement s )
{
    Block res;

    if( s->tag == TAGBlockStatement ){
	BlockStatement bs = to_BlockStatement(s);
	res = rdup_Block( bs->body );
	if( bs->pragmas != Pragma_listNIL && bs->pragmas->sz != 0 ){
	    if( res->pragmas == Pragma_listNIL ){
		res->pragmas = rdup_Pragma_list( bs->pragmas );
	    }
	    else {
		res->pragmas = concat_Pragma_list(
		    rdup_Pragma_list( bs->pragmas ),
		    res->pragmas
		);
	    }
	}
	rfre_statement( s );
    }
    else {
	statement_list sl = new_statement_list();
	sl = append_statement_list( sl, s );
	res = new_Block( tmsymbolNIL, new_Pragma_list(), sl );
    }
    return res;
}

/* Given a MethodHeader, sort out which kind of
 * interface declaration this is, and return the appropriate one.
 *
 * Parts of the input structures are used in the return value, the
 * remainder is freed.
 */
static declaration unwrap_abstract_declaration( MethodHeader hdr )
{
    declaration res;

    hdr->t = wrap_ranked_type( hdr->t, hdr->wrap );
    res = new_AbstractFunctionDeclaration(
	rdup_origin( hdr->name->org ),
	Pragma_listNIL,			// Pragmas
	origsymbol_listNIL,		// Labels
	hdr->modifiers|ACC_ABSTRACT,
	false,
	hdr->name,
	hdr->formals,
	hdr->throws,
	hdr->t
    );
    fre_MethodHeader( hdr );
    return res;
}

/* Given a method header and a body, return a constructor declaration */
static declaration unwrap_constructor_declaration( MethodHeader hdr, Block body )
{
    ConstructorDeclaration res = new_ConstructorDeclaration(
	rdup_origin( hdr->name->org ),
	Pragma_listNIL,			// Pragmas
	origsymbol_listNIL,		// Labels
	hdr->modifiers,
	false,
	hdr->name,
	hdr->formals,
	hdr->throws,
	body
    );
    fre_MethodHeader( hdr );
    return res;
}

static origsymbol qualify_name( origsymbol prefix, tmsymbol suffix )
{
    const char *prestr = prefix->sym->name;
    const char *suffstr = suffix->name;

    tmstring buf = qualify_tmstring( prestr, suffstr );
    rfre_tmsymbol( prefix->sym );
    rfre_tmsymbol( suffix );
    prefix->sym = add_tmsymbol( buf );
    rfre_tmstring( buf );
    return prefix;
}

static statement_list enforce_interface_flags( statement_list sl )
{
    for( unsigned int ix=0; ix<sl->sz; ix++ ){
	statement s = sl->arr[ix];

	switch( s->tag ){
	    case TAGFieldDeclaration:
		to_declaration( s )->flags |= ACC_PUBLIC|ACC_FINAL|ACC_STATIC;
		break;

	    case TAGAbstractFunctionDeclaration:
		// Every method declaration in the body of an interface is
		// implicitly public abstract. See JLS2 9.4.
		to_declaration( s )->flags |= ACC_PUBLIC|ACC_ABSTRACT;
		break;

	    case TAGClassDeclaration:
	    case TAGInterfaceDeclaration:
		// Every member type declaration in the body of an interface is
		// implicitly public static. See JLS2 9.5.
		to_declaration( s )->flags |= ACC_PUBLIC|ACC_STATIC;
		break;

	    default:
		internal_error( "Only fields and abstract methods allowed in an interface" );
		break;

	}
	if( is_declaration( s ) ){
	    declaration dcl = to_declaration( s );
	    dcl->flags = check_access_modifiers( dcl->org, dcl->flags );
	}
    }
    return sl;
}

// Given a list of statements that presumably are constant declarations
// in an interface, make sure that these fields only have the allowed
// fields for an interface constant.
statement_list enforce_constant_restrictions( statement_list sl )
{
    for( unsigned int ix=0; ix<sl->sz; ix++ ){
	statement s = sl->arr[ix];

	assert( is_declaration( s ) );
	declaration decl = to_declaration( s );

	if( decl->flags & ~MODS_CONSTANT ){
	    complain_modifiers( s->org, decl->flags & ~MODS_CONSTANT, "constant" );
	    decl->flags &= MODS_CONSTANT;
	}
    }
    return sl;
}

static expression generate_ClassIdConstructorCall( origsymbol t )
{
    const char *classnm = "java.lang.Class";

    expression res = new_NewClassExpression(
	expressionNIL,
	new_ObjectType( add_origsymbol( classnm ) ),
	append_expression_list(
	    new_expression_list(),
	    new_ClassIdExpression( t )
	),
	statement_listNIL
    );
    return res;
}

static expression generate_basetype_ClassIdConstructorCall( BASETYPE t )
{
    const char *classnm = "java.lang.Class";
    const char *tnm = NULL;

    switch( t ){
	case BT_BOOLEAN:	tnm = "java.lang.Boolean";	break;
	case BT_STRING:		tnm = "java.lang.String";	break;
	case BT_COMPLEX:	tnm = "java.lang.Complex";	break;
	case BT_BYTE:		tnm = "java.lang.Byte";		break;
	case BT_SHORT:		tnm = "java.lang.Short";	break;
	case BT_CHAR:		tnm = "java.lang.Character";	break;
	case BT_INT:		tnm = "java.lang.Integer";	break;
	case BT_LONG:		tnm = "java.lang.Long";		break;
	case BT_FLOAT:		tnm = "java.lang.Float";	break;
	case BT_DOUBLE:		tnm = "java.lang.Double";	break;
    }

    origsymbol t_class = add_origsymbol( tnm );

    expression res = new_NewClassExpression(
	expressionNIL,
	new_ObjectType( add_origsymbol( classnm ) ),
	append_expression_list(
	    new_expression_list(),
	    new_ClassIdExpression( t_class )
	),
	statement_listNIL
    );
    return res;
}

// Given two operands `r' and `l', and an operator name `op', construct
// an PragmaExpressionList representing this binary expression.
//
// If `r' is an expression list with the same operator as the symbol
// at the first position in the list, splice in the operands of that
// expression list.
static PragmaExpression_list build_binop_PragmaExpression(
 PragmaExpression l,
 const char *op,
 origin org,
 PragmaExpression r
)
{
    PragmaExpression_list res = new_PragmaExpression_list();
    tmsymbol opsym = add_tmsymbol( op );

    res = append_PragmaExpression_list(
	res,
	new_NamePragmaExpression( new_origsymbol( opsym, org ) )
    );
    if( l->tag == TAGListPragmaExpression ){
	PragmaExpression_list ll = to_ListPragmaExpression( l )->l;
	if( ll->sz>0 ){
	    PragmaExpression lop = ll->arr[0];

	    if( lop->tag == TAGNamePragmaExpression ){
		origsymbol nm = to_NamePragmaExpression( lop )->name;

		if( nm->sym == opsym ){
		    res = concat_PragmaExpression_list(
			res,
			delete_PragmaExpression_list(
			    rdup_PragmaExpression_list( ll ),
			    0
			)
		    );
		    rfre_PragmaExpression( l );
		    l = PragmaExpressionNIL;
		}
	    }
	}
    }
    if( l != PragmaExpressionNIL ){
	res = append_PragmaExpression_list( res, l );
    }
    res = append_PragmaExpression_list( res, r );
    return res;
}

// Build the statement `super();'. This is inserted if there is
// no explict super or this call in a constructor body.
static statement build_default_super()
{
    statement super = new_SuperConstructorInvocationStatement(
	gen_origin(),
	Pragma_listNIL,
	origsymbol_listNIL,	// Labels
	new_expression_list()
    );
    return super;
}

// Given a set of modifiers that are intended for a method, check
// that the right combination of modifiers.
static modflags check_method_modifiers( modflags flags )
{
    if( has_flags( flags, ACC_ABSTRACT|ACC_STATIC ) ){
	parserror( "a static method cannot be abstract" );
	flags &= ~ACC_STRICTFP;
    }
    if( has_flags( flags, ACC_NATIVE|ACC_ABSTRACT ) ){
	parserror( "a native method cannot be abstract" );
	flags &= ~ACC_ABSTRACT;
    }
    if( has_flags( flags, ACC_ABSTRACT|ACC_STRICTFP ) ){
	parserror( "an abstract method cannot be strictftp" );
	flags &= ~ACC_STRICTFP;
    }
    if( has_flags( flags, ACC_ABSTRACT|ACC_SYNCHRONIZED ) ){
	parserror( "an abstract method cannot be synchronized" );
	flags &= ~ACC_SYNCHRONIZED;
    }
    if( has_flags( flags, ACC_ABSTRACT|ACC_FINAL ) ){
	parserror( "an abstract method cannot be final" );
	flags &= ~ACC_FINAL;
    }
    if( has_flags( flags, ACC_ABSTRACT|ACC_PRIVATE ) ){
	parserror( "an abstract method cannot be private" );
	flags &= ~ACC_FINAL;
    }
    if( has_flags( flags, ACC_NATIVE|ACC_STRICTFP ) ){
	parserror( "a native method cannot be strictftp" );
	flags &= ~ACC_STRICTFP;
    }
    if( flags & ACC_INLINE ){
	if( !has_any_flag( flags, (ACC_STATIC|ACC_FINAL) ) ){
	    modflags class_flags = get_top_class_modifiers();
	    if( !has_any_flag( class_flags, ACC_FINAL ) ){
		parserror( "an inline method must be either static or final" );
		flags |= ACC_FINAL;
	    }
	}
	if( flags & ~MODS_INLINE ){
	    origin org = make_origin();
	    complain_modifiers( org, flags & ~MODS_INLINE, "inline method" );
	    flags &= MODS_INLINE;
	    rfre_origin( org );
	}
    }
    else {
	if( flags & ~MODS_METHOD ){
	    origin org = make_origin();
	    complain_modifiers( org, flags & ~MODS_METHOD, "method" );
	    flags &= MODS_METHOD;
	    rfre_origin( org );
	}
    }
    return flags;
}

/* Given PreIncrement expression, convert it to an assignment statement. */
static AssignStatement transmog_PreIncrementExpression( PreIncrementExpression x )
{
    assert( x->tag == TAGPreIncrementExpression );
    AssignStatement res = new_AssignStatement(
	originNIL,
	Pragma_listNIL,
	origsymbol_listNIL,	// Labels
	ASSIGN_PLUS,
	x->x,
	new_IntExpression( 1 )
    );
    fre_expression( x );
    return res;
}

/* Given PreDecrement expression, convert it to an assignment statement. */
static AssignStatement transmog_PreDecrementExpression( PreDecrementExpression x )
{
    assert( x->tag == TAGPreDecrementExpression );
    AssignStatement res = new_AssignStatement(
	originNIL,
	Pragma_listNIL,
	origsymbol_listNIL,	// Labels
	ASSIGN_MINUS,
	x->x,
	new_IntExpression( 1 )
    );
    fre_expression( x );
    return res;
}

/* Given PostIncrement expression, convert it to an assignment statement. */
static AssignStatement transmog_PostIncrementExpression( PostIncrementExpression x )
{
    assert( x->tag == TAGPostIncrementExpression );
    AssignStatement res = new_AssignStatement(
	originNIL,
	Pragma_listNIL,
	origsymbol_listNIL,	// Labels
	ASSIGN_PLUS,
	x->x,
	new_IntExpression( 1 )
    );
    fre_expression( x );
    return res;
}

/* Given PostDecrement expression, convert it to an assignment statement. */
static AssignStatement transmog_PostDecrementExpression( PostDecrementExpression x )
{
    assert( x->tag == TAGPostDecrementExpression );
    AssignStatement res = new_AssignStatement(
	originNIL,
	Pragma_listNIL,
	origsymbol_listNIL,	// Labels
	ASSIGN_MINUS,
	x->x,
	new_IntExpression( 1 )
    );
    fre_expression( x );
    return res;
}

/* Given a MethodInvocation expression, convert it to a MethodInvocation
 * statement.
 */
static statement transmog_invocation_expression( expression x )
{
    statement res = statementNIL;

    switch( x->tag ){
        case TAGMethodInvocationExpression:
	{
	    MethodInvocationExpression call = to_MethodInvocationExpression ( x );

	    res = new_MethodInvocationStatement(
		originNIL,
		Pragma_listNIL,
		origsymbol_listNIL,	// Labels
		call->invocation
	    );
	    fre_expression( x );
	    break;
	}

	case TAGFieldInvocationExpression:
	{
	    FieldInvocationExpression call = to_FieldInvocationExpression ( x );

	    res = new_FieldInvocationStatement(
		originNIL,
		Pragma_listNIL,
		origsymbol_listNIL,	// Labels
		call->var,
		call->field,
		call->parameters
	    );
	    fre_expression( x );
	    break;
	}

	case TAGTypeInvocationExpression:
	{
	    TypeInvocationExpression call = to_TypeInvocationExpression ( x );

	    res = new_TypeInvocationStatement(
		call->org,
		Pragma_listNIL,
		origsymbol_listNIL,	// Labels
		call->t,
		call->field,
		call->parameters
	    );
	    fre_expression( x );
	    break;
	}

	case TAGSuperInvocationExpression:
	{
	    SuperInvocationExpression call = to_SuperInvocationExpression ( x );

	    res = new_SuperInvocationStatement(
		call->org,
		Pragma_listNIL,
		origsymbol_listNIL,	// Labels
		call->field,
		call->parameters
	    );
	    fre_expression( x );
	    break;
	}

	case TAGOuterSuperInvocationExpression:
	{
	    OuterSuperInvocationExpression call = to_OuterSuperInvocationExpression ( x );

	    res = new_OuterSuperInvocationStatement(
		call->org,
		Pragma_listNIL,
		origsymbol_listNIL,	// Labels
		call->t,
		call->field,
		call->parameters
	    );
	    fre_expression( x );
	    break;
	}

	default:
	    internal_error( "not an invocation expression" );
	    break;
    }
    return res;
}

// Given an expression, make sure that it can be assigned to.
// Of course, at this point we can only descide based on structure, but
// that leaves room enough for some loud complaints.
static void verify_assignable_expression( const_expression x )
{
    switch( x->tag ){
        case TAGMethodInvocationExpression:
	case TAGFieldInvocationExpression:
	case TAGTypeInvocationExpression:
	case TAGSuperInvocationExpression:
	case TAGOuterSuperInvocationExpression:
	    parserror( "cannot assign to an invocation" );
	    break;

	case TAGByteExpression:
	case TAGShortExpression:
	case TAGIntExpression:
	case TAGLongExpression:
	case TAGFloatExpression:
	case TAGDoubleExpression:
	case TAGClassIdExpression:
	case TAGCharExpression:
	case TAGBooleanExpression:
	case TAGStringExpression:
	case TAGNullExpression:
	case TAGSizeofExpression:
	case TAGClassExpression:
	case TAGDefaultValueExpression:
	case TAGInternalizeExpression:
	    parserror( "cannot assign to a literal" );
	    break;

	case TAGAssignOpExpression:
	    parserror( "cannot assign to assignment expression" );
	    break;

	case TAGPostIncrementExpression:
	case TAGPreIncrementExpression:
	    parserror( "cannot assign to an increment expression" );
	    break;

	case TAGPostDecrementExpression:
	case TAGPreDecrementExpression:
	    parserror( "cannot assign to a decrement expression" );
	    break;

	case TAGTypeExpression:
	    parserror( "cannot assign to a type expression" );
	    break;

	case TAGBracketExpression:
#if 0
	    parserror( "cannot assign to a bracketed expression" );
#endif
	    verify_assignable_expression( to_const_BracketExpression(x)->x );
	    break;

	case TAGArrayInitExpression:
	    parserror( "cannot assign to an array init expression" );
	    break;

	case TAGCastExpression:
	case TAGForcedCastExpression:
	    parserror( "cannot assign to a cast expression" );
	    break;

	case TAGAnnotationExpression:
	    verify_assignable_expression( to_const_AnnotationExpression(x)->x );
	    break;

	case TAGNotNullAssertExpression:
	    verify_assignable_expression( to_const_NotNullAssertExpression(x)->x );
	    break;

	case TAGIfExpression:
	case TAGWhereExpression:
	case TAGUnopExpression:
	case TAGBinopExpression:
	case TAGShortopExpression:
	case TAGInstanceOfExpression:
	case TAGClassInstanceOfExpression:
	case TAGInterfaceInstanceOfExpression:
	case TAGTypeInstanceOfExpression:
	case TAGGetBufExpression:
	case TAGGetSizeExpression:
	case TAGGetLengthExpression:
	    parserror( "cannot assign to an operator expression" );
	    break;

	case TAGNewArrayExpression:
	case TAGNewRecordExpression:
	case TAGNewInitArrayExpression:
	case TAGNewClassExpression:
	    parserror( "cannot assign to a `new' expression" );
	    break;

	case TAGVariableNameExpression:
	case TAGSubscriptExpression:
	case TAGVectorSubscriptExpression:
	case TAGFieldExpression:
	case TAGSuperFieldExpression:
	case TAGOuterSuperFieldExpression:
	case TAGOuterThisExpression:
	case TAGTypeFieldExpression:
	case TAGVectorExpression:
	case TAGComplexExpression:	// Future extension
	    // No problem.
	    break;

    }
}

/* ------------------------------------------------- */

%}

%union {
    BASETYPE			_basetype;
    Block			_Block;
    Cardinality			_Cardinality;
    Cardinality_list		_CardinalityList;
    Catch			_Catch;
    Catch_list			_CatchList;
    ActualDim_list		_ActualDimList;
    FormalDim			_Dim;
    FormalDim_list		_DimList;
    FormalParameter		_parm;
    FormalParameter_list	_parmList;
    Import			_Import;
    Import_list			_ImportList;
    MethodHeader		_MethodHeader;
    Pragma			_APragma;
    Pragma_list			_PragmaList;
    SwitchCase			_SwitchCase;
    SwitchCase_list		_SwitchCaseList;
    WaitCase			_WaitCase;
    WaitCase_list		_WaitCaseList;
    declaration			_declaration;
    expression			_expression;
    expression_list		_expressionList;
    modflags			_modflags;
    optexpression		_optexpression;
    origin			_origin;
    origsymbol			_Identifier;
    statement			_statement;
    statement_list		_statementList;
    vnus_double			_vnus_double;
    vnus_float			_vnus_float;
    tmstring			_vnus_string;
    tmsymbol			_identifier;
    type			_type;
    type_list			_typeList;
    varid			_varid;
    varid_list			_varidList;
    tmuint			_uint;
    vnus_char			_vnus_char;
    vnus_int			_vnus_int;
    vnus_long			_vnus_long;
    PragmaExpression		_pragmaExpr;
    PragmaExpression_list	_pragmaExprList;
}

%start CompilationUnit

/* The typed tokens first. */
%token <_identifier>	IDENTIFIER
%token <_vnus_char>	CHAR_LITERAL
%token <_vnus_double>	DOUBLE_LITERAL
%token <_vnus_double>	IMAGINARY_LITERAL
%token <_vnus_float>	FLOAT_LITERAL
%token <_vnus_int>	INT_LITERAL
%token <_vnus_long>	LONG_LITERAL
%token <_vnus_string>	STRING_LITERAL

%token KEY_ABSTRACT
%token KEY_ARRAY
%token KEY_ASSERT
%token KEY_BOOLEAN
%token KEY_BREAK
%token KEY_BYTE
%token KEY_CASE
%token KEY_TIMEOUT
%token KEY_CATCH
%token KEY_CHAR
%token KEY_CLASS
%token KEY_COMPLEX
%token KEY_CONST	// Reserved word, but unused
%token KEY_CONTINUE
%token KEY_DEFAULT
%token KEY_DELETE
%token KEY_DO
%token KEY_DOUBLE
%token KEY_DOUBLEBITS
%token KEY_EACH
%token KEY_ELSE
%token KEY_EXTENDS
%token KEY_FALSE
%token KEY_FINAL
%token KEY_FINALLY
%token KEY_FLOAT
%token KEY_FLOATBITS
%token KEY_FOR
%token KEY_FOREACH
%token KEY_GETBUF
%token KEY_GLOBALPRAGMAS
%token KEY_GOTO		// Reserved word, but unused
%token KEY_IF
%token KEY_IMPLEMENTS
%token KEY_IMPORT
%token KEY_INLINE
%token KEY_INSTANCEOF
%token KEY_INT
%token KEY_INTERFACE
%token KEY_LONG
%token KEY_NATIVE
%token KEY_NEW
%token KEY_NULL
%token KEY_PACKAGE
%token KEY_PRINT
%token KEY_PRINTLN
%token KEY_PRIVATE
%token KEY_PROTECTED
%token KEY_PUBLIC
%token KEY_RETURN
%token KEY_SHORT
%token KEY_SIZEOF
%token KEY_STATIC
%token KEY_STRICTFP
%token KEY_STRING
%token KEY_SUPER
%token KEY_SWITCH
%token KEY_SYNCHRONIZED
%token KEY_THIS
%token KEY_THROW
%token KEY_THROWS
%token KEY_TRANSIENT
%token KEY_TRUE
%token KEY_TRY
%token KEY_TYPE
%token KEY_VOID
%token KEY_VOLATILE
%token KEY_WAIT
%token KEY_WHILE
%token OP_AND
%token OP_ASSIGNAND
%token OP_ASSIGNDIVIDE
%token OP_ASSIGNMENT
%token OP_ASSIGNMINUS
%token OP_ASSIGNMOD
%token OP_ASSIGNOR
%token OP_ASSIGNPLUS
%token OP_ASSIGNSHIFTLEFT
%token OP_ASSIGNSHIFTRIGHT
%token OP_ASSIGNTIMES
%token OP_ASSIGNUSHIFTRIGHT
%token OP_ASSIGNXOR
%token OP_DECREMENT
%token OP_DIVIDE
%token OP_EQUAL
%token OP_GENERIC_CLOSE
%token OP_GENERIC_OPEN
%token OP_GREATER
%token OP_GREATEREQUAL
%token OP_INCREMENT
%token OP_INVERT
%token OP_ITERATES
%token OP_LESS
%token OP_FLOWTO
%token OP_LESSEQUAL
%token OP_MINUS
%token OP_MOD
%token OP_NOT
%token OP_NOTEQUAL
%token OP_OR
%token OP_PLUS
%token OP_PRAGMAEND
%token OP_PRAGMASTART
%token OP_SHIFTLEFT
%token OP_SHIFTRIGHT
%token OP_SHORTAND
%token OP_SHORTOR
%token OP_TIMES
%token OP_USHIFTRIGHT
%token OP_XOR

/* This implements the classical operator precedence.
 * The lower an operator is on the list of binding directions, the stronger it
 * binds.
 */
%left OP_ASSIGNMENT OP_ASSIGNAND OP_ASSIGNOR OP_ASSIGNDIVIDE OP_ASSIGNTIMES OP_ASSIGNMINUS OP_ASSIGNMOD OP_ASSIGNPLUS OP_ASSIGNSHIFTLEFT OP_ASSIGNSHIFTRIGHT OP_ASSIGNUSHIFTRIGHT OP_ASSIGNXOR
%right '?' ':'
%left OP_DIY_INFIX
%left OP_SHORTOR
%left OP_SHORTAND
%left OP_OR
%left OP_XOR
%left OP_AND
%left OP_EQUAL OP_NOTEQUAL
%left OP_LESS OP_LESSEQUAL OP_GREATER OP_GREATEREQUAL KEY_INSTANCEOF
%left OP_SHIFTLEFT OP_SHIFTRIGHT OP_USHIFTRIGHT
%left OP_PLUS OP_MINUS
%left OP_TIMES OP_DIVIDE OP_MOD
%left OP_UNOP
%left OP_CAST '[' '@'

/* To resolve the classical if-then-else ambiguity. No, I don't
 * understand either why the ')' has to be there.
 */
%right ')' KEY_ELSE

/* use sort -b +2 */

%type	<_declaration>			AbstractMethodDeclaration
%type	<_expression>			Argument
%type	<_expressionList>		ArgumentList
%type	<_expressionList>		ArgumentListOpt
%type	<_pragmaExprList>		ArithmeticPragmaExpression
%type	<_expression>			ArrayAccess
%type	<_expression>			ArrayCreationExpression
%type	<_expression>			ArrayInitializer
%type	<_type>				ArrayType
%type	<_expression>			AssignableExpression
%type	<_expression>			Assignment
%type	<_expression>			AssignmentExpression
%type	<_Block>			Block
%type	<_statementList>		BlockStatement
%type	<_statementList>		BlockStatements
%type	<_statementList>		BlockStatementsOpt
%type	<_expression>			BooleanLiteral
%type	<_statement>			BreakStatement
%type	<_statement>			CardForStatement
%type	<_CardinalityList>		Cardinalities
%type	<_Cardinality>			Cardinality
%type	<_CardinalityList>		CardinalityList
%type	<_expression>			CardinalityLowerbound
%type	<_expression>			CastExpression
%type	<_Catch>			CatchClause
%type	<_CatchList>			Catches
%type	<_CatchList>			CatchesOpt
%type	<_Identifier>			ClassAsName
%type	<_statementList>		ClassBody
%type	<_statementList>		ClassBodyDeclaration
%type	<_statementList>		ClassBodyDeclarations
%type	<_statementList>		ClassBodyDeclarationsOpt
%type	<_statementList>		ClassBodyOpt
%type	<_declaration>			ClassDeclaration
%type	<_expression>			ClassExpression
%type	<_expression>			ClassInstanceCreationExpression
%type	<_statementList>		ClassMemberDeclaration
%type	<_modflags>			ClassModifiersOpt
%type	<_type>				ClassOrInterfaceType
%type	<_type>				ClassType
%type	<_typeList>			ClassTypeList
%type	<_statementList>		ConstantDeclaration
%type	<_expression>			ConstantExpression
%type	<_Block>			ConstructorBody
%type	<_declaration>			ConstructorDeclaration
%type	<_MethodHeader>			ConstructorDeclarator
%type	<_statementList>		ConstructorStatementsOpt
%type	<_statement>			ContinueStatement
%type	<_statement>			Control
%type	<_statement>			Delete
%type	<_Dim>				Dim
%type	<_DimList>			Dims
%type	<_DimList>			DimsOpt
%type	<_statement>			DoStatement
%type	<_statement>			EachStatement
%type	<_statement>			EmptyStatement
%type	<_statement>			ExplicitConstructorInvocation
%type	<_expression>			Expression
%type	<_expressionList>		ExpressionList
%type	<_optexpression>		ExpressionOpt
%type	<_statement>			ExpressionStatement
%type	<_typeList>			ExtendsInterfaces
%type	<_typeList>			ExtendsInterfacesOpt
%type	<_expression>			FieldAccess
%type	<_statementList>		FieldDeclaration
%type	<_Block>			Finally
%type	<_expression>			FloatingPointLiteral
%type	<_statementList>		ForInit
%type	<_statementList>		ForInitOpt
%type	<_statement>			ForStatement
%type	<_statementList>		ForUpdate
%type	<_statementList>		ForUpdateOpt
%type	<_statement>			ForeachStatement
%type	<_parm>				FormalParameter
%type	<_parmList>			FormalParameterList
%type	<_parmList>			FormalParameterListOpt
%type	<_type>				GenericClassOrInterfaceType
%type	<_PragmaList>			GlobalPragmasOpt
%type	<_Identifier>			Identifier
%type	<_Identifier>			IdentifierOpt
%type	<_statement>			IfStatement
%type	<_statement>			Imperative
%type	<_Import>			ImportDeclaration
%type	<_ImportList>			ImportDeclarations
%type	<_ImportList>			ImportDeclarationsOpt
%type	<_expression>			IntegerLiteral
%type	<_statementList>		InterfaceBody
%type	<_declaration>			InterfaceDeclaration
%type	<_statementList>		InterfaceMemberDeclaration
%type	<_statementList>		InterfaceMemberDeclarations
%type	<_modflags>			InterfaceModifiersOpt
%type	<_type>				InterfaceType
%type	<_typeList>			InterfaceTypeList
%type	<_typeList>			Interfaces
%type	<_typeList>			InterfacesOpt
%type	<_Identifier>			LabelName
%type	<_pragmaExpr>			ListPragmaExpression
%type	<_expression>			Literal
%type	<_pragmaExpr>			LiteralPragmaExpression
%type	<_statementList>		LocalVariableDeclaration
%type	<_statementList>		LocalVariableDeclarationStatement
%type	<_expression>			MacroExpression
%type	<_statement>			MemoryManagement
%type	<_Block>			MethodBody
%type	<_declaration>			MethodDeclaration
%type	<_MethodHeader>			MethodDeclarator
%type	<_MethodHeader>			MethodHeader
%type	<_expression>			MethodInvocation
%type	<_modflags>			Modifier
%type	<_modflags>			Modifiers
%type	<_modflags>			ModifiersOpt
%type	<_Identifier>			Name
%type	<_pragmaExpr>			NamePragmaExpression
%type	<_expression>			OperatorExpression
%type	<_origin>			Origin
%type	<_Identifier>			PackageDeclaration
%type	<_Identifier>			PackageDeclarationOpt
%type	<_statement>			Parallelization
%type	<_expression>			PostDecrementExpression
%type	<_expression>			PostIncrementExpression
%type	<_expression>			PostfixExpression
%type	<_APragma>			Pragma
%type	<_pragmaExpr>			PragmaExpression
%type	<_pragmaExprList>		PragmaExpressionList
%type	<_pragmaExprList>		PragmaExpressions
%type	<_PragmaList>			PragmaList
%type	<_PragmaList>			Pragmas
%type	<_PragmaList>			PragmasOpt
%type	<_expression>			PreDecrementExpression
%type	<_expression>			PreIncrementExpression
%type	<_type>				PrimArrayType
%type	<_expression>			Primary
%type	<_expression>			PrimaryNoNewArray
%type	<_basetype>			PrimitiveType
%type	<_statement>			Print
%type	<_statement>			PrintLn
%type	<_Identifier>			QualifiedName
%type	<_type>				ReferenceType
%type	<_pragmaExprList>		RelationalPragmaExpression
%type	<_statement>			ReturnStatement
%type	<_Identifier>			SimpleName
%type	<_Import>			SingleTypeImportDeclaration
%type	<_uint>				Size
%type	<_uint>				SizeList
%type	<_statement>			Statement
%type	<_statement>			StatementBlock
%type	<_statement>			StatementExpression
%type	<_statementList>		StatementExpressionList
%type	<_statement>			StaticInitializer
%type	<_pragmaExprList>		SubscriptPragmaExpression
%type	<_type>				Super
%type	<_type>				SuperOpt
%type	<_statement>			Support
%type	<_WaitCase>			WaitCase
%type	<_WaitCaseList>			WaitCaseList
%type	<_SwitchCase>			SwitchCase
%type	<_SwitchCaseList>		SwitchCaseList
%type	<_statement>			WaitStatement
%type	<_statement>			SwitchStatement
%type	<_statement>			SynchronizedStatement
%type	<_expression>			TemplateArgument
%type	<_expressionList>		TemplateArgumentList
%type	<_expressionList>		TemplateArgumentListOpt
%type	<_parm>				TemplateFormalParameter
%type	<_parmList>			TemplateFormalParameterList
%type	<_parmList>			TemplateFormalParameterListOpt
%type	<_statement>			ThrowStatement
%type	<_typeList>			Throws
%type	<_typeList>			ThrowsOpt
%type	<_statement>			TryStatement
%type	<_type>				TupleType
%type	<_type>				Type
%type	<_declaration>			TypeDeclaration
%type	<_statementList>		TypeDeclarations
%type	<_statementList>		TypeDeclarationsOpt
%type	<_Import>			TypeImportOnDemandDeclaration
%type	<_parmList>			TypeParameters
%type	<_parmList>			TypeParametersOpt
%type	<_expression>			UnaryExpression
%type	<_statement>			UnlabeledStatement
%type	<_statement>			ValueReturnStatement
%type	<_varid>			VariableDeclarator
%type	<_varid>			VariableDeclaratorId
%type	<_varidList>			VariableDeclarators
%type	<_expression>			VariableInitializer
%type	<_expressionList>		VariableInitializers
%type	<_expressionList>		Vector
%type	<_expression>			VectorExpr
%type	<_ActualDimList>		Vectors
%type	<_type>				VerboseType
%type	<_typeList>			VerboseTypeList
%type	<_statement>			WhileStatement

%%

/* Helper definitions */
CommaOpt:
    /* empty */
|
    ','
;

PragmaExpression:
    LiteralPragmaExpression
	{ $$ = $1; }
|
    NamePragmaExpression
	{ $$ = $1; }
|
    SubscriptPragmaExpression
	{ $$ = new_ListPragmaExpression( $1 ); }
|
    ArithmeticPragmaExpression
	{ $$ = new_ListPragmaExpression( $1 ); }
|
    RelationalPragmaExpression
	{ $$ = new_ListPragmaExpression( $1 ); }
|
    ListPragmaExpression
	{ $$ = $1; }
;

LiteralPragmaExpression:
    INT_LITERAL
	{ $$ = new_NumberPragmaExpression( (vnus_double) $1 ); }
|
    FLOAT_LITERAL
	{ $$ = new_NumberPragmaExpression( (vnus_double) $1 ); }
|
    DOUBLE_LITERAL
	{ $$ = new_NumberPragmaExpression( $1 ); }
|
    STRING_LITERAL
	{ $$ = new_StringPragmaExpression( $1 ); }
|
    KEY_TRUE
	{ $$ = new_BooleanPragmaExpression( true ); }
|
    KEY_FALSE
	{ $$ = new_BooleanPragmaExpression( false ); }
;

NamePragmaExpression:
    Identifier
	{ $$ = new_NamePragmaExpression( $1 ); }
|
    '@' Identifier
	{ $$ = new_ExternalNamePragmaExpression( $2 ); }
;

SubscriptPragmaExpression:
    PragmaExpression '[' Origin PragmaExpressionList ']'
	{
	    $$ = $4;
	    $$ = insert_PragmaExpression_list( $$, 0, $1 );
	    $$ = insert_PragmaExpression_list(
		$$,
		0,
		new_NamePragmaExpression(
		    new_origsymbol( add_tmsymbol( "at" ), $3 )
		)
	    );
	}
;

ArithmeticPragmaExpression:
    PragmaExpression OP_PLUS Origin PragmaExpression
	{ $$ = build_binop_PragmaExpression( $1, "sum", $3, $4 ); }
|
    PragmaExpression OP_TIMES Origin PragmaExpression
	{ $$ = build_binop_PragmaExpression( $1, "prod", $3, $4 ); }
|
    PragmaExpression OP_MINUS Origin PragmaExpression
	{ $$ = build_binop_PragmaExpression( $1, "subtract", $3, $4 ); }
|
    PragmaExpression OP_DIVIDE Origin PragmaExpression
	{ $$ = build_binop_PragmaExpression( $1, "div", $3, $4 ); }
|
    PragmaExpression OP_MOD Origin PragmaExpression
	{ $$ = build_binop_PragmaExpression( $1, "mod", $3, $4 ); }
;


RelationalPragmaExpression:
    PragmaExpression OP_EQUAL Origin PragmaExpression
	{ $$ = build_binop_PragmaExpression( $1, "eq", $3, $4 ); }
|
    PragmaExpression OP_NOTEQUAL Origin PragmaExpression
	{ $$ = build_binop_PragmaExpression( $1, "ne", $3, $4 ); }
|
    PragmaExpression OP_LESSEQUAL Origin PragmaExpression
	{ $$ = build_binop_PragmaExpression( $1, "le", $3, $4 ); }
|
    PragmaExpression OP_LESS Origin PragmaExpression
	{ $$ = build_binop_PragmaExpression( $1, "lt", $3, $4 ); }
|
    PragmaExpression OP_GREATEREQUAL Origin PragmaExpression
	{ $$ = build_binop_PragmaExpression( $1, "ge", $3, $4 ); }
|
    PragmaExpression OP_GREATER Origin PragmaExpression
	{ $$ = build_binop_PragmaExpression( $1, "gt", $3, $4 ); }
;

ListPragmaExpression:
    '(' PragmaExpressions ')'
	{ $$ = new_ListPragmaExpression( $2 ); }
;

PragmaExpressionList:
    /* empty */
	{ $$ = new_PragmaExpression_list(); }
|
    PragmaExpression
	{ $$ = append_PragmaExpression_list( new_PragmaExpression_list(), $1 ); }
|
    PragmaExpressionList ',' PragmaExpression
	{ $$ = append_PragmaExpression_list( $1, $3 ); }
;

PragmaExpressions:
    /* empty */
	{ $$ = new_PragmaExpression_list(); }
|
    PragmaExpressions PragmaExpression
	{ $$ = append_PragmaExpression_list( $1, $2 ); }
;

Pragma:
    Identifier
        { $$ = new_FlagPragma( $1 ); }
|
    Identifier OP_ASSIGNMENT PragmaExpression
        { $$ = new_ValuePragma( $1, $3 ); }
;

PragmaList:
    /* empty */
        { $$ = new_Pragma_list(); }
|
    Pragma
        { $$ = append_Pragma_list( new_Pragma_list(), $1 ); }
|
    PragmaList ',' Pragma
        { $$ = append_Pragma_list( $1, $3 ); }
;

PragmaStartSymbol:
    OP_PRAGMASTART
	{ enter_pragma_state(); }
;

PragmaEndSymbol:
    OP_PRAGMAEND
	{ leave_pragma_state(); }
;

Pragmas:
     PragmaStartSymbol PragmaList PragmaEndSymbol
         { $$ = $2; }
|
    PragmaStartSymbol error PragmaEndSymbol
        { $$ = new_Pragma_list(); }
;

GlobalPragmasOpt:
    /* empty */
        { $$ = new_Pragma_list(); }
|
    KEY_GLOBALPRAGMAS Pragmas ';'
        { $$ = $2; }
|
    KEY_GLOBALPRAGMAS error ';'
        { $$ = new_Pragma_list(); }
;

PragmasOpt:
    /* empty */
        { $$ = new_Pragma_list(); }
|
    Pragmas
        { $$ = $1; }
;

Origin:
    /* empty */
	{ $$ = make_origin(); }
;

LabelName:
    Identifier
	{ $$ = $1; }
;

CardinalityList:
    /* empty */
	{ $$ = new_Cardinality_list(); }
|
    Cardinality
	{ $$ = append_Cardinality_list( new_Cardinality_list(), $1 ); }
|
    CardinalityList ',' Cardinality
	{ $$ = append_Cardinality_list( $1, $3 ); }
|
    error ',' Cardinality
	{ $$ = append_Cardinality_list( new_Cardinality_list(), $3 ); }
;

CardinalityLowerbound:
    /* empty */
	{ $$ = expressionNIL; }
|
    Expression
	{ $$ = $1; }
;

Cardinality:
    SimpleName OP_ITERATES CardinalityLowerbound ':' Expression
	{ $$ = new_Cardinality( $1, new_Pragma_list(), $3, $5, expressionNIL ); }
|
    SimpleName OP_ITERATES CardinalityLowerbound ':' Expression ':' Expression
	{ $$ = new_Cardinality( $1, new_Pragma_list(), $3, $5, $7 ); }
|
    '(' Cardinality ')'
	{ $$ = $2; }
|
    '(' error ')'
	{ $$ = CardinalityNIL; }
;

Vectors:
    VectorExpr PragmasOpt
	{
	    $$ = append_ActualDim_list(
		new_ActualDim_list(),
		new_ActualDim( $1, $2 )
	    );
	}
|
    Vectors VectorExpr PragmasOpt
	{ $$ = append_ActualDim_list( $1, new_ActualDim( $2, $3 ) ); }
;

VectorExpr:
    Vector
	{ $$ = new_VectorExpression( $1 ); }
/*
|
    '@' MacroExpression
	{ $$ = $2; }
*/
|
    '@' '(' Expression ')'
        { $$ = $3; }
|
    '@' Vector
	{ $$ = new_VectorExpression( $2 ); }
|
    '@' SimpleName
	{ $$ = new_VariableNameExpression( $2, 0 ); }
;

Vector:
    '[' ExpressionList ']'
	{ $$ = $2; }
;

IdentifierOpt:
    /* empty */
	{ $$ = 0; }
|
    Identifier
	{ $$ = $1; }
;

Identifier:
    IDENTIFIER
	{ $$ = make_origsymbol( $1 ); }
;

/* --- below this everything is after the java book in both order and
 * --- rules.
 */

/* Lexical Structure */

IntegerLiteral:
    INT_LITERAL
	{ $$ = new_IntExpression( $1 ); }
|
    LONG_LITERAL
	{ $$ = new_LongExpression( $1 ); }
;

FloatingPointLiteral:
    FLOAT_LITERAL
	{ $$ = new_FloatExpression( $1 ); }
|
    KEY_FLOATBITS INT_LITERAL
	{ $$ = new_FloatExpression( intbits_to_vnus_float( $2 ) ); }
|
    DOUBLE_LITERAL
	{ $$ = new_DoubleExpression( $1 ); }
|
    KEY_DOUBLEBITS LONG_LITERAL
	{ $$ = new_DoubleExpression( longbits_to_vnus_double( $2 ) ); }
|
    IMAGINARY_LITERAL
	{
	    $$ = new_ComplexExpression(
		new_DoubleExpression( 0.0 ),
		new_DoubleExpression( $1 )
	    );
	}
;

BooleanLiteral:
    KEY_TRUE
	{ $$ = new_BooleanExpression( TRUE ); }
|
    KEY_FALSE
	{ $$ = new_BooleanExpression( FALSE ); }
;

Literal:
    IntegerLiteral
	{ $$ = $1; }
|
    FloatingPointLiteral
	{ $$ = $1; }
|
    BooleanLiteral
	{ $$ = $1; }
|
    CHAR_LITERAL
	{ $$ = new_CharExpression( $1 ); }
|
    STRING_LITERAL
	{ $$ = new_StringExpression( $1 ); }
|
    KEY_NULL
	{ $$ = new_NullExpression(); }
|
    KEY_SIZEOF '(' Type ')'
	{ $$ = new_SizeofExpression( $3 ); }
;

/* Types, Values, and Variables */

Type:
    PrimitiveType PragmasOpt
	{ $$ = wrap_pragmas_type( new_PrimitiveType( $1 ), $2 ); }
|
    TupleType PragmasOpt
	{ $$ = wrap_pragmas_type( $1, $2 ); }
|
    ReferenceType
	{ $$ = $1; }
;

VerboseType:
    PrimitiveType PragmasOpt
	{ $$ = wrap_pragmas_type( new_PrimitiveType( $1 ), $2 ); }
|
    TupleType PragmasOpt
	{ $$ = wrap_pragmas_type( $1, $2 ); }
|
    KEY_TYPE Type
	{ $$ = $2; }
;

PrimitiveType:
    KEY_STRING
	{ $$ = BT_STRING; }
|
    KEY_BOOLEAN
	{ $$ = BT_BOOLEAN; }
|
    KEY_INT
	{ $$ = BT_INT; }
|
    KEY_LONG
	{ $$ = BT_LONG; }
|
    KEY_SHORT
	{ $$ = BT_SHORT; }
|
    KEY_BYTE
	{ $$ = BT_BYTE; }
|
    KEY_CHAR
	{ $$ = BT_CHAR; }
|
    KEY_DOUBLE
	{ $$ = BT_DOUBLE; }
|
    KEY_FLOAT
	{ $$ = BT_FLOAT; }
|
    KEY_COMPLEX
	{ $$ = BT_COMPLEX; }
;

TupleType:
    '[' VerboseType OP_XOR Expression ']'
	{ $$ = new_VectorType( $2, $4 ); }
|
    '[' VerboseTypeList ']'
	{ $$ = new_TupleType( $2 ); }
;

ReferenceType:
    ClassOrInterfaceType PragmasOpt
	{ $$ = wrap_pragmas_type( $1, $2 ); }
|
    ArrayType
	{ $$ = $1; }
|
    PrimArrayType
	{ $$ = $1; }
;

GenericClassOrInterfaceType:
    Name OP_GENERIC_OPEN TemplateArgumentListOpt OP_GENERIC_CLOSE
	{ $$ = new_GenericObjectType( $1, $3 ); }
;

ClassOrInterfaceType:
    Name
	{ $$ = new_ObjectType( $1 ); }
|
    GenericClassOrInterfaceType
	{ $$ = $1; }
;

ClassType:
    ClassOrInterfaceType
	{ $$ = $1; }
;

InterfaceType:
    ClassOrInterfaceType
	{ $$ = $1; }
;

PrimArrayType:
    KEY_ARRAY '(' Type ')' 
	{ $$ = new_PrimArrayType( $3 ); }
;

ArrayType:
    PrimitiveType Dim
	{ $$ = unwrap_array_FormalDim( new_PrimitiveType( $1 ), $2 ); }
|
    TupleType Dim
	{ $$ = unwrap_array_FormalDim( $1, $2 ); }
|
    PrimArrayType Dim
	{ $$ = unwrap_array_FormalDim( $1, $2 ); }
|
    Name Dim
	{ $$ = unwrap_array_FormalDim( new_ObjectType( $1 ), $2 ); }
|
    ArrayType Dim
	{ $$ = unwrap_array_FormalDim( $1, $2 ); }
;

VerboseTypeList:
    VerboseType
	{ $$ = append_type_list( new_type_list(), $1 ); }
|
    VerboseTypeList ',' VerboseType
	{ $$ = append_type_list( $$, $3 ); }
|
    error ',' VerboseType
	{ $$ = append_type_list( new_type_list(), $3 ); }
;

/* Names */

Name:
    SimpleName
	{ $$ = $1; }
|
    QualifiedName
	{ $$ = $1; }
;

ClassAsName:
    KEY_CLASS Origin
	{ $$ = new_origsymbol( add_tmsymbol( "class" ), $2 ); }
;

SimpleName:
    Identifier
	{ $$ = $1; }
;

QualifiedName:
    Name '.' IDENTIFIER
	{ $$ = qualify_name( $1, $3 ); }
|
    Name '.' ClassAsName
	{ $$ = qualify_name( $1, $3->sym ); rfre_origsymbol( $3 ); }
;

/* Packages */

CompilationUnit:
    GlobalPragmasOpt PackageDeclarationOpt ImportDeclarationsOpt TypeDeclarationsOpt
	{
	    origsymbol_list directs;
	    origsymbol_list ondemands;

	    separate_imports( $3, &directs, &ondemands );
	    rfre_Import_list( $3 );
	    result = new_SparProgramUnit(
		$2,
		$1,
		new_tmsymbol_list(),
		directs,
		ondemands,
		$4,
		new_TypeBinding_list(),
		tmstringNIL
	    );
	}
;

ImportDeclarationsOpt:
    /* empty */
	{ $$ = new_Import_list(); }
|
    ImportDeclarations
	{ $$ = $1; }
;

ImportDeclarations:
    ImportDeclaration
	{ $$ = append_Import_list( new_Import_list(), $1 ); }
|
    ImportDeclarations ImportDeclaration
	{ $$ = append_Import_list( $1, $2 ); }
;

TypeDeclarationsOpt:
    /* empty */
	{ $$ = new_statement_list(); }
|
    TypeDeclarations
	{ $$ = $1; }
;

TypeDeclarations:
    TypeDeclaration
	{
	    $$ = new_statement_list();
	    if( $1 != declarationNIL ){
		$$ = append_statement_list( $$, $1 );
	    }
	}
|
    TypeDeclarations TypeDeclaration
	{
	    $$ = $1;
	    if( $2 != declarationNIL ){
		$$ = append_statement_list( $$, $2 );
	    }
	}
;

PackageDeclarationOpt:
    Origin
	{ $$ = new_origsymbol( add_tmsymbol( "" ), $1 ); }
|
    PackageDeclaration
	{ $$ = $1; }
;

PackageDeclaration:
    KEY_PACKAGE Name ';'
	{ $$ = $2; }
|
    KEY_PACKAGE error ';'
	{ $$ = origsymbolNIL; }
;

ImportDeclaration:
    SingleTypeImportDeclaration
	{ $$ = $1; }
|
    TypeImportOnDemandDeclaration
	{ $$ = $1; }
;

SingleTypeImportDeclaration:
    KEY_IMPORT Name ';'
	{ $$ = new_DirectImport( $2 ); }
|
    KEY_IMPORT error ';'
	{ $$ = ImportNIL; }
;

TypeImportOnDemandDeclaration:
    KEY_IMPORT Name '.' OP_TIMES ';'
	{ $$ = new_OnDemandImport( $2 ); }
;

TypeDeclaration:
    ClassDeclaration
	{ $$ = $1; }
|
    InterfaceDeclaration
	{ $$ = $1; }
|
    ';'
	{ $$ = declarationNIL; }
;

ModifiersOpt:
    /* empty */
	{ $$ = 0; }
|
    Modifiers
	{ $$ = check_access_modifiers( originNIL, $1 ); }
;

Modifiers:
    Modifier
	{ $$ = $1; }
|
    Modifiers Modifier
	{
	    if( $1 & $2 ){
		parserror( "duplicate modifier" );
	    }
	    $$ = $1 | $2;
	}
;

Modifier:
    KEY_INLINE
	{ $$ = ACC_INLINE; }
|
    KEY_PUBLIC
	{ $$ = ACC_PUBLIC; }
|
    KEY_PROTECTED
	{ $$ = ACC_PROTECTED; }
|
    KEY_PRIVATE
	{ $$ = ACC_PRIVATE; }
|
    KEY_STATIC
	{ $$ = ACC_STATIC; }
|
    KEY_ABSTRACT
	{ $$ = ACC_ABSTRACT; }
|
    KEY_FINAL
	{ $$ = ACC_FINAL; }
|
    KEY_NATIVE
	{ $$ = ACC_NATIVE; }
|
    KEY_SYNCHRONIZED
	{ $$ = ACC_SYNCHRONIZED; }
|
    KEY_STRICTFP
	{ $$ = ACC_STRICTFP; }
|
    KEY_TRANSIENT
	{ $$ = ACC_TRANSIENT; }
|
    KEY_VOLATILE
	{ $$ = ACC_VOLATILE; }
;

/* Classes */

/* Class Declaration */

ClassModifiersOpt:
    ModifiersOpt
	{
	    if( $1 & ~MODS_CLASS ){
		origin org = make_origin();
		complain_modifiers( org, $1 & ~MODS_CLASS, "class" );
		$1 &= MODS_CLASS;
		rfre_origin( org );
	    }
	    if( has_flags( $1, ACC_ABSTRACT|ACC_FINAL ) ){
		parserror( "A class cannot be both abstract and final" );
	    }
	    push_class_modifiers( $1 );
	    $$ = $1;
	}
;

TypeParameters:
    OP_GENERIC_OPEN TemplateFormalParameterListOpt OP_GENERIC_CLOSE
	{ $$ = $2; }
;

TypeParametersOpt:
    /* empty */
	{ $$ = new_FormalParameter_list(); }
|
    TypeParameters
	{ $$ = $1; }
;

ClassDeclaration:
    PragmasOpt ClassModifiersOpt KEY_CLASS Origin Identifier TypeParametersOpt SuperOpt InterfacesOpt ClassBody
	{
	    type super = $7;

	    if( super == typeNIL ){
		const tmsymbol obj = add_tmsymbol( "java.lang.Object" );

		if( $5->sym != obj ){
		    super = new_ObjectType( new_origsymbol( obj, gen_origin() ) );
		}
	    }
	    $$ = new_ClassDeclaration(
		$4,		// origin
		$1,
		NULL,		// Labels
		$2,		// modifiers
		false,		// Used?
		$5,		// name
		$6,		// formals
		$8,		// interfaces
		$9,		// body
		tmsymbolNIL,	// name of dynamic init fn.
		tmsymbolNIL,	// name of static init fn.
		tmsymbolNIL,	// name of static init 'done' var.
		false,		// Trivial local init block?
		HiddenParameter_listNIL,	// Hidden parms to constructors.
		super		// super
	    );
	    pop_class_modifiers();
	}
;

SuperOpt:
    /* empty */
	{ $$ = typeNIL; }
|
    Super
	{ $$ = $1; }
;

Super:
    KEY_EXTENDS ClassType
	{ $$ = $2; }
;

InterfacesOpt:
    /* empty */
	{ $$ = new_type_list(); }
|
    Interfaces
	{ $$ = $1; }
;

Interfaces:
    KEY_IMPLEMENTS InterfaceTypeList
	{ $$ = $2; }
;

InterfaceTypeList:
    InterfaceType
	{ $$ = append_type_list( new_type_list(), $1 ); }
|
    InterfaceTypeList ',' InterfaceType
	{ $$ = append_type_list( $1, $3 ); }
;

ClassBodyOpt:
    /* empty */
	{ $$ = statement_listNIL; }
|
    ClassBody
	{ $$ = $1; }
;

ClassBody:
    '{' ClassBodyDeclarationsOpt '}'
	{ $$ = $2; }
|
    '{' error '}'
	{ $$ = new_statement_list(); }
;

ClassBodyDeclarationsOpt:
    /* Empty */
	{ $$ = new_statement_list(); }
|
    ClassBodyDeclarations
	{ $$ = $1; }
;

ClassBodyDeclarations:
    ClassBodyDeclaration
	{ $$ = $1; }
|
    ClassBodyDeclarations ClassBodyDeclaration
	{ $$ = concat_statement_list( $1, $2 ); }
|
    error ';' ClassBodyDeclaration
	{ $$ = $3; }
;

ClassBodyDeclaration:
    ClassMemberDeclaration
	{ $$ = $1; }
|
    PragmasOpt StaticInitializer
	{ $$ = append_statement_list( new_statement_list(), add_statement_pragmas( $2, $1 ) ); }
|
    PragmasOpt ConstructorDeclaration
	{ $$ = append_statement_list( new_statement_list(), add_statement_pragmas( $2, $1 ) ); }
|
    PragmasOpt Origin Block
	{
	    $$ = append_statement_list(
		new_statement_list(),
		new_InstanceInitializer( $2, $1, NULL, $3 )
	    );
	}
|
    ';'
	{
	    if( warn_semicolon ){
		parsewarning( "This semicolon is only tolerated for backward compatibility" );
	    }
	    $$ = new_statement_list();
	}
;

ClassMemberDeclaration:
    FieldDeclaration
	{ $$ = $1; }
|
    MethodDeclaration
	{ $$ = append_statement_list( new_statement_list(), $1 ); }
|
    ClassDeclaration
	{ $$ = append_statement_list( new_statement_list(), $1 ); }
|
    InterfaceDeclaration
	{ $$ = append_statement_list( new_statement_list(), $1 ); }
;

/* Field Declarations */

FieldDeclaration:
    PragmasOpt ModifiersOpt Type VariableDeclarators ';'
	{
	    if( ($2 & (ACC_FINAL|ACC_VOLATILE)) == (ACC_FINAL|ACC_VOLATILE) ){
		parserror( "a final field cannot be volatile" );
		$2 &= ~ACC_VOLATILE;
	    }
	    if( $2 & ~MODS_FIELD ){
		origin org = make_origin();
		complain_modifiers( org, $2 & ~MODS_FIELD, "field" );
		$2 &= MODS_FIELD;
		rfre_origin( org );
	    }
	    $$ = unwrap_variableDeclarators( $2, $3, $4 );
	    rfre_type( $3 );
	    rfre_varid_list( $4 );
	    $$ = add_statement_list_pragmas( $$, $1 );
	}
;

VariableDeclarators:
    VariableDeclarator
	{ $$ = append_varid_list( new_varid_list(), $1 ); }
|
    VariableDeclarators ',' VariableDeclarator
	{ $$ = append_varid_list( $1, $3 ); }
;

VariableDeclarator:
    VariableDeclaratorId
	{ $$ = $1; }
|
    VariableDeclaratorId OP_ASSIGNMENT VariableInitializer
	{
	    $$ = $1;
	    $$->init = new_OptExpr( $3 );
	}
;

VariableDeclaratorId:
    Identifier
	{ $$ = new_varid( $1, new_FormalDim_list(), 0 ); }
|
    VariableDeclaratorId Dim
	{
	    $$ = $1;
	    $$->wrap = append_FormalDim_list( $$->wrap, $2 );
	}
;

VariableInitializer:
    Expression
	{ $$ = $1; }
|
    ArrayInitializer
	{ $$ = $1; }
;

/* Method Declarations */

MethodDeclaration:
    PragmasOpt MethodHeader MethodBody
	{
	    $$ = unwrap_method_declaration( $2, $3 );
	    $$ = add_declaration_pragmas( $$, $1 );
	}
;

MethodHeader:
    ModifiersOpt Type MethodDeclarator ThrowsOpt
	{
	    $$ = $3;
	    $$->modifiers = check_method_modifiers( $1 );
	    $$->t = $2;
	    $$->throws = $4;
	}
|
    ModifiersOpt KEY_VOID MethodDeclarator ThrowsOpt
	{
	    $$ = $3;
	    $$->modifiers = check_method_modifiers( $1 );
	    $$->t = new_VoidType();
	    $$->throws = $4;
	}
;

MethodDeclarator:
    Identifier '(' FormalParameterListOpt ')'
	{
	    $$ = new_MethodHeader(
		0,
		0,
		new_FormalDim_list(),
		$1,
		$3,
		0
	    );
	}
|
    MethodDeclarator Dim
	{ $$ = $1; $$->wrap = append_FormalDim_list( $$->wrap, $2 ); }
;

FormalParameterListOpt:
    /* empty */
	{ $$ = new_FormalParameter_list(); }
|
    FormalParameterList
	{ $$ = $1; }
;

FormalParameterList:
    FormalParameter
	{ $$ = append_FormalParameter_list( new_FormalParameter_list(), $1 ); }
|
    FormalParameterList ',' FormalParameter
	{ $$ = append_FormalParameter_list( $1, $3 ); }
|
    error ',' FormalParameter
	{ $$ = append_FormalParameter_list( new_FormalParameter_list(), $3 ); }
;

FormalParameter:
    PragmasOpt ModifiersOpt Type VariableDeclaratorId
	{
	    type t;
	    origsymbol s;
	    optexpression init;

	    unwrap_varid( $3, $4, &t, &s, &init );
	    assert( init == optexpressionNIL );
	    $$ = new_FormalParameter( s, $1, $2, t );
	}
;

TemplateFormalParameterListOpt:
    /* empty */
	{ $$ = new_FormalParameter_list(); }
|
    TemplateFormalParameterList
	{ $$ = $1; }
;

TemplateFormalParameterList:
    TemplateFormalParameter
	{ $$ = append_FormalParameter_list( new_FormalParameter_list(), $1 ); }
|
    TemplateFormalParameterList ',' TemplateFormalParameter
	{ $$ = append_FormalParameter_list( $1, $3 ); }
|
    error ',' TemplateFormalParameter
	{ $$ = append_FormalParameter_list( new_FormalParameter_list(), $3 ); }
;

TemplateFormalParameter:
    FormalParameter
	{ $$ = $1; }
|
    PragmasOpt ModifiersOpt KEY_TYPE Identifier
	{ $$ = new_FormalParameter( $4, $1, $2, new_TypeType() ); }
;

ThrowsOpt:
    /* empty */
	{ $$ = new_type_list(); }
|
    Throws
	{ $$ = $1; }
;

Throws:
    KEY_THROWS ClassTypeList
	{ $$ = $2; }
;

ClassTypeList:
    ClassType
	{ $$ = append_type_list( new_type_list(), $1 ); }
|
    ClassTypeList ',' ClassType
	{ $$ = append_type_list( $1, $3 ); }
;

MethodBody:
    Block
	{ $$ = $1; }
|
    ';'
	{ $$ = 0; }
;

/* Static Initializers */

StaticInitializer:
    KEY_STATIC Origin Block
	{
	    $$ = new_StaticInitializer( $2, Pragma_listNIL, NULL, $3 );
	}
;

/* Constructor Declarations */

ConstructorDeclaration:
    ModifiersOpt ConstructorDeclarator ThrowsOpt ConstructorBody
	{
	    if( ($1 & (ACC_PUBLIC|ACC_PRIVATE)) == (ACC_PUBLIC|ACC_PRIVATE) ){
		parserror( "Constructors cannot be both public and private" );
	    }
	    if( $1 & ~MODS_CONSTRUCTOR ){
		complain_modifiers( $2->name->org, $1 & ~MODS_CONSTRUCTOR, "constructor" );
		$1 &= MODS_CONSTRUCTOR;
	    }
	    $2->modifiers = $1;
	    $2->throws = $3;
	    $$ = unwrap_constructor_declaration( $2, $4 );
	}
;

ConstructorDeclarator:
    SimpleName '(' FormalParameterListOpt ')'
	{ $$ = new_MethodHeader( 0, 0, 0, $1, $3, 0 ); }
;

ConstructorBody:
    '{' ConstructorStatementsOpt '}'
	{ $$ = new_Block( tmsymbolNIL, Pragma_listNIL, $2 ); }
|
    '{' error '}'
	{ $$ = new_Block( tmsymbolNIL, Pragma_listNIL, new_statement_list() ); }
;

ConstructorStatementsOpt:
    ExplicitConstructorInvocation BlockStatementsOpt
	{ $$ = insert_statement_list( $2, 0, $1 ); }
|
    /* Per JLS 8.6.5: 'If a constructor body does not begin with an        */
    /* explicit constructor invocation and the constructor being declared  */
    /* is not part of the primordial class Object, then the constructor    */
    /* body is implicitly assumed by the compiler to begin with a          */
    /* superclass constructor invocation 'super()'.                        */
    BlockStatementsOpt
	{ $$ = insert_statement_list( $1, 0, build_default_super() ); }
;

ExplicitConstructorInvocation:
    PragmasOpt KEY_THIS Origin '(' ArgumentListOpt ')' ';'
	{ $$ = new_ThisConstructorInvocationStatement( $3, $1, NULL, $5 ); }
|
    PragmasOpt KEY_SUPER Origin '(' ArgumentListOpt ')' ';'
	{ $$ = new_SuperConstructorInvocationStatement( $3, $1, NULL, $5 ); }
|
    PragmasOpt Primary '.' KEY_SUPER Origin '(' ArgumentListOpt ')' ';'
	{ $$ = new_OuterSuperConstructorInvocationStatement( $5, $1, NULL, $2, $7 ); }
|
    PragmasOpt Name '.' KEY_SUPER Origin '(' ArgumentListOpt ')' ';'
	{ $$ = new_OuterSuperConstructorInvocationStatement( $5, $1, NULL, new_VariableNameExpression( $2, 0 ), $7 ); }
;

/* Interfaces */

/* Interface Declarations */

InterfaceModifiersOpt:
    ModifiersOpt
	{
	    if( $1 & ~MODS_INTERFACE ){
		origin org = make_origin();
		complain_modifiers( org, $1 & ~MODS_INTERFACE, "interface" );
		$1 &= MODS_INTERFACE;
		rfre_origin( org );
	    }
	    $$ = $1;
	    push_class_modifiers( $1 );
	}
;

InterfaceDeclaration:
    PragmasOpt InterfaceModifiersOpt KEY_INTERFACE Origin Identifier TypeParametersOpt ExtendsInterfacesOpt InterfaceBody
	{
	    $8 = enforce_interface_flags( $8 );
	    $$ = new_InterfaceDeclaration(
		$4,			// origin
		$1,
		NULL,			// Labels
		$2|ACC_ABSTRACT,	// modifiers
		TMFALSE,
		$5,			// name
		$6,			// type parameters
		$7,			// supers
		$8,			// body
		tmsymbolNIL,		// name of dynamic init fn.
		tmsymbolNIL,		// name of static init fn.
		tmsymbolNIL,		// name of static init 'done' flag.
		TMFALSE			// Trivial local init?
	    );
	    pop_class_modifiers();
	}
;

ExtendsInterfacesOpt:
    /* empty */
	{ $$ = new_type_list(); }
|
    ExtendsInterfaces
	{ $$ = $1; }
;

ExtendsInterfaces:
    KEY_EXTENDS InterfaceType
	{ $$ = append_type_list( new_type_list(), $2 ); }
|
    ExtendsInterfaces ',' InterfaceType
	{ $$ = append_type_list( $1, $3 ); }
;

InterfaceBody:
    '{' InterfaceMemberDeclarations '}'
	{ $$ = $2; }
|
    '{' error '}'
	{ $$ = new_statement_list(); }
;

InterfaceMemberDeclarations:
    /* empty */
	{ $$ = new_statement_list(); }
|
    InterfaceMemberDeclarations InterfaceMemberDeclaration
	{ $$ = concat_statement_list( $1, $2 ); }
;

InterfaceMemberDeclaration:
    ConstantDeclaration
	{ $$ = $1; }
|
    AbstractMethodDeclaration
	{
	    if( $1->flags & ~MODS_INTERFACE_METHOD ){
		complain_modifiers( $1->org, $1->flags & ~MODS_INTERFACE_METHOD, "interface method" );
		$1->flags &= MODS_INTERFACE_METHOD;
	    }
	    $$ = append_statement_list( new_statement_list(), $1 );
	}
|
    ClassDeclaration
	{ $$ = append_statement_list( new_statement_list(), $1 ); }
|
    InterfaceDeclaration
	{ $$ = append_statement_list( new_statement_list(), $1 ); }
|
    ';'
        {
	    if( warn_semicolon ){
		parsewarning( "This semicolon is only tolerated for backward compatibility" );
	    }
	    $$ = new_statement_list();
	}
;

ConstantDeclaration:
    FieldDeclaration
        {
	    $$ = enforce_constant_restrictions( $1 );
	}
;

AbstractMethodDeclaration:
    PragmasOpt MethodHeader ';'
	{
	    $$ = unwrap_abstract_declaration( $2 );
	    $$ = add_declaration_pragmas( $$, $1 );
	}
;

/* Arrays */

ArrayInitializer:
    '{' VariableInitializers CommaOpt '}'
	{ $$ = new_ArrayInitExpression( typeNIL, $2 ); }
|
    '{' error '}'
	{ $$ = new_ArrayInitExpression( typeNIL, new_expression_list() ); }
;

VariableInitializers:
    /* empty */
	{ $$ = new_expression_list(); }
|
    VariableInitializer
	{ $$ = append_expression_list( new_expression_list(), $1 ); }
|
    VariableInitializers ',' VariableInitializer
	{ $$ = append_expression_list( $1, $3 ); }
;

/* Blocks and Statements */

Block:
    '{' BlockStatementsOpt '}'
	{ $$ = new_Block( tmsymbolNIL, Pragma_listNIL, $2 ); }
|
    '{' error '}'
	{ $$ = new_Block( tmsymbolNIL, Pragma_listNIL, new_statement_list() ); }
;

BlockStatementsOpt:
    /* empty */
	{ $$ = new_statement_list(); }
|
    BlockStatements
	{ $$ = $1; }
;

BlockStatements:
    BlockStatement
	{ $$ = $1; }
|
    BlockStatements BlockStatement
	{ $$ = concat_statement_list( $1, $2 ); }
|
    error ';' BlockStatement
	{ $$ = $3; }
;

BlockStatement:
    LocalVariableDeclarationStatement
	{ $$ = $1; }
|
    Statement
	{ $$ = append_statement_list( new_statement_list(), $1 ); }
|
    ClassDeclaration
	{
	    if( has_any_flag( $1->flags, ACC_PUBLIC ) ){
		parserror( "A local class cannot be declared 'public'" );
	    }
	    $$ = append_statement_list( new_statement_list(), $1 );
	}
;

LocalVariableDeclarationStatement:
    PragmasOpt LocalVariableDeclaration ';'
	{ $$ = add_statement_list_pragmas( $2, $1 ); }
;

LocalVariableDeclaration:
    Type VariableDeclarators
	{
	    $$ = unwrap_variableDeclarators( 0, $1, $2 );
	    rfre_type( $1 );
	    rfre_varid_list( $2 );
	}
|
    KEY_FINAL Type VariableDeclarators
	{
	    $$ = unwrap_variableDeclarators( ACC_FINAL, $2, $3 );
	    rfre_type( $2 );
	    rfre_varid_list( $3 );
	}
;

Statement:
    PragmasOpt LabelName ':' Statement
	{
	    $$ = add_statement_label( $4, $2 );
	    $$ = add_statement_pragmas( $$, $1 );
	}
|
    PragmasOpt UnlabeledStatement
	{ $$ = add_statement_pragmas( $2, $1 ); }
;

UnlabeledStatement:
    Imperative
 	{ $$ = $1; }
|
    Control
 	{ $$ = $1; }
|
    Parallelization
 	{ $$ = $1; }
|
    MemoryManagement
	{ $$ = $1; }
|
    Support
        { $$ = $1; }
;

/* Imperative */

Imperative:
    ExpressionStatement
 	{ $$ = $1; }
;


/* Support */

Support:
    EmptyStatement
        { $$ = $1; }
|
    Print
        { $$ = $1; }
|
    PrintLn
        { $$ = $1; }
;

Print:
    KEY_PRINT Origin '(' ArgumentList ')' ';'
        { $$ = new_PrintStatement( $2, Pragma_listNIL, NULL, $4 ); }
;

PrintLn:
    KEY_PRINTLN Origin '(' ArgumentList ')' ';'
        { $$ = new_PrintLineStatement( $2, Pragma_listNIL, NULL, $4 ); }
;


/* Control */

Control:
    WhileStatement
 	{ $$ = $1; }
|
    ForStatement
	{ $$ = $1; }
|
    DoStatement
 	{ $$ = $1; }
|
    CardForStatement
 	{ $$ = $1; }
|
    IfStatement
 	{ $$ = $1; }
|
    WaitStatement
 	{ $$ = $1; }
|
    SwitchStatement
 	{ $$ = $1; }
|
    ReturnStatement
 	{ $$ = $1; }
|
    ValueReturnStatement
 	{ $$ = $1; }
|
    StatementBlock
	{ $$ = $1; }
|
    BreakStatement
	{ $$ = $1; }
|
    ContinueStatement
	{ $$ = $1; }
|
    SynchronizedStatement
	{ $$ = $1; }
|
    ThrowStatement
	{ $$ = $1; }
|
    TryStatement
	{ $$ = $1; }
;

CardForStatement:
    KEY_FOR Origin Cardinalities Statement
	{
	    $$ = new_ForStatement(
		$2,
		NULL,		// Pragmas
		NULL,		// Labels
		TMFALSE,
		$3,
		make_statement_Block( $4 )
	    );
	}
|
    KEY_INLINE KEY_FOR Origin Cardinalities Statement
	{
	    $$ = new_ForStatement(
		$3,
		NULL,		// Pragmas
		NULL,		// Labels
		TMTRUE,
		$4,
		make_statement_Block( $5 )
	    );
	}
;

/*
GotoStatement:
    KEY_GOTO Origin LabelName ';'
	{ $$ = new_GotoStatement( $2, Pragma_listNIL, NULL, $3 ); }
;
*/

StatementBlock:
    Origin Block
	{ $$ = new_BlockStatement( $1, Pragma_listNIL, NULL, $2 ); }
;

BreakStatement:
    KEY_BREAK Origin IdentifierOpt ';'
	{ $$ = new_BreakStatement( $2, NULL, NULL, $3 ); }
;

ContinueStatement:
    KEY_CONTINUE Origin IdentifierOpt ';'
	{ $$ = new_ContinueStatement( $2, NULL, NULL, $3 ); }
;

ThrowStatement:
    KEY_THROW Origin Expression ';'
	{ $$ = new_ThrowStatement( $2, NULL, NULL, $3 ); }
;

SynchronizedStatement:
    KEY_SYNCHRONIZED Origin '(' Expression ')' Block
	{ $$ = new_SynchronizedStatement( $2, NULL, NULL, $4, $6 ); }
;

TryStatement:
    KEY_TRY Origin Block Catches
	{ $$ = new_TryStatement( $2, NULL, NULL, $3, $4, BlockNIL ); }
|
    KEY_TRY Origin Block CatchesOpt Finally
	{ $$ = new_TryStatement( $2, NULL, NULL, $3, $4, $5 ); }
;

CatchesOpt:
    /* empty */
	{ $$ = new_Catch_list(); }
|
    Catches
	{ $$ = $1; }
;

Catches:
    CatchClause
	{ $$ = append_Catch_list( new_Catch_list(), $1 ); }
|
    Catches CatchClause
	{ $$ = append_Catch_list( $1, $2 ); }
;

CatchClause:
    KEY_CATCH Origin '(' FormalParameter ')' Block
	{ $$ = new_Catch( $2, $4, $6 ); }
;

Finally:
    KEY_FINALLY Block
	{ $$ = $2; }
;

/* Parallelization */

Parallelization:
    EachStatement
	{ $$ = $1; }
|
    ForeachStatement
	{ $$ = $1; }
;

EachStatement:
    KEY_EACH Origin '{' BlockStatementsOpt '}'
	{ $$ = new_EachStatement( $2, Pragma_listNIL, NULL, $4 ); }
;

ForeachStatement:
    KEY_FOREACH Origin Cardinalities Statement
	{ $$ = new_ForeachStatement( $2, Pragma_listNIL, NULL, $3, make_statement_Block( $4 ) ); }
;

Cardinalities:
    '(' CardinalityList CommaOpt ')'
	{
	    if( (features & FEAT_CARDS) == 0 ){
		parserror( "Cardinality lists are not allowed in Java" );
	    }
	    $$ = $2;
	}
|
    error ')'
	{ $$ = new_Cardinality_list(); }
|
    '[' CardinalityList CommaOpt ']'
	{
	    if( (features & FEAT_CARDS) == 0 ){
		parserror( "Cardinality lists are not allowed in Java" );
	    }
	    $$ = $2;
	}
|
    error ']'
	{ $$ = new_Cardinality_list(); }
;

/* Memory management */

MemoryManagement:
    Delete
	{ $$ = $1; }
;

Delete:
    KEY_DELETE Origin Expression ';'
	{ $$ = new_DeleteStatement( $2, Pragma_listNIL, NULL, $3 ); }
;

EmptyStatement:
    Origin ';'
        { $$ = new_EmptyStatement( $1, Pragma_listNIL, NULL ); }
;

ExpressionStatement:
    StatementExpression Origin ';'
	{
	    $$ = $1;
	    if( $$ != 0 && $$->org == 0 ){
		$$->org = $2;
	    }
	    else {
		rfre_origin( $2 );
	    }
	}
;

StatementExpression:
    Assignment Origin
	{
	    $$ = transmog_AssignmentExpression( to_AssignOpExpression( $1 ), $2 );
	    rfre_origin( $2 );
	}
|
    PreIncrementExpression
	{ $$ = transmog_PreIncrementExpression( to_PreIncrementExpression( $1 ) ); }
|
    PreDecrementExpression
	{ $$ = transmog_PreDecrementExpression( to_PreDecrementExpression( $1 ) ); }
|
    PostIncrementExpression
	{ $$ = transmog_PostIncrementExpression( to_PostIncrementExpression( $1 ) ); }
|
    PostDecrementExpression
	{ $$ = transmog_PostDecrementExpression( to_PostDecrementExpression( $1 ) ); }
|
    MethodInvocation
	{ $$ = transmog_invocation_expression( $1 ); }
|
    ClassInstanceCreationExpression Origin
	{ $$ = new_ExpressionStatement( $2, NULL, NULL, $1 ); }
;

IfStatement:
    KEY_IF Origin '(' Expression ')' Statement KEY_ELSE Statement
	{
	    Block then_block = make_statement_Block( $6 );
	    Block else_block = make_statement_Block( $8 );

	    $$ = new_IfStatement(
		$2,
		Pragma_listNIL,
		NULL,		// Labels
		false,		// eval both
		$4,
		then_block,
		else_block
	    );
	}
|
    KEY_IF Origin '(' Expression ')' Statement
	{
	    $$ = new_IfStatement(
		$2,
		Pragma_listNIL,
		NULL,		// Labels
		false,		// eval both
		$4,
		make_statement_Block( $6 ),
		new_Block( tmsymbolNIL, Pragma_listNIL, new_statement_list() )
	    );
	}
;

SwitchStatement:
    KEY_SWITCH Origin '(' Expression ')' '{' SwitchCaseList '}'
	{ $$ = new_SwitchStatement( $2, Pragma_listNIL, NULL, $4, $7 ); }
;

SwitchCaseList:
    /* empty */
	{ $$ = new_SwitchCase_list(); }
|
    SwitchCaseList SwitchCase
	{ $$ = append_SwitchCase_list( $1, $2 ); }
;

SwitchCase:
    KEY_CASE ConstantExpression Origin ':' BlockStatementsOpt
	{ $$ = new_SwitchCaseValue( $5, $3, $2 ); }
|
    KEY_DEFAULT Origin ':' BlockStatementsOpt
	{ $$ = new_SwitchCaseDefault( $4, $2 ); }
;

WaitStatement:
    KEY_WAIT Origin '{' WaitCaseList '}'
	{ $$ = new_WaitStatement( $2, Pragma_listNIL, NULL, $4 ); }
;

WaitCaseList:
    /* empty */
	{ $$ = new_WaitCase_list(); }
|
    WaitCaseList WaitCase
	{ $$ = append_WaitCase_list( $1, $2 ); }
;

WaitCase:
    KEY_CASE Expression Origin ':' BlockStatementsOpt
        { $$ = new_WaitCaseValue( $5, $3, $2 ); }
|
    KEY_TIMEOUT Expression Origin ':' BlockStatementsOpt
        { $$ = new_WaitCaseTimeout( $5, $3, $2 ); }
;

WhileStatement:
    KEY_WHILE Origin '(' Expression ')' Statement
	{
	    $$ = new_WhileStatement(
		$2,
		Pragma_listNIL,
		NULL,
		$4,
		make_statement_Block( $6 ),
		new_statement_list()	// update statements
	    );
	}
;

DoStatement:
    KEY_DO Origin Statement KEY_WHILE '(' Expression ')' ';'
	{
	    $$ = new_DoWhileStatement(
		$2,
		Pragma_listNIL,
		NULL,
		$6,
		make_statement_Block( $3 ),
		new_statement_list()	// update statements
	    );
	}
;

ForStatement:
    KEY_FOR Origin '(' ForInitOpt ';' ExpressionOpt ';' ForUpdateOpt ')' Statement
	{ $$ = new_ClassicForStatement( $2, Pragma_listNIL, NULL, $4, $6, $8, make_statement_Block( $10 ) ); }
;

ForInitOpt:
    /* empty */
	{ $$ = new_statement_list(); }
|
    ForInit
	{ $$ = $1; }
;

ForInit:
    StatementExpressionList
	{ $$ = $1; }
|
    LocalVariableDeclaration
	{ $$ = $1; }
;

ForUpdateOpt:
    /* empty */
	{ $$ = new_statement_list(); }
|
    ForUpdate
	{ $$ = $1; }
;

ForUpdate:
    StatementExpressionList
	{ $$ = $1; }
;

StatementExpressionList:
    StatementExpression
	{ $$ = append_statement_list( new_statement_list(), $1 ); }
|
    StatementExpressionList ',' StatementExpression
	{ $$ = append_statement_list( $1, $3 ); }
;

ReturnStatement:
    KEY_RETURN Origin ';'
	{ $$ = new_ReturnStatement( $2, Pragma_listNIL, NULL ); }
;

ValueReturnStatement:
    KEY_RETURN Origin Expression ';'
	{ $$ = new_ValueReturnStatement( $2, Pragma_listNIL, NULL, $3 ); }
;

/* Expressions */

Primary:
    PrimaryNoNewArray
	{ $$ = $1; }
|
    ArrayCreationExpression
	{ $$ = $1; }
;

/* Expressions that may also occur in macros. */

MacroExpression:
    Literal
	{ $$ = $1; }
|
    '(' Expression ')'
        { $$ = new_BracketExpression( $2 ); }
|
    KEY_GETBUF '(' Expression ')'
	{ $$ = new_GetBufExpression( $3 ); }
|
    KEY_COMPLEX '(' Expression ',' Expression ')'
	{ $$ = new_ComplexExpression( $3, $5 ); }
|
    KEY_COMPLEX '(' error ',' Expression ')'
	{ $$ = new_ComplexExpression( expressionNIL, $5 ); }
|
    KEY_COMPLEX '(' Expression ',' error ')'
	{ $$ = new_ComplexExpression( $3, expressionNIL ); }
|
    KEY_THIS Origin
	{
	    $$ = new_VariableNameExpression(
		new_origsymbol( add_tmsymbol( "this" ), $2 ),
		VAR_THIS
	    );
	}
|
    Name '.' KEY_THIS
	{ $$ = new_OuterThisExpression( new_ObjectType( $1 ) ); }
|
    ClassExpression
	{ $$ = $1; }
;

PrimaryNoNewArray:
    MacroExpression
	{ $$ = $1; }
|
    Vector
	{ $$ = new_VectorExpression( $1 ); }
|
    ClassInstanceCreationExpression
	{ $$ = $1; }
|
    ArrayAccess
	{ $$ = $1; }
|
    MethodInvocation
	{ $$ = $1; }
|
    FieldAccess
	{ $$ = $1; }
;

/* See
 * java.sun.com/products/jdk/1.1/docs/guide/innerclasses/spec/innerclasses.doc9.html
 */
ClassExpression:
    PrimitiveType '.' KEY_CLASS
	{ $$ = generate_basetype_ClassIdConstructorCall( $1 ); }
|
    KEY_VOID Origin '.' KEY_CLASS
	{ $$ = generate_ClassIdConstructorCall( add_origsymbol( "java.lang.Void" ) ); }
/*|
    TupleType '.' KEY_CLASS
	{ $$ = generate_ClassIdConstructorCall( $1 ); }
*/
;

ClassInstanceCreationExpression:
    KEY_NEW ClassType '(' ArgumentListOpt ')' ClassBodyOpt
	{ $$ = new_NewClassExpression( expressionNIL, $2, $4, $6 ); }
|
    /* TODO: allow parameterized types here. */
    Primary '.' KEY_NEW Identifier '(' ArgumentListOpt ')' ClassBodyOpt
	{ $$ = new_NewClassExpression( $1, new_ObjectType( $4 ), $6, $8 ); }
|
    /* TODO: allow parameterized types here. */
    Name '.' KEY_NEW Identifier '(' ArgumentListOpt ')' ClassBodyOpt
	{ $$ = new_NewClassExpression( new_VariableNameExpression( $1, 0 ), new_ObjectType( $4 ), $6, $8 ); }
;

ExpressionList:
    Expression
	{ $$ = append_expression_list( new_expression_list(), $1 ); }
|
    ExpressionList ',' Expression
	{ $$ = append_expression_list( $1, $3 ); }
|
    error ',' Expression
	{ $$ = append_expression_list( new_expression_list(), $3 ); }
;

ArgumentListOpt:
    /* empty */
	{ $$ = new_expression_list(); }
|
    ArgumentList
	{ $$ = $1; }
;

ArgumentList:
    Argument
	{ $$ = append_expression_list( new_expression_list(), $1 ); }
|
    ArgumentList ',' Argument
	{ $$ = append_expression_list( $1, $3 ); }
|
    error ',' Argument
	{ $$ = append_expression_list( new_expression_list(), $3 ); }
;

Argument:
    Expression
	{ $$ = $1; }
;

TemplateArgumentListOpt:
    /* empty */
	{ $$ = new_expression_list(); }
|
    TemplateArgumentList
	{ $$ = $1; }
;

TemplateArgumentList:
    TemplateArgument
	{ $$ = append_expression_list( new_expression_list(), $1 ); }
|
    TemplateArgumentList ',' TemplateArgument
	{ $$ = append_expression_list( $1, $3 ); }
|
    error ',' TemplateArgument
	{ $$ = append_expression_list( new_expression_list(), $3 ); }
;

TemplateArgument:
    Expression
	{ $$ = $1; }
|
    VerboseType
	{ $$ = new_TypeExpression( $1 ); }
;

ArrayCreationExpression:
    KEY_NEW PrimitiveType Vectors DimsOpt
	{
	    type t = wrap_ranked_type( new_PrimitiveType( $2 ), $4 );
	    $$ = new_NewArrayExpression(
		t,
		$3,
		build_DefaultInit( t )
	    );
	}
|
    KEY_NEW PrimitiveType Dims ArrayInitializer
	{
	    $$ = new_NewInitArrayExpression(
		wrap_ranked_type( new_PrimitiveType( $2 ), $3 ),
		$4
	    );
	}
|
    KEY_NEW ClassOrInterfaceType Vectors DimsOpt
	{
	    type t = wrap_ranked_type( $2, $4 );
	    $$ = new_NewArrayExpression(
		t,
		$3,
		build_DefaultInit( t )
	    );
	}
|
    KEY_NEW ClassOrInterfaceType Dims ArrayInitializer
	{
	    $$ = new_NewInitArrayExpression(
		wrap_ranked_type( $2, $3 ),
		$4
	    );
	}
|
    KEY_NEW TupleType Vectors DimsOpt
	{
	    type t = wrap_ranked_type( $2, $4 );
	    $$ = new_NewArrayExpression(
		t,
		$3,
		build_DefaultInit( t )
	    );
	}
|
    KEY_NEW TupleType Dims ArrayInitializer
	{
	    $$ = new_NewInitArrayExpression(
		wrap_ranked_type( $2, $3 ),
		$4
	    );
	}
;

DimsOpt:
    /* empty */
	{ $$ = new_FormalDim_list(); }
|
    Dims
	{ $$ = $1; }
;

Dims:
    Dim
	{ $$ = append_FormalDim_list( new_FormalDim_list(), $1 ); }
|
    Dims Dim
	{ $$ = append_FormalDim_list( $1, $2 ); }
;

Dim:
    '[' ']' PragmasOpt
	{ $$ = new_FormalDim( new_IntExpression( 1 ), $3 ); }
|
    '[' Size OP_XOR Expression ']' PragmasOpt
	{
	    if( (features & FEAT_ARRAY) == 0 ){
		parserror( "In Java, multidimensional arrays are not allowed" );
	    }
	    $$ = new_FormalDim( $4, $6 );
	}
|
    '[' SizeList CommaOpt ']' PragmasOpt
	{
	    if( (features & FEAT_ARRAY) == 0 ){
		if( $2 == 1 ){
		    parserror( "In Java array types, '*' is not allowed" );
		}
		else {
		    parserror( "In Java, multidimensional arrays are not allowed" );
		}
	    }
	    $$ = new_FormalDim( new_IntExpression( (int) $2 ), $5 );
	}
|
    '[' error ']'
	{ $$ = new_FormalDim( 0, new_Pragma_list() ); }
;

SizeList:
    Size
	{ $$ = 1u; }
|
    SizeList ',' Size
	{ $$ = $1+$3; }
|
    error ',' Size
	{ $$ = 1u; }
;

Size:
    OP_TIMES
	{ $$ = 1u; }
;

FieldAccess:
    Primary '.' Identifier
	{ $$ = new_FieldExpression( $1, $3 ); }
|
    KEY_SUPER Origin '.' Identifier
	{ $$ = new_SuperFieldExpression( $2, $4 ); }
|
    GenericClassOrInterfaceType '.' Identifier
	{ $$ = new_TypeFieldExpression( $1, $3 ); }
|
    Name '.' KEY_SUPER Origin '.' Identifier
	{ $$ = new_OuterSuperFieldExpression( new_ObjectType( $1 ), $4, $6 ); }
;

MethodInvocation:
    Name '(' ArgumentListOpt ')'
	{
	    $$ = new_MethodInvocationExpression(
		new_MethodInvocation( NULL, $1, new_expression_list(), $3, 0 )
	    );
	}
|
    Primary '.' Identifier '(' ArgumentListOpt ')'
	{ $$ = new_FieldInvocationExpression( $1, $3, $5 ); }
|
    GenericClassOrInterfaceType '.' Identifier Origin '(' ArgumentListOpt ')'
	{ $$ = new_TypeInvocationExpression( $4, $1, $3, $6 ); }
|
    KEY_SUPER Origin '.' Identifier '(' ArgumentListOpt ')'
	{ $$ = new_SuperInvocationExpression( $2, $4, $6 ); }
|
    Name '.' KEY_SUPER Origin '.' Identifier '(' ArgumentListOpt ')'
	{ $$ = new_OuterSuperInvocationExpression( new_ObjectType( $1 ), $4, $6, $8 ); }
;

ArrayAccess:
    Name VectorExpr
	{ $$ = new_SubscriptExpression( new_VariableNameExpression( $1, 0 ), $2 ); }
|
    PrimaryNoNewArray VectorExpr
	{ $$ = new_SubscriptExpression( $1, $2 ); }
;

PostfixExpression:
    Primary
	{ $$ = $1; }
|
    Name
	{
	    if( is_qualified_origsymbol( $1 ) ){
		origsymbol first;
		origsymbol last;

		break_qualified_name( $1, &first, &last );
		if( last->sym == add_tmsymbol( "class" ) ){
		    $$ = generate_ClassIdConstructorCall( first );
		    rfre_origsymbol( $1 );
		}
		else {
		    varflags flags = 0;
		    rfre_origsymbol( first );
		    if( last->sym == add_tmsymbol( "this" ) ){
			flags |= VAR_THIS;
		    }
		    $$ = new_VariableNameExpression( $1, flags );
		}
		rfre_origsymbol( last );
	    }
	    else {
		varflags flags = 0;

		if( $1->sym == add_tmsymbol( "this" ) ){
		    flags |= VAR_THIS;
		}
		$$ = new_VariableNameExpression( $1, flags );
	    }
	}
|
    PostIncrementExpression
	{ $$ = $1; }
|
    PostDecrementExpression
	{ $$ = $1; }
;

PostIncrementExpression:
    PostfixExpression OP_INCREMENT
	{ $$ = new_PostIncrementExpression( $1 ); }
;

PostDecrementExpression:
    PostfixExpression OP_DECREMENT
	{ $$ = new_PostDecrementExpression( $1 ); }
;

PreIncrementExpression:
    OP_INCREMENT OperatorExpression %prec OP_UNOP
	{ $$ = new_PreIncrementExpression( $2 ); }
;

PreDecrementExpression:
    OP_DECREMENT OperatorExpression %prec OP_UNOP
	{ $$ = new_PreDecrementExpression( $2 ); }
;

CastExpression:
    '(' PrimitiveType DimsOpt ')' OperatorExpression %prec OP_CAST
	{ $$ = new_CastExpression( wrap_ranked_type( new_PrimitiveType( $2 ), $3 ), $5 ); }
|
    '(' PrimArrayType DimsOpt ')' OperatorExpression %prec OP_CAST
	{ $$ = new_CastExpression( wrap_ranked_type( $2, $3 ), $5 ); }
|
    '(' TupleType DimsOpt ')' OperatorExpression %prec OP_CAST
	{ $$ = new_CastExpression( wrap_ranked_type( $2, $3 ), $5 ); }
|
    '(' Expression ')' UnaryExpression
	{
	    if( $2->tag == TAGVariableNameExpression ){
		$$ = new_CastExpression(
		    new_ObjectType(
			rdup_origsymbol( to_VariableNameExpression( $2 )->name )
		    ),
		    $4
		);
		rfre_expression( $2 );
	    }
	    else {
		$$ = new_SubscriptExpression( $2, $4 );
	    }
	}
|
    '(' Name Dims ')' OperatorExpression %prec OP_CAST
	{ $$ = new_CastExpression( wrap_ranked_type( new_ObjectType( $2 ), $3 ), $5 ); }
|
    '('  GenericClassOrInterfaceType DimsOpt ')' OperatorExpression %prec OP_CAST
	{ $$ = new_CastExpression( wrap_ranked_type( $2, $3 ), $5 ); }
;

UnaryExpression:
    PostfixExpression
	{ $$ = $1; }
|
    CastExpression
	{ $$ = $1; }
;

OperatorExpression:
    UnaryExpression
	{ $$ = $1; }
|
    PreIncrementExpression
	{ $$ = $1; }
|
    PreDecrementExpression
	{ $$ = $1; }
|
    OP_PLUS OperatorExpression %prec OP_UNOP
	{ $$ = new_UnopExpression( UNOP_PLUS, $2 ); }
|
    OP_MINUS OperatorExpression %prec OP_UNOP
	{ $$ = new_UnopExpression( UNOP_NEGATE, $2 ); }
|
    OP_INVERT OperatorExpression %prec OP_UNOP
	{ $$ = new_UnopExpression( UNOP_INVERT, $2 ); }
|
    OP_NOT OperatorExpression %prec OP_UNOP
	{ $$ = new_UnopExpression( UNOP_NOT, $2 ); }
|
    OperatorExpression OP_TIMES OperatorExpression
	{ $$ = new_BinopExpression( $1, BINOP_TIMES, $3 ); }
|
    OperatorExpression OP_DIVIDE OperatorExpression
	{ $$ = new_BinopExpression( $1, BINOP_DIVIDE, $3 ); }
|
    OperatorExpression OP_MOD OperatorExpression
	{ $$ = new_BinopExpression( $1, BINOP_MOD, $3 ); }
|
    OperatorExpression OP_PLUS OperatorExpression
	{ $$ = new_BinopExpression( $1, BINOP_PLUS, $3 ); }
|
    OperatorExpression OP_MINUS OperatorExpression
	{ $$ = new_BinopExpression( $1, BINOP_MINUS, $3 ); }
|
    OperatorExpression OP_SHIFTLEFT OperatorExpression
	{ $$ = new_BinopExpression( $1, BINOP_SHIFTLEFT, $3 ); }
|
    OperatorExpression OP_SHIFTRIGHT OperatorExpression
	{ $$ = new_BinopExpression( $1, BINOP_SHIFTRIGHT, $3 ); }
|
    OperatorExpression OP_USHIFTRIGHT OperatorExpression
	{ $$ = new_BinopExpression( $1, BINOP_USHIFTRIGHT, $3 ); }
|
    OperatorExpression OP_LESS OperatorExpression
	{ $$ = new_BinopExpression( $1, BINOP_LESS, $3 ); }
|
    OperatorExpression OP_LESSEQUAL OperatorExpression
	{ $$ = new_BinopExpression( $1, BINOP_LESSEQUAL, $3 ); }
|
    OperatorExpression OP_GREATER OperatorExpression
	{ $$ = new_BinopExpression( $1, BINOP_GREATER, $3 ); }
|
    OperatorExpression OP_GREATEREQUAL OperatorExpression
	{ $$ = new_BinopExpression( $1, BINOP_GREATEREQUAL, $3 ); }
|
    VerboseType KEY_INSTANCEOF Type
	{ $$ = new_TypeInstanceOfExpression( $1, $3 ); }
|
    OperatorExpression KEY_INSTANCEOF Type
	{ $$ = new_InstanceOfExpression( $1, $3 ); }
|
    OperatorExpression OP_EQUAL OperatorExpression
	{ $$ = new_BinopExpression( $1, BINOP_EQUAL, $3 ); }
|
    OperatorExpression OP_NOTEQUAL OperatorExpression
	{ $$ = new_BinopExpression( $1, BINOP_NOTEQUAL, $3 ); }
|
    OperatorExpression OP_AND OperatorExpression
	{ $$ = new_BinopExpression( $1, BINOP_AND, $3 ); }
|
    OperatorExpression OP_XOR OperatorExpression
	{ $$ = new_BinopExpression( $1, BINOP_XOR, $3 ); }
|
    OperatorExpression OP_OR OperatorExpression
	{ $$ = new_BinopExpression( $1, BINOP_OR, $3 ); }
|
    OperatorExpression OP_SHORTAND OperatorExpression
	{ $$ = new_ShortopExpression( $1, SHORTOP_AND, $3 ); }
|
    OperatorExpression OP_SHORTOR OperatorExpression
	{ $$ = new_ShortopExpression( $1, SHORTOP_OR, $3 ); }
|
    OperatorExpression OP_DIY_INFIX Name OperatorExpression
	{
	    expression_list parms = new_expression_list();
	    parms = append_expression_list( parms, $1 );
	    parms = append_expression_list( parms, $4 );
	    $$ = new_MethodInvocationExpression(
		new_MethodInvocation( NULL, $3, new_expression_list(), parms, 0 )
	    );
	}
|
    OperatorExpression '?' Origin Expression ':' OperatorExpression
	{ $$ = new_IfExpression( $1, $4, $6, $3 ); }
;

AssignmentExpression:
    OperatorExpression
	{ $$ = $1; }
|
    Assignment
	{ $$ = $1; }
;

AssignableExpression:
    OperatorExpression
        {
	    verify_assignable_expression( $1 );
	    $$ = $1;
	}
;

Assignment:
    AssignableExpression OP_ASSIGNMENT Expression
	{ $$ = new_AssignOpExpression( $1, ASSIGN, $3 ); }
|
    AssignableExpression OP_ASSIGNAND Expression
	{ $$ = new_AssignOpExpression( $1, ASSIGN_AND, $3 ); }
|
    AssignableExpression OP_ASSIGNDIVIDE Expression
	{ $$ = new_AssignOpExpression( $1, ASSIGN_DIVIDE, $3 ); }
|
    AssignableExpression OP_ASSIGNMINUS Expression
	{ $$ = new_AssignOpExpression( $1, ASSIGN_MINUS, $3 ); }
|
    AssignableExpression OP_ASSIGNMOD Expression
	{ $$ = new_AssignOpExpression( $1, ASSIGN_MOD, $3 ); }
|
    AssignableExpression OP_ASSIGNOR Expression
	{ $$ = new_AssignOpExpression( $1, ASSIGN_OR, $3 ); }
|
    AssignableExpression OP_ASSIGNPLUS Expression
	{ $$ = new_AssignOpExpression( $1, ASSIGN_PLUS, $3 ); }
|
    AssignableExpression OP_ASSIGNSHIFTLEFT Expression
	{ $$ = new_AssignOpExpression( $1, ASSIGN_SHIFTLEFT, $3 ); }
|
    AssignableExpression OP_ASSIGNSHIFTRIGHT Expression
	{ $$ = new_AssignOpExpression( $1, ASSIGN_SHIFTRIGHT, $3 ); }
|
    AssignableExpression OP_ASSIGNTIMES Expression
	{ $$ = new_AssignOpExpression( $1, ASSIGN_TIMES, $3 ); }
|
    AssignableExpression OP_ASSIGNUSHIFTRIGHT Expression
	{ $$ = new_AssignOpExpression( $1, ASSIGN_USHIFTRIGHT, $3 ); }
|
    AssignableExpression OP_ASSIGNXOR Expression
	{ $$ = new_AssignOpExpression( $1, ASSIGN_XOR, $3 ); }
;

ExpressionOpt:
    /* empty */
	{ $$ = new_OptExprNone(); }
|
    Expression
	{ $$ = new_OptExpr( $1 ); }
;

Expression:
    AssignmentExpression
	{ $$ = $1; }
|
    Pragmas Expression
	{ $$ = new_AnnotationExpression( $1, $2 ); }
;

ConstantExpression:
    Expression
	{ $$ = $1; }
;

%%

/* A simple wrapper function to give a nicer interface to the parser.
 * Given the file handle of the input file 'infile', the name of the
 * input file 'infilename', and the file handle of the output file
 * 'outfile', parse that input file, and write the generated code
 * to the output.
 */
SparProgramUnit parse( FILE *infile, const_tmstring infilename, bool strict_java )
{
    unsigned int old_features = features;
    if( strict_java ){
	features = java_features;
    }
    else {
	features = spar_features;
    }
    setlexfile( infile, infilename );
    class_modifiers_stack = new_modflags_list();
    if( yyparse() != 0 ){
	error( "cannot recover from earlier parse errors, goodbye" );
	return 0;
    }
    else {
	result->path = new_tmstring( infilename );
    }
    rfre_modflags_list( class_modifiers_stack );
    features = old_features;
    return result;
}
