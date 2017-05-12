/*****

$Author: leo $
$Date: 1995/10/17 13:33:58 $
$Log: Scope.c,v $
Revision 1.1.1.1  1995/10/17 13:33:58  leo
Initial frontend source tree.

Revision 1.1  1995/10/13 15:26:29  joachim
Initial revision

$Revision: 1.1.1.1 $
$Source: /home/rama/timber/CVS/compiler/frontends/toyfront/src/Scope.c,v $ 

*****/


# ifdef __cplusplus
extern "C" {
# endif

# include "Scope.h"

# ifdef __cplusplus
}
# endif



int ScopeCounter = 0;  /* global counter to mark each scope with an unique */
                       /* label. This label is used to obtain unique       */
                       /* identifiers when translating the nested name     */
                       /* space of a Booster program to the flat name space*/
                       /* of a Vnus program                                */
                       /* note: there are no checks which ensure that this */
                       /* counter overflows (beware of large programs :-)  */

tObject  nNoObject;    /* the node nNoObject represents a generic          */
                       /* "constant" that allows the "Identify"            */
                       /* functions to return an identifiable              */
                       /* non-object in case an identifier is not          */
                       /* member of an environment.                        */
                       /* nNoObject is initialized in the "BEGIN" section  */
                       /* of module "Environment".                         */

tEnv EnterScope
# if defined __STDC__ | defined __cplusplus
(tEnv EnvToHide)
# else
(EnvToHide)
tEnv EnvToHide;
# endif
{
  return mNestedScopes(mScope(false, ScopeCounter++, mNoObject()), EnvToHide);
}


tEnv LeaveScope
# if defined __STDC__ | defined __cplusplus
(tEnv NestedEnv)
# else
(NestedEnv)
tEnv NestedEnv;
# endif
{
  if (NestedEnv->Kind == kTopLevel) {
    Message((char *) ErrStr[60], xxFatal, NoPosition);
    return NestedEnv;
  } else {
    return NestedEnv->NestedScopes.Hidden;
  };
}


tEnv AddVarDecl
# if defined __STDC__ | defined __cplusplus
(tIdent Ident, tPosition Pos, tTree Type, tEnv Env)
# else
(Ident, Pos, Type, Env)
tIdent Ident;
tPosition Pos;
tTree Type;
tEnv Env;
# endif
{
  tScope    AuxScope;
  tObject   AuxObject;


  if (IdentifyVarOrConst(Ident, Env) == nNoObject) {
    AuxScope = Env->NestedScopes.OuterScope;
    AuxObject = mVarEntry(AuxScope->Scope.Objects, Ident, namespaceD, Type);
    AuxScope->Scope.Objects = AuxObject;
    return Env;
  } else {
    Message((char *) ErrStr[37], xxError, Pos);
    return Env;
  };
}


tEnv DeclareVariables
# if defined __STDC__ | defined __cplusplus
(tTree Identifiers, tTree Type, tEnv Env)
# else
(Identifiers, Type, Env)
tTree Identifiers;
tTree Type;
tEnv Env;
# endif
{
  tEnv AuxEnv;

  if (Identifiers->Kind == kIdent) {
    return AddVarDecl(Identifiers->Ident.Ident, Identifiers->Ident.Pos, 
             Type, Env);
  } else {
    AuxEnv = AddVarDecl(Identifiers->Idents.Ident, Identifiers->Idents.Pos, 
               Type, Env);
    return DeclareVariables(Identifiers->Idents.Next, Type, Env);
  };
}

tEnv AddConstDecl
# if defined __STDC__ | defined __cplusplus
(tIdent Ident, tPosition Pos, tTree Expression, tEnv Env)
# else
(Ident, Pos, Type, Env)
tIdent Identifier;
tPosition Pos;
tTree Expression;
tEnv Env;
# endif
{

  tScope    AuxScope;
  tObject   AuxObject;


  if (IdentifyVarOrConst(Ident, Env) == nNoObject) {
    AuxScope = Env->NestedScopes.OuterScope;
    AuxObject = mConstEntry(AuxScope->Scope.Objects, Ident, namespaceD,
                  Expression);
    AuxScope->Scope.Objects = AuxObject;
    return Env;
  } else {
    Message((char *) ErrStr[37], xxError, Pos);
    return Env;
  };
}

tEnv AddTypeDecl
# if defined __STDC__ | defined __cplusplus
(tIdent Ident, tPosition Pos, tTree TypeSpec, tEnv Env)
# else
(Ident, Pos, TypeSpec, Env)
tIdent Ident;
tPosition Pos;
tTree TypeSpec;
tEnv Env;
# endif
{
  tScope    AuxScope;
  tObject   AuxObject;


  if (IdentifyType(Ident, Env) == nNoObject) {
    AuxScope = Env->NestedScopes.OuterScope;
    AuxObject = mTypeEntry(AuxScope->Scope.Objects, Ident, namespaceT,
                  TypeSpec);
    AuxScope->Scope.Objects = AuxObject;
    return Env;
  } else {
    Message((char *) ErrStr[37], xxError, Pos);
    return Env;
  };

}

tEnv AddProcDecl
# if defined __STDC__ | defined __cplusplus
(tIdent Ident, tPosition Pos, tTree Formals, tEnv Env)
# else
(Ident, Pos, Formals, Env)
tIdent Ident;
tPosition Pos;
tTree Formals;
tEnv Env;
# endif
{
  tScope    AuxScope;
  tObject   AuxObject;


  if (IdentifyProc(Ident, Env) == nNoObject) {
    AuxScope = Env->NestedScopes.OuterScope;
    AuxObject = mProcEntry(AuxScope->Scope.Objects, Ident, namespaceP,
                  Formals);
    AuxScope->Scope.Objects = AuxObject;
    return Env;
  } else {
    Message((char *) ErrStr[37], xxError, Pos);
    return Env;
  };

}

tEnv AddFuncDecl
# if defined __STDC__ | defined __cplusplus
(tIdent Ident, tPosition Pos, tTree Formals, tTree Result, tEnv Env)
# else
(Identifier, Pos, Formals, Result, Env)
tIdent Ident;
tPosition Pos;
tTree Formals;
tTree Result;
tEnv Env;
# endif
{
  tScope    AuxScope;
  tObject   AuxObject;


  if (IdentifyFunc(Ident, Env) == nNoObject) {
    AuxScope = Env->NestedScopes.OuterScope;
    AuxObject = mFuncEntry(AuxScope->Scope.Objects, Ident, namespaceF,
                  Formals, Result);
    AuxScope->Scope.Objects = AuxObject;
    return Env;
  } else {
    Message((char *) ErrStr[37], xxError, Pos);
    return Env;
  };
}


tObject IdentifyVarOrConst
# if defined __STDC__ | defined __cplusplus
(tIdent Identifier, tEnv Env)
# else
(Identifier, Env)
tIdent Identifier;
tEnv Env;
# endif
{
  tObject Object;

  while (Env->Kind != kTopLevel) {
    Object = Env->NestedScopes.OuterScope->Scope.Objects;
    while (Object->Kind != kNoObject) {
      if ((Object->Object.NameSpace == namespaceD) &&
          (Object->Object.Ident == Identifier)) {
        return Object;
      };
      Object = Object->Object.Next;
    };
    Env = Env->NestedScopes.Hidden;
  };
  return nNoObject;
}


tObject IdentifyProc
# if defined __STDC__ | defined __cplusplus
(tIdent Identifier, tEnv Env)
# else
(Identifier, Env)
tIdent Identifier;
tEnv Env;
# endif
{
  tObject Object;

  while (Env->Kind != kTopLevel) {
    Object = Env->NestedScopes.OuterScope->Scope.Objects;
    while (Object->Kind != kNoObject) {
      if ((Object->Object.NameSpace == namespaceP) &&
          (Object->Object.Ident == Identifier)) {
        return Object;
      };
      Object = Object->Object.Next;
    };
  Env = Env->NestedScopes.Hidden;
  };
  return nNoObject;
}


tObject IdentifyFunc
# if defined __STDC__ | defined __cplusplus
(tIdent Identifier, tEnv Env)
# else
(Identifier, Env)
tIdent Identifier;
tEnv Env;
# endif
{
  tObject Object;

  while (Env->Kind != kTopLevel) {
    Object = Env->NestedScopes.OuterScope->Scope.Objects;
    while (Object->Kind != kNoObject) {
      if ((Object->Object.NameSpace == namespaceF) &&
          (Object->Object.Ident == Identifier)) {
        return Object;
      };
      Object = Object->Object.Next;
    };
  Env = Env->NestedScopes.Hidden;
  };
  return nNoObject;
}


tObject IdentifyType
# if defined __STDC__ | defined __cplusplus
(tIdent Identifier, tEnv Env)
# else
(Identifier, Env)
tIdent Identifier;
tEnv Env;
# endif
{
  tObject Object;

  while (Env->Kind != kTopLevel) {
    Object = Env->NestedScopes.OuterScope->Scope.Objects;
    while (Object->Kind != kNoObject) {
      if ((Object->Object.NameSpace == namespaceT) &&
          (Object->Object.Ident == Identifier)) {
        return Object;
      };
      Object = Object->Object.Next;
    };
  Env = Env->NestedScopes.Hidden;
  };
  return nNoObject;
}
