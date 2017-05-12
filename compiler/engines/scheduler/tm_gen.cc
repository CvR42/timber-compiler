#include <tmc.h>
#include "vnusbasetypes.h"
#include "tmadmin.h"
#include <vnusctl.h>

#include "scheduler/Scheduling.H"
#include "dag/NoDupDAGNode.H"
#include "tm_gen.h"

void
tm_gen (vnusprog the_prog)
{
  ExprArray arr;
  expression_list elms;

  // Generate the shape containing the task priorities
  ExprShape task_prio_shape;

  elms = new_expression_list();
  for (int nid = 0; nid < dag->GetNrNodes(); nid++)
  {
      append_expression_list (elms, new_ExprInt (((PNoDupDAGNode) dag->GetNode(nid))->GetSeqNrOnProc()));
  }
  arr = new_ExprArray (
      new_TypeBase(BT_INT),
      elms);

  task_prio_shape = new_ExprShape (
    append_size_list (new_size_list(),
		      new_SizeExpression(new_ExprInt(arr->elms->sz))),
    new_TypeBase(BT_INT),
    arr, arr->elms->sz);

  // Add a new declaration for the one-dimentional task-priorities shape
  DeclGlobalVariable v = new_DeclGlobalVariable (
    gen_origsymbol ("task_prio"), 0, new_Pragma_list(),
    new_TypeShape (
      append_size_list (new_size_list(), 
			new_SizeExpression(new_ExprInt(arr->elms->sz))),
      new_TypeBase(BT_INT)),
    new_OptExpr (task_prio_shape));
  // Add an identification pragma to the variable declaration
  v->pragmas = append_Pragma_list (
    v->pragmas,
    new_FlagPragma (add_origsymbol("task_prio")));

  the_prog->declarations = append_declaration_list (the_prog->declarations, v);


  // Generate the shape containing the task mappings
  ExprShape task_proc_shape;

  elms = new_expression_list();
  for (int nid = 0; nid < dag->GetNrNodes(); nid++)
  {
    append_expression_list (elms, new_ExprInt (((PNoDupDAGNode) dag->GetNode(nid))->GetProcID()));
  }
  arr = new_ExprArray (
      new_TypeBase(BT_INT),
      elms);

  task_proc_shape = new_ExprShape (
    append_size_list (new_size_list(), 
		      new_SizeExpression(new_ExprInt(arr->elms->sz))),
    new_TypeBase(BT_INT),
    arr, arr->elms->sz);

  // Add a new declaration for the one-dimentional task-mappings shape
  v = new_DeclGlobalVariable (
    gen_origsymbol ("task_proc"), 0, new_Pragma_list(),
    new_TypeShape (
      append_size_list (new_size_list(), 
			new_SizeExpression(new_ExprInt(arr->elms->sz))),
      new_TypeBase(BT_INT)),
    new_OptExpr (task_proc_shape));
  // Add an identification pragma to the variable declaration
  v->pragmas = append_Pragma_list (
    v->pragmas,
    new_FlagPragma (add_origsymbol("task_proc")));

  the_prog->declarations = append_declaration_list (the_prog->declarations, v);
}

