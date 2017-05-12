%{
#include <stdlib.h>
#include <stdio.h>
#include <tmc.h>
#include <string.h>

#include "defs.h"
#include "tmadmin.h"
#include <vnusctl.h>
#include "error.h"
#include "lex.h"
#include "parser.h"
#include "global.h"
#include "symbol_table.h"
#include "generate.h"
#include "service.h"
#include "type.h"

/* Always allow parse trace, since it is switchable from the
 * command line.
 */
#define YYDEBUG 1

/* Current Bison (1.35) refuses to resize the stack when __cplusplus
 * is defined. Don't ask me why. The only solution is to increase
 * the initial stack size.
 */
#define YYINITDEPTH 2000

/* This is only defined because bison.simple assumes it is... */
#define YYMAXDEPTH 20000

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

/* Used to discriminate against fillednew on anything else than a shape */
static bool is_shape_type( type t )
{
    switch( t->tag ){
	case TAGTypeShape:
	    return true;

	case TAGTypePragmas:
	    return is_shape_type( to_TypePragmas(t)->t );

	case TAGTypeMap:
	    return is_shape_type( to_TypeMap(t)->t );

	default:
	    break;
    }
    return false;
}
%}

/* use sort -b +2 */

%union {
    BINOP		_binop;
    UNOP		_unop;
    block		_block;
    cardinality		_cardinality;
    cardinality_list	_cardinalityList;
    secondary		_secondary;
    secondary_list	_secondaryList;
    switchCase		_switchCase;
    switchCase_list	_switchCaseList;
    waitCase		_waitCase;
    waitCase_list	_waitCaseList;
    declaration		_declaration;
    declaration_list	_declarationList;
    expression		_expression;
    expression_list	_expressionList;
    field		_field;
    field_list		_fieldList;
    origsymbol		_formal;
    origsymbol_list	_formalList;
    int			_int;
    statement_list	_statementList;
    location		_location;
    optexpression	_optexpression;
    origin		_origin;
    origsymbol		_originIdentifier;
    Pragma		_pragma;
    Pragma_list		_pragmaList;
    size		_size;
    size_list		_sizeList;
    statement		_statement;
    vnus_byte		_vnus_byte;
    vnus_short		_vnus_short;
    vnus_int		_vnus_int;
    vnus_long		_vnus_long;
    vnus_float		_vnus_float;
    vnus_double		_vnus_double;
    vnus_char		_vnus_char;
    vnus_string		_vnus_string;
    tmsymbol		_identifier;
    type		_type;
    type_list		_typeList;
    PragmaExpression	_pragmaexpr;
    PragmaExpression_list	_pragmaexprList;
}

%start program

/* The typed tokens first. */
%token <_identifier>	IDENTIFIER
%token <_vnus_byte>	BYTE_LITERAL
%token <_vnus_short>	SHORT_LITERAL
%token <_vnus_int>	INT_LITERAL
%token <_vnus_float>	FLOAT_LITERAL
%token <_vnus_double>	DOUBLE_LITERAL
%token <_vnus_long>	LONG_LITERAL
%token <_vnus_string>	STRING_LITERAL
%token <_vnus_char>	CHAR_LITERAL

%token KEY_ABLOCKRECEIVE
%token KEY_ABLOCKSEND
%token KEY_ADDTASK
%token KEY_AND
%token KEY_ARECEIVE
%token KEY_ARRAY
%token KEY_ASEND
%token KEY_ASSIGN
%token KEY_ASSIGNOP
%token KEY_BARRIER
%token KEY_BLOCKRECEIVE
%token KEY_BLOCKSEND
%token KEY_BOOLEAN
%token KEY_BYTE
%token KEY_CARDINALITYVARIABLE
%token KEY_CAST
%token KEY_CATCH
%token KEY_CHAR
%token KEY_CHECKEDINDEX
%token KEY_COMPLEX
%token KEY_DECLARATIONS
%token KEY_DEFAULT
%token KEY_DELETE
%token KEY_DONTCARE
%token KEY_DOUBLE
%token KEY_DOWHILE
%token KEY_EACH
%token KEY_EMPTY
%token KEY_EXECUTETASKS
%token KEY_EXPRESSION
%token KEY_EXPRESSIONPRAGMA
%token KEY_EXTERNALFUNCTION
%token KEY_EXTERNALPROCEDURE
%token KEY_EXTERNALVARIABLE
%token KEY_FALSE
%token KEY_FIELD
%token KEY_FILLEDNEW
%token KEY_FINAL
%token KEY_FLOAT
%token KEY_FOR
%token KEY_FORALL
%token KEY_FOREACH
%token KEY_FORK
%token KEY_FORKALL
%token KEY_FORMALVARIABLE
%token KEY_FUNCTION
%token KEY_FUNCTIONCALL
%token KEY_GARBAGECOLLECT
%token KEY_GETBUF
%token KEY_GETLENGTH
%token KEY_GETSIZE
%token KEY_GLOBALVARIABLE
%token KEY_GOTO
%token KEY_IF
%token KEY_INT
%token KEY_ISRAISED
%token KEY_LOCAL
%token KEY_LOCALVARIABLE
%token KEY_LONG
%token KEY_MOD
%token KEY_NEW
%token KEY_NEWARRAY
%token KEY_NEWFILLEDARRAY
%token KEY_NEWRECORD
%token KEY_NOT
%token KEY_NOTNULLASSERT
%token KEY_NULL
%token KEY_NULLEDNEW
%token KEY_OR
%token KEY_POINTER
%token KEY_PRAGMA
%token KEY_PRINT
%token KEY_PRINTLN
%token KEY_PROCEDURE
%token KEY_PROCEDURECALL
%token KEY_PROGRAM
%token KEY_READONLY
%token KEY_RECEIVE
%token KEY_RECORD
%token KEY_REDUCTION
%token KEY_REGISTERTASK
%token KEY_RETHROW
%token KEY_RETURN
%token KEY_SEND
%token KEY_SHAPE
%token KEY_SHORT
%token KEY_SHORTAND
%token KEY_SHORTOR
%token KEY_SIGNAL
%token KEY_SIZEOF
%token KEY_STATEMENTS
%token KEY_STRING
%token KEY_SWITCH
%token KEY_THROW
%token KEY_TIMEOUT
%token KEY_TRUE
%token KEY_UNCHECKED
%token KEY_VALUE
%token KEY_VOLATILE
%token KEY_WAIT
%token KEY_WAITPENDING
%token KEY_WHERE
%token KEY_WHILE
%token KEY_XOR
%token OP_ADDRESS
%token OP_DIVIDE
%token OP_EQUAL
%token OP_GREATER
%token OP_GREATEREQUAL
%token OP_LESS
%token OP_LESSEQUAL
%token OP_MINUS
%token OP_NEGATE
%token OP_NOTEQUAL
%token OP_PLUS
%token OP_SHIFTLEFT
%token OP_SHIFTRIGHT
%token OP_TIMES
%token OP_USHIFTRIGHT

/* use sort -b +2 */

%type	<_pragmaexpr>		ListPragmaExpression
%type	<_pragmaexpr>		LiteralPragmaExpression
%type	<_pragmaexpr>		NamePragmaExpression
%type	<_pragmaexpr>		PragmaExpression
%type	<_pragmaexprList>	PragmaExpressions
%type	<_statement>		aBlockReceive
%type	<_statement>		aBlockSend
%type	<_statement>		aReceive
%type	<_statement>		aSend
%type	<_expression>		accessExpression
%type	<_expression>		actualParameter
%type	<_expressionList>	actualParameterList
%type	<_expressionList>	actualParameters
%type   <_statement>            addTask
%type	<_expression>		assertExpression
%type	<_statement>		assignment
%type	<_statement>		assignmentop
%type	<_statement>		barrier
%type	<_type>			basetype
%type	<_binop>		binaryOperator
%type	<_block>		block
%type	<_statement>		blockReceive
%type	<_statement>		blockSend
%type	<_cardinalityList>	cardinalities
%type	<_cardinality>		cardinality
%type	<_cardinalityList>	cardinalityList
%type	<_declaration>		cardinalityVariableDeclaration
%type	<_statement>		catch
%type	<_statement>		communication
%type	<_expression>		constructorExpression
%type	<_statement>		control
%type	<_declaration>		declaration
%type	<_declarationList>	declarationList
%type	<_declarationList>	declarations
%type	<_statement>		delete
%type	<_statement>		dowhile
%type	<_statement>		each
%type	<_statement>		empty
%type	<_statement>		executeTasks
%type	<_expression>		expression
%type	<_expressionList>	expressionList
%type	<_statement>		expressionStatement
%type	<_optexpression>	expression_opt
%type	<_declaration>		externalFunctionDeclaration
%type	<_declaration>		externalProcedureDeclaration
%type	<_declaration>		externalVariableDeclaration
%type	<_field>		field
%type	<_fieldList>		fieldList
%type	<_statement>		for
%type	<_statement>		forall
%type	<_statement>		foreach
%type	<_statement>		fork
%type	<_statement>		forkall
%type	<_formal>		formalParameter
%type	<_formalList>		formalParameterList
%type	<_formalList>		formalParameters
%type	<_declaration>		formalVariableDeclaration
%type	<_declaration>		functionDeclaration
%type	<_statement>		garbageCollect
%type	<_declaration>		globalVariableDeclaration
%type	<_statement>		goto
%type	<_identifier>		identifier
%type	<_statement>		if
%type	<_statement>		imperative
%type	<_originIdentifier>	labelName
%type	<_originIdentifier>	label_opt
%type	<_expression>		literalExpression
%type	<_declaration>		localVariableDeclaration
%type	<_location>		location
%type	<_statement>		memoryManagement
%type	<_expression>		miscellaneousExpression
%type	<_int>			modifier
%type	<_int>			modifiers
%type	<_int>			modifiers_opt
%type	<_expression>		operatorExpression
%type	<_origin>		origin
%type	<_originIdentifier>	originIdentifier
%type	<_originIdentifier>	markerName
%type	<_statement>		parallelization
%type	<_pragma>		pragma
%type	<_pragmaList>		pragmaList
%type	<_pragmaList>		pragmas
%type	<_pragmaList>		pragmas_opt
%type	<_statement>		print
%type	<_statement>		println
%type	<_statement>		procedureCall
%type	<_declaration>		procedureDeclaration
%type	<_statement>		receive
%type	<_declaration>		recordDeclaration
%type	<_statement>		registerTask
%type	<_statement>		rethrow
%type	<_statement>		return
%type   <_declaration>          routineDeclaration
%type	<_expression>		routineExpression
%type	<_identifier>		scopename
%type	<_secondaryList>	secondaries
%type	<_secondaryList>	secondariesOpt
%type	<_secondary>		secondary
%type	<_secondaryList>	secondaryList
%type	<_location>		selectionLocation
%type	<_expression>		selector
%type	<_expressionList>	selectorList
%type	<_expressionList>	selectors
%type	<_statement>		send
%type	<_expression>		shapeInfoExpression
%type	<_size>			size
%type	<_sizeList>		sizeList
%type	<_sizeList>		sizes
%type	<_statement>		statement
%type	<_statementList>	statementList
%type	<_statementList>	statements
%type	<_statement>		support
%type	<_statement>		switch
%type	<_switchCase>		switchCase
%type	<_switchCaseList>	switchCaseList
%type	<_waitCase>		waitCase
%type	<_waitCaseList>		waitCaseList
%type   <_statement>            taskparallel
%type	<_statement>		throw
%type	<_type>			type
%type	<_declaration>		typeDeclaration
%type	<_typeList>		typeList
%type	<_unop>			unaryOperator
%type	<_statement>		unlabeledStatement
%type	<_statement>		valueReturn
%type   <_declaration>          variableDeclaration
%type	<_statement>		wait
%type   <_statement>            waitpending
%type	<_statement>		while

%%


/* Program */

program:
    KEY_PROGRAM pragmas_opt declarations block
	{ result = new_vnusprog( $2, $3, $4 ); }
;


/* Declarations */

commaOpt:
    /* empty */
|
    ','
;

declarations:
    KEY_DECLARATIONS '[' declarationList commaOpt ']'
	{ $$ = $3; }
|
    KEY_DECLARATIONS error ']'
	{ $$ = new_declaration_list(); }
;

declarationList:
    /* empty */
	{ $$ = new_declaration_list(); }
|
    declaration
	{ $$ = append_declaration_list( new_declaration_list(), $1 ); }
|
    declarationList ',' declaration
	{ $$ = append_declaration_list( $1, $3 ); }
|
    error ',' declaration
	{ $$ = append_declaration_list( new_declaration_list(), $3 ); }
;

declaration:
    routineDeclaration
	{ $$ = $1; }
|
    variableDeclaration
	{ $$ = $1; }
|
    typeDeclaration
	{ $$ = $1; }
;

routineDeclaration:
    functionDeclaration
	{ $$ = $1; }
|
    procedureDeclaration
	{ $$ = $1; }
|
    externalFunctionDeclaration
	{ $$ = $1; }
|
    externalProcedureDeclaration
	{ $$ = $1; }
;

variableDeclaration:
    globalVariableDeclaration
	{ $$ = $1; }
|
    externalVariableDeclaration
	{ $$ = $1; }
|
    cardinalityVariableDeclaration
	{ $$ = $1; }
|
    localVariableDeclaration
	{ $$ = $1; }
|
    formalVariableDeclaration
	{ $$ = $1; }
;

typeDeclaration:
    recordDeclaration
	{ $$ = $1; }
;

globalVariableDeclaration:
    KEY_GLOBALVARIABLE originIdentifier type expression_opt modifiers_opt pragmas_opt
	{ $$ = new_DeclGlobalVariable( $2, $5, $6, $3, $4 ); }
;

functionDeclaration:
    KEY_FUNCTION originIdentifier formalParameters type modifiers_opt pragmas_opt block
	{ $$ = new_DeclFunction( $2, $5, $6, $3, $4, $7 ); }
;

procedureDeclaration:
    KEY_PROCEDURE originIdentifier formalParameters modifiers_opt pragmas_opt block
	{ $$ = new_DeclProcedure( $2, $4, $5, $3, $6 ); }
;

localVariableDeclaration:
    KEY_LOCALVARIABLE originIdentifier scopename type expression_opt modifiers_opt pragmas_opt
	{ $$ = new_DeclLocalVariable( $2, $6, $7, $3, $4, $5 ); }
;

formalVariableDeclaration:
    KEY_FORMALVARIABLE originIdentifier scopename type modifiers_opt pragmas_opt
	{ $$ = new_DeclFormalVariable( $2, $5, $6, $3, $4 ); }
;

cardinalityVariableDeclaration:
    KEY_CARDINALITYVARIABLE originIdentifier modifiers_opt pragmas_opt
	{ $$ = new_DeclCardinalityVariable( $2, $3, $4 ); }
;            

externalVariableDeclaration:
    KEY_EXTERNALVARIABLE originIdentifier type modifiers_opt pragmas_opt
	{ $$ = new_DeclExternalVariable( $2, $4, $5, $3 ); }
;

externalFunctionDeclaration:
    KEY_EXTERNALFUNCTION originIdentifier formalParameters type modifiers_opt pragmas_opt
	{ $$ = new_DeclExternalFunction( $2, $5, $6, $3, $4 ); }
;

externalProcedureDeclaration:
    KEY_EXTERNALPROCEDURE originIdentifier formalParameters modifiers_opt pragmas_opt
	{ $$ = new_DeclExternalProcedure( $2, $4, $5, $3 ); }
;

recordDeclaration:
    KEY_RECORD originIdentifier '[' fieldList ']' modifiers_opt pragmas_opt
	{ $$ = new_DeclRecord( $2, $6, $7, $4 ); }
;

modifiers_opt:
    /* empty */
	{ $$ = 0; }
|
    modifiers
	{ $$ = $1; }
;

modifiers:
    modifier
	{ $$ = $1; }
|
    modifiers modifier
	{ $$ = $1 | $2; }
;

modifier:
    KEY_FINAL
	{ $$ = MOD_FINAL; }
|
    KEY_READONLY
	{ $$ = MOD_READONLY; }
|
    KEY_LOCAL
	{ $$ = MOD_LOCAL; }
|
    KEY_UNCHECKED
	{ $$ = MOD_UNCHECKED; }
|
    KEY_VOLATILE
	{ $$ = MOD_VOLATILE; }
;

block:
    KEY_STATEMENTS scopename origin pragmas_opt statements
	{ $$ = new_block( $2, $3, $4, OwnerExpr_listNIL, $5 ); }
|
    KEY_STATEMENTS origin pragmas_opt statements
	{ $$ = new_block( tmsymbolNIL, $2, $3, OwnerExpr_listNIL, $4 ); }
;

statements:
    '[' statementList commaOpt ']'
	{ $$ = $2; }
|
    error ']'
	{ $$ = new_statement_list(); }
;

statementList:
    /* empty */
	{ $$ = new_statement_list(); }
|
    statement
	{ $$ = append_statement_list( new_statement_list(), $1 ); }
|
    statementList ',' statement
	{ $$ = append_statement_list( $1, $3 ); }
|
    error ',' statement
	{ $$ = append_statement_list( new_statement_list(), $3 ); }
;

statement:
    label_opt pragmas_opt unlabeledStatement
	{
	    $$ = $3;
	    $$->label = $1;
	    $$->pragmas = $2;
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
|
	taskparallel
        { $$ = $1; }

;


/* Declarations */

formalParameters:
    '[' formalParameterList commaOpt ']'
	{ $$ = $2; }
|
    '[' error ']'
	{ $$ = new_origsymbol_list(); }
;

formalParameterList:
    /* empty */
	{ $$ = new_origsymbol_list(); }
|
    formalParameter
	{ $$ = append_origsymbol_list( new_origsymbol_list(), $1 ); }
|
    formalParameterList ',' formalParameter
	{ $$ = append_origsymbol_list( $1, $3 ); }
|
    error ',' formalParameter
	{ $$ = append_origsymbol_list( new_origsymbol_list(), $3 ); }
;

formalParameter:
     originIdentifier
	{ $$ = $1; }
;


/* Flow of Control */

control:
    while
 	{ $$ = $1; }
|
    dowhile
 	{ $$ = $1; }
|
    for
 	{ $$ = $1; }
|
    if
 	{ $$ = $1; }
|
    origin block
	{ $$ = new_SmtBlock( origsymbolNIL, $1, Pragma_listNIL, OwnerExpr_listNIL, $2 ); }
|
    switch
 	{ $$ = $1; }
|
    wait
	{ $$ = $1; }
|
    return
 	{ $$ = $1; }
|
    valueReturn
 	{ $$ = $1; }
|
    goto
 	{ $$ = $1; }
|
    throw
 	{ $$ = $1; }
|
    rethrow
 	{ $$ = $1; }
|
    catch
 	{ $$ = $1; }
;

while:
    KEY_WHILE origin expression block
	{ $$ = new_SmtWhile( origsymbolNIL, $2, Pragma_listNIL, OwnerExpr_listNIL, $3, $4 ); }
;

dowhile:
    KEY_DOWHILE origin expression block
	{ $$ = new_SmtDoWhile( origsymbolNIL, $2, Pragma_listNIL, OwnerExpr_listNIL, $3, $4 ); }
;

for:
    KEY_FOR origin cardinalities block
	{ $$ = new_SmtFor( origsymbolNIL, $2, Pragma_listNIL, OwnerExpr_listNIL, $3, $4 ); }
;

if:
    KEY_IF origin expression block block
	{ $$ = new_SmtIf( origsymbolNIL, $2, Pragma_listNIL, OwnerExpr_listNIL, $3, $4, $5 ); }
;

switch:
    KEY_SWITCH origin expression '[' switchCaseList commaOpt ']'
	{ $$ = new_SmtSwitch( origsymbolNIL, $2, Pragma_listNIL, OwnerExpr_listNIL, $3, $5 ); }
;

return:
    KEY_RETURN origin
	{ $$ = new_SmtReturn( origsymbolNIL, $2, Pragma_listNIL, OwnerExpr_listNIL ); }
;

valueReturn:
    KEY_RETURN origin expression
	{ $$ = new_SmtValueReturn( origsymbolNIL, $2, Pragma_listNIL, OwnerExpr_listNIL, $3 ); }
;

goto:
    KEY_GOTO origin labelName
	{ $$ = new_SmtGoto( origsymbolNIL, $2, Pragma_listNIL, OwnerExpr_listNIL, $3 ); }
;

throw:
    KEY_THROW origin expression
	{ $$ = new_SmtThrow( origsymbolNIL, $2, Pragma_listNIL, OwnerExpr_listNIL, $3 ); }
;

rethrow:
    KEY_RETHROW origin
	{ $$ = new_SmtRethrow( origsymbolNIL, $2, Pragma_listNIL, OwnerExpr_listNIL ); }
;

catch:
    KEY_CATCH origin formalParameter block block
	{ $$ = new_SmtCatch( origsymbolNIL, $2, Pragma_listNIL, OwnerExpr_listNIL, $3, $4, $5 ); }
;

cardinalities:
    '[' cardinalityList commaOpt ']'
	{ $$ = $2; }
|
    '[' error ']'
	{ $$ = new_cardinality_list(); }
;

cardinalityList:
    /* empty */
	{ $$ = new_cardinality_list(); }
|
    cardinality
	{ $$ = append_cardinality_list( new_cardinality_list(), $1 ); }
|
    cardinalityList ',' cardinality
	{ $$ = append_cardinality_list( $1, $3 ); }
|
    error ',' cardinality
	{ $$ = append_cardinality_list( new_cardinality_list(), $3 ); }
;

cardinality:
    originIdentifier OP_EQUAL expression ':' expression secondariesOpt
	{ $$ = new_cardinality( $1, $3, $5, new_ExprInt((vnus_int) 1), $6 ); }
|
    originIdentifier OP_EQUAL expression ':' expression ':' expression secondariesOpt
	{ $$ = new_cardinality( $1, $3, $5, $7, $8 ); }
|
    originIdentifier ':' expression secondariesOpt
	{ $$ = new_cardinality( $1, new_ExprInt((vnus_int) 0), $3, new_ExprInt((vnus_int) 1), $4 ); }
;

secondariesOpt:
    /* empty */
        { $$ = new_secondary_list(); }
|
    secondaries
	{ $$ = $1; }
;

secondaries:
    '(' secondaryList commaOpt ')'
	{ $$ = $2; }
|
    '(' error ')'
	{ $$ = new_secondary_list(); }
;

secondaryList:
    /* empty */
	{ $$ = new_secondary_list(); }
|
    secondary
	{ $$ = append_secondary_list( new_secondary_list(), $1 ); }
|
    secondaryList ',' secondary
	{ $$ = append_secondary_list( $1, $3 ); }
|
    error ',' secondary
	{ $$ = append_secondary_list( new_secondary_list(), $3 ); }
;


secondary:
     originIdentifier OP_EQUAL expression ':' expression
	 { $$ = new_secondary( $1, $3, $5 ); }
;

switchCaseList:
    /* empty */
	{ $$ = new_switchCase_list(); }
|
    switchCase
	{ $$ = append_switchCase_list( new_switchCase_list(), $1 ); }
|
    switchCaseList ',' switchCase
	{ $$ = append_switchCase_list( $1, $3 ); }
|
    error ',' switchCase
	{ $$ = append_switchCase_list( new_switchCase_list(), $3 ); }
;

switchCase:
    '(' INT_LITERAL ',' block ')'
        { $$ = new_SwitchCaseValue( $4, $2 ); }
|
    '(' KEY_DEFAULT ',' block ')'
        { $$ = new_SwitchCaseDefault( $4 ); }
;


waitCaseList:
    /* empty */
	{ $$ = new_waitCase_list(); }
|
    waitCase
	{ $$ = append_waitCase_list( new_waitCase_list(), $1 ); }
|
    waitCaseList ',' waitCase
	{ $$ = append_waitCase_list( $1, $3 ); }
|
    error ',' waitCase
	{ $$ = append_waitCase_list( new_waitCase_list(), $3 ); }
;

waitCase:
    KEY_VALUE expression block
        { $$ = new_WaitCaseValue( $3, $2 ); }
|
    KEY_TIMEOUT expression block
        { $$ = new_WaitCaseTimeout( $3, $2 ); }
;


/* Parallelization */

parallelization:
    forall
	{ $$ = $1; }
|
    forkall
	{ $$ = $1; }
|
    fork
	{ $$ = $1; }
|
    foreach
	{ $$ = $1; }
|
    each
	{ $$ = $1; }
;

forall:
    KEY_FORALL origin cardinalities block
	{ $$ = new_SmtForall( origsymbolNIL, $2, Pragma_listNIL, OwnerExpr_listNIL, $3, OwnerExpr_listNIL, $4 ); }
;

forkall:
    KEY_FORKALL origin cardinalities block
	{ $$ = new_SmtForkall( origsymbolNIL, $2, Pragma_listNIL, OwnerExpr_listNIL, $3, OwnerExpr_listNIL, $4 ); }
;

fork:
    KEY_FORK origin '[' statementList commaOpt ']'
	{ $$ = new_SmtFork( origsymbolNIL, $2, Pragma_listNIL, OwnerExpr_listNIL, $4 ); }
;

foreach:
    KEY_FOREACH origin cardinalities block
	{ $$ = new_SmtForeach( origsymbolNIL, $2, Pragma_listNIL, OwnerExpr_listNIL, $3, OwnerExpr_listNIL, $4 ); }
;

each:
    KEY_EACH origin '[' statementList commaOpt ']'
	{ $$ = new_SmtEach( origsymbolNIL, $2, Pragma_listNIL, OwnerExpr_listNIL, $4 ); }
;


/* Communication */

communication:
    barrier
	{ $$ = $1; }
|
    waitpending
	{ $$ = $1; }
|
    send
	{ $$ = $1; }
|
    receive
	{ $$ = $1; }
|
    blockSend
	{ $$ = $1; }
|
    blockReceive
	{ $$ = $1; }
|
    aSend
	{ $$ = $1; }
|
    aReceive
	{ $$ = $1; }
|
    aBlockSend
	{ $$ = $1; }
|
    aBlockReceive
	{ $$ = $1; }
;

barrier:
    KEY_BARRIER origin
	{ $$ = new_SmtBarrier( origsymbolNIL, $2, Pragma_listNIL, OwnerExpr_listNIL ); }
;

wait:
    KEY_WAIT origin '[' waitCaseList commaOpt ']'
	{ $$ = new_SmtWait( origsymbolNIL, $2, Pragma_listNIL, OwnerExpr_listNIL, $4 ); }
;

waitpending:
    KEY_WAITPENDING origin
	{ $$ = new_SmtWaitPending( origsymbolNIL, $2, Pragma_listNIL, OwnerExpr_listNIL ); }
;

send:
    KEY_SEND origin expression expression
	{ $$ = new_SmtSend( origsymbolNIL, $2, Pragma_listNIL, OwnerExpr_listNIL, $3, $4 ); }
;

receive:
    KEY_RECEIVE origin expression location
	{ $$ = new_SmtReceive( origsymbolNIL, $2, Pragma_listNIL, OwnerExpr_listNIL, $3, $4 ); }
;

blockSend:
    /*                    dest       buf        nelm */
    KEY_BLOCKSEND origin expression expression expression
	{ $$ = new_SmtBlocksend( origsymbolNIL, $2, Pragma_listNIL, OwnerExpr_listNIL, $3, $4, $5 ); }
;

blockReceive:
    /*                      src        buf        nelm */
    KEY_BLOCKRECEIVE origin expression expression expression
	{ $$ = new_SmtBlockreceive( origsymbolNIL, $2, Pragma_listNIL, OwnerExpr_listNIL, $3, $4, $5 ); }
;

aSend:
    KEY_ASEND origin expression expression
	{ $$ = new_SmtASend( origsymbolNIL, $2, Pragma_listNIL, OwnerExpr_listNIL, $3, $4 ); }
;

aReceive:
    KEY_ARECEIVE origin expression location
	{ $$ = new_SmtAReceive( origsymbolNIL, $2, Pragma_listNIL, OwnerExpr_listNIL, $3, $4 ); }
;

aBlockSend:
    /*                    dest       buf        nelm */
    KEY_ABLOCKSEND origin expression expression expression
	{ $$ = new_SmtABlocksend( origsymbolNIL, $2, Pragma_listNIL, OwnerExpr_listNIL, $3, $4, $5 ); }
;

aBlockReceive:
    /*                      src        buf        nelm */
    KEY_ABLOCKRECEIVE origin expression expression expression
	{ $$ = new_SmtABlockreceive( origsymbolNIL, $2, Pragma_listNIL, OwnerExpr_listNIL, $3, $4, $5 ); }
;


/* Imperative.  */

imperative:
    assignment
	{ $$ = $1; }
|
    assignmentop
	{ $$ = $1; }
|
    procedureCall
	{ $$ = $1; }
|
    expressionStatement
	{ $$ = $1; }
;

assignment:
    KEY_ASSIGN origin location expression
	{ $$ = new_SmtAssign( origsymbolNIL, $2, Pragma_listNIL, OwnerExpr_listNIL, $3, $4 ); }
;

assignmentop:
    KEY_ASSIGNOP origin location binaryOperator expression
	{ $$ = new_SmtAssignOp( origsymbolNIL, $2, Pragma_listNIL, OwnerExpr_listNIL, $3, $4, $5 ); }
;

procedureCall:
    KEY_PROCEDURECALL origin routineExpression actualParameters
	{ $$ = new_SmtProcedureCall( origsymbolNIL, $2, Pragma_listNIL, OwnerExpr_listNIL, $3, $4 ); }
;

expressionStatement:
    KEY_EXPRESSION origin expression
	{ $$ = new_SmtExpression( origsymbolNIL, $2, Pragma_listNIL, OwnerExpr_listNIL, $3 ); }
;

routineExpression:
    originIdentifier
	{ $$ = new_ExprName( $1 ); }
|
    OP_TIMES expression
	{ $$ = new_ExprDeref( $2 ); }
;


/* Memory management. */

memoryManagement:
    delete
	{ $$ = $1; }
|
    garbageCollect
	{ $$ = $1; }
;

delete:
    KEY_DELETE origin expression
	{ $$ = new_SmtDelete( origsymbolNIL, $2, Pragma_listNIL, OwnerExpr_listNIL, $3 ); }
;

garbageCollect:
    KEY_GARBAGECOLLECT origin
	{ $$ = new_SmtGarbageCollect( origsymbolNIL, $2, Pragma_listNIL, OwnerExpr_listNIL ); }
;


/* support */

support:
    empty
        { $$ = $1; }
|
    print
        { $$ = $1; }
|
    println
        { $$ = $1; }
;

empty:
    KEY_EMPTY origin
	{ $$ = new_SmtEmpty( origsymbolNIL, $2, Pragma_listNIL, OwnerExpr_listNIL ); }
;

print:
    KEY_PRINT origin actualParameters
        { $$ = new_SmtPrint( origsymbolNIL, $2, Pragma_listNIL, OwnerExpr_listNIL, $3 ); }
;

println:
    KEY_PRINTLN origin actualParameters
        { $$ = new_SmtPrintLn( origsymbolNIL, $2, Pragma_listNIL, OwnerExpr_listNIL, $3 ); }
;

/* Task parallel */

taskparallel:
    addTask
        { $$ = $1; }
|
    executeTasks
        { $$ = $1; }
|
    registerTask
        { $$ = $1; }
;

addTask:
    KEY_ADDTASK origin expression expression
        { $$ = new_SmtAddTask( origsymbolNIL, $2, Pragma_listNIL, OwnerExpr_listNIL, $3, $4 ); }
;

executeTasks:
    KEY_EXECUTETASKS origin
        { $$ = new_SmtExecuteTasks( origsymbolNIL, $2, Pragma_listNIL, OwnerExpr_listNIL ); }
;

registerTask:
    KEY_REGISTERTASK origin expression expression
        { $$ = new_SmtRegisterTask( origsymbolNIL, $2, Pragma_listNIL, OwnerExpr_listNIL, $3, $4 ); }
;


/* Expressions */

location:
    originIdentifier
        { $$ = new_LocName( $1 ); }
|
    KEY_FIELD expression originIdentifier
	{ $$ = new_LocField( $2, $3 ); }
|
    KEY_FIELD expression INT_LITERAL
	{ $$ = new_LocFieldNumber( $2, $3 ); }
|
    selectionLocation
	{ $$ = $1; }
|
    KEY_EXPRESSIONPRAGMA pragmas location
        { $$ = new_LocWrapper( $2, OwnerExpr_listNIL, $3 ); }
|
    OP_TIMES expression
	{ $$ = new_LocDeref( $2 ); }
|
    KEY_WHERE scopename location
	{ $$ = new_LocWhere( $2, $3 ); }
;

selectionLocation:
    '(' expression ',' selectors ')'
	{ $$ = new_LocSelection( $2, $4 ); }
;

expression_opt:
    /* empty. */
	{ $$ = new_OptExprNone(); }
|
    expression
	{ $$ = new_OptExpr( $1 ); }
;

expression:
    literalExpression
	{ $$ = $1; }
|
    accessExpression
	{ $$ = $1; }
|
    constructorExpression
	{ $$ = $1; }
|
    operatorExpression
	{ $$ = $1; }
|
    shapeInfoExpression
	{ $$ = $1; }
|
    assertExpression
	{ $$ = $1; }
|
    miscellaneousExpression
	{ $$ = $1; }
;

literalExpression:
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
    CHAR_LITERAL
	{ $$ = new_ExprChar( $1 ); }
|
    KEY_FALSE
	{ $$ = new_ExprBoolean( false ); }
|
    KEY_TRUE
	{ $$ = new_ExprBoolean( true ); }
|
    STRING_LITERAL
	{ $$ = new_ExprString( $1 ); }
|
    KEY_NULL
	{ $$ = new_ExprNull(); }
|
    KEY_SIZEOF type
	{ $$ = new_ExprSizeof( $2 ); }
;

accessExpression:
    originIdentifier
	{ $$ = new_ExprName( $1 ); }
|
    '(' expression ',' selectors ')'
	{ $$ = new_ExprSelection( $2, $4 ); }
|
    KEY_FIELD expression originIdentifier
	{ $$ = new_ExprField( $2, $3 ); }
|
    KEY_FIELD expression INT_LITERAL
	{ $$ = new_ExprFieldNumber( $2, $3 ); }
|
    OP_TIMES expression
	{ $$ = new_ExprDeref( $2 ); }
|
    KEY_FUNCTIONCALL routineExpression actualParameters
	{ $$ = new_ExprFunctionCall( $2, $3 ); }
;

constructorExpression:
    KEY_COMPLEX expression expression
	{ $$ = new_ExprComplex( $2, $3 ); }
|
    KEY_ARRAY type '[' expressionList commaOpt ']'
	{ $$ = new_ExprArray( $2, $4 ); }
|
    KEY_SHAPE sizes type '[' expressionList commaOpt ']'
	{ $$ = new_ExprShape( $2, $3, new_ExprArray( rdup_type( $3 ), $5 ), $5->sz ); }
|
    KEY_RECORD '[' expressionList commaOpt ']'
	{ $$ = new_ExprRecord( $3 ); }
|
    OP_ADDRESS location
	{ $$ = new_ExprAddress( $2 ); }
|
    KEY_NEW type
	{
	    if( is_shape_type( $2 ) ){
		parserror( "new is not allowed for shape types" );
	    }
	    $$ = new_ExprNew( $2 );
	}
|
    KEY_FILLEDNEW type expression markerName
	{
	    if( !is_shape_type( $2 ) ){
		parserror( "fillednew is only allowed for shape types" );
	    }
	    $$ = new_ExprFilledNew( $2, $3, $4 ); 
	}
|
    KEY_NULLEDNEW type markerName
	{
	    if( !is_shape_type( $2 ) ){
		parserror( "nullednew is only allowed for shape types" );
	    }
	    $$ = new_ExprNulledNew( $2, $3 ); 
	}
|
    KEY_NEWARRAY type expression
	{ $$ = new_ExprNewArray( $2, $3, new_OptExprNone() ); }
|
    KEY_NEWFILLEDARRAY type expression expression
	{ $$ = new_ExprNewArray( $2, $3, new_OptExpr( $4 ) ); }
|
    KEY_NEWRECORD type '[' expressionList commaOpt ']'
	{
	    if( !is_record_type( $2 ) ){
		parserror ( "newrecord is only allowed for record types" );
	    }
	    $$ = new_ExprNewRecord( $2, $4 );
	}
;

operatorExpression:
    KEY_CAST type expression
	{ $$ = new_ExprCast( $2, $3 ); }
|
    KEY_IF expression expression expression
	{ $$ = new_ExprIf( $2, $3, $4 ); }
|
    KEY_WHERE scopename expression
	{ $$ = new_ExprWhere( $2, $3 ); }
|
    '(' unaryOperator ',' expression ')'
	{ $$ = new_ExprUnop( $2, $4 ); }
|
    '(' unaryOperator ',' error ')'
	{ $$ = new_ExprUnop( $2, expressionNIL ); }
|
    '(' error ',' expression ')'
	{ $$ = new_ExprUnop( UNOP_NOT, $4 ); }
|
    '(' expression ',' binaryOperator ',' expression ')'
	{ $$ = new_ExprBinop( $2, $4, $6 ); }
|
    '(' expression ',' binaryOperator ',' error ')'
	{ $$ = new_ExprBinop( $2, $4, expressionNIL ); }
|
    '(' expression ',' error ',' expression ')'
	{ $$ = new_ExprBinop( $2, BINOP_AND, $6 ); }
|
    '(' error ',' binaryOperator ',' expression ')'
	{ $$ = new_ExprBinop( expressionNIL, $4, $6 ); }
|
    KEY_REDUCTION binaryOperator  '[' expressionList commaOpt ']'
	{ $$ = new_ExprReduction( $2, $4 ); }
;

shapeInfoExpression:
    KEY_GETSIZE expression expression
	{ $$ = new_ExprGetSize( $2, $3 ); }
|
    KEY_GETLENGTH expression
	{ $$ = new_ExprGetLength( $2 ); }
|
    KEY_GETBUF expression
	{ $$ = new_ExprGetBuf( $2 ); }
;

assertExpression:
    KEY_NOTNULLASSERT expression
	{ $$ = new_ExprNotNullAssert( $2 ); }
|
    /*                  val     upperbound */
    KEY_CHECKEDINDEX expression expression
	{ $$ = new_ExprCheckedIndex( $2, $3 ); }
;

miscellaneousExpression:
    KEY_EXPRESSIONPRAGMA pragmas expression
        { $$ = new_ExprWrapper( $2, OwnerExpr_listNIL, $3 ); }
|
    KEY_ISRAISED expression
	{ $$ = new_ExprIsRaised( $2 ); }
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

selectors:
    '[' selectorList commaOpt ']'
	{ $$ = $2; }
|
    '[' error ']'
	{ $$ = new_expression_list(); }
;

selectorList:
    /* empty */
	{ $$ = new_expression_list(); }
|
    selector
	{ $$ = append_expression_list( new_expression_list(), $1 ); }
|
    selectorList ',' selector
	{ $$ = append_expression_list( $1, $3 ); }
|
    error ',' selector
	{ $$ = append_expression_list( new_expression_list(), $3 ); }
;

selector:
    expression
	{ $$ = $1; }
;

actualParameters:
    '[' actualParameterList commaOpt ']'
	{ $$ = $2; }
|
    '[' error ']'
	{ $$ = new_expression_list(); }
;

actualParameterList:
    /* empty */
	{ $$ = new_expression_list(); }
|
    actualParameter
	{ $$ = append_expression_list( new_expression_list(), $1 ); }
|
    actualParameterList ',' actualParameter
	{ $$ = append_expression_list( $1, $3 ); }
|
    error ',' actualParameter
	{ $$ = append_expression_list( new_expression_list(), $3 ); }
;

actualParameter:
    expression
	{ $$ = $1; }
;

type:
    basetype
	{ $$ = $1; }
|
    KEY_ARRAY type
	{ $$ = new_TypeUnsizedArray( $2 ); }
|
    KEY_ARRAY INT_LITERAL type
	{ $$ = new_TypeArray( $2, $3 ); }
|
    KEY_SHAPE sizes type
	{ $$ = new_TypeShape( $2, $3 ); }
|
    KEY_RECORD '[' fieldList commaOpt ']'
	{ $$ = new_TypeRecord( $3 ); }
|
    KEY_RECORD originIdentifier
	{ $$ = new_TypeNamedRecord( $2 ); }
|
    KEY_POINTER type
	{ $$ = new_TypePointer( $2 ); }
|
    KEY_PROCEDURE '[' typeList commaOpt ']'
        { $$ = new_TypeProcedure( $3 ); }
|
    KEY_FUNCTION '[' typeList commaOpt ']' type
        { $$ = new_TypeFunction( $3, $6 ); }
|
    KEY_PRAGMA pragmas type
        { $$ = new_TypePragmas( $2, $3 ); }
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

basetype:
    KEY_STRING
	{ $$ = new_TypeBase( BT_STRING ); }
|
    KEY_BOOLEAN
	{ $$ = new_TypeBase( BT_BOOLEAN ); }
|
    KEY_BYTE
	{ $$ = new_TypeBase( BT_BYTE ); }
|
    KEY_SHORT
	{ $$ = new_TypeBase( BT_SHORT ); }
|
    KEY_INT
	{ $$ = new_TypeBase( BT_INT ); }
|
    KEY_LONG
	{ $$ = new_TypeBase( BT_LONG ); }
|
    KEY_FLOAT
	{ $$ = new_TypeBase( BT_FLOAT ); }
|
    KEY_DOUBLE
	{ $$ = new_TypeBase( BT_DOUBLE ); }
|
    KEY_CHAR
	{ $$ = new_TypeBase( BT_CHAR ); }
|
    KEY_COMPLEX
	{ $$ = new_TypeBase( BT_COMPLEX ); }
|
    IDENTIFIER
	{
	    parserror( "type expected, not an identifier" );
	    $$ = new_TypeBase( BT_INT );
	}
;

field:
    originIdentifier ':' type
	{ $$ = new_field( $1, $3 ); }
;

sizes:
    '[' sizeList commaOpt ']'
	{ $$ = $2; }
|
    '[' error ']'
	{ $$ = new_size_list(); }
;

sizeList:
    /* empty */
	{ $$ = new_size_list(); }
|
    size
	{ $$ = append_size_list( new_size_list(), $1 ); }
|
    sizeList ',' size
	{ $$ = append_size_list( $$, $3 ); }
|
    error ',' size
	{ $$ = append_size_list( new_size_list(), $3 ); }
;

size:
    origin KEY_DONTCARE
	{ $$ = new_SizeDontcare( $1 ); }
|
    expression
	{ $$ = new_SizeExpression( $1 ); }
;


/* Miscellaneous */

origin:
    /* empty */
	{ $$ = make_origin(); }
;

scopename:
    identifier
	{ $$ = $1; }
;

label_opt:
    /* empty */
	{ $$ = origsymbolNIL; }
|
    labelName ':'
	{ $$ = $1; }
;

labelName:
    originIdentifier
	{ $$ = $1; }
;

pragmas_opt:
    /* empty */
        { $$ = new_Pragma_list(); }
|
    KEY_PRAGMA pragmas
        { $$ = $2; }
;

pragma_open_bracket:
    '['
	{ enter_pragma_state(); }
;

pragma_close_bracket:
    ']'
	{ leave_pragma_state(); }
;

pragmas:
    pragma_open_bracket pragmaList commaOpt pragma_close_bracket
        { $$ = $2; }
|
    error pragma_close_bracket
        { $$ = new_Pragma_list(); }
;

pragmaList:
    /* empty */
        { $$ = new_Pragma_list(); }
|
    pragma
        { $$ = append_Pragma_list( new_Pragma_list(), $1 ); }
|
    pragmaList ',' pragma
        { $$ = append_Pragma_list( $1, $3 ); }
;

pragma:
    originIdentifier
        { $$ = new_FlagPragma( $1 ); }
|
    originIdentifier OP_EQUAL PragmaExpression
        { $$ = new_ValuePragma( $1, $3 ); }
;

PragmaExpression:
    LiteralPragmaExpression
	{ $$ = $1; }
|
    NamePragmaExpression
	{ $$ = $1; }
|
    ListPragmaExpression
	{ $$ = $1; }
;

LiteralPragmaExpression:
    INT_LITERAL
	{ $$ = new_NumberPragmaExpression( (vnus_double) $1 ); }
|
    FLOAT_LITERAL
	{ $$ = new_NumberPragmaExpression( $1 ); }
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
    originIdentifier
	{ $$ = new_NamePragmaExpression( $1 ); }
|
    '@' originIdentifier
	{ $$ = new_ExternalNamePragmaExpression( $2 ); }
;

ListPragmaExpression:
    '(' PragmaExpressions ')'
	{ $$ = new_ListPragmaExpression( $2 ); }
;

PragmaExpressions:
    /* empty */
	{ $$ = new_PragmaExpression_list(); }
|
    PragmaExpressions PragmaExpression
	{ $$ = append_PragmaExpression_list( $1, $2 ); }
;


/* Tokens */

unaryOperator:
    KEY_NOT
	{ $$ = UNOP_NOT; }
|
    OP_PLUS
	{ $$ = UNOP_PLUS; }
|
    OP_NEGATE
	{ $$ = UNOP_NEGATE; }
;

binaryOperator:
    KEY_AND
	{ $$ = BINOP_AND; }
|
    KEY_SHORTAND
	{ $$ = BINOP_SHORTAND; }
|
    KEY_OR
	{ $$ = BINOP_OR; }
|
    KEY_SHORTOR
	{ $$ = BINOP_SHORTOR; }
|
    KEY_XOR
	{ $$ = BINOP_XOR; }
|
    KEY_MOD
	{ $$ = BINOP_MOD; }
|
    OP_PLUS
	{ $$ = BINOP_PLUS; }
|
    OP_MINUS
	{ $$ = BINOP_MINUS; }
|
    OP_TIMES
	{ $$ = BINOP_TIMES; }
|
    OP_DIVIDE
	{ $$ = BINOP_DIVIDE; }
|
    OP_EQUAL
	{ $$ = BINOP_EQUAL; }
|
    OP_NOTEQUAL
	{ $$ = BINOP_NOTEQUAL; }
|
    OP_LESS
	{ $$ = BINOP_LESS; }
|
    OP_LESSEQUAL
	{ $$ = BINOP_LESSEQUAL; }
|
    OP_GREATER
	{ $$ = BINOP_GREATER; }
|
    OP_GREATEREQUAL
	{ $$ = BINOP_GREATEREQUAL; }
|
    OP_SHIFTLEFT
	{ $$ = BINOP_SHIFTLEFT; }
|
    OP_SHIFTRIGHT
	{ $$ = BINOP_SHIFTRIGHT; }
|
    OP_USHIFTRIGHT
	{ $$ = BINOP_USHIFTRIGHT; }
;

markerName:
    originIdentifier
	{ $$ = $1; }
|
    KEY_NULL
	{ $$ = origsymbolNIL; }
;

originIdentifier:
    origin identifier
	{ $$ = new_origsymbol( $2, $1 ); }
;

identifier:
    IDENTIFIER
	{ $$ = $1; }
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
    if( yyparse() != 0 ){
	error( "cannot recover from earlier parse errors, goodbye" );
    }
    return result;
}
