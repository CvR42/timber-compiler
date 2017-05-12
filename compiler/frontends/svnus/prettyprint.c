/* File: prettyprint.c
 *
 * Pretty printing of kernel Vnus.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <tmc.h>

#include "defs.h"
#include "vnus_types.h"
#include "tmadmin.h"
#include "global.h"
#include "error.h"
#include "prettyprint.h"
#include "symbol_table.h"

/* Forward declaration of mutually recursive functions. */
static void pp_statements( FILE *f, unsigned int indent, statement_list smts );
static void pp_statement_list( FILE *f, unsigned int indent, statement_list smts );
static void pp_block( FILE *f, unsigned int indent, block smts );
static void pp_expression( FILE *f, expression x );
static void pp_expression_list( FILE *f, expression_list xl );
static void pp_actualParameters( FILE *f, const expression_list xl );
static void pp_type_list( FILE *f, const type_list l );

/* Given a file handle 'f' and a indent 'indent', generate this
 * indent.
 */
static void pp_indent( FILE *f, unsigned int indent )
{
    unsigned int n;

    for( n=0; n<indent; n++ ){
	fputc( ' ', f );
    }
}

/* Given a file handle 'f', an indent 'indent' and a tmstring 's',
 * generate a line consisting of 's' indented to the given indent.
 */
static void pp_indent_line( FILE *f, unsigned int indent, const char *s )
{
    pp_indent( f, indent );
    fprintf( f, "%s\n", s );
}

static void pp_tmsymbol( FILE *f, const tmsymbol s )
{
    fputs( s->name, f );
}

static void pp_origsymbol( FILE *f, const origsymbol s )
{
    pp_tmsymbol( f, s->sym );
}

/* A table of strings for the basetypes. */
static const char *basetype_strings[] =
{
    "boolean",	/* BT_BOOLEAN */
    "string",	/* BT_STRING */
    "complex",	/* BT_COMPLEX */
    "byte",	/* BT_BYTE */
    "short",	/* BT_SHORT */
    "char",	/* BT_CHAR */
    "int",	/* BT_INTEGER */
    "long",	/* BT_LONG */
    "float",	/* BT_FLOAT */
    "double"	/* BT_DOUBLE */
};

/* Given a file handle 'f' and a type 't', generate a type expression
 * for that type.
 */
static void pp_basetype( FILE *f, const BASETYPE t )
{
     fputs( basetype_strings[(int) t], f );
}

static void pp_distribution( FILE *f, const distribution d )
{
    switch( d->tag ){
	case TAGDistBlock:
	    fputs( "block", f );
	    break;

	case TAGDistCyclic:
	    fputs( "cyclic", f );
	    break;

	case TAGDistBC:
	    fputs( "blockcyclic ", f );
	    pp_expression( f, to_DistBC(d)->blocksize );
	    break;

	case TAGDistCollapsed:
	    fputs( "collapsed", f );
	    break;

	case TAGDistReplicated:
	    fputs( "replicated", f );
	    break;

	case TAGDistDontcare:
	    fputs( "dontcare", f );
	    break;

	case TAGDistLocal:
	    fputs( "local ", f );
	    pp_expression( f, to_DistLocal(d)->proc );
	    break;

    }
}

static void pp_distributions( FILE *f, distribution_list dl )
{
    unsigned int ix;

    if( dl == distribution_listNIL ){
        return;
    }
    fputs( "[", f );
    for( ix=0; ix<dl->sz; ix++ ){
	if( ix != 0 ){
	    fputs( ", ", f );
	}
	pp_distribution( f, dl->arr[ix] );
    }
    fputs( "]", f );
}

static void pp_size( FILE *f, size frm )
{
    switch( frm->tag ){
	case TAGSizeDontcare:
	    fprintf( f, "dontcare" );
	    break;

	case TAGSizeExpression:
	    pp_expression( f, to_SizeExpression(frm)->x );
	    break;

    }
}

static void pp_sizes( FILE *f, const size_list fl )
{
    unsigned int ix;

    fputs( "[", f );
    for( ix=0; ix<fl->sz; ix++ ){
	if( ix != 0 ){
	    fputs( ", ", f );
	}
	pp_size( f, fl->arr[ix] );
    }
    fputs( "]", f );
}

/* Given a file handle 'f' and a field 'fld', print the field. */
static void pp_field( FILE *f, const field fld )
{
    pp_origsymbol( f, fld->name );
    fputs( ":", f );
    pp_type( f, fld->elmtype );
}

/* Given a file handle 'f' and a field list 'l', print the field list. */
static void pp_field_list( FILE *f, const field_list l )
{
    unsigned int ix;

    fputs( "[ ", f );
    for( ix=0; ix<l->sz; ix++ ){
	if( ix != 0 ){
	    fputs( ", ", f );
	}
	pp_field( f, l->arr[ix] );
    }
    fputs( " ]", f );
}

/* Given a file handle 'f' and a type 't', generate a type expression
 * for that type.
 */
void pp_type( FILE *f, const type t )
{
    switch( t->tag ){
	case TAGTypeBase:
	    pp_basetype( f, to_TypeBase(t)->base );
	    break;

        case TAGTypeView:
	    fprintf( f, "view %u ", to_TypeView(t)->rank );
	    pp_type( f, to_TypeView(t)->elmtype );
	    break;

        case TAGTypeShape:
	    fputs( "shape ", f );
	    pp_sizes( f, to_TypeShape(t)->sizes );
	    fputs( " ", f );
	    pp_distributions( f, to_TypeShape(t)->distr );
	    fputs( " ", f );
	    pp_type( f, to_TypeShape(t)->elmtype );
	    break;

	case TAGTypePointer:
	    fputs( "pointer ", f );
	    pp_type( f, to_TypePointer(t)->elmtype );
	    break;

	case TAGTypeRecord:
	    fputs( "record ", f );
	    pp_field_list( f, to_TypeRecord(t)->fields );
	    break;

	case TAGTypeNeutralPointer:
	    /* This is wrong, but why not. */
	    fputs( "pointer anything", f );
	    break;

	case TAGTypeFunction:
	    fputs( "function [", f );
	    pp_type_list( f, to_TypeFunction(t)->formals );
	    fputs( "] ", f );
	    pp_type( f, to_TypeFunction(t)->retval );
	    break;

	case TAGTypeProcedure:
	    fputs( "procedure [", f );
	    pp_type_list( f, to_TypeProcedure(t)->formals );
	    fputs( "] ", f );
	    break;

    }
}

/* Given a list of types, print a formal parameter list from these types. */
static void pp_type_list( FILE *f, const type_list l )
{
    unsigned int ix;

    if( l->sz == 0 ){
	fputs( "()", f );
	return;
    }
    fputs( "( ", f );
    for( ix=0; ix<l->sz; ix++ ){
	if( ix != 0 ){
	    fputs( ", ", f );
	}
	pp_type( f, l->arr[ix] );
    }
    fputs( " )", f );
}

/* Given a formal parameter 'arg', generate a formal parameter declaration. */
static void pp_formalParameter( FILE *f, const formalParameter arg )
{
    pp_origsymbol( f, arg->name );
}

static void pp_formalParameters( FILE *f, const formalParameter_list argv )
{
    unsigned int ix;

    fputs( "[", f );
    for( ix=0; ix<argv->sz; ix++ ){
	if( ix != 0 ){
	    fputs( ", ", f );
	}
	pp_formalParameter( f, argv->arr[ix] );
    }
    fputs( "]", f );
}

/* A table of strings for the unary operators.  */
static const char *unop_strings[] =
{
    " not ",	/* UNOP_NOT */
    "+",	/* UNOP_PLUS */
    "~"		/* UNOP_NEGATE */
};

/* Given an operand and an operand, generate a unary operator expression.  */
static void pp_unary_operator( FILE *f, UNOP tor )
{
    fprintf( f, "%s", unop_strings[(int) tor] );
}

/* A table of strings for the binary operators. */
static const char *binop_strings[] =
{
    " and ",	/* BINOP_AND */
    " or ",	/* BINOP_OR */
    " mod ",	/* BINOP_MOD */
    "+",	/* BINOP_PLUS */
    "-",	/* BINOP_MINUS */
    "*",	/* BINOP_TIMES */
    "/",	/* BINOP_DIVIDE */
    "=",	/* BINOP_EQUAL */
    "<>",	/* BINOP_NOTEQUAL */
    "<",	/* BINOP_LESS */
    "<=",	/* BINOP_LESSEQUAL */
    ">",	/* BINOP_GREATER */
    ">=",	/* BINOP_GREATEREQUAL */
    "xor",	/* BINOP_XOR */
    "<<",	/* BINOP_SHIFTLEFT */
    ">>",	/* BINOP_SHIFTRIGHT */
    ">>>",	/* BINOP_USHIFTRIGHT */
};

/* Given two operands and an operator, generate a binary operator
 * expression.
 */
static void pp_binary_operator( FILE *f, BINOP tor )
{
    fprintf( f, "%s", binop_strings[(int) tor] );
}

/* Given a function call descriptor 'call', generate a function
 * call expression for it.
 */
static void pp_functioncall( FILE *f, ExprFunctionCall call )
{
    fputs( "functioncall ", f );
    pp_expression( f, call->function );
    fputs( " ", f );
    pp_actualParameters( f, call->parameters );
}
 
/* Given a file handle 'f' and a cardinality 'cl', print it. */
static void pp_cardinality( FILE *f, cardinality card )
{
   pp_origsymbol( f, card->name );
   fputs( ":", f );
   pp_expression( f, card->bound );
}

/* Given a file handle 'f' and a list of cardinalities 'cl', print it. */
static void pp_cardinalities( FILE *f, cardinality_list cl )
{
    unsigned int ix;

    fputs( "[", f );
    for( ix=0; ix<cl->sz; ix++ ){
	if( ix != 0 ){
	    fputs( ", ", f );
	}
	pp_cardinality( f, cl->arr[ix] );
    }
    fputs( "]", f );
}

static void pp_selectors( FILE *f, expression_list sels )
{
    unsigned int ix;

    fputs( "[", f );
    for( ix=0; ix<sels->sz; ix++ ){
	if( ix != 0 ){
	    fputs( ", ", f );
	}
	pp_expression( f, sels->arr[ix] );
    }
    fputs( "]", f );
}

#if 0
/* Given a selection, generate an expression for it.  */
static void pp_selection( FILE *f, selection sel )
{
    fputs( "( ", f );
    pp_expression( f, sel->selected );
    fputs( ", ", f );
    pp_selectors( f, sel->selectors );
    fputs( " )", f );
}
#endif

static void pp_pragma( FILE *f, pragma pr )
{
    switch( pr->tag ){
        case TAGPragmaFlag:
	    fputs( "flag ", f );
	    pp_origsymbol( f, pr->name );
            break;

	case TAGPragmaExpression:
	    fputs( "value ", f );
	    pp_origsymbol( f, pr->name );
	    fputs( " ", f );
	    pp_expression( f, to_PragmaExpression(pr)->x );
	    break;

    }
}

static void pp_pragma_list( FILE *f, pragma_list pl )
{
    unsigned int ix;

    fputs( "[", f );
    for( ix=0; ix<pl->sz; ix++ ){
        if( ix!=0 ){
	    fputs( ",", f );
        }
        pp_pragma( f, pl->arr[ix] );
    }
    fputs( "]", f );
}

/* Given a quantified selection, generate an expression for it.  */
static void pp_view( FILE *f, view sel)
{
    fputs( "view ", f );
    pp_cardinalities( f, sel->cardinalities );
    fputs( " ", f );
    pp_expression( f, sel->loc );
}

/* Given a file handle 'f' and an expression 'x', generate C code
 * to evaluate the expression.
 */
void pp_expression( FILE *f, expression x )
{
    switch( x->tag )
    {
	case TAGExprNull:
	    fputs( "null", f );
	    break;

	case TAGExprByte:
	    fprintf( f, "%sb", to_ExprByte(x)->v );
	    break;

	case TAGExprShort:
	    fprintf( f, "%ss", to_ExprShort(x)->v );
	    break;

	case TAGExprInt:
	    fprintf( f, "%s", to_ExprInt(x)->v );
	    break;

	case TAGExprLong:
	    fprintf( f, "%sl", to_ExprLong(x)->v );
	    break;

	case TAGExprFloat:
	    fprintf( f, "%s", to_ExprFloat(x)->v );
	    break;

	case TAGExprDouble:
	    fprintf( f, "%sd", to_ExprDouble(x)->v );
	    break;

	case TAGExprChar:
	    fprintf( f, "'%s'", to_ExprChar(x)->c );
	    break;

	case TAGExprCast:
	    fputs( "cast ", f );
	    pp_type( f, to_ExprCast(x)->t );
	    fputs( " ", f );
	    pp_expression( f, to_ExprCast(x)->x );
	    break;

	case TAGExprGetBlocksize:
	    fputs( "getblocksize ", f );
	    pp_expression( f, to_ExprGetBlocksize(x)->shape );
	    fputs( " ", f );
	    pp_expression( f, to_ExprGetBlocksize(x)->dim );
	    break;

	case TAGExprIsOwner:
	    fputs( "isowner ", f );
	    pp_expression( f, to_ExprIsOwner(x)->shape );
	    fputs( " ", f );
	    pp_expression( f, to_ExprIsOwner(x)->proc );
	    break;

	case TAGExprOwner:
	    fputs( "owner ", f );
	    pp_expression( f, to_ExprOwner(x)->shape );
	    break;

	case TAGExprSender:
	    fputs( "sender ", f );
	    pp_expression( f, to_ExprSender(x)->shape );
	    break;

	case TAGExprGetSize:
	    fputs( "getsize ", f );
	    pp_expression( f, to_ExprGetSize(x)->shape );
	    fputs( " ", f );
	    pp_expression( f, to_ExprGetSize(x)->dim );
	    break;

	case TAGExprGetRoom:
	    fputs( "getroom ", f );
	    pp_expression( f, to_ExprGetRoom(x)->shape );
	    break;

	case TAGExprIsMultidimDist:
	    fputs( "ismultidimdist ", f );
	    pp_expression( f, to_ExprIsMultidimDist(x)->shape );
	    break;

	case TAGExprString:
	    fprintf( f, "\"%s\"", to_ExprString(x)->s );
	    break;

	case TAGExprBoolean:
	    fprintf( f, "%s", to_ExprBoolean(x)->b ? "true" : "false" );
	    break;

	case TAGExprComplex:
	    fputs( "complex ", f );
	    pp_expression( f, to_ExprComplex(x)->re );
	    fputs( " ", f );
	    pp_expression( f, to_ExprComplex(x)->im );
	    break;

	case TAGExprRecord:
	    fputs( "record [", f );
	    pp_expression_list( f, to_ExprRecord(x)->fields );
	    fputs( "]", f );
	    break;

	case TAGExprFunctionCall:
	    pp_functioncall( f, to_ExprFunctionCall(x) );
	    break;

	case TAGExprSelection:
	    fputs( "(", f );
	    pp_expression( f, to_ExprSelection(x)->selected );
	    fputs( ", ", f );
	    pp_selectors( f, to_ExprSelection(x)->selectors );
	    fputs( ")", f );
	    break;

	case TAGExprView:
	    pp_view( f, to_ExprView(x)->v );
	    break;

	case TAGExprUnop:
	    fputs( "( ", f );
	    pp_unary_operator( f, to_ExprUnop(x)->optor );
	    fputs( ", ", f );
	    pp_expression( f, to_ExprUnop(x)->operand );
	    fputs( " )", f );
	    break;

	case TAGExprBinop:
	    fputs( "( ", f );
	    pp_expression( f, to_ExprBinop(x)->operanda );
	    fputs( ", ", f );
	    pp_binary_operator( f, to_ExprBinop(x)->optor );
	    fputs( ", ", f );
	    pp_expression( f, to_ExprBinop(x)->operandb );
	    fputs( " )", f );
	    break;

	case TAGExprPragma:
	    fputs( "expressionpragma ", f );
	    pp_pragma_list( f, to_ExprPragma(x)->pragmas );
	    fputs( " ", f );
	    pp_expression( f, to_ExprPragma(x)->x );
	    break;

	case TAGExprIf:
	    fputs( "if ", f );
	    pp_expression( f, to_ExprIf(x)->cond );
	    fputs( " ", f );
	    pp_expression( f, to_ExprIf(x)->thenval );
	    fputs( " ", f );
	    pp_expression( f, to_ExprIf(x)->elseval );
	    break;

	case TAGExprName:
	    pp_origsymbol( f, to_ExprName(x)->name );
	    break;

	case TAGExprField:
	    fputs( "field ", f );
	    pp_expression( f, to_ExprField(x)->rec );
	    fputs( " ", f );
	    pp_origsymbol( f, to_ExprField(x)->field );
	    break;

	case TAGExprDeref:
	    fputs( "*", f );
	    pp_expression( f, to_ExprDeref(x)->ref );
	    break;

	case TAGExprAddress:
	    fputs( "&", f );
	    pp_expression( f, to_ExprAddress(x)->adr );
	    break;

	case TAGExprNew:
	    fputs( "new ", f );
	    pp_type( f, to_ExprNew(x)->t );
	    break;

    }
}

/* Given a file handle and a list of expressions, generate an expression
 * list.
 */
static void pp_expression_list( FILE *f, expression_list xl )
{
    unsigned int ix;

    for( ix=0; ix<xl->sz; ix++ ){
	if( ix != 0 ){
	    fputs( ", ", f );
	}
	pp_expression( f, xl->arr[ix] );
    }
}

/* Given a file handle and a list of expressions, generate an expression
 * list.
 */
static void pp_actualParameters( FILE *f, const expression_list xl )
{
    fputs( "[", f );
    pp_expression_list( f, xl );
    fputs( "]", f );
}

static void pp_assignment( FILE *f, const int indent, const AssignStatement act )
{
    pp_indent( f, indent );
    fputs( "assign ", f );
    pp_expression( f, act->lhs );
    fputs( " ", f );
    pp_expression( f, act->rhs );
    fputs( "\n", f );
}

static void pp_switchCase( FILE *f, const int indent, const switchCase cs )
{
    pp_indent_line( f, indent, "(" );
    pp_indent( f, indent+INDENTSTEP );
    switch( cs->tag ){
	case TAGSwitchCaseValue:
	    fprintf( f, "%s,\n", to_SwitchCaseValue(cs)->cond );
	    break;

	case TAGSwitchCaseDefault:
	    fputs( "default,\n", f );
	    break;

    }
    pp_block( f, indent+INDENTSTEP, cs->body );
    pp_indent_line( f, indent, ")" );
}

static void pp_switchCase_list( FILE *f, const int indent, const switchCase_list l )
{
    unsigned int ix;

    for( ix=0; ix<l->sz; ix++ ){
	if( ix!=0 ){
	    pp_indent_line( f, indent, "," );
	}
	pp_switchCase( f, indent, l->arr[ix] );
    }
}

static void pp_statement( FILE *f, unsigned int indent, const statement smt )
{
    if( smt->label != origsymbolNIL ){
	pp_indent( f, indent );
	pp_origsymbol( f, smt->label );
        fputs( ":\n", f );
    }
    if( smt->pragmas != pragma_listNIL && smt->pragmas->sz != 0 ){
        pp_indent( f, indent );
        fputs( "pragma ", f );
        pp_pragma_list( f, smt->pragmas );
        fputs( "\n", f );
    }
    switch( smt->tag ){
        case TAGIfStatement:
	    pp_indent( f, indent );
	    fputs( "if ", f );
	    pp_expression( f, to_IfStatement(smt)->cond );
	    fputs( "\n", f );
	    pp_block( f, indent+INDENTSTEP, to_IfStatement(smt)->thenbody );
	    pp_block( f, indent+INDENTSTEP, to_IfStatement(smt)->elsebody );
	    break;

	case TAGBlockStatement:
	    pp_block( f, indent, to_BlockStatement(smt)->body );
	    break;

        case TAGWhileStatement:
	    pp_indent( f, indent );
	    fputs( "while ", f );
	    pp_expression( f, to_WhileStatement(smt)->cond );
	    fputs( "\n", f );
	    pp_block( f, indent+INDENTSTEP, to_WhileStatement(smt)->body );
	    break;

	case TAGReturnStatement:
	    pp_indent_line( f, indent, "return" );
	    break;

	case TAGValueReturnStatement:
	    pp_indent( f, indent );
	    fputs( "return ", f );
	    pp_expression( f, to_ValueReturnStatement(smt)->v );
	    fputs( "\n", f );
	    break;

	case TAGGotoStatement:
	    pp_indent( f, indent );
	    fputs( "goto ", f );
	    pp_origsymbol( f, to_GotoStatement(smt)->target );
	    break;

        case TAGForStatement:
	    pp_indent( f, indent );
	    fputs( "for ", f );
	    pp_cardinalities( f, to_ForStatement(smt)->cards );
	    fputs( "\n", f );
	    pp_block( f, indent+INDENTSTEP, to_ForStatement(smt)->body );
	    break;

        case TAGForallStatement:
	    pp_indent( f, indent );
	    fputs( "forall ", f );
	    pp_cardinalities( f, to_ForallStatement(smt)->cards );
	    fputs( "\n", f );
	    pp_block( f, indent+INDENTSTEP, to_ForallStatement(smt)->body );
	    break;

        case TAGForkallStatement:
	    pp_indent( f, indent );
	    fputs( "forkall ", f );
	    pp_cardinalities( f, to_ForkallStatement(smt)->cards );
	    fputs( "\n", f );
	    pp_block( f, indent+INDENTSTEP, to_ForkallStatement(smt)->body );
	    break;

	case TAGSwitchStatement:
	    pp_indent( f, indent );
	    fputs( "switch ", f );
	    pp_expression( f, to_SwitchStatement(smt)->cond );
	    fputs( " [\n ", f );
	    pp_switchCase_list( f, indent, to_SwitchStatement(smt)->cases );
	    pp_indent_line( f, indent, "]" );
	    break;

	case TAGEachStatement:
	    pp_indent_line( f, indent, "each [" );
	    pp_statement_list( f, indent+INDENTSTEP, to_EachStatement(smt)->statements );
	    pp_indent_line( f, indent, "]" );
	    break;

        case TAGForeachStatement:
	    pp_indent( f, indent );
	    fputs( "foreach ", f );
	    pp_cardinalities( f, to_ForeachStatement(smt)->cards );
	    fputs( "\n", f );
	    pp_block( f, indent+INDENTSTEP, to_ForeachStatement(smt)->body );
	    break;

	case TAGSendStatement:
	    pp_indent( f, indent );
	    fputs( "send ", f );
	    pp_expression( f, to_SendStatement(smt)->me );
	    fputs( " ", f );
	    pp_expression( f, to_SendStatement(smt)->dest );
	    fputs( " ", f );
	    pp_expression( f, to_SendStatement(smt)->elm );
	    fputs( "\n", f );
	    break;

	case TAGReceiveStatement:
	    pp_indent( f, indent );
	    fputs( "receive ", f );
	    pp_expression( f, to_ReceiveStatement(smt)->me );
	    fputs( " ", f );
	    pp_expression( f, to_ReceiveStatement(smt)->src );
	    fputs( " ", f );
	    pp_expression( f, to_ReceiveStatement(smt)->elm );
	    fputs( "\n", f );
	    break;

	case TAGBlockSendStatement:
	    pp_indent( f, indent );
	    fputs( "send ", f );
	    pp_expression( f, to_BlockSendStatement(smt)->me );
	    fputs( " ", f );
	    pp_expression( f, to_BlockSendStatement(smt)->dest );
	    fputs( " ", f );
	    pp_expression( f, to_BlockSendStatement(smt)->elm );
	    fputs( " ", f );
	    pp_expression( f, to_BlockSendStatement(smt)->nelm );
	    fputs( "\n", f );
	    break;

	case TAGBlockReceiveStatement:
	    pp_indent( f, indent );
	    fputs( "receive ", f );
	    pp_expression( f, to_BlockReceiveStatement(smt)->me );
	    fputs( " ", f );
	    pp_expression( f, to_BlockReceiveStatement(smt)->src );
	    fputs( " ", f );
	    pp_expression( f, to_BlockReceiveStatement(smt)->elm );
	    fputs( " ", f );
	    pp_expression( f, to_BlockReceiveStatement(smt)->nelm );
	    fputs( "\n", f );
	    break;

	case TAGEmptyStatement:
	    pp_indent_line( f, indent, "empty" );
	    break;

	case TAGBarrierStatement:
	    pp_indent_line( f, indent, "barrier" );
	    break;

        case TAGAssignStatement:
	    pp_assignment( f, indent, to_AssignStatement(smt) );
	    break;

        case TAGViewStatement:
	    pp_indent( f, indent );
	    fputs( "view ", f );
	    pp_expression( f, to_ViewStatement(smt)->viewaggr );
	    fputs( " (", f );
	    pp_cardinalities( f, to_ViewStatement(smt)->viewcards );
	    fputs( ",", f );
	    pp_expression( f, to_ViewStatement(smt)->viewstr );
	    fputs( ")\n", f );
	    break;

        case TAGPrintStatement:
            pp_indent( f, indent );
	    fputs( "print ", f );
	    pp_actualParameters( f, to_PrintStatement(smt)->argv );
	    fputs( "\n", f );
	    break;

        case TAGPrintLineStatement:
            pp_indent( f, indent );
	    fputs( "println ", f );
	    pp_actualParameters( f, to_PrintLineStatement(smt)->argv );
	    fputs( "\n", f );
	    break;

        case TAGProcedureCallStatement:
	{
	    ProcedureCallStatement call = to_ProcedureCallStatement(smt);
	    pp_indent( f, indent );
	    fputs( "procedurecall ", f );
	    pp_expression( f, call->proc );
	    fputs( " ", f );
	    pp_actualParameters( f, call->parameters );
	    fputs( "\n", f );
	    break;
	}

	case TAGSetSizeStatement:
	    pp_indent( f, indent );
	    fputs( "setsize ", f );
	    pp_expression( f, to_SetSizeStatement(smt)->shape );
	    fputs( " ", f );
	    pp_sizes( f, to_SetSizeStatement(smt)->sizes );
	    fputs( "\n", f );
	    break;

	case TAGSetRoomStatement:
	    pp_indent( f, indent );
	    fputs( "setroom ", f );
	    pp_expression( f, to_SetRoomStatement(smt)->shape );
	    fputs( " ", f );
	    pp_expression( f, to_SetRoomStatement(smt)->sz );
	    fputs( "\n", f );
	    break;

	case TAGFitRoomStatement:
	    pp_indent( f, indent );
	    fputs( "fitroom ", f );
	    pp_expression( f, to_FitRoomStatement(smt)->shape );
	    fputs( "\n", f );
	    break;

	case TAGDeleteStatement:
	    pp_indent( f, indent );
	    fputs( "delete ", f );
	    pp_expression( f, to_DeleteStatement(smt)->adr );
	    fputs( "\n", f );
	    break;

	case TAGGarbageCollectStatement:
	    pp_indent_line( f, indent, "garbagecollect" );
	    break;

    }
}

static void pp_statement_list( FILE *f, unsigned int indent, statement_list smts )
{
    unsigned int ix;

    for( ix=0; ix<smts->sz; ix++ ){
	if( ix != 0 ){
	    pp_indent_line( f, indent, "," );
	}
	pp_statement( f, indent, smts->arr[ix] );
    }
}

static void pp_statements( FILE *f, unsigned int indent, statement_list smts )
{
    pp_indent( f, indent );
    fputs( "statements [\n", f );
    pp_statement_list( f, indent+INDENTSTEP, smts );
    pp_indent( f, indent );
    fputs( "]\n", f );
}

static void pp_block( FILE *f, unsigned int indent, block blk )
{
    pp_indent( f, indent );
    fputs( "statements ", f );
    if( blk->scope != tmsymbolNIL ){
	pp_tmsymbol( f, blk->scope );
	fputs( " ", f );
    }
    fputs( "[\n", f );
    pp_statement_list( f, indent+INDENTSTEP, blk->statements );
    pp_indent( f, indent );
    fputs( "]\n", f );
}

static void pp_function(
 FILE *f, 
 const int indent,
 const origsymbol name,
 const formalParameter_list parms,
 const origsymbol retval,
 const type rettype,
 const pragma_list pragmas,
 const block body
)
{
    pp_indent( f, indent );
    fputs( "function ", f );
    pp_origsymbol( f, name );
    fputs( " ", f );
    pp_formalParameters( f, parms );
    fputs( " ", f );
    pp_origsymbol( f, retval );
    fputs( " ", f );
    pp_type( f, rettype );
    fputs( " ", f );
    if( pragmas->sz != 0 ){
	fputs( " pragma ", f );
	pp_pragma_list( f, pragmas );
    }
    fputs( "\n", f );
    pp_block( f, indent, body );
    pp_indent( f, indent );
}

static void pp_procedure(
 FILE *f, 
 int indent,
 origsymbol name,
 formalParameter_list parms,
 const pragma_list pragmas,
 block body
)
{
    pp_indent( f, indent );
    fputs( "procedure ", f );
    pp_origsymbol( f, name );
    fputs( " ", f );
    pp_formalParameters( f, parms );
    if( pragmas->sz != 0 ){
	fputs( " pragma ", f );
	pp_pragma_list( f, pragmas );
    }
    fputs( "\n", f );
    pp_block( f, indent, body );
    pp_indent( f, indent );
}

static void pp_optexpression( FILE *f, const optexpression x )
{
    switch( x->tag ){
	case TAGOptExprNone:
	    break;

	case TAGOptExpr:
	    fputs( " ", f );
	    pp_expression( f, to_OptExpr(x)->x );
    }
}

static void pp_declaration( FILE *f, int indent, vnusdeclaration decl )
{
    const origsymbol name = decl->name;
    const pragma_list pragmas = decl->pragmas;
    const vnusdeclarationdata d = decl->data;

    switch( d->tag ){
	case TAGVDeclFunction:
	    pp_function(
		f,
		indent,
		name,
		to_VDeclFunction(d)->parms,
		to_VDeclFunction(d)->retval,
		to_VDeclFunction(d)->t,
		pragmas,
		to_VDeclFunction(d)->body
	    );
	    break;

	case TAGVDeclProcedure:
	    pp_procedure(
		f,
		indent,
		name,
		to_VDeclProcedure(d)->parms,
		pragmas,
		to_VDeclProcedure(d)->body
	    );
	    break;

        case TAGVDeclCardinalityVariable:
	{
	    pp_indent( f, indent );
	    fputs( "cardinalityvariable ", f );
	    pp_origsymbol( f, name );
	    if(  pragmas->sz != 0 ){
		fputs( " pragma ", f );
		pp_pragma_list( f, pragmas );
	    }
	    break;
	}

        case TAGVDeclGlobalVariable:
	{
	    pp_indent( f, indent );
	    fputs( "globalvariable ", f );
	    pp_origsymbol( f, name );
	    fputs( " ", f );
	    pp_type( f, to_VDeclGlobalVariable(d)->t );
	    pp_optexpression( f, to_VDeclGlobalVariable(d)->init );
	    if(  pragmas->sz != 0 ){
		fputs( " pragma ", f );
		pp_pragma_list( f, pragmas );
	    }
	    break;
	}

        case TAGVDeclLocalVariable:
	{
	    pp_indent( f, indent );
	    fputs( "localvariable ", f );
	    pp_origsymbol( f, name );
	    fputs( " ", f );
	    pp_tmsymbol( f, to_VDeclLocalVariable(d)->scope );
	    fputs( " ", f );
	    pp_type( f, to_VDeclLocalVariable(d)->t );
	    pp_optexpression( f, to_VDeclLocalVariable(d)->init );
	    if(  pragmas->sz != 0 ){
		fputs( " pragma ", f );
		pp_pragma_list( f, pragmas );
	    }
	    break;
	}

        case TAGVDeclFormalVariable:
	    pp_indent( f, indent );
	    fputs( "formalvariable ", f );
	    pp_origsymbol( f, name );
	    fputs( " ", f );
	    pp_tmsymbol( f, to_VDeclFormalVariable(d)->scope );
	    fputs( " ", f );
	    pp_type( f, to_VDeclFormalVariable(d)->t );
	    if(  pragmas->sz != 0 ){
		fputs( " pragma ", f );
		pp_pragma_list( f, pragmas );
	    }
	    break;

        case TAGVDeclReturnVariable:
	    pp_indent( f, indent );
	    fputs( "returnvariable ", f );
	    pp_origsymbol( f, name );
	    fputs( " ", f );
	    pp_tmsymbol( f, to_VDeclReturnVariable(d)->scope );
	    fputs( " ", f );
	    pp_type( f, to_VDeclReturnVariable(d)->t );
	    if(  pragmas->sz != 0 ){
		fputs( " pragma ", f );
		pp_pragma_list( f, pragmas );
	    }
	    break;

        case TAGVDeclExternalVariable:
	    pp_indent( f, indent );
	    fputs( "externalvariable ", f );
	    pp_origsymbol( f, name );
	    fputs( " ", f );
	    pp_type( f, to_VDeclExternalVariable(d)->t );
	    if(  pragmas->sz != 0 ){
		fputs( " pragma ", f );
		pp_pragma_list( f, pragmas );
	    }
	    break;

        case TAGVDeclExternalFunction:
	{
	    pp_indent( f, indent );
	    fputs( "externalfunction ", f );
	    pp_origsymbol( f, name );
	    fputs( " ", f );
	    pp_formalParameters( f, to_VDeclExternalFunction(d)->parms );
	    fputs( " ", f );
	    pp_type( f, to_VDeclExternalFunction(d)->t );
	    if(  pragmas->sz != 0 ){
		fputs( " pragma ", f );
		pp_pragma_list( f, pragmas );
	    }
	    break;
	}

        case TAGVDeclExternalProcedure:
	{
	    pp_indent( f, indent );
	    fputs( "externalprocedure ", f );
	    pp_origsymbol( f, name );
	    fputs( " ", f );
	    pp_formalParameters( f, to_VDeclExternalProcedure(d)->parms );
	    if(  pragmas->sz != 0 ){
		fputs( " pragma ", f );
		pp_pragma_list( f, pragmas );
	    }
	    break;
	}

    }
}

static void pp_declarations( FILE *f, vnusdeclaration_list decls )
{
    unsigned int ix;

    fputs( "declarations [\n", f );
    for( ix=0; ix<decls->sz; ix++ ){
	pp_declaration( f, INDENTSTEP, decls->arr[ix] );
	if( ix+1<decls->sz ){
	    fputs( ",", f );
	}
	fputs( "\n", f );
    }
    fputs( "\n]\n", f );
}

#if 0
static void pp_procedure_vnusdeclarationdata(
    FILE *f,
    const int indent,
    const formalParameter_list parms,
    const block body
)
{
    pp_indent( f, indent );
    fputs( "procedure ", f );
    pp_formalParameters( f, parms );
    fputs( "\n", f );
}

static void pp_externalfunction_vnusdeclarationdata(
    FILE *f,
    const int indent,
    const formalParameter_list parms,
    const type rettype
)
{
    pp_indent( f, indent );
    fputs( "externalfunction ", f );
    pp_formalParameters( f, parms );
    pp_type( f, rettype );
    fputs( "\n", f );
}

static void pp_externalprocedure_vnusdeclarationdata(
    FILE *f,
    int indent,
    formalParameter_list parms
)
{
    pp_indent( f, indent );
    fputs( "externalprocedure ", f );
    pp_formalParameters( f, parms );
    fputs( "\n", f );
}

static void pp_vnusdeclaration_name( FILE *f, const origsymbol s )
{
    pp_origsymbol( f, s );
}

static bool pp_vnusdeclaration( FILE *f, int indent, const vnusdeclaration e, const bool gensep )
{
    const vnusdeclarationdata d = e->data;
    const origsymbol name = e->name;
    const pragma_list pragmas = e->pragmas;

    switch( d->tag ){
        case TAGVDeclFunction:
	    if( gensep ){
		pp_indent_line( f, INDENTSTEP, "," );
	    }
	    gensep = TRUE;
	    pp_function_vnusdeclarationdata(
		f,
		indent+INDENTSTEP,
		name,
		pragmas,
		to_VDeclFunction(d)->parms,
		to_VDeclFunction(d)->retval,
		to_VDeclFunction(d)->t,
		to_VDeclFunction(d)->body
	    );
	    break;

        case TAGVDeclProcedure:
	    if( gensep ){
		pp_indent_line( f, INDENTSTEP, "," );
	    }
	    gensep = TRUE;
	    pp_procedure_vnusdeclarationdata(
		f,
		name,
		pragmas,
		indent+INDENTSTEP,
		to_VDeclProcedure(d)->parms,
		to_VDeclProcedure(d)->body
	    );
	    pp_indent( f, indent );
	    break;

	case TAGVDeclCardinalityVariable:
	    if( gensep ){
		pp_indent_line( f, INDENTSTEP, "," );
	    }
	    gensep = TRUE;
	    pp_indent( f, indent );
	    fputs( "cardinalityvariable ", f );
	    pp_vnusdeclaration_name( f, e->name );
	    fputs( " ", f );
	    pp_pragma_list( f, e->pragmas );
	    fputs( "\n", f );
	    break;

        case TAGVDeclReturnVariable:
	    if( gensep ){
		pp_indent_line( f, INDENTSTEP, "," );
	    }
	    gensep = TRUE;
	    pp_indent( f, indent );
	    fputs( "returnvariable ", f );
	    pp_vnusdeclaration_name( f, e->name );
	    fprintf( f, " %s ", to_VDeclReturnVariable(d)->scope->name );
	    pp_type( f, to_VDeclReturnVariable(d)->t );
	    fputs( " ", f );
	    pp_pragma_list( f, e->pragmas );
	    fputs( "\n", f );
	    break;

        case TAGVDeclLocalVariable:
	    if( gensep ){
		pp_indent_line( f, INDENTSTEP, "," );
	    }
	    gensep = TRUE;
	    pp_indent( f, indent );
	    fputs( "( ", f );
	    pp_vnusdeclaration_name( f, e->name );
	    fputs( ", ", f );
	    pp_pragma_list( f, e->pragmas );
	    fprintf( f, ", localvariable %s ", to_VDeclLocalVariable(d)->scope->name );
	    pp_type( f, to_VDeclLocalVariable(d)->t );
	    fputs( " )\n", f );
	    break;

        case TAGVDeclFormalVariable:
	    if( gensep ){
		pp_indent_line( f, INDENTSTEP, "," );
	    }
	    gensep = TRUE;
	    pp_indent( f, indent );
	    fputs( "( ", f );
	    pp_vnusdeclaration_name( f, e->name );
	    fputs( ", ", f );
	    pp_pragma_list( f, e->pragmas );
	    fprintf( f, ", formalvariable %s ", to_VDeclFormalVariable(d)->scope->name );
	    pp_passingmechanism( f, to_VDeclFormalVariable(d)->mech );
	    fputs( " ", f );
	    pp_type( f, to_VDeclFormalVariable(d)->t );
	    fputs( " )\n", f );
	    break;

        case TAGVDeclGlobalVariable:
	    if( gensep ){
		pp_indent_line( f, INDENTSTEP, "," );
	    }
	    gensep = TRUE;
	    pp_indent( f, indent );
	    fputs( "( ", f );
	    pp_vnusdeclaration_name( f, e->name );
	    fputs( ", ", f );
	    pp_pragma_list( f, e->pragmas );
	    fputs( ", globalvariable ", f  );
	    pp_type( f, to_VDeclGlobalVariable(d)->t );
	    fputs( " )\n", f );
	    break;

	case TAGVDeclExternalVariable:
	    if( gensep ){
		pp_indent_line( f, INDENTSTEP, "," );
	    }
	    gensep = TRUE;
	    pp_indent( f, indent );
	    fputs( "( ", f );
	    pp_vnusdeclaration_name( f, e->name );
	    fputs( ", ", f );
	    pp_pragma_list( f, e->pragmas );
	    fputs( ", externalvariable ", f  );
	    pp_type( f, to_VDeclExternalVariable(d)->t );
	    fputs( " )\n", f );
	    break;

	case TAGVDeclExternalFunction:
	    if( gensep ){
		pp_indent_line( f, INDENTSTEP, "," );
	    }
	    gensep = TRUE;
	    pp_indent( f, indent );
	    fputs( "(\n", f );
	    pp_indent( f, indent+INDENTSTEP );
	    pp_vnusdeclaration_name( f, e->name );
	    fputs( ",\n", f );
	    pp_indent( f, indent+INDENTSTEP );
	    pp_pragma_list( f, e->pragmas );
	    fputs( ",\n", f );
	    pp_externalfunction_vnusdeclarationdata(
		f,
		indent+INDENTSTEP,
		to_VDeclExternalFunction(d)->parms,
		to_VDeclExternalFunction(d)->t
	    );
	    pp_indent( f, indent );
	    fputs( ")\n", f );
	    break;

	case TAGVDeclExternalProcedure:
	    if( gensep ){
		pp_indent_line( f, INDENTSTEP, "," );
	    }
	    gensep = TRUE;
	    pp_indent( f, indent );
	    fputs( "(\n", f );
	    pp_indent( f, indent+INDENTSTEP );
	    pp_vnusdeclaration_name( f, e->name );
	    fputs( ",\n", f );
	    pp_indent( f, indent+INDENTSTEP );
	    pp_pragma_list( f, e->pragmas );
	    fputs( ",\n", f );
	    pp_externalprocedure_vnusdeclarationdata(
		f,
		indent+INDENTSTEP,
		to_VDeclExternalProcedure(d)->parms
	    );
	    pp_indent( f, indent );
	    fputs( ")\n", f );
	    break;

    }
    return gensep;
}

static void pp_symboltable( FILE *f, const vnusdeclaration_list symtab )
{
    unsigned int ix;
    bool gensep;

    gensep = FALSE;
    fputs( "declarations [\n", f );
    for( ix=0; ix<symtab->sz; ix++ ){
	gensep = pp_vnusdeclaration( f, INDENTSTEP, symtab->arr[ix], gensep );
    }
    fputs( "\n]\n", f );
    
}
#endif

void pp_vnusprog( FILE *f, vnusprog theprog )
{
    fputs( "// Generated by sugar Vnus front-end\n", f );
    fputs( "\n", f );
    fputs( "program\n", f );
    fputs( "\n", f );
    if( theprog->pragmas->sz != 0 ){
	fputs( "pragma ", f );
	pp_pragma_list( f, theprog->pragmas );
	fputs( "\n\n", f );
    }
    pp_declarations( f, theprog->vnusdecls );
    fputs( "\n", f );
    pp_statements( f, 0, theprog->statements );
}
