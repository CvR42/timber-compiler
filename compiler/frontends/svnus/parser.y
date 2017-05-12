/* File: parser.y */

%{
#include <stdlib.h>
#include <stdio.h>
#include <tmc.h>
#include <assert.h>

#include "defs.h"
#include "vnus_types.h"
#include "tmadmin.h"
#include "error.h"
#include "lex.h"
#include "parser.h"
#include "check.h"
#include "global.h"
#include "symbol_table.h"
#include "prettyprint.h"

/* Always allow parse trace, since it is switchable from the
 * command line.
 */
#define YYDEBUG 1

static vnusprog result;

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

/* Return an empty statement block. */
static block empty_block( void )
{
    return new_block(
	tmsymbolNIL,
	new_declaration_list(),
	new_statement_list()
    );
}

/* Given a declaration, and a list of pragmas, return a new declaration
 * that contains the pragmas.
 */
static declaration add_declaration_pragmas( declaration d, pragma_list pl )
{
    switch( d->tag ){
        case TAGDeclFunction:
            to_DeclFunction(d)->pragmas = concat_pragma_list(
		to_DeclFunction(d)->pragmas,
		pl
	    );
            break;

        case TAGDeclProcedure:
            to_DeclProcedure(d)->pragmas = concat_pragma_list( to_DeclProcedure(d)->pragmas, pl );
            break;

        case TAGDeclVariable:
            to_DeclVariable(d)->pragmas = concat_pragma_list( to_DeclVariable(d)->pragmas, pl );
            break;

        case TAGDeclExternalFunction:
            to_DeclExternalFunction(d)->pragmas = concat_pragma_list( to_DeclExternalFunction(d)->pragmas, pl );
            break;

        case TAGDeclExternalProcedure:
            to_DeclExternalProcedure(d)->pragmas = concat_pragma_list( to_DeclExternalProcedure(d)->pragmas, pl );
            break;

        case TAGDeclExternalVariable:
            to_DeclExternalVariable(d)->pragmas = concat_pragma_list( to_DeclExternalVariable(d)->pragmas, pl );
            break;
    }
    return d;
}

%}

%union {
    unsigned int	_unsigned;
    BASETYPE		_basetype;
    origsymbol		_orig_identifier;
    tmsymbol		_identifier;
    size		_size;
    size_list		_sizelist;
    formalParameter	_parm;
    formalParameter_list	_parmList;
    tmstring		_vnus_int;
    tmstring		_vnus_byte;
    tmstring		_vnus_short;
    tmstring		_vnus_long;
    tmstring		_vnus_char;
    tmstring		_vnus_float;
    tmstring		_vnus_double;
    tmstring		_vnus_string;
    tmstring		_string;
    expression		_expression;
    origin		_origin;
    optexpression	_optexpression;
    expression_list	_expressionList;
    cardinality		_cardinality;
    cardinality_list	_cardinalitylist;
    BINOP		_binop;
    UNOP		_unop;
    type		_type;
    statement		_statement;
    switchCase		_switchCase;
    switchCase_list	_switchCaseList;
    statement_list	_statementList;
    field		_field;
    field_list		_fieldList;
    type_list		_typeList;
    block		_block;
    declaration_list	_declarationList;
    declaration		_declaration;
    pragma		_pragma;
    pragma_list		_pragmaList;
    distribution	_distribution;
    distribution_list	_distributionlist;
}

%start V_nus

/* The typed tokens first. */
%token <_identifier>	IDENTIFIER
%token <_vnus_byte>	BYTE_LITERAL
%token <_vnus_char>	CHAR_LITERAL
%token <_vnus_double>	DOUBLE_LITERAL
%token <_vnus_float>	FLOAT_LITERAL
%token <_vnus_int>	INT_LITERAL
%token <_vnus_long>	LONG_LITERAL
%token <_vnus_short>	SHORT_LITERAL
%token <_vnus_string>	STRING_LITERAL
%token <_identifier>	IDENTIFIER

%token KEY_AND
%token KEY_BARRIER
%token KEY_BLOCK
%token KEY_BLOCKRECEIVE
%token KEY_BLOCKSEND
%token KEY_BOOLEAN
%token KEY_BYTE
%token KEY_CHAR
%token KEY_COMPLEX
%token KEY_CYCLIC
%token KEY_DEFAULT
%token KEY_DELETE
%token KEY_DOUBLE
%token KEY_EACH
%token KEY_ELSE
%token KEY_EXTERNAL
%token KEY_FALSE
%token KEY_FITROOM
%token KEY_FLOAT
%token KEY_FOR
%token KEY_FORALL
%token KEY_FOREACH
%token KEY_FORKALL
%token KEY_FUNCTION
%token KEY_GARBAGECOLLECT
%token KEY_GETBLOCKSIZE
%token KEY_GETROOM
%token KEY_GETSIZE
%token KEY_GLOBALPRAGMAS
%token KEY_GOTO
%token KEY_IF
%token KEY_INT
%token KEY_ISMULTIDIM
%token KEY_ISOWNER
%token KEY_LONG
%token KEY_MOD
%token KEY_NEW
%token KEY_NOT
%token KEY_NULL
%token KEY_OF
%token KEY_OR
%token KEY_OWNER
%token KEY_POINTER
%token KEY_PRINT
%token KEY_PRINTLN
%token KEY_PROCEDURE
%token KEY_RECEIVE
%token KEY_RECORD
%token KEY_RETURN
%token KEY_RETURNS
%token KEY_SEND
%token KEY_SENDER
%token KEY_SETROOM
%token KEY_SETSIZE
%token KEY_SHAPE
%token KEY_SHORT
%token KEY_STRING
%token KEY_SWITCH
%token KEY_TO
%token KEY_TRUE
%token KEY_VIEW
%token KEY_WHILE
%token KEY_XOR
%token OP_ASSIGNMENT
%token OP_DIVIDE
%token OP_EQUAL
%token OP_GREATER
%token OP_GREATEREQUAL
%token OP_LABEL
%token OP_LESS
%token OP_LESSEQUAL
%token OP_MINUS
%token OP_NOTEQUAL
%token OP_PLUS
%token OP_PRAGMAEND
%token OP_PRAGMASTART
%token OP_SHIFTLEFT
%token OP_SHIFTRIGHT
%token OP_TIMES
%token OP_USHIFTRIGHT

/* Precedence of infix and prefix operators. */
%left '?' ':'
%left KEY_OR KEY_AND KEY_XOR
%left OP_LESS OP_LESSEQUAL OP_GREATER OP_GREATEREQUAL OP_EQUAL OP_NOTEQUAL
%left OP_SHIFTLEFT OP_SHIFTRIGHT OP_USHIFTRIGHT
%left OP_PLUS OP_MINUS
%left OP_TIMES OP_DIVIDE KEY_MOD
%left OP_UNOP
%left '[' '.' '('

/* use sort -b +2 */

%type	<_declaration>		actual_declaration
%type	<_statement>		assignment
%type	<_statement>		barrier
%type	<_basetype>		baseType
%type	<_block>		block
%type	<_origin>		origin
%type	<_statement>		blockreceive
%type	<_statement>		blocksend
%type	<_cardinalitylist>	cardinalities
%type	<_cardinality>		cardinality
%type	<_cardinalitylist>	cardinalitylist
%type	<_statement>		communication
%type	<_statement>		control
%type	<_declaration>		declaration
%type	<_declarationList>	declarationList
%type	<_statement>		delete
%type	<_distribution>		distribution
%type	<_distributionlist>	distributionlist
%type	<_distributionlist>	distributions
%type	<_statement>		each
%type	<_statement>		empty_statement
%type	<_expression>		expression
%type	<_expression>		expression
%type	<_expressionList>	expressionList
%type	<_expressionList>	expressions
%type	<_declaration>		external_function
%type	<_declaration>		external_procedure
%type	<_declaration>		external_variable
%type	<_field>		field
%type	<_fieldList>		fieldList
%type	<_statement>		fitRoom
%type	<_statement>		forall
%type	<_statement>		foreach
%type	<_statement>		forkall
%type	<_parm>			formalParameter
%type	<_parmList>		formalParameterlist
%type	<_parmList>		formalParameters
%type	<_declaration>		function
%type	<_statement>		garbageCollect
%type	<_statement>		goto
%type	<_statement>		if
%type	<_statement>		imperative
%type	<_optexpression>	initialization
%type	<_statement>		iteration
%type	<_orig_identifier>	labelName
%type	<_statement>		memoryManagement
%type	<_pragmaList>		opt_globalpragmas
%type	<_pragmaList>		opt_pragmas
%type	<_orig_identifier>	orig_identifier
%type	<_statement>		parallelization
%type	<_pragma>		pragma
%type	<_expression>		pragmaExpression
%type	<_pragmaList>		pragmaList
%type	<_pragmaList>		pragmas
%type	<_statement>		print
%type	<_statement>		println
%type	<_declaration>		procedure
%type	<_statement>		procedurecall
%type	<_statement>		receive
%type	<_statement>		return
%type	<_expression>		selector
%type	<_expressionList>	selectorlist
%type	<_expressionList>	selectors
%type	<_statement>		send
%type	<_statement>		setRoom
%type	<_statement>		setSize
%type	<_size>			size
%type	<_sizelist>		sizelist
%type	<_sizelist>		sizes
%type	<_statement>		statement
%type	<_statementList>	statementList
%type	<_statement>		statementblock
%type	<_statement>		support
%type	<_statement>		switch
%type	<_switchCase>		switchCase
%type	<_switchCaseList>	switchCaseList
%type	<_type>			type
%type	<_typeList>		typeList
%type	<_unop>			unary_operator
%type	<_statement>		unlabeledStatement
%type	<_statement>		valueReturn
%type	<_declaration>		variable
%type	<_statement>		while

%%

V_nus:
    opt_globalpragmas declarationList statementList
	{ result = new_vnusprog( $1, $2, $3, new_vnusdeclaration_list() ); }
|
    opt_globalpragmas statementList
	{ result = new_vnusprog( $1, new_declaration_list(), $2, new_vnusdeclaration_list() ); }
;

opt_comma:
    /* empty */
|
    ','
;

statement_terminator:
    ';'
;

declarationList:
    declaration
	{ $$ = append_declaration_list( new_declaration_list(), $1 ); }
|
    declarationList declaration
	{ $$ = append_declaration_list( $1, $2 ); }
|
    error statement_terminator declaration
	{ $$ = append_declaration_list( new_declaration_list(), $3 ); }
;

actual_declaration:
    external_function
	{ $$ = $1; }
|
    function
	{ $$ = $1; }
|
    external_procedure
	{ $$ = $1; }
|
    procedure
	{ $$ = $1; }
|
    external_variable
	{ $$ = $1; }
|
    variable
	{ $$ = $1; }
;

declaration:
    opt_pragmas actual_declaration
        { $$ = add_declaration_pragmas( $2, $1 ); }
;

external_function:
    KEY_EXTERNAL KEY_FUNCTION orig_identifier formalParameters KEY_RETURNS opt_pragmas orig_identifier ':' type statement_terminator
	{
	    $$ = new_DeclExternalFunction(
		new_pragma_list(),
		make_origin(),
		$3,
		$4,
		$6,
		$7,
		$9
	    );
	}
|
    KEY_EXTERNAL KEY_FUNCTION orig_identifier formalParameters KEY_RETURNS opt_pragmas type statement_terminator
	{
	    $$ = new_DeclExternalFunction(
		new_pragma_list(),
		make_origin(),
		$3,
		$4,
		$6,
		origsymbolNIL,
		$7
	    );
        }
;

external_procedure:
    KEY_EXTERNAL KEY_PROCEDURE orig_identifier formalParameters statement_terminator
	{
	    $$ = new_DeclExternalProcedure(
		new_pragma_list(),
		make_origin(),
		$3,
		$4
	    );
	}
;

external_variable:
    KEY_EXTERNAL orig_identifier ':' type statement_terminator
	{ $$ = new_DeclExternalVariable( new_pragma_list(), make_origin(), $2, $4 ); }
;

function:
    KEY_FUNCTION orig_identifier formalParameters KEY_RETURNS orig_identifier ':' opt_pragmas type block
	{
	    if( $9->scope == 0 ){
		$9->scope = rdup_tmsymbol( $2->sym );
	    }
	    $$ = new_DeclFunction(
		new_pragma_list(),
		make_origin(),
		$2,
		$3,
		$5,
		$7,
		$8,
		$9
	    );
	}
;

procedure:
    KEY_PROCEDURE orig_identifier formalParameters block
	{
	    if( $4->scope == 0 ){
		$4->scope = rdup_tmsymbol( $2->sym );
	    }
	    $$ = new_DeclProcedure(
		new_pragma_list(),
		make_origin(),
		$2,
		$3,
		$4
	    ); 
	}
;

initialization:
    /* empty */
	{ $$ = new_OptExprNone(); }
|
    OP_EQUAL expression
	{ $$ = new_OptExpr( $2 ); }
;

variable:
    orig_identifier ':' type initialization statement_terminator
	{ $$ = new_DeclVariable( new_pragma_list(), make_origin(), $1, $3, $4 ); }
;

distribution:
    KEY_BLOCK
        { $$ = new_DistBlock(); }
|
    KEY_CYCLIC
        { $$ = new_DistCyclic(); }
|
    KEY_BLOCK expression
        { $$ = new_DistBC( $2 ); }
|
    OP_PLUS
        { $$ = new_DistReplicated(); }
|
    OP_TIMES
        { $$ = new_DistDontcare(); }
|
    OP_MINUS
        { $$ = new_DistCollapsed(); }
;

distributionlist:
    /* empty */
	{ $$ = new_distribution_list(); }
|
    distribution
	{ $$ = append_distribution_list( new_distribution_list(), $1 ); }
|
    distributionlist ',' distribution
	{ $$ = append_distribution_list( $$, $3 ); }
|
    error ',' distribution
	{ $$ = append_distribution_list( new_distribution_list(), $3 ); }
;

distributions:
    /* empty. */
	{ $$ = distribution_listNIL; }
|
    '[' distributionlist ']'
        { $$ = $2; }
;

type:
    baseType
	{ $$ = new_TypeBase( $1 ); }
|
    KEY_VIEW INT_LITERAL KEY_OF type
	{
	    $$ = new_TypeView( atoi( $2 ), $4 );
	    rfre_tmstring( $2 );
	}
|
    KEY_VIEW INT_LITERAL type
	{
	    parserror( "keyword \"of\" missing" );
	    $$ = new_TypeView( atoi( $2 ), $3 );
	    rfre_tmstring( $2 );
	}
|
    KEY_SHAPE sizes distributions KEY_OF type
	{ $$ = new_TypeShape( $2, $3, $5 ); }
|
    KEY_SHAPE sizes distributions
	{
	    parserror( "you must specify an element type" );
	    $$ = new_TypeShape( $2, $3, typeNIL );
	}
|
    KEY_SHAPE sizes distributions type
	{
	    parserror( "keyword \"of\" missing" );
	    $$ = new_TypeShape( $2, $3, $4 );
	}
|
    KEY_RECORD '[' fieldList ']'
	{ $$ = new_TypeRecord( $3 ); }
|
    KEY_POINTER type
	{
	    parserror( "keyword \"to\" missing" );
	    $$ = new_TypePointer( $2 );
	}
|
    KEY_POINTER KEY_TO type
	{ $$ = new_TypePointer( $3 ); }
|
    KEY_PROCEDURE '(' typeList ')'
        { $$ = new_TypeProcedure( $3 ); }
|
    KEY_FUNCTION '(' typeList ')' KEY_RETURNS type
        { $$ = new_TypeFunction( $3, $6 ); }
;

typeList:
    /* empty */
	{ $$ = new_type_list(); }
|
    type
	{ $$ = append_type_list( new_type_list(), $1 ); }
|
    typeList ',' type
	{ $$ = append_type_list( $$, $3 ); }
|
    error ',' type
	{ $$ = append_type_list( new_type_list(), $3 ); }
;

fieldList:
    /* empty */
	{ $$ = new_field_list(); }
|
    field
	{ $$ = append_field_list( new_field_list(), $1 ); }
|
    fieldList ',' field
	{ $$ = append_field_list( $$, $3 ); }
|
    error ',' field
	{ $$ = append_field_list( new_field_list(), $3 ); }
;

field:
    orig_identifier ':' type
	{ $$ = new_field( $1, $3 ); }
;

sizes:
    '[' sizelist opt_comma ']'
	    { $$ = $2; }
|
    '[' error ']'
	{ $$ = new_size_list(); }
;

sizelist:
    /* empty */
	    { $$ = new_size_list(); }
|
    size
	    { $$ = append_size_list( new_size_list(), $1 ); }
|
    sizelist ',' size
	    { $$ = append_size_list( $$, $3 ); }
|
    error ',' size
	    { $$ = append_size_list( new_size_list(), $3 ); }
;

size:
    OP_TIMES
	{ $$ = new_SizeDontcare( make_origin() ); }
|
    expression
	{ $$ = new_SizeExpression( $1 ); }
;

baseType:
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

formalParameters:
    '(' formalParameterlist opt_comma ')'
	{ $$ = $2; }
|
    '(' error ')'
	{ $$ = new_formalParameter_list(); }
;

formalParameterlist:
    /* empty */
	{ $$ = new_formalParameter_list(); }
|
    formalParameter
	{ $$ = append_formalParameter_list( new_formalParameter_list(), $1 ); }
|
    formalParameterlist ',' formalParameter
	{ $$ = append_formalParameter_list( $1, $3 ); }
|
    error ',' formalParameter
	{ $$ = append_formalParameter_list( new_formalParameter_list(), $3 ); }
;

formalParameter:
    orig_identifier
	{
	    parserror( "parameter type missing" );
	    $$ = new_formalParameter( $1, new_pragma_list(), new_TypeBase( BT_INT ) );
	}
|
    orig_identifier ':' opt_pragmas type
	{ $$ = new_formalParameter( $1, $3, $4 ); }
;

block:
    '{' declarationList statementList '}'
	{ $$ = new_block( tmsymbolNIL, $2, $3 ); }
|
    '{' statementList '}'
	{ $$ = new_block( tmsymbolNIL, new_declaration_list(), $2 ); }
|
    '{' declarationList '}'
	{ $$ = new_block( tmsymbolNIL, $2, new_statement_list() ); }
|
    '{' '}'
	{ $$ = new_block( tmsymbolNIL, new_declaration_list(), new_statement_list() ); }
;

statementList:
    statement
	{ $$ = append_statement_list( new_statement_list(), $1 ); }
|
    statementList statement
	{ $$ = append_statement_list( $1, $2 ); }
|
    error statement_terminator statement
	{ $$ = append_statement_list( new_statement_list(), $3 ); }
;

pragma:
    orig_identifier
        { $$ = new_PragmaFlag( $1 ); }
|
    orig_identifier OP_EQUAL expression
        { $$ = new_PragmaExpression( $1, $3 ); }
;

pragmaList:
    /* empty */
        { $$ = new_pragma_list(); }
|
    pragma
        { $$ = append_pragma_list( new_pragma_list(), $1 ); }
|
    pragmaList ',' pragma
        { $$ = append_pragma_list( $1, $3 ); }
;

pragmas:
     OP_PRAGMASTART pragmaList OP_PRAGMAEND
         { $$ = $2; }
|
    error OP_PRAGMAEND
        { $$ = new_pragma_list(); }
;

opt_globalpragmas:
    /* empty */
        { $$ = new_pragma_list(); }
|
    KEY_GLOBALPRAGMAS pragmas statement_terminator
        { $$ = $2; }
;

opt_pragmas:
    /* empty */
        { $$ = new_pragma_list(); }
|
    pragmas
        { $$ = $1; }
;

origin:
    /* empty */
	{ $$ = make_origin(); }
;

labelName:
    orig_identifier
	{ $$ = $1; }
;

statement:
    opt_pragmas labelName OP_LABEL unlabeledStatement
	{
	    $$ = $4;
	    $$->label = $2;
	    $$->pragmas = $1;
	}
|
    opt_pragmas unlabeledStatement
	{
	    $$ = $2;
	    $$->pragmas = $1;
	}
;

unlabeledStatement:
    imperative
 	{ $$ = $1; }
|
    control
 	{ $$ = $1; }
|
    parallelization
 	{ $$ = $1; }
|
    communication
 	{ $$ = $1; }
|
    memoryManagement
	{ $$ = $1; }
|
    support
        { $$ = $1; }
;


/* Imperative */

imperative:
    assignment
 	{ $$ = $1; }
|
    procedurecall
 	{ $$ = $1; }
|
    setSize
	{ $$ = $1; }
|
    setRoom
	{ $$ = $1; }
|
    fitRoom
	{ $$ = $1; }
;

assignment:
    expression OP_ASSIGNMENT origin expression statement_terminator
	{ $$ = new_AssignStatement( origsymbolNIL, $3, pragma_listNIL, $1, $4 ); }
;

procedurecall:
    expression origin statement_terminator
	{
	    
	    if( $1->tag == TAGExprFunctionCall ){
		ExprFunctionCall call = to_ExprFunctionCall( $1 );

		$$ = new_ProcedureCallStatement(
		    origsymbolNIL,
		    $2,
		    pragma_listNIL,
		    rdup_expression( call->function ),
		    rdup_expression_list( call->parameters )
		);
		rfre_expression( call );
	    }
	    else {
		    parserror( "expression has no effect" );
	    }
	}
;
/*
procedurecall:
    expression origin expressions statement_terminator
	{ $$ = new_ProcedureCallStatement( origsymbolNIL, $2, pragma_listNIL, $1, $3 ); }
;
*/

setSize:
    KEY_SETSIZE origin '(' expression ',' sizes ')' statement_terminator
	{ $$ = new_SetSizeStatement( origsymbolNIL, $2, pragma_listNIL, $4, $6 ); }
;

setRoom:
    KEY_SETROOM origin '(' expression ',' expression ')' statement_terminator
	{ $$ = new_SetRoomStatement( origsymbolNIL, $2, pragma_listNIL, $4, $6 ); }
;

fitRoom:
    KEY_FITROOM origin '(' expression ')' statement_terminator
	{ $$ = new_FitRoomStatement( origsymbolNIL, $2, pragma_listNIL, $4 ); }
;


/* Support */

support:
    empty_statement
        { $$ = $1; }
|
    print
        { $$ = $1; }
|
    println
        { $$ = $1; }
;

empty_statement:
    origin statement_terminator
        { $$ = new_EmptyStatement( origsymbolNIL, $1, pragma_listNIL ); }
;

print:
    KEY_PRINT origin expressions statement_terminator
        { $$ = new_PrintStatement( origsymbolNIL, $2, pragma_listNIL, $3 ); }
;

println:
    KEY_PRINTLN origin expressions statement_terminator
        { $$ = new_PrintLineStatement( origsymbolNIL, $2, pragma_listNIL, $3 ); }
;


/* Control */

control:
    while
 	{ $$ = $1; }
|
    iteration
 	{ $$ = $1; }
|
    if
 	{ $$ = $1; }
|
    switch
 	{ $$ = $1; }
|
    return
 	{ $$ = $1; }
|
    goto
 	{ $$ = $1; }
|
    valueReturn
 	{ $$ = $1; }
|
    statementblock
	{ $$ = $1; }
;

while:
    KEY_WHILE origin expression block
	{ $$ = new_WhileStatement( origsymbolNIL, $2, pragma_listNIL, $3, $4 ); }
;

iteration:
    KEY_FOR origin cardinalities block
	{ $$ = new_ForStatement( origsymbolNIL, $2, pragma_listNIL, $3, $4 ); }
;

if:
    KEY_IF origin expression block KEY_ELSE block
	{ $$ = new_IfStatement( origsymbolNIL, $2, pragma_listNIL, $3, $4, $6 ); }
|
    KEY_IF origin expression block
	{ $$ = new_IfStatement( origsymbolNIL, $2, pragma_listNIL, $3, $4, empty_block() ); }
;

switch:
    KEY_SWITCH origin expression '{' switchCaseList '}'
	{ $$ = new_SwitchStatement( origsymbolNIL, $2, pragma_listNIL, $3, $5 ); }
;

return:
    KEY_RETURN origin statement_terminator
	{ $$ = new_ReturnStatement( origsymbolNIL, $2, pragma_listNIL ); }
;

valueReturn:
    KEY_RETURN origin expression statement_terminator
	{ $$ = new_ValueReturnStatement( origsymbolNIL, $2, pragma_listNIL, $3 ); }
;

goto:
    KEY_GOTO origin labelName statement_terminator
	{ $$ = new_GotoStatement( origsymbolNIL, $2, pragma_listNIL, $3 ); }
;

switchCaseList:
    /* empty */
	{ $$ = new_switchCase_list(); }
|
    switchCaseList switchCase
	{ $$ = append_switchCase_list( $1, $2 ); }
;

switchCase:
    INT_LITERAL ':' block
	{ $$ = new_SwitchCaseValue( $3, $1 ); }
|
    KEY_DEFAULT ':' block
	{ $$ = new_SwitchCaseDefault( $3 ); }
;

statementblock:
    origin block
	{ $$ = new_BlockStatement( origsymbolNIL, $1, pragma_listNIL, $2 ); }
;


/* parallelization */

parallelization:
    forkall
 	{ $$ = $1; }
|
    forall
 	{ $$ = $1; }
|
    each
	{ $$ = $1; }
|
    foreach
	{ $$ = $1; }
;

forall:
    KEY_FORALL origin cardinalities block
	{ $$ = new_ForallStatement( origsymbolNIL, $2, pragma_listNIL, $3, $4 ); }
;

forkall:
    KEY_FORKALL origin cardinalities block
	{ $$ = new_ForkallStatement( origsymbolNIL, $2, pragma_listNIL, $3, $4 ); }
;

each:
    KEY_EACH origin '{' statementList '}'
	{ $$ = new_EachStatement( origsymbolNIL, $2, pragma_listNIL, $4 ); }
;

foreach:
    KEY_FOREACH origin cardinalities block
	{ $$ = new_ForeachStatement( origsymbolNIL, $2, pragma_listNIL, $3, $4 ); }
;


/* communication */

communication:
    receive
	{ $$ = $1; } /* not tested. */
|
    send
	{ $$ = $1; } /* not tested. */
|
    blockreceive
	{ $$ = $1; } /* not tested. */
|
    blocksend
	{ $$ = $1; } /* not tested. */
|
    barrier
	{ $$ = $1; }
;

receive:
    KEY_RECEIVE origin '(' expression ',' expression ',' expression ')' statement_terminator
	{ $$ = new_ReceiveStatement( origsymbolNIL, $2, pragma_listNIL, $4, $6, $8 ); }
|
    KEY_RECEIVE origin '(' error ')' statement_terminator
	{ $$ = new_EmptyStatement( origsymbolNIL, $2, pragma_listNIL ); }
;

send:
    KEY_SEND origin '(' expression ',' expression ',' expression ')' statement_terminator
	{ $$ = new_SendStatement( origsymbolNIL, $2, pragma_listNIL, $4, $6, $8 ); }
|
    KEY_SEND origin '(' error ')' statement_terminator
	{ $$ = new_EmptyStatement( origsymbolNIL, $2, pragma_listNIL ); }
;

blockreceive:
    KEY_BLOCKRECEIVE origin '(' expression ',' expression ',' expression ',' expression ')' statement_terminator
	{ $$ = new_BlockReceiveStatement( origsymbolNIL, $2, pragma_listNIL, $4, $6, $8, $10 ); }
|
    KEY_BLOCKRECEIVE origin '(' error ')' statement_terminator
	{ $$ = new_EmptyStatement( origsymbolNIL, $2, pragma_listNIL ); }
;

blocksend:
    KEY_BLOCKSEND origin '(' expression ',' expression ',' expression ',' expression ')' statement_terminator
	{ $$ = new_BlockSendStatement( origsymbolNIL, $2, pragma_listNIL, $4, $6, $8, $10 ); }
|
    KEY_BLOCKSEND origin '(' error ')' statement_terminator
	{ $$ = new_EmptyStatement( origsymbolNIL, $2, pragma_listNIL ); }
;

barrier:
    KEY_BARRIER origin statement_terminator
	{ $$ = new_BarrierStatement( origsymbolNIL, $2, pragma_listNIL ); }
;

cardinalities:
    '[' cardinalitylist opt_comma ']'
	{ $$ = $2; }
|
    '[' error ']'
	{ $$ = new_cardinality_list(); }
;

/* Memory management */

memoryManagement:
    delete
	{ $$ = $1; }
|
    garbageCollect
	{ $$ = $1; }
;

delete:
    KEY_DELETE origin expression statement_terminator
	{ $$ = new_DeleteStatement( origsymbolNIL, $2, pragma_listNIL, $3 ); }
;

garbageCollect:
    KEY_GARBAGECOLLECT origin statement_terminator
	{ $$ = new_GarbageCollectStatement( origsymbolNIL, $2, pragma_listNIL ); }
;

cardinalitylist:
    /* empty */
	{ $$ = new_cardinality_list(); }
|
    cardinality
	{ $$ = append_cardinality_list( new_cardinality_list(), $1 ); }
|
    cardinalitylist ',' cardinality
	{ $$ = append_cardinality_list( $1, $3 ); }
|
    error ',' cardinality
	{ $$ = append_cardinality_list( new_cardinality_list(), $3 ); }
;

cardinality:
    orig_identifier ':' expression
	{ $$ = new_cardinality( $1, new_pragma_list(), $3 ); }
|
    '(' cardinality ')'
	{ $$ = $2; }
|
    error ')'
	{ $$ = cardinalityNIL; }
;

selectors:
    '[' selectorlist ']'
	{ $$ = $2; }
|
    '[' error ']'
	{ $$ = new_expression_list(); }
;

selectorlist:
    /* empty */
	{ $$ = new_expression_list(); }
|
    selector
	{ $$ = append_expression_list( new_expression_list(), $1 ); }
|
    selectorlist ',' selector
	{ $$ = append_expression_list( $1, $3 ); }
|
    error ',' selector
	{ $$ = append_expression_list( new_expression_list(), $3 ); }
;

selector:
    expression
	{ $$ = $1; }
;

expressions:
    '(' expressionList ')'
	{ $$ = $2; }
|
    '(' error ')'
	{ $$ = new_expression_list(); }
;

expressionList:
    /* empty */
	{ $$ = new_expression_list(); }
|
    expression
	{ $$ = append_expression_list( new_expression_list(), $1 ); }
|
    expressionList ',' expression
	{ $$ = append_expression_list( $1, $3 ); }
|
    error ',' expression
	{ $$ = append_expression_list( new_expression_list(), $3 ); }
;

expression:
    KEY_NULL
	{ $$ = new_ExprNull(); }
|
    KEY_TRUE
	{ $$ = new_ExprBoolean( TRUE ); }
|
    KEY_FALSE
	{ $$ = new_ExprBoolean( FALSE ); }
|
    BYTE_LITERAL
	{ $$ = new_ExprByte( $1 ); }
|
    SHORT_LITERAL
	{ $$ = new_ExprShort( $1 ); }
|
    INT_LITERAL
	{ $$ = new_ExprInt( $1 ); }
|
    LONG_LITERAL
	{ $$ = new_ExprLong( $1 ); }
|
    FLOAT_LITERAL
	{ $$ = new_ExprFloat( $1 ); }
|
    DOUBLE_LITERAL
	{ $$ = new_ExprDouble( $1 ); }
|
    STRING_LITERAL
	{ $$ = new_ExprString( $1 ); }
|
    CHAR_LITERAL
	{ $$ = new_ExprChar( $1 ); }
|
    orig_identifier
        { $$ = new_ExprName( $1 ); }
|
    '(' type ')' expression %prec OP_UNOP
	{ $$ = new_ExprCast( $2, $4 ); }
|
    '[' expressionList ']'
	{ $$ = new_ExprRecord( $2 ); }
|
    expression selectors
	{ $$ = new_ExprSelection( $1, $2 ); }
|
    KEY_VIEW cardinalities expression %prec OP_UNOP
	{ $$ = new_ExprView( new_view( $2, $3 ) ); }
|
    expression expressions
	{ $$ = new_ExprFunctionCall( $1, $2 ); }
|
    expression '.' orig_identifier
	{ $$ = new_ExprField( $1, $3 ); }
|
    KEY_COMPLEX '(' expression ',' expression ')'
	{ $$ = new_ExprComplex( $3, $5 ); }
|
    KEY_COMPLEX '(' error ',' expression ')'
	{ $$ = new_ExprComplex( expressionNIL, $5 ); }
|
    KEY_COMPLEX '(' expression ',' error ')'
	{ $$ = new_ExprComplex( $3, expressionNIL ); }
|
    '(' pragmaExpression ')'
	{ $$ = $2; }
|
    OP_TIMES expression %prec OP_UNOP
	{ $$ = new_ExprDeref( $2 ); }
|
    '&' expression %prec OP_UNOP
	{ $$ = new_ExprAddress( $2 ); }
|
    unary_operator expression %prec OP_UNOP
	{ $$ = new_ExprUnop( $1, $2 ); }
|
    expression '?' expression ':' expression
	{ $$ = new_ExprIf( $1, $3, $5 ); }
|
    expression KEY_AND expression
	{ $$ = new_ExprBinop( $1, BINOP_AND, $3 ); }
|
    expression KEY_OR expression
	{ $$ = new_ExprBinop( $1, BINOP_OR, $3 ); }
|
    expression KEY_MOD expression
	{ $$ = new_ExprBinop( $1, BINOP_MOD, $3 ); }
|
    expression OP_PLUS expression
	{ $$ = new_ExprBinop( $1, BINOP_PLUS, $3 ); }
|
    expression OP_MINUS expression
	{ $$ = new_ExprBinop( $1, BINOP_MINUS, $3 ); }
|
    expression OP_TIMES expression
	{ $$ = new_ExprBinop( $1, BINOP_TIMES, $3 ); }
|
    expression OP_DIVIDE expression
	{ $$ = new_ExprBinop( $1, BINOP_DIVIDE, $3 ); }
|
    expression OP_EQUAL expression
	{ $$ = new_ExprBinop( $1, BINOP_EQUAL, $3 ); }
|
    expression OP_NOTEQUAL expression
	{ $$ = new_ExprBinop( $1, BINOP_NOTEQUAL, $3 ); }
|
    expression OP_LESS expression
	{ $$ = new_ExprBinop( $1, BINOP_LESS, $3 ); }
|
    expression OP_LESSEQUAL expression
	{ $$ = new_ExprBinop( $1, BINOP_LESSEQUAL, $3 ); }
|
    expression OP_GREATER expression
	{ $$ = new_ExprBinop( $1, BINOP_GREATER, $3 ); }
|
    expression OP_GREATEREQUAL expression
	{ $$ = new_ExprBinop( $1, BINOP_GREATEREQUAL, $3 ); }
|
    expression KEY_XOR expression
	{ $$ = new_ExprBinop( $1, BINOP_XOR, $3 ); }
|
    expression OP_SHIFTLEFT expression
	{ $$ = new_ExprBinop( $1, BINOP_SHIFTLEFT, $3 ); }
|
    expression OP_SHIFTRIGHT expression
	{ $$ = new_ExprBinop( $1, BINOP_SHIFTRIGHT, $3 ); }
|
    expression OP_USHIFTRIGHT expression
	{ $$ = new_ExprBinop( $1, BINOP_USHIFTRIGHT, $3 ); }
|
    KEY_NEW '(' type ')' %prec OP_UNOP
	{ $$ = new_ExprNew( $3 ); }
|
    KEY_GETBLOCKSIZE '(' expression ',' expression ')'
	{ $$ = new_ExprGetBlocksize( $3, $5 ); }
|
    KEY_GETBLOCKSIZE error ')'
	{ $$ = new_ExprGetBlocksize( expressionNIL, expressionNIL ); }
|
    KEY_GETSIZE '(' expression ',' expression ')'
	{ $$ = new_ExprGetSize( $3, $5 ); }
|
    KEY_GETSIZE error ')'
	{ $$ = new_ExprGetSize( expressionNIL, expressionNIL ); }
|
    KEY_GETROOM '(' expression ')'
	{ $$ = new_ExprGetRoom( $3 ); }
|
    KEY_GETROOM error ')'
	{ $$ = new_ExprGetRoom( expressionNIL ); }
|
    KEY_SENDER '(' expression ')'
	{ $$ = new_ExprSender( $3 ); }
|
    KEY_SENDER error ')'
	{ $$ = new_ExprSender( expressionNIL ); }
|
    KEY_OWNER '(' expression ')'
	{ $$ = new_ExprOwner( $3 ); }
|
    KEY_OWNER error ')'
	{ $$ = new_ExprOwner( expressionNIL ); }
|
    KEY_ISOWNER '(' expression ',' expression ')'
	{ $$ = new_ExprIsOwner( $3, $5 ); }
|
    KEY_ISOWNER error ')'
	{ $$ = new_ExprIsOwner( expressionNIL, expressionNIL ); }
|
    KEY_ISMULTIDIM '(' expression ')'
	{ $$ = new_ExprIsMultidimDist( $3 ); }
|
    KEY_ISMULTIDIM error ')'
	{ $$ = new_ExprIsMultidimDist( expressionNIL ); }
;

pragmaExpression:
    expression
	{ $$ = $1; }
|
    pragmas pragmaExpression %prec OP_UNOP
	{ $$ = new_ExprPragma( $1, $2 ); }
;

unary_operator:
    KEY_NOT
	{ $$ = UNOP_NOT; }
|
    OP_PLUS
	{ $$ = UNOP_PLUS; }
|
    OP_MINUS
	{ $$ = UNOP_NEGATE; }
;

orig_identifier:
    IDENTIFIER
	{ $$ = make_origsymbol( $1 ); }
;

%%

/* A simple wrapper function to give a nicer interface to the parser.
 * Given the file handle of the input file 'infile', the name of the
 * input file 'infilename', and the file handle of the output file
 * 'outfile', parse that input file, and write the generated code
 * to the output.
 */
vnusprog parse( FILE *infile, tmstring infilename )
{
    setlexfile( infile, infilename );
    goterr = FALSE;
    if( yyparse() != 0 ){
	error( "cannot recover from earlier parse errors, goodbye" );
    }
    return result;
}
