/*****

$Author: leo $
$Date: 1995/10/17 13:33:58 $
$Log: Scope.h,v $
Revision 1.1.1.1  1995/10/17 13:33:58  leo
Initial frontend source tree.

Revision 1.1  1995/10/13 15:30:29  joachim
Initial revision

$Revision: 1.1.1.1 $
$Source: /home/rama/timber/CVS/compiler/frontends/toyfront/src/Scope.h,v $ 

*****/

# ifndef jatScope
# define jatScope

# if defined __STDC__ | defined __cplusplus
# define ARGS(parameters)	parameters
# else
# define ARGS(parameters)	()
# endif


# ifdef __cplusplus
extern "C" {
# endif


# include "Errors.h"
# include "ErrorStr.h"
# include "Idents.h"
# include "Positions.h"
# include "Tree.h"

# ifdef __cplusplus
}
# endif


tEnv EnterScope ARGS ((tEnv EnvToHide));

/****
*
* input  : an environment EnvToHide
* returns: an environment that adds a new empty scope as the outermost
*          scope to EnvToHide
*
****/

tEnv LeaveScope ARGS ((tEnv NestedEnv));

/****
*
* input  : an environment NestedEnv
* returns: the environment obtained by peeling off the outermost scope
*          of NestedEnv
* errors : attempt to leave the outermost scope 
*
****/

tEnv AddVarDecl ARGS ((tIdent Ident, tPosition Pos, tTree Type, tEnv Env));

/****
*
* input  : variable name Ident
*          position in the source file Pos
*          type specification Type
*          current environment Env
* returns: the scope Env extended by a declaration of the variable Ident
* errors : attempt to declare an already declared identifier
*
****/

tEnv DeclareVariables ARGS ((tTree Identifiers, tTree Type, tEnv  Env));

/****
*
* input  : a list of variable names Identifiers
*          type specification Type
*          current environment Env
* returns: the scope Env extended by a declaration of the variables
*          in the list Identifiers
*
****/

tEnv AddConstDecl ARGS ((tIdent Ident, tPosition Pos, tTree Expression, 
  tEnv Env));

/****
*
* input  : constant name Ident
*          value specification Expression
*          current environment Env
* returns: the scope Env extended by a declaration of the constant Ident
* errors : attempt to declare an already declared identifier
*
****/

tEnv AddTypeDecl ARGS((tIdent Ident, tPosition Pos, tTree TypeSpec, tEnv Env));

/****
*
* input  : type name Ident
*          type specification Type
*          current environment Env
* returns: the scope Env extended by a declaration of the type Ident
* errors : attempt to declare an already declared identifier
*
****/

tEnv AddProcDecl ARGS ((tIdent Ident, tPosition Pos, tTree Formals, 
  tEnv Env));

/****
*
* input  : procedure name Ident
*          formal parameters Formals
*          current environment Env
* returns: the scope Env extended by a declaration of the procedure Ident
* errors : attempt to declare an already declared identifier
*
****/


tEnv AddFuncDecl ARGS ((tIdent Ident, tPosition Pos, tTree Formals, 
  tTree Result, tEnv Env));

/****
*
* input  : function name Ident
*          result parameter Result
*          current environment Env
* returns: the scope Env extended by a declaration of the function Ident
* errors : attempt to declare an already declared identifier
*
****/

tObject IdentifyVarOrConst ARGS ((tIdent Ident, tEnv Env));

/****
*
* input  : identifier Ident
*          environment Env
* returns: the object defining the variable or constant Ident in 
*          scope Env
*
****/

tObject IdentifyProc ARGS ((tIdent Ident, tEnv Env));

/****
*
* input  : identifier Ident
*          environment Env
* returns: the object defining the procedure Ident in scope Env
*
****/

tObject IdentifyFunc ARGS ((tIdent Ident, tEnv Env));

/****
*
* input  : identifier Ident
*          environment Env
* returns: the object defining the function Ident in scope Env
*
****/

tObject IdentifyType ARGS ((tIdent Ident, tEnv Env));

/****
*
* input  : identifier Ident
*          environment Env
* returns: the object defining the type Ident in scope Env
*
****/

# endif
