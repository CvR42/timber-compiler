// File: analyze.c
//
// Simple analysis functions.

#include <tmc.h>
#include <string.h>

#include "defs.h"
#include "tmadmin.h"
#include "analyze.h"
#include "service.h"
#include "global.h"
#include "symbol_table.h"
#include "error.h"
#include "constfold.h"

// Given two AFLAGS, return the result if either one of them can occur.
AFLAG aflag_either( AFLAG a, AFLAG b )
{
    if( a == b ) return a;
    if( a == AF_UNKNOWN ) return b;
    if( b == AF_UNKNOWN ) return a;
    return AF_MAYBE;
}

// Given two AFLAGS, return the result if both of them occur.
AFLAG aflag_both( AFLAG a, AFLAG b )
{
    if( a == AF_UNKNOWN || b == AF_UNKNOWN ){
	return AF_UNKNOWN;
    }
    if( a == AF_YES || b == AF_YES ){
	return AF_YES;
    }
    if( a == AF_MAYBE || b == AF_MAYBE ){
	return AF_MAYBE;
    }
    return AF_NO;
}

// Given two abstract values, return the result if either of them
// can occur.
void abstract_either( AbstractValue a, AbstractValue b )
{
    a->zero = aflag_either( a->zero, b->zero );
    a->positive = aflag_either( a->positive, b->positive );
    a->constant &= b->constant;

    if( !isequal_expression( a->value, b->value ) ){
	rfre_expression( a->value );
	a->value = expressionNIL;
    }
}

// Given two abstract values, return the result if both of them occur.
void abstract_thenelse(
 AbstractValue a,
 const_AbstractValue thenval,
 const_AbstractValue elseval
)
{
    a->zero = aflag_both( a->zero, aflag_either( thenval->zero, elseval->zero ) );
    a->positive = aflag_both( a->positive, aflag_either( thenval->positive, elseval->positive ) );
    a->constant = thenval->constant & elseval->constant;

    if( a->value != expressionNIL ){
	rfre_expression( a->value );
    }
    if( isequal_expression( thenval->value, elseval->value ) ){
	a->value = rdup_expression( thenval->value );
    }
    else {
	a->value = expressionNIL;
    }
}

static void dump_AFLAG( FILE *f, char c, const AFLAG fl )
{
    fputc( c, f );
    fputc( ':', f );
    if( fl == AF_YES ){
	fputc( 'y', f );
    }
    else if( fl == AF_NO ){
	fputc( 'n', f );
    }
    else {
	fputc( '?', f );
    }
}

static void dump_AbstractValue( FILE *f, const_AbstractValue av )
{
    fputc( '(', f );
    dump_AFLAG( f, 'z', av->zero );
    fputc( ' ', f );
    dump_AFLAG( f, 'p', av->positive );
    fputc( ' ', f );
    if( av->constant ){
	fputc( 'c', f );
    }
    if( av->value != NULL ){
	fputc( 'v', f );
    }
    fputc( ')', f );
}

static void dump_VarState( FILE *f, const_VarState s )
{
    fprintf( f, "%s=A:", s->name->name );
    fputc( s->defassigned?'y':'n', f );
    fputs( " U:", f );
    fputc( s->defunassigned?'y':'n', f );
    fputc( ' ', f );
    if( s->final ){
	fputc( 'f', f );
    }
    if( s->escapes ){
	fputc( 'e', f );
    }
    fputc( ' ', f );
    dump_AbstractValue( f, s->value );
}

static void dump_VarState_list( FILE *f, const_VarState_list sl )
{
    for( unsigned int ix=0; ix<sl->sz; ix++ ){
	if( ix != 0 ){
	    fputc( ' ', f );
	}
	dump_VarState( f, sl->arr[ix] );
    }
}

void dump_programstate( FILE *f, unsigned int lvl, const_ProgramState st )
{
    fprintf( f, "%2d: %s", lvl, st->iterated?"(iterated) ":"" );
    dump_AFLAG( f, 'T', st->throws );
    fputc( ' ', f );
    dump_AFLAG( f, 'J', st->jumps );
    fputc( ' ', f );
    dump_AFLAG( f, 'R', st->returns );
    fputc( ' ', f );
    dump_VarState_list( f, st->varStates );
    fputc( '\n', f );
}

void dump_programstates( FILE *f, const_ProgramState_list states )
{
    fprintf( f, "Program state is now:\n" );

    unsigned int ix = states->sz;
    while( ix>0 ){
	ix--;
	dump_programstate( f, ix, states->arr[ix] );
    }
}

// Given a variable name, search the top program state for a
// definition of the variable.
// Return true iff the variable is found in the top program state,
// and then set '*pos' to the position of the variable in the list.
bool search_var_programstate(
 const_VarState_list vl,
 tmsymbol nm,
 unsigned int *pos
)
{
    for( unsigned int ix=0; ix<vl->sz; ix++ ){
	if( vl->arr[ix]->name == nm ){
	    *pos = ix;
	    return true;
	}
    }
    return false;
}

// Given a variable name, search the program states for a
// definition of the variable.
// Return true iff the variable is found in any of the program states,
// and then set '*lvl' to the index of the program state, and '*pos' to
// the position of the variable in the list of that program state.
bool search_var_programstates(
 const_ProgramState_list pl,
 unsigned int toplvl,
 tmsymbol nm,
 unsigned int *lvl,
 unsigned int *pos
)
{
    unsigned int ix=toplvl+1;

    if( ix>pl->sz ){
	ix = pl->sz;
    }
    while( ix>0 ){
	ix--;
	if( search_var_programstate( pl->arr[ix]->varStates, nm, pos ) ){
	    *lvl = ix;
	    return true;
	}
    }
    if( trace_analysis ){
	fprintf( stderr, "Variable '%s' not found in program state\n", nm->name );
    }
    return false;
}


// Given an expression 'x', return true if the expression is trivial
// in an initialization expression.
bool is_trivial_initialization( const_expression x )
{
    bool res = false;

    switch( x->tag ){
	case TAGByteExpression:
	case TAGShortExpression:
	case TAGIntExpression:
	case TAGLongExpression:
	case TAGFloatExpression:
	case TAGDoubleExpression:
	case TAGCharExpression:
	case TAGBooleanExpression:
	case TAGNullExpression:
	case TAGTypeExpression:
	case TAGSizeofExpression:
	case TAGClassIdExpression:
	    res = true;
	    break;

	case TAGComplexExpression:
	    res = is_trivial_initialization( to_const_ComplexExpression(x)->re )
	     && is_trivial_initialization( to_const_ComplexExpression(x)->im );
	    break;

	case TAGVectorExpression:
	{
	    const_expression_list xl = to_const_VectorExpression(x)->fields;

	    res = true;
	    for( unsigned int ix=0; ix<xl->sz; ix++ ){
		if( !is_trivial_initialization( xl->arr[ix] ) ){
		    res = false;
		    break;
		}
	    }
	    break;
	}

	case TAGUnopExpression:
	    res = is_trivial_initialization( to_const_UnopExpression(x)->operand );
	    break;

	case TAGInternalizeExpression:
	    res = is_trivial_initialization( to_const_InternalizeExpression(x)->x );
	    break;

	case TAGAnnotationExpression:
	    res = is_trivial_initialization( to_const_AnnotationExpression(x)->x );
	    break;

	case TAGBracketExpression:
	    res = is_trivial_initialization( to_const_BracketExpression(x)->x );
	    break;

	case TAGIfExpression:
	    res =
	     is_trivial_initialization( to_const_IfExpression(x)->cond ) &&
	     is_trivial_initialization( to_const_IfExpression(x)->thenval ) &&
	     is_trivial_initialization( to_const_IfExpression(x)->elseval );
	    break;

	case TAGBinopExpression:
	    res =
	     is_trivial_initialization( to_const_BinopExpression(x)->left ) &&
	     is_trivial_initialization( to_const_BinopExpression(x)->right );
	    break;

	case TAGShortopExpression:
	    res =
	     is_trivial_initialization( to_const_ShortopExpression(x)->left ) &&
	     is_trivial_initialization( to_const_ShortopExpression(x)->right );
	    break;

	case TAGCastExpression:
	{
	    const_type t = to_const_CastExpression(x)->t;

	    if(
		t->tag == TAGPrimitiveType &&
		to_const_PrimitiveType(t)->base != BT_STRING
	    ){
		res = is_trivial_initialization(
		    to_const_CastExpression(x)->x
		);
	    }
	    else {
		res = false;
	    }
	    break;
	}

	case TAGForcedCastExpression:
	{
	    const_type t = to_const_ForcedCastExpression(x)->t;

	    if(
		t->tag == TAGPrimitiveType &&
		to_const_PrimitiveType(t)->base != BT_STRING
	    ){
		res = is_trivial_initialization(
		    to_const_ForcedCastExpression(x)->x
		);
	    }
	    else {
		res = false;
	    }
	    break;
	}

	case TAGTypeInstanceOfExpression:
	    res = true;
	    break;

	case TAGInstanceOfExpression:
	    res = is_trivial_initialization( to_const_InstanceOfExpression(x)->x );
	    break;

	case TAGArrayInitExpression:
	case TAGAssignOpExpression:
	case TAGClassExpression:
	case TAGClassInstanceOfExpression:
	case TAGFieldExpression:
	case TAGFieldInvocationExpression:
	case TAGGetBufExpression:
	case TAGGetLengthExpression:
	case TAGGetSizeExpression:
	case TAGInterfaceInstanceOfExpression:
	case TAGMethodInvocationExpression:
	case TAGNewArrayExpression:
	case TAGNewClassExpression:
	case TAGNewInitArrayExpression:
	case TAGNewRecordExpression:
	case TAGNotNullAssertExpression:
	case TAGOuterSuperFieldExpression:
	case TAGOuterSuperInvocationExpression:
	case TAGOuterThisExpression:
	case TAGPostDecrementExpression:
	case TAGPostIncrementExpression:
	case TAGPreDecrementExpression:
	case TAGPreIncrementExpression:
	case TAGStringExpression:
	case TAGSubscriptExpression:
	case TAGSuperFieldExpression:
	case TAGSuperInvocationExpression:
	case TAGTypeFieldExpression:
	case TAGTypeInvocationExpression:
	case TAGVariableNameExpression:
	case TAGVectorSubscriptExpression:
	case TAGWhereExpression:
	case TAGDefaultValueExpression:		// Don't substitute in this
	    res = false;
	    break;

#if 0
	default:
	    break;
#endif
    }
    return res;
}

bool is_trivial_initialization( const_optexpression x )
{
    bool res = false;

    switch( x->tag ){
	case TAGOptExprNone:
	    res = false;
	    break;

	case TAGOptExpr:
	    res = is_trivial_initialization( to_const_OptExpr( x )->x );
	    break;
    }
    return res;
}

#if 0
// Given an expression 'x', return true iff the expression is the default
// initialization expression.
static bool is_default_initialization( const_expression x )
{
    bool res = false;

    switch( x->tag ){
	case TAGByteExpression:
	    res = to_const_ByteExpression(x)->v == 0;
	    break;

	case TAGShortExpression:
	    res = to_const_ShortExpression(x)->v == 0;
	    break;

	case TAGIntExpression:
	    res = to_const_IntExpression(x)->v == 0;
	    break;

	case TAGLongExpression:
	    res = to_const_LongExpression(x)->v == 0;
	    break;

	case TAGFloatExpression:
	{
	    vnus_float v = to_const_FloatExpression(x)->v;

	    res = (v == 0.0);
	    break;
	}

	case TAGDoubleExpression:
	{
	    vnus_double v = to_const_DoubleExpression(x)->v;

	    res = (v == 0.0);
	    break;
	}

	case TAGCharExpression:
	{
	    vnus_char v = to_const_CharExpression(x)->c;
	    res = (v==0);
	    break;
	}

	case TAGBooleanExpression:
	    res = to_const_BooleanExpression(x)->b == false;
	    break;

	case TAGNullExpression:
	case TAGDefaultValueExpression:
	    res = true;
	    break;

	case TAGTypeExpression:
	case TAGClassIdExpression:
	    res = false;
	    break;

	case TAGComplexExpression:
	    res = is_default_initialization( to_const_ComplexExpression(x)->re )
	     && is_default_initialization( to_const_ComplexExpression(x)->im );
	    break;

	case TAGVectorExpression:
	{
	    const_expression_list xl = to_const_VectorExpression(x)->fields;

	    res = true;
	    for( unsigned int ix=0; ix<xl->sz; ix++ ){
		if( !is_default_initialization( xl->arr[ix] ) ){
		    res = false;
		    break;
		}
	    }
	    break;
	}

	case TAGUnopExpression:
	    // TODO: this is not entirely correct.
	    res = false;
	    break;

	case TAGAnnotationExpression:
	    res = is_default_initialization( to_const_AnnotationExpression(x)->x );
	    break;

	case TAGIfExpression:
	    res =
	     is_default_initialization( to_const_IfExpression(x)->thenval ) &&
	     is_default_initialization( to_const_IfExpression(x)->elseval );
	    break;

	case TAGBinopExpression:
	    res = false;
	    break;

	case TAGCastExpression:
	    res = is_default_initialization( to_const_CastExpression(x)->x );
	    break;

	case TAGForcedCastExpression:
	    res = is_default_initialization( to_const_ForcedCastExpression(x)->x );
	    break;

	case TAGArrayInitExpression:
	case TAGAssignOpExpression:
	case TAGClassExpression:
	case TAGClassInstanceOfExpression:
	case TAGFieldExpression:
	case TAGFieldInvocationExpression:
	case TAGGetBufExpression:
	case TAGGetLengthExpression:
	case TAGGetSizeExpression:
	case TAGInstanceOfExpression:
	case TAGInterfaceInstanceOfExpression:
	case TAGMethodInvocationExpression:
	case TAGNewArrayExpression:
	case TAGNewClassExpression:
	case TAGNewInitArrayExpression:
	case TAGNewRecordExpression:
	case TAGNotNullAssertExpression:
	case TAGOuterSuperFieldExpression:
	case TAGOuterSuperInvocationExpression:
	case TAGPostDecrementExpression:
	case TAGPostIncrementExpression:
	case TAGPreDecrementExpression:
	case TAGPreIncrementExpression:
	case TAGSizeofExpression:
	case TAGStringExpression:
	case TAGInternalizeExpression:
	case TAGSubscriptExpression:
	case TAGSuperFieldExpression:
	case TAGSuperInvocationExpression:
	case TAGTypeFieldExpression:
	case TAGOuterThisExpression:
	case TAGTypeInstanceOfExpression:
	case TAGTypeInvocationExpression:
	case TAGVariableNameExpression:
	case TAGVectorSubscriptExpression:
	case TAGWhereExpression:
	    res = false;
	    break;

    }
    return res;
}
#endif

// Given an operator 'op', return true iff it is an operator that
// is allowed in compile-time constants according to JLS2 15.28
bool is_compiletime_constant_operator( BINOP op )
{
    switch( op ){
	case BINOP_TIMES:
	case BINOP_DIVIDE:
	case BINOP_MOD:

	case BINOP_PLUS:
	case BINOP_MINUS:

	case BINOP_SHIFTRIGHT:
	case BINOP_USHIFTRIGHT:
	case BINOP_SHIFTLEFT:

	case BINOP_LESS:
	case BINOP_LESSEQUAL:
	case BINOP_GREATER:
	case BINOP_GREATEREQUAL:

	case BINOP_EQUAL:
	case BINOP_NOTEQUAL:

	case BINOP_AND:
	case BINOP_OR:
	case BINOP_XOR:
	    return true;
    }
    return false;
}

// Given a type 't', return true iff casting an expression to this type
// still makes it a compile-time constant.
bool is_compiletime_constant_type( const_type t )
{
    bool res = true;
    if( is_String_type( t ) ){
	return true;
    }

    switch( t->tag ){
	case TAGObjectType:
	    // The one exception, String, has already been filtered out.
	    res = false;
	    break;

	case TAGPrimitiveType:
	    res = true;
	    break;

	case TAGTypeType:
	case TAGNullType:
	case TAGFunctionType:
	case TAGVoidType:
	case TAGPrimArrayType:
	case TAGArrayType:
	case TAGGenericObjectType:
	case TAGExceptionVariableType:
	case TAGGCRefLinkType:
	case TAGGCTopRefLinkType:
	    res = false;
	    break;

	case TAGPragmaType:
	    res = is_compiletime_constant_type( to_const_PragmaType(t)->t );
	    break;

	case TAGVectorType:
	    res = is_compiletime_constant_type( to_const_VectorType(t)->elmtype );
	    break;

	case TAGTupleType:
	{
	    const_type_list tl = to_const_TupleType(t)->fields;
	    res = true;
	    for( unsigned int ix=0; ix<tl->sz; ix++ ){
		if( !is_compiletime_constant_type( tl->arr[ix] ) ){
		    res = false;
		    break;
		}
	    }
	    break;
	}

	case TAGTypeOfIf:
	    internal_error( "TypeOfIf should have been rewritten" );
	    res = false;
	    break;

	case TAGTypeOf:
	    internal_error( "TypeOf should have been rewritten" );
	    res = false;
	    break;

    }
    return res;
}

// Given an expression 'x', return true if the expression is a
// compile-time constant according to JLS2 15.28.
//
// If 'vars' is not null, assume that variables are compile-time constants,
// and add them to 'vars'. In effect, if 'true' is returned, the meaning
// is: 'the expression is a compile-time constant, provided that all variables
// in 'vars' are compile-time constants too.'
bool is_compiletime_constant( const_expression x, origsymbol_list vars )
{
    bool res = false;

    if( x == expressionNIL ){
	return res;
    }
    switch( x->tag ){
	case TAGBooleanExpression:
	case TAGByteExpression:
	case TAGCharExpression:
	case TAGDoubleExpression:
	case TAGFloatExpression:
	case TAGIntExpression:
	case TAGLongExpression:
	case TAGShortExpression:
	case TAGSizeofExpression:
	case TAGStringExpression:
	case TAGTypeExpression:
	    res = true;
	    break;

	case TAGNullExpression:
	    // According to the JLS null is not a compiletime constant,
	    // but that's because of implementation problems, not
	    // anything logical.
	    res = !pref_strictanalysis;
	    break;

	case TAGDefaultValueExpression:		// Because will be overwritten
	case TAGArrayInitExpression:
	case TAGAssignOpExpression:
	case TAGClassExpression:
	case TAGClassIdExpression:
	case TAGClassInstanceOfExpression:
	case TAGFieldInvocationExpression:
	case TAGGetLengthExpression:
	case TAGGetSizeExpression:
	case TAGGetBufExpression:
	case TAGInstanceOfExpression:
	case TAGInterfaceInstanceOfExpression:
	case TAGMethodInvocationExpression:
	case TAGNewArrayExpression:
	case TAGNewClassExpression:
	case TAGNewInitArrayExpression:
	case TAGNewRecordExpression:
	case TAGOuterSuperFieldExpression:
	case TAGOuterSuperInvocationExpression:
	case TAGPostDecrementExpression:
	case TAGPostIncrementExpression:
	case TAGPreDecrementExpression:
	case TAGPreIncrementExpression:
	case TAGSuperFieldExpression:
	case TAGSuperInvocationExpression:
	case TAGTypeFieldExpression:
	case TAGOuterThisExpression:
	case TAGTypeInstanceOfExpression:
	case TAGTypeInvocationExpression:
	case TAGWhereExpression:
	    res = false;
	    break;

	case TAGSubscriptExpression:
	{
	    const_SubscriptExpression sx = to_const_SubscriptExpression(x);

	    res = is_compiletime_constant( sx->array, vars ) && 
		is_compiletime_constant( sx->subscripts, vars );
	    break;
	}

	case TAGVectorExpression:
	{
	    const_expression_list xl = to_const_VectorExpression(x)->fields;

	    for( unsigned int ix=0; ix<xl->sz; ix++ ){
		if( !is_compiletime_constant( xl->arr[ix], vars ) ){
		    res = false;
		    break;
		}
	    }
	    res = true;
	    break;
	}

	case TAGVectorSubscriptExpression:
	{
	    const_VectorSubscriptExpression vx = to_const_VectorSubscriptExpression(x);

	    res = is_compiletime_constant( vx->vector, vars );
	    break;

	}

	case TAGNotNullAssertExpression:
	{
	    const_NotNullAssertExpression cx = to_const_NotNullAssertExpression( x );
	    res = is_compiletime_constant( cx->x, vars );
	    break;
	}

	case TAGInternalizeExpression:
	{
	    const_InternalizeExpression cx = to_const_InternalizeExpression( x );
	    res = is_compiletime_constant( cx->x, vars );
	    break;
	}

	case TAGAnnotationExpression:
	{
	    const_AnnotationExpression cx = to_const_AnnotationExpression( x );
	    res = is_compiletime_constant( cx->x, vars );
	    break;
	}

	case TAGBracketExpression:
	{
	    const_BracketExpression cx = to_const_BracketExpression( x );
	    res = is_compiletime_constant( cx->x, vars );
	    break;
	}

	case TAGVariableNameExpression:
	    // TODO: be more accurate than this.
	    if( vars == origsymbol_listNIL ){
		res = false;
	    }
	    else {
		vars = append_origsymbol_list( vars, rdup_origsymbol( to_const_VariableNameExpression(x)->name ) );
		res = true;
	    }
	    break;

	case TAGFieldExpression:
	    res = false;
	    break;

	case TAGForcedCastExpression:
	    res = is_compiletime_constant( to_const_ForcedCastExpression(x)->x, vars );
	    break;

	case TAGCastExpression:
	{
	    res =
		is_compiletime_constant_type( to_const_CastExpression(x)->t ) &&
		is_compiletime_constant( to_const_CastExpression(x)->x, vars );
	    break;
	}

	case TAGComplexExpression:
	{
	    const_ComplexExpression cx = to_const_ComplexExpression( x );
	    res = is_compiletime_constant( cx->re, vars ) &&
		is_compiletime_constant( cx->im, vars );
	    break;
	}

	case TAGIfExpression:
	{
	    const_IfExpression cx = to_const_IfExpression( x );
	    res = is_compiletime_constant( cx->cond, vars ) &&
		is_compiletime_constant( cx->thenval, vars ) &&
		is_compiletime_constant( cx->elseval, vars );
	    break;
	}

	case TAGUnopExpression:
	{
	    const_UnopExpression cx = to_const_UnopExpression( x );
	    res = is_compiletime_constant( cx->operand, vars );
	    break;
	}

	case TAGBinopExpression:
	{
	    const_BinopExpression cx = to_const_BinopExpression( x );
	    if( is_compiletime_constant_operator( cx->optor ) ){
		res = is_compiletime_constant( cx->left, vars ) &&
		    is_compiletime_constant( cx->right, vars );
	    }
	    else {
		res = false;
	    }
	    break;
	}

	case TAGShortopExpression:
	{
	    const_ShortopExpression cx = to_const_ShortopExpression( x );
	    res = is_compiletime_constant( cx->left, vars ) &&
		is_compiletime_constant( cx->right, vars );
	    break;
	}

    }
    return res;
}

// Given an expression 'x', return true if the expression is a
// compile-time constant according to JLS2 15.28.
bool is_compiletime_constant( const_optexpression x, origsymbol_list vars )
{
    bool res = false;

    switch( x->tag ){
	case TAGOptExprNone:
	    res = false;
	    break;

	case TAGOptExpr:
	    res = is_compiletime_constant( to_const_OptExpr( x )->x, vars );
	    break;
    }
    return res;
}

static bool maybe_has_sideeffect( const_ActualDim_list dl, int flags );

// Given an expression, return true if it may have a side-effect.
bool maybe_has_sideeffect( const_expression x, int flags )
{
    bool res = true;

    if( x == expressionNIL ){
	return false;
    }
    if( has_any_flag( flags, NO_SE_NEW ) ){
	flags |= NO_SE_ARRAY_NEW;
    }
    if( has_any_flag( flags, NO_SE_EXCEPTION ) ){
	flags |= NO_SE_NULLPOINTER;
    }
    switch( x->tag ){
	case TAGBooleanExpression:
	case TAGByteExpression:
	case TAGCharExpression:
	case TAGClassIdExpression:
	case TAGDoubleExpression:
	case TAGFloatExpression:
	case TAGIntExpression:
	case TAGLongExpression:
	case TAGNullExpression:
	case TAGShortExpression:
	case TAGStringExpression:
	case TAGSuperFieldExpression:
	case TAGOuterSuperFieldExpression:
	case TAGTypeFieldExpression:
	case TAGOuterThisExpression:
	case TAGTypeExpression:
	case TAGDefaultValueExpression:
	case TAGSizeofExpression:
	    res = false;
	    break;

	case TAGMethodInvocationExpression:
	case TAGFieldInvocationExpression:
	case TAGSuperInvocationExpression:
	case TAGOuterSuperInvocationExpression:
	case TAGTypeInvocationExpression:
	case TAGAssignOpExpression:
	case TAGPostIncrementExpression:
	case TAGPostDecrementExpression:
	case TAGPreIncrementExpression:
	case TAGPreDecrementExpression:
	    res = true;
	    break;


	case TAGNewArrayExpression:
	    if( has_any_flag( flags, NO_SE_ARRAY_NEW ) ){
		res = maybe_has_sideeffect( to_const_NewArrayExpression(x)->sizes, flags )
		    || maybe_has_sideeffect( to_const_NewArrayExpression(x)->initval, flags );
	    }
	    else {
		res = true;
	    }
	    break;

	case TAGNewRecordExpression:
	    res = !has_any_flag( flags, NO_SE_NEW );
	    break;

	case TAGNewClassExpression:
	    if( has_any_flag( flags, NO_SE_NEW ) ){
		res = maybe_has_sideeffect( to_const_NewClassExpression(x)->parameters, flags ) ||
		    maybe_has_sideeffect( to_const_NewClassExpression(x)->outer, flags );
	    }
	    else {
		res = true;
	    }
	    if( to_const_NewClassExpression(x)->body != statement_listNIL ){
		res = true;
	    }
	    break;

	case TAGArrayInitExpression:
	    res = maybe_has_sideeffect( to_const_ArrayInitExpression(x)->elements, flags );
	    break;

	case TAGNewInitArrayExpression:
	    if( has_any_flag( flags, NO_SE_ARRAY_NEW ) ){
		res = maybe_has_sideeffect( to_const_NewInitArrayExpression(x)->init, flags );
	    }
	    else {
		res = true;
	    }
	    break;

	// Unfortunately, a variable name reference may have a static
	// init as sideeffect, so we have to be careful.
	case TAGVariableNameExpression:
	{
	    varflags vflags = to_const_VariableNameExpression(x)->flags;

	    if(
		has_any_flag( flags, NO_SE_CLASSINIT ) ||
		vflags & (VAR_LOCAL|VAR_FORMAL|VAR_DYNFIELD|VAR_GLOBAL)
	    ){
		res = false;
	    }
	    else {
		res = is_qualified_origsymbol( to_const_VariableNameExpression(x)->name );
	    }
	    break;
	}

	case TAGCastExpression:
	{
	    const_CastExpression cx = to_const_CastExpression( x );
	    res = maybe_has_sideeffect( cx->x, flags );
	    break;
	}

	case TAGComplexExpression:
	{
	    const_ComplexExpression cx = to_const_ComplexExpression( x );
	    res = maybe_has_sideeffect( cx->re, flags ) ||
		maybe_has_sideeffect( cx->im, flags );
	    break;
	}

	case TAGVectorExpression:
	    res = maybe_has_sideeffect( to_const_VectorExpression( x )->fields, flags );
	    break;

	case TAGIfExpression:
	{
	    const_IfExpression cx = to_const_IfExpression( x );
	    res = maybe_has_sideeffect( cx->cond, flags ) ||
		maybe_has_sideeffect( cx->thenval, flags )||
		maybe_has_sideeffect( cx->elseval, flags );
	    break;
	}

	case TAGWhereExpression:
	    // TODO: look at this more closely.
	    res = true;
	    break;

	case TAGUnopExpression:
	{
	    const_UnopExpression cx = to_const_UnopExpression( x );
	    res = maybe_has_sideeffect( cx->operand, flags );
	    break;
	}

	case TAGBinopExpression:
	{
	    const_BinopExpression cx = to_const_BinopExpression( x );
	    res = maybe_has_sideeffect( cx->left, flags ) ||
		maybe_has_sideeffect( cx->right, flags );
	    break;
	}

	case TAGShortopExpression:
	{
	    const_ShortopExpression cx = to_const_ShortopExpression( x );
	    res = maybe_has_sideeffect( cx->left, flags ) ||
		maybe_has_sideeffect( cx->right, flags );
	    break;
	}

	case TAGInstanceOfExpression:
	{
	    const_InstanceOfExpression cx = to_const_InstanceOfExpression( x );
	    res = maybe_has_sideeffect( cx->x, flags );
	    break;
	}

	case TAGInterfaceInstanceOfExpression:
	{
	    const_InterfaceInstanceOfExpression cx = to_const_InterfaceInstanceOfExpression( x );
	    res = maybe_has_sideeffect( cx->x, flags );
	    break;
	}

	case TAGTypeInstanceOfExpression:
	    res = false;
	    break;

	case TAGClassInstanceOfExpression:
	{
	    const_ClassInstanceOfExpression cx = to_const_ClassInstanceOfExpression( x );
	    res = maybe_has_sideeffect( cx->x, flags );
	    break;
	}

	case TAGGetSizeExpression:
	{
	    const_GetSizeExpression cx = to_const_GetSizeExpression( x );
	    res = maybe_has_sideeffect( cx->array, flags ) ||
		maybe_has_sideeffect( cx->dim, flags );
	    break;
	}

	case TAGGetBufExpression:
	{
	    const_GetBufExpression cx = to_const_GetBufExpression( x );
	    res = maybe_has_sideeffect( cx->array, flags );
	    break;
	}

	case TAGGetLengthExpression:
	{
	    const_GetLengthExpression cx = to_const_GetLengthExpression( x );
	    res = maybe_has_sideeffect( cx->array, flags );
	    break;
	}

	case TAGAnnotationExpression:
	{
	    const_AnnotationExpression cx = to_const_AnnotationExpression( x );
	    res = maybe_has_sideeffect( cx->x, flags );
	    break;
	}

	case TAGBracketExpression:
	{
	    const_BracketExpression cx = to_const_BracketExpression( x );
	    res = maybe_has_sideeffect( cx->x, flags );
	    break;
	}

	case TAGVectorSubscriptExpression:
	{
	    const_VectorSubscriptExpression cx = to_const_VectorSubscriptExpression( x );
	    res = maybe_has_sideeffect( cx->vector, flags );
	    break;
	}

	case TAGSubscriptExpression:
	{
	    const_SubscriptExpression cx = to_const_SubscriptExpression( x );
	    res = maybe_has_sideeffect( cx->array, flags ) ||
		maybe_has_sideeffect( cx->subscripts, flags );
	    break;
	}

	// Unfortunately, this may have a static
	// init as sideeffect, so..
	case TAGFieldExpression:
	    if(
		has_any_flag( flags, NO_SE_CLASSINIT ) &&
		has_any_flag( flags, NO_SE_EXCEPTION|NO_SE_NULLPOINTER )
	    ){
		res = maybe_has_sideeffect( to_const_FieldExpression(x)->rec, flags );
	    }
	    else {
		res = true;
	    }
	    break;

	case TAGInternalizeExpression:
	    res = maybe_has_sideeffect( to_const_InternalizeExpression(x)->x, flags );
	    break;

	case TAGNotNullAssertExpression:
	    if( has_any_flag( flags, NO_SE_EXCEPTION|NO_SE_NULLPOINTER ) ){
		res = maybe_has_sideeffect( to_const_NotNullAssertExpression(x)->x, flags );
	    }
	    else {
		// We consider a NullPointerException a sideeffect.
		 res = true;
	    }
	    break;

	case TAGClassExpression:
	    res = true;
	    break;

	case TAGForcedCastExpression:
	{
	    const_ForcedCastExpression cx = to_const_ForcedCastExpression( x );
	    res = maybe_has_sideeffect( cx->x, flags );
	    break;
	}

    }
    return res;
}

// Given a list of expressions, return true if one of the expressions
// may have a side-effect.
bool maybe_has_sideeffect( const_expression_list xl, int flags )
{
    for( unsigned int ix=0; ix<xl->sz; ix++ ){
	if( maybe_has_sideeffect( xl->arr[ix], flags ) ){
	    return true;
	}
    }
    return false;
}

static bool maybe_has_sideeffect( const_ActualDim_list dl, int flags )
{
    for( unsigned int ix=0; ix<dl->sz; ix++ ){
	const_ActualDim d = dl->arr[ix];
	if( maybe_has_sideeffect( d->vector, flags ) ){
	    return true;
	}
    }
    return false;
}

// Given an optional expression, return true if it may have  side-effect.
bool maybe_has_sideeffect( const_optexpression x, int flags )
{
    bool res = false;

    switch( x->tag ){
	case TAGOptExpr:
	    res =  maybe_has_sideeffect( to_const_OptExpr(x)->x, flags );
	    break;

	case TAGOptExprNone:
	    res = false;
	    break;
    }
    return res;
}

// Forward declaration.
static bool is_inlinable_actual( const_expression_list xl );

// Given an expression, return 'true' iff the expression should be substituted
// when inlining a method.
static bool is_inlinable_actual( const_expression x )
{
    bool res = false;

    if( x == expressionNIL ){
	return res;
    }
    switch( x->tag ){
	case TAGBooleanExpression:
	case TAGByteExpression:
	case TAGCharExpression:
	case TAGClassIdExpression:
	case TAGDoubleExpression:
	case TAGFloatExpression:
	case TAGGetBufExpression:
	case TAGIntExpression:
	case TAGLongExpression:
	case TAGNullExpression:
	case TAGShortExpression:
	case TAGSizeofExpression:
	case TAGStringExpression:
	case TAGTypeExpression:
	case TAGTypeFieldExpression:
	case TAGOuterThisExpression:
	case TAGTypeInstanceOfExpression:
	case TAGVariableNameExpression:
	    res = true;
	    break;

	case TAGArrayInitExpression:
	case TAGAssignOpExpression:
	case TAGDefaultValueExpression:		// Because is overwritten later
	case TAGFieldInvocationExpression:
	case TAGInstanceOfExpression:
	case TAGInterfaceInstanceOfExpression:
	case TAGMethodInvocationExpression:
	case TAGNewArrayExpression:
	case TAGNewClassExpression:
	case TAGNewInitArrayExpression:
	case TAGNewRecordExpression:
	case TAGOuterSuperFieldExpression:
	case TAGOuterSuperInvocationExpression:
	case TAGPostDecrementExpression:
	case TAGPostIncrementExpression:
	case TAGPreDecrementExpression:
	case TAGPreIncrementExpression:
	case TAGSuperFieldExpression:
	case TAGSuperInvocationExpression:
	case TAGTypeInvocationExpression:
	    res = false;
	    break;

	case TAGClassInstanceOfExpression:
	{
	    const_ClassInstanceOfExpression ix = to_const_ClassInstanceOfExpression(x);
	    res = is_inlinable_actual( ix->x );
	    break;
	}

	case TAGComplexExpression:
	{
	    const_ComplexExpression cx = to_const_ComplexExpression( x );
	    res = is_inlinable_actual( cx->re ) && is_inlinable_actual( cx->im );
	    break;
	}

	case TAGVectorExpression:
	    res = is_inlinable_actual( to_const_VectorExpression( x )->fields );
	    break;

	case TAGIfExpression:
	{
	    const_IfExpression cx = to_const_IfExpression( x );
	    res = is_inlinable_actual( cx->cond ) &&
		is_inlinable_actual( cx->thenval ) &&
		is_inlinable_actual( cx->elseval );
	    break;
	}

	case TAGWhereExpression:
	    // TODO: look at the expression more closely.
	    res = false;
	    break;

	case TAGUnopExpression:
	{
	    const_UnopExpression cx = to_const_UnopExpression( x );
	    res = is_inlinable_actual( cx->operand );
	    break;
	}

	case TAGShortopExpression:
	{
	    const_ShortopExpression cx = to_const_ShortopExpression( x );
	    res = is_inlinable_actual( cx->left ) && is_inlinable_actual( cx->right );
	    break;
	}

	case TAGBinopExpression:
	{
	    const_BinopExpression cx = to_const_BinopExpression( x );
	    res = is_inlinable_actual( cx->left ) && is_inlinable_actual( cx->right );
	    break;
	}

	case TAGGetSizeExpression:
	{
	    const_GetSizeExpression cx = to_const_GetSizeExpression( x );
	    res = is_inlinable_actual( cx->array ) && is_inlinable_actual( cx->dim );
	    break;
	}

	case TAGGetLengthExpression:
	{
	    const_GetLengthExpression cx = to_const_GetLengthExpression( x );
	    res = is_inlinable_actual( cx->array );
	    break;
	}

	case TAGAnnotationExpression:
	{
	    const_AnnotationExpression cx = to_const_AnnotationExpression( x );
	    res = is_inlinable_actual( cx->x );
	    break;
	}

	case TAGBracketExpression:
	{
	    const_BracketExpression cx = to_const_BracketExpression( x );
	    res = is_inlinable_actual( cx->x );
	    break;
	}

	case TAGVectorSubscriptExpression:
	{
	    const_VectorSubscriptExpression cx = to_const_VectorSubscriptExpression( x );
	    res = is_inlinable_actual( cx->vector );
	    break;
	}

	case TAGSubscriptExpression:
	{
	    const_SubscriptExpression cx = to_const_SubscriptExpression( x );
	    res = is_inlinable_actual( cx->array ) && is_inlinable_actual( cx->subscripts );
	    break;
	}

	case TAGFieldExpression:
	{
	    const_FieldExpression cx = to_const_FieldExpression( x );
	    res = is_inlinable_actual( cx->rec );
	    break;
	}

	case TAGClassExpression:
	    res = true;
	    break;

	case TAGNotNullAssertExpression:
	{
	    const_NotNullAssertExpression cx = to_const_NotNullAssertExpression( x );
	    res = is_inlinable_actual( cx->x );
	    break;
	}

	case TAGInternalizeExpression:
	{
	    const_InternalizeExpression cx = to_const_InternalizeExpression( x );
	    res = is_inlinable_actual( cx->x );
	    break;
	}

	case TAGCastExpression:
	{
	    const_CastExpression cx = to_const_CastExpression( x );
	    res = is_inlinable_actual( cx->x );
	    break;
	}

	case TAGForcedCastExpression:
	{
	    const_ForcedCastExpression cx = to_const_ForcedCastExpression( x );
	    res = is_inlinable_actual( cx->x );
	    break;
	}

    }
    return res;
}

// Given a list of expressions, return true if one of the expressions
// may have a side-effect.
static bool is_inlinable_actual( const_expression_list xl )
{
    for( unsigned int ix=0; ix<xl->sz; ix++ ){
	if( !is_inlinable_actual( xl->arr[ix] ) ){
	    return false;
	}
    }
    return true;
}

// Given an expression 'x', return the abstract evaluation of it.
AbstractValue abstract_value( const_expression x, const_ProgramState_list state, const_Entry_list symtab )
{
    AbstractValue res = AbstractValueNIL;

    if( x == expressionNIL ){
	return new_AbstractValue( AF_MAYBE, AF_MAYBE, false, expressionNIL );
    }
    switch( x->tag ){
	case TAGBinopExpression:	// TODO: check on positive
	    // Maybe zero, maybe positive.
	    res = new_AbstractValue( AF_MAYBE, AF_MAYBE, false, expressionNIL );
	    break;

	case TAGShortopExpression:
	    // Maybe zero, definitely positive.
	    res = new_AbstractValue( AF_MAYBE, AF_YES, false, expressionNIL );
	    break;

	case TAGBooleanExpression:
	{
	    vnus_boolean b = to_const_BooleanExpression(x)->b;
	    if( b ){
		// True is not zero
		res = new_AbstractValue( AF_NO, AF_YES, true, rdup_expression( x ) );
	    }
	    else {
		// False is zero
		res = new_AbstractValue( AF_YES, AF_YES, true, rdup_expression( x ) );
	    }
	    break;
	}

	case TAGByteExpression:
	{
	    vnus_byte v = to_const_ByteExpression(x)->v;
	    // Maybe zero, maybe positive.
	    res = new_AbstractValue(
		v==0?AF_YES:AF_NO,
		v>=0?AF_YES:AF_NO,
		true,
		rdup_expression( x )
	    );
	    break;
	}

	case TAGCharExpression:		// No check on positive
	    // Maybe zero, maybe positive.
	    res = new_AbstractValue( AF_MAYBE, AF_MAYBE, true, expressionNIL );
	    break;

	case TAGClassExpression:
	    // Maybe zero, maybe positive.
	    // NOTE: once we return a proper class for the 
	    // primitive types, we know that this can never return null.
	    res = new_AbstractValue( AF_MAYBE, AF_YES, true, expressionNIL );
	    break;

	case TAGClassIdExpression:
	    // Maybe zero, must be positive.
	    res = new_AbstractValue( AF_MAYBE, AF_YES, true, expressionNIL );
	    break;

	case TAGClassInstanceOfExpression:	// No check on positive
	    // Maybe zero, maybe positive.
	    res = new_AbstractValue( AF_MAYBE, AF_MAYBE, true, expressionNIL );
	    break;

	case TAGComplexExpression:	// No check on positive
	    // Maybe zero, maybe positive.
	    res = new_AbstractValue( AF_MAYBE, AF_MAYBE, true, expressionNIL );
	    break;

	case TAGDoubleExpression:	// No check on positive
	    // Maybe zero, maybe positive.
	    res = new_AbstractValue( AF_MAYBE, AF_MAYBE, true, expressionNIL );
	    break;

	case TAGFloatExpression:	// No check on positive
	    // Maybe zero, maybe positive.
	    res = new_AbstractValue( AF_MAYBE, AF_MAYBE, false, expressionNIL );
	    break;

	case TAGGetBufExpression:
	    // Not zero, positive.
	    res = new_AbstractValue( AF_NO, AF_YES, true, expressionNIL );
	    break;
	    
	case TAGGetSizeExpression:
	case TAGGetLengthExpression:
	    // Maybe zero, must be positive.
	    res = new_AbstractValue( AF_MAYBE, AF_YES, true, expressionNIL );
	    break;

	case TAGInstanceOfExpression:
	    // Maybe zero, maybe positive.
	    res = new_AbstractValue( AF_MAYBE, AF_MAYBE, true, expressionNIL );
	    break;

	case TAGTypeInstanceOfExpression:
	    // Maybe zero, maybe positive.
	    res = new_AbstractValue( AF_MAYBE, AF_MAYBE, true, expressionNIL );
	    break;

	case TAGIntExpression:
	{
	    vnus_int v = to_const_IntExpression(x)->v;
	    res = new_AbstractValue( v==0?AF_YES:AF_NO, v>=0?AF_YES:AF_NO, true, rdup_expression( x ) );
	    break;
	}

	case TAGInterfaceInstanceOfExpression:
	    // Maybe zero, maybe positive.
	    res = new_AbstractValue( AF_MAYBE, AF_MAYBE, true, expressionNIL );
	    break;

	case TAGLongExpression:
	{
	    vnus_long v = to_const_LongExpression(x)->v;
	    res = new_AbstractValue( v==0?AF_YES:AF_NO, v>=0?AF_YES:AF_NO, true, rdup_expression( x ) );
	    break;
	}

	case TAGPostDecrementExpression:
	case TAGPostIncrementExpression:
	case TAGPreDecrementExpression:
	case TAGPreIncrementExpression:
	    // Maybe zero, maybe positive.
	    res = new_AbstractValue( AF_MAYBE, AF_MAYBE, false, expressionNIL );
	    break;

	case TAGShortExpression:
	    // Maybe zero, maybe positive.
	    res = new_AbstractValue( AF_MAYBE, AF_MAYBE, true, expressionNIL );
	    break;

	case TAGStringExpression:
	    // Maybe zero, maybe positive.
	    res = new_AbstractValue( AF_MAYBE, AF_YES, true, expressionNIL );
	    break;

	case TAGTypeExpression:
	    // Maybe zero, maybe positive.
	    res = new_AbstractValue( AF_MAYBE, AF_MAYBE, true, expressionNIL );
	    break;

	case TAGUnopExpression:
	    // Maybe zero, maybe positive.
	    res = new_AbstractValue( AF_MAYBE, AF_MAYBE, false, expressionNIL );
	    break;

	case TAGVectorExpression:
	    // Maybe zero, maybe positive.
	    res = new_AbstractValue( AF_MAYBE, AF_MAYBE, false, expressionNIL );
	    break;

	case TAGArrayInitExpression:
	case TAGNewArrayExpression:
	case TAGNewClassExpression:
	case TAGNewInitArrayExpression:
	case TAGNewRecordExpression:
	    // Not zero, positive.
	    res = new_AbstractValue( AF_NO, AF_YES, true, expressionNIL );
	    break;

	case TAGFieldExpression:
	case TAGFieldInvocationExpression:
	case TAGSubscriptExpression:
	case TAGSuperFieldExpression:
	case TAGOuterSuperFieldExpression:
	case TAGSuperInvocationExpression:
	case TAGOuterSuperInvocationExpression:
	case TAGTypeFieldExpression:
	case TAGTypeInvocationExpression:
	    // Perhaps zero, perhaps positive.
	    res = new_AbstractValue( AF_MAYBE, AF_MAYBE, false, expressionNIL );
	    break;

	case TAGOuterThisExpression:
	    // A 'this' expression cannot be null
	    res = new_AbstractValue( AF_NO, AF_YES, true, expressionNIL );
	    break;

	case TAGMethodInvocationExpression:
	{
	    const_MethodInvocation call = to_const_MethodInvocationExpression(x)->invocation;
	    if( symtab != NULL && call->vtab == expressionNIL ){
		// This is not a virtual call, so we know which method
		// it actually will be. See if there is any useful
		// analysis information in the method entry.
		const_Entry me = search_Entry( symtab, call->name->sym );
		if( me != EntryNIL && me->tag == TAGFunctionEntry ){
		    const_AbstractValue av = to_const_FunctionEntry( me )->retval; 
		    if( av != AbstractValueNIL ){
			if( trace_analysis ){
			    fprintf(
				stderr,
				"Method '%s' has an abstract return value ",
				me->name->name
			    );
			    dump_AbstractValue( stderr, av );
			    fputs( "\n", stderr );
			}
			res = rdup_AbstractValue( av );
			break;
		    }
		}
	    }
	    res = new_AbstractValue( AF_MAYBE, AF_MAYBE, false, expressionNIL );
	    break;
	}

	case TAGVariableNameExpression:		// No check on positive
	{
	    unsigned int lvl;
	    unsigned int pos;
	    tmsymbol nm = to_const_VariableNameExpression(x)->name->sym;

	    if(  search_var_programstates( state, state->sz, nm, &lvl, &pos ) ){
		VarState v = state->arr[lvl]->varStates->arr[pos];
		bool iterated = false;

		if( !v->final ){
		    for( unsigned int ix=lvl; ix<state->sz; ix++ ){
			if( state->arr[ix]->iterated ){
			    iterated = true;
			    break;
			}
		    }
		}
		if( iterated ){
		    res = new_AbstractValue( AF_MAYBE, AF_MAYBE, false, expressionNIL );
		}
		else {
		    res = rdup_AbstractValue( v->value );
		}
	    }
	    else {
		res = new_AbstractValue( AF_MAYBE, AF_MAYBE, false, expressionNIL );
	    }
	    if( has_any_flag( to_const_VariableNameExpression(x)->flags, VAR_THIS ) ){
		// This is a 'this' variable. We know it can never be zero.
		res->zero = AF_NO;
		res->positive = AF_YES;
	    }
	    break;
	}

	case TAGSizeofExpression:
	    // Not zero, positive
	    res = new_AbstractValue(
		AF_NO,		// Zero
		AF_YES,		// Positive
		true,		// constant
		rdup_expression( x )
	    );
	    break;

	case TAGNullExpression:
	    // Zero, positive
	    res = new_AbstractValue(
		AF_YES,		// Zero
		AF_YES,		// Positive
		true,		// constant
		rdup_expression( x )
	    );
	    break;

	case TAGDefaultValueExpression:
	    // Zero, positive
	    res = new_AbstractValue( AF_YES, AF_YES, true, expressionNIL );
	    break;

	case TAGVectorSubscriptExpression:
	    // Perhaps zero, perhaps positive.
	    // TODO: be smarter about this.
	    res = new_AbstractValue( AF_MAYBE, AF_MAYBE, false, expressionNIL );
	    break;

	case TAGAssignOpExpression:
	    res = abstract_value( to_const_AssignOpExpression(x)->right, state, symtab );
	    break;

	case TAGIfExpression:
	{
	    // TODO: be smarter about constant conditions.
	    const_IfExpression cx = to_const_IfExpression( x );
	    AbstractValue thenval = abstract_value( cx->thenval, state, symtab );
	    AbstractValue elseval = abstract_value( cx->elseval, state, symtab );
	    abstract_either( thenval, elseval );
	    res = thenval;
	    rfre_AbstractValue( elseval );
	    break;
	}

	case TAGWhereExpression:
	    // TODO: look at the expression more closely.
	    res = new_AbstractValue( AF_MAYBE, AF_MAYBE, false, expressionNIL );
	    break;

	case TAGAnnotationExpression:
	{
	    // TODO: support an annotation that says yes or no here.
	    const_AnnotationExpression cx = to_const_AnnotationExpression( x );
	    res = abstract_value( cx->x, state, symtab );
	    break;
	}

	case TAGBracketExpression:
	{
	    const_BracketExpression cx = to_const_BracketExpression( x );
	    res = abstract_value( cx->x, state, symtab );
	    break;
	}

	// An expression with a null assert can only evaluate to not-null.
	case TAGNotNullAssertExpression:
	    res = new_AbstractValue( AF_NO, AF_MAYBE, false, expressionNIL );
	    break;

	// A string internalization assert can only evaluate to not-null.
	case TAGInternalizeExpression:
	    res = new_AbstractValue( AF_NO, AF_MAYBE, false, expressionNIL );
	    break;

	case TAGCastExpression:
	{
	    // TODO: be less paranoid when you can be. We only say
	    // perhaps because a narrowing conversion could alter the
	    // number of bits that are relevant, and hence the abstract
	    // value.
	    AbstractValue castval = abstract_value(
		to_const_CastExpression(x)->x,
		state,
		symtab
	    );
	    res = new_AbstractValue( AF_MAYBE, AF_MAYBE, castval->constant, expressionNIL );
	    if( castval->zero == AF_YES ){
		res->zero = AF_YES;
		res->positive = AF_YES;
		if(
		    castval->value != expressionNIL &&
		    castval->value->tag == TAGNullExpression
		){
		    res->value = castval->value;
		    castval->value = expressionNIL;
		}
	    }
	    rfre_AbstractValue( castval );
	    break;
	}

	case TAGForcedCastExpression:
	{
	    // TODO: be less paranoid when you can be. We only say
	    // perhaps because a narrowing conversion could alter the
	    // number of bits that are relevant, and hence the abstract
	    // value.
	    AbstractValue castval = abstract_value(
		to_const_ForcedCastExpression(x)->x,
		state,
		symtab
	    );
	    res = new_AbstractValue( AF_MAYBE, AF_MAYBE, castval->constant, expressionNIL );

	    if( castval->zero == AF_YES ){
		res->zero = AF_YES;
		res->positive = AF_YES;
		if(
		    castval->value != expressionNIL &&
		    castval->value->tag == TAGNullExpression
		){
		    res->value = castval->value;
		    castval->value = expressionNIL;
		}
	    }
	    rfre_AbstractValue( castval );
	    break;
	}

    }
    return res;
}

static AFLAG is_zerotrip_Cardinality( const_Cardinality card )
{
    expression lower;
    expression upper;
    expression diff;
    AFLAG res;

    if( card->lowerbound == expressionNIL ){
	lower = new_IntExpression( 0 );
    }
    else {
	lower = rdup_expression( card->lowerbound );
    }
    upper = rdup_expression( card->upperbound );
    diff = constant_fold_expression( new_BinopExpression( upper, BINOP_MINUS, lower ) );
    if( diff->tag == TAGIntExpression ){
	vnus_int v = to_IntExpression(diff)->v;

	if( v <= 0 ){
	    res = AF_YES;
	}
	else {
	    res = AF_NO;
	}
    }
    else {
	res = AF_MAYBE;
    }
    rfre_expression( diff );
    return res;
}

AFLAG is_zerotrip_Cardinality_list( const_Cardinality_list cards )
{
    AFLAG res = AF_NO;

    for( unsigned int ix=0; ix<cards->sz; ix++ ){
	AFLAG r1 = is_zerotrip_Cardinality( cards->arr[ix] );

	if( r1 == AF_YES ){
	    return AF_YES;
	}
	if( r1 == AF_MAYBE ){
	    res = AF_MAYBE;
	}
    }
    return res;
}

