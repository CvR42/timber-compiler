// File: analysis.c

#include <tmc.h>

#include "defs.h"
#include "tmadmin.h"
#include "analysis.h"
#include "global.h"
#include "vnusctl.h"
#include "pragma.h"

// Given two AFLAGS, return the result if either one of them can occur.
static AFLAG aflag_either( AFLAG a, AFLAG b )
{
    if( a == b ) return a;
    return AF_MAYBE;
}

// Given two AFLAGS, return the result if both of them occur.
static AFLAG aflag_both( AFLAG a, AFLAG b )
{
    if( a == AF_YES || b == AF_YES ){
	return AF_YES;
    }
    if( a == AF_MAYBE || b == AF_MAYBE ){
	return AF_MAYBE;
    }
    return AF_NO;
}

// Given an expression 'x', try to determine if the expression evaluates
// to zero/null/false. Possible answers are yes, no, or maybe.
AFLAG is_zero( const_expression x, const_declaration_list decls, const_ProgramState_list state )
{
    AFLAG res = AF_MAYBE;

    switch( x->tag ){
	case TAGExprNew:
	case TAGExprFilledNew:
	case TAGExprNulledNew:
	case TAGExprArray:
	case TAGExprShape:
	case TAGExprNewRecord:
	case TAGExprNewArray:
	case TAGExprString:
	case TAGExprSizeof:
	case TAGExprAddress:
	case TAGExprGetBuf:
	    res = AF_NO;
	    break;

	case TAGExprGetLength:
	case TAGExprGetSize:
	case TAGExprChar:
	case TAGExprShort:		// TODO: look at actual constant
	case TAGExprFloat:		// TODO: look at actual constant
	case TAGExprDouble:		// TODO: look at actual constant
	case TAGExprComplex:		// TODO: look at actual constant
	case TAGExprRecord:		// TODO: look more closely
	case TAGExprIsNewOwner:
	case TAGExprIsNewMaster:
	case TAGExprGetNewOwner:
	case TAGExprGetNewMaster:
	case TAGExprDeref:
	case TAGExprIsRaised:
	case TAGExprReduction:		// TODO: look more closely
	case TAGExprFieldNumber:	// TODO: look more closely
	case TAGExprSelection:
	case TAGExprFlatSelection:
	    res = AF_MAYBE;
	    break;

	case TAGExprFunctionCall:
	{
	    const_expression fn = to_const_ExprFunctionCall(x)->function;
	    bool val;

	    if( fn->tag == TAGExprName ){
		tmsymbol nm = to_const_ExprName(fn)->name->sym;
		const_declaration d = search_declaration( decls, nm );

		if( d != declarationNIL ){
		    if( get_boolean_pragma( Pragma_listNIL, d->pragmas, "nullvalue", &val ) ){
			res = val ? AF_YES:AF_NO;
			break;
		    }
		}
	    }
	    res = AF_MAYBE;
	    break;
	}

	case TAGExprLong:
	{
	    vnus_long v = to_const_ExprLong(x)->v;
	    res = v == 0?AF_YES:AF_NO;
	    break;
	}

	case TAGExprInt:
	{
	    vnus_int v = to_const_ExprInt(x)->v;
	    res = v == 0?AF_YES:AF_NO;
	    break;
	}

	case TAGExprByte:
	{
	    vnus_byte v = to_const_ExprByte(x)->v;
	    res = (v == 0)?AF_YES:AF_NO;
	    break;
	}

	case TAGExprBoolean:
	{
	    vnus_boolean b = to_const_ExprBoolean(x)->b;
	    res = b?AF_YES:AF_NO;
	    break;
	}

	case TAGExprBinop:
	case TAGExprCheckedIndex:		// TODO: look more closely
	case TAGExprUpperCheckedIndex:		// TODO: look more closely
	case TAGExprLowerCheckedIndex:		// TODO: look more closely
	case TAGExprField:
	case TAGExprIsBoundViolated:		// TODO: look more closely
	case TAGExprIsLowerBoundViolated:	// TODO: look more closely
	case TAGExprIsUpperBoundViolated:	// TODO: look more closely
	case TAGExprUnop:
	    res = AF_MAYBE;
	    break;

	case TAGExprName:
	{
	    unsigned int lvl;
	    unsigned int pos;
	    tmsymbol nm = to_const_ExprName(x)->name->sym;

	    if( search_var_programstates( state, state->sz, nm, &lvl, &pos ) ){
		VarState v= state->arr[lvl]->varStates->arr[pos];
		bool maybe_overwritten = false;

		if( !v->final ){
		    // See wether there are any iterations between our level
		    // and the level where the variable state is registered.
		    // We don't care wether the level of the variable is an
		    // iteration, since if so, the state change was caused
		    // within the iteration, and is therefore genuine.
		    for( unsigned int ix=lvl+1; ix<state->sz; ix++ ){
			if( state->arr[ix]->iterated ){
			    maybe_overwritten = true;
			    break;
			}
		    }
		}
		if( maybe_overwritten || v->aliased ){
		    res = AF_MAYBE;
		}
		else {
		    res = v->value->zero;
		}
	    }
	    else {
		res = AF_MAYBE;
	    }
	    break;
	}

	case TAGExprNull:
	    res = AF_YES;
	    break;

	case TAGExprIf:
	{
	    // TODO: be smarter about constant conditions.
	    const_ExprIf cx = to_const_ExprIf( x );
	    res = aflag_either( is_zero( cx->thenval, decls, state ), is_zero( cx->elseval, decls, state ) );
	    break;
	}

	case TAGExprWhere:
	    // TODO: look at the expression more closely.
	    res = AF_MAYBE;
	    break;

	case TAGExprWrapper:
	{
	    // TODO: support an annotation that says yes or no here.
	    const_ExprWrapper cx = to_const_ExprWrapper( x );
	    res = is_zero( cx->x, decls, state );
	    break;
	}

	// An expression with a null assert can only evaluate to not-null.
	case TAGExprNotNullAssert:
	    res = AF_NO;
	    break;

	case TAGExprCast:
	{
	    const_ExprCast cx = to_const_ExprCast( x );
	    res = is_zero( cx->x, decls, state );
	    break;
	}

    }
    return res;
}

// Given an expression 'x', try to determine if the expression evaluates
// to zero/null/false. Possible answers are yes, no, or maybe.
AFLAG is_zero( const_location l, const_ProgramState_list state )
{
    AFLAG res = AF_MAYBE;

    switch( l->tag ){
	case TAGLocField:
	case TAGLocFieldNumber:
	case TAGLocSelection:
	case TAGLocFlatSelection:
	case TAGLocDeref:
	case TAGLocWhere:	// TODO: have a closer look
	    res = AF_MAYBE;
	    break;

	case TAGLocName:
	{
	    unsigned int lvl;
	    unsigned int pos;
	    tmsymbol nm = to_const_LocName(l)->name->sym;

	    if( search_var_programstates( state, state->sz, nm, &lvl, &pos ) ){
		VarState v= state->arr[lvl]->varStates->arr[pos];
		bool maybe_overwritten = false;

		if( !v->final ){
		    // See wether there are any iterations between our level
		    // and the level where the variable state is registered.
		    // We don't care wether the level of the variable is an
		    // iteration, since if so, the state change was caused
		    // within the iteration, and is therefore genuine.
		    for( unsigned int ix=lvl+1; ix<state->sz; ix++ ){
			if( state->arr[ix]->iterated ){
			    maybe_overwritten = true;
			    break;
			}
		    }
		}
		if( maybe_overwritten || v->aliased ){
		    res = AF_MAYBE;
		}
		else {
		    res = v->value->zero;
		}
	    }
	    else {
		res = AF_MAYBE;
	    }
	    break;
	}

	case TAGLocWrapper:
	{
	    // TODO: support an annotation that says yes or no here.
	    const_LocWrapper cx = to_const_LocWrapper( l );
	    res = is_zero( cx->l, state );
	    break;
	}

	case TAGLocNotNullAssert:
	    // An location with a null assert can only evaluate to not-null.
	    res = AF_NO;
	    break;

    }
    return res;
}

static AFLAG is_zerotrip_cardinality( const_cardinality card )
{
    vnus_int lower;
    vnus_int upper;

    if( card->lowerbound == expressionNIL ){
	lower = 0;
    }
    else if( card->lowerbound->tag == TAGExprInt ){
	lower = to_const_ExprInt( card->lowerbound )->v;
    }
    else {
	return AF_MAYBE;
    }
    if( card->upperbound->tag == TAGExprInt ){
	upper = to_const_ExprInt( card->upperbound )->v;
    }
    else {
	return AF_MAYBE;
    }
    if( lower == upper ){
	return AF_YES;
    }
    return AF_NO;
}

AFLAG is_zerotrip_cardinality_list( const_cardinality_list cards )
{
    AFLAG res = AF_NO;

    for( unsigned int ix=0; ix<cards->sz; ix++ ){
	AFLAG r1 = is_zerotrip_cardinality( cards->arr[ix] );

	if( r1 == AF_YES ){
	    return AF_YES;
	}
	if( r1 == AF_MAYBE ){
	    res = AF_MAYBE;
	}
    }
    return res;
}


// Given two abstract values, return the result if either of them
// can occur.
void abstract_either( AbstractValue a, AbstractValue b )
{
    a->zero = aflag_either( a->zero, b->zero );
    a->positive = aflag_either( a->positive, b->positive );
    a->constant &= b->constant;

    if( !is_equivalent_expression( a->value, b->value ) ){
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

    // TODO: be smarter when both are the same.
    if( a->value != expressionNIL ){
	rfre_expression( a->value );
    }
    if( is_equivalent_expression( thenval->value, elseval->value ) ){
	a->value = rdup_expression( thenval->value );
    }
    else {
	a->value = expressionNIL;
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

// Given an expression 'x', return the abstract evaluation of it.
AbstractValue abstract_value( const_expression x, const_declaration_list defs, const_ProgramState_list state )
{
    AbstractValue res = AbstractValueNIL;

    if( x == expressionNIL ){
	return new_AbstractValue( AF_MAYBE, AF_MAYBE, false, expressionNIL );
    }
    switch( x->tag ){
	case TAGExprBinop:	// No check on positive
	case TAGExprReduction:	// TODO: have a closer look
	    // Maybe zero, maybe positive.
	    res = new_AbstractValue( AF_MAYBE, AF_MAYBE, false, expressionNIL );
	    break;

	case TAGExprBoolean:
	{
	    vnus_boolean b = to_const_ExprBoolean(x)->b;
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

	case TAGExprByte:
	{
	    vnus_byte v = to_const_ExprByte(x)->v;
	    // Maybe zero, maybe positive.
	    res = new_AbstractValue(
		v==0?AF_YES:AF_NO,
		v>=0?AF_YES:AF_NO,
		true,
		rdup_expression( x )
	    );
	    break;
	}

	case TAGExprChar:
	    // Maybe zero, maybe positive.
	    res = new_AbstractValue( AF_YES, AF_MAYBE, true, expressionNIL );
	    break;

	case TAGExprComplex:
	    // Maybe zero, maybe positive.
	    res = new_AbstractValue( AF_MAYBE, AF_MAYBE, false, expressionNIL );
	    break;

	case TAGExprDouble:	// No check on positive
	    // Maybe zero, maybe positive.
	    res = new_AbstractValue( AF_MAYBE, AF_MAYBE, true, expressionNIL );
	    break;

	case TAGExprFloat:	// No check on positive
	    // Maybe zero, maybe positive.
	    res = new_AbstractValue( AF_MAYBE, AF_MAYBE, false, expressionNIL );
	    break;

	case TAGExprGetSize:
	case TAGExprGetLength:
	    // Maybe zero, must be positive.
	    res = new_AbstractValue( AF_MAYBE, AF_YES, true, expressionNIL );
	    break;

	case TAGExprInt:
	{
	    vnus_int v = to_const_ExprInt(x)->v;
	    res = new_AbstractValue( v==0?AF_YES:AF_NO, v>=0?AF_YES:AF_NO, true, rdup_expression( x ) );
	    break;
	}

	case TAGExprLong:
	{
	    vnus_long v = to_const_ExprLong(x)->v;
	    res = new_AbstractValue( v==0?AF_YES:AF_NO, v>=0?AF_YES:AF_NO, true, rdup_expression( x ) );
	    break;
	}

	case TAGExprShort:
	{
	    vnus_short v = to_const_ExprShort(x)->v;
	    res = new_AbstractValue( v==0?AF_YES:AF_NO, v>=0?AF_YES:AF_NO, true, rdup_expression( x ) );
	    break;
	}

	case TAGExprString:
	    // Maybe zero, maybe positive.
	    res = new_AbstractValue( AF_MAYBE, AF_YES, true, expressionNIL );
	    break;

	case TAGExprUnop:
	    // Maybe zero, maybe positive.
	    res = new_AbstractValue( AF_MAYBE, AF_MAYBE, false, expressionNIL );
	    break;

	case TAGExprGetBuf:
	    // Not zero, positive.
	    res = new_AbstractValue( AF_NO, AF_YES, true, expressionNIL );
	    break;

	case TAGExprNew:
	    // Not zero, positive.
	    res = new_AbstractValue( AF_NO, AF_YES, true, expressionNIL );
	    break;

	case TAGExprFilledNew:
	    // Not zero, positive.
	    res = new_AbstractValue( AF_NO, AF_YES, true, expressionNIL );
	    break;

	case TAGExprNulledNew:
	    // Not zero, positive.
	    res = new_AbstractValue( AF_NO, AF_YES, true, expressionNIL );
	    break;

	case TAGExprNewRecord:
	case TAGExprNewArray:
	    // Not zero, positive.
	    res = new_AbstractValue( AF_NO, AF_YES, true, expressionNIL );
	    break;

	case TAGExprFieldNumber:
	case TAGExprArray:		// TODO: have a closer look
	case TAGExprShape:		// TODO: have a closer look
	case TAGExprAddress:
	case TAGExprDeref:
	case TAGExprField:
	case TAGExprFunctionCall:
	case TAGExprGetNewMaster:
	case TAGExprGetNewOwner:
	case TAGExprIsNewMaster:
	case TAGExprIsNewOwner:
	case TAGExprIsRaised:
	case TAGExprIsBoundViolated:
	case TAGExprIsUpperBoundViolated:
	case TAGExprIsLowerBoundViolated:
	case TAGExprSelection:
	case TAGExprFlatSelection:
	    // Perhaps zero, perhaps positive.
	    res = new_AbstractValue( is_zero( x, defs, state ), AF_MAYBE, false, expressionNIL );
	    break;

	case TAGExprRecord:		// TODO: have a closer look
	    // Perhaps zero, perhaps positive.
	    res = new_AbstractValue( AF_MAYBE, AF_MAYBE, false, expressionNIL );
	    break;

	case TAGExprName:
	{
	    unsigned int lvl;
	    unsigned int pos;
	    tmsymbol nm = to_const_ExprName(x)->name->sym;

	    if( search_var_programstates( state, state->sz, nm, &lvl, &pos ) ){
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
		if( iterated || v->aliased ){
		    res = new_AbstractValue( AF_MAYBE, AF_MAYBE, false, expressionNIL );
		}
		else {
		    res = rdup_AbstractValue( v->value );
		}
	    }
	    else {
		res = new_AbstractValue( AF_MAYBE, AF_MAYBE, false, expressionNIL );
	    }
	    break;
	}

	case TAGExprCheckedIndex:
	    // Not zero, positive
	    res = new_AbstractValue(
		AF_MAYBE,	// Zero
		AF_YES,		// Positive
		false,		// constant
		expressionNIL
	    );
	    break;

	case TAGExprLowerCheckedIndex:
	    // Not zero, positive
	    res = new_AbstractValue(
		AF_MAYBE,	// Zero
		AF_YES,		// Positive
		false,		// constant
		expressionNIL
	    );
	    break;

	case TAGExprUpperCheckedIndex:
	    // Not zero, positive
	    res = new_AbstractValue(
		AF_MAYBE,	// Zero
		AF_YES,		// Positive
		false,		// constant
		expressionNIL
	    );
	    break;

	case TAGExprSizeof:
	    // Not zero, positive
	    res = new_AbstractValue(
		AF_NO,		// Zero
		AF_YES,		// Positive
		true,		// constant
		rdup_expression( x )
	    );
	    break;

	case TAGExprNull:
	    // Zero, positive
	    res = new_AbstractValue(
		AF_YES,		// Zero
		AF_YES,		// Positive
		true,		// constant
		rdup_expression( x )
	    );
	    break;

	case TAGExprIf:
	{
	    // TODO: be smarter about constant conditions.
	    const_ExprIf cx = to_const_ExprIf( x );
	    AbstractValue thenval = abstract_value( cx->thenval, defs, state );
	    AbstractValue elseval = abstract_value( cx->elseval, defs, state );
	    abstract_either( thenval, elseval );
	    res = thenval;
	    rfre_AbstractValue( elseval );
	    break;
	}

	case TAGExprWhere:
	    // TODO: look at the expression more closely.
	    res = new_AbstractValue( AF_MAYBE, AF_MAYBE, false, expressionNIL );
	    break;

	case TAGExprWrapper:
	{
	    // TODO: support an annotation that says yes or no here.
	    const_ExprWrapper cx = to_const_ExprWrapper( x );
	    res = abstract_value( cx->x, defs, state );
	    break;
	}

	// An expression with a null assert can only evaluate to not-null.
	case TAGExprNotNullAssert:
	    res = new_AbstractValue( AF_NO, AF_MAYBE, false, expressionNIL );
	    break;

	case TAGExprCast:
	{
	    const_ExprCast xc = to_const_ExprCast( x );
	    AbstractValue castval = abstract_value( xc->x, defs, state );
	    type tc = xc->t;

	    if(
		tc->tag == TAGTypePointer ||
		tc->tag == TAGTypeNeutralPointer ||
		tc->tag == TAGTypeUnsizedArray ||
		tc->tag == TAGTypeArray
	    ){
		// We're talking about a pointer cast here. The analysis
		// results of the cast victim apply, but when we have a
		// value in the abstract expression, wrap it in the cast.
		if( castval->value != expressionNIL ){
		    // There is a value to be substituted. Wrap it
		    // in the cast expression.
		    // We do not simply stick a copy of x in res->value,
		    // since castval->value may be simplified.
		    ExprCast x1 = to_ExprCast( rdup_expression( x ) );
		    rfre_expression( x1->x );
		    x1->x = castval->value;
		    castval->value = x1;
		}
		res = castval;
	    }
	    else {
		// TODO: be less paranoid when you can be. We only say
		// perhaps because a narrowing conversion could alter the
		// number of bits that are relevant, and hence the abstract
		// value.
		res = new_AbstractValue( AF_MAYBE, AF_MAYBE, castval->constant, expressionNIL );
		if( castval->zero == AF_YES ){
		    res->zero = AF_YES;
		    res->positive = AF_YES;
		    if( castval->value != expressionNIL ){
			// There is a value to be substituted. Wrap it
			// in the cast expression.
			// We do not simply stick a copy of x in res->value,
			// since castval->value may be simplified.
			ExprCast x1 = to_ExprCast( rdup_expression( x ) );
			rfre_expression( x1->x );
			x1->x = castval->value;
			castval->value = expressionNIL;
			res->value = x1;
		    }
		}
		rfre_AbstractValue( castval );
	    }
	    break;
	}

    }
    return res;
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

static void dump_VarState( FILE *f, const_VarState s )
{
    fprintf( f, "%s=", s->name->name );
    dump_AFLAG( f, 'z', s->value->zero );
    fputc( ' ', f );
    dump_AFLAG( f, 'p', s->value->positive );
    fputc( ' ', f );
    if( s->value->constant ){
	fputc( 'c', f );
    }
    if( s->value->value != NULL ){
	fputc( 'v', f );
    }
    fputs( " fv:", f );
    if( s->fieldvalues == NULL ){
	fputc( '-', f );
    }
    else {
	fprintf( f, "%u", s->fieldvalues->sz );
    }
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

static void dump_programstate( FILE *f, unsigned int lvl, const_ProgramState st )
{
    fprintf( f, "%2d: %s", lvl, st->iterated?"(iterated) ":"" );
    dump_AFLAG( f, 't', st->throws ); 
    fputc( ' ', f );
    dump_AFLAG( f, 'j', st->jumps ); 
    fputc( ' ', f );
    dump_AFLAG( f, 'r', st->returns ); 
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
