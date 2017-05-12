#include <string.h>
#include <assert.h>

#include "evaluate.h"
#include "cardinals.h"
#include "tm2dag.h"


int cardinality_evaluate (cardinality e)
{
  return (int) expression_evaluate (e->upperbound);
}

int expression_evaluate (expression e)
{
  switch (e->tag)
  {
    case TAGExprInt:
      return ExprInt_evaluate (to_ExprInt(e));
    case TAGExprBinop:
      return ExprBinop_evaluate (to_ExprBinop(e));
    case TAGExprUnop:
      return ExprUnop_evaluate (to_ExprUnop(e));
    case TAGExprName:
      return ExprName_evaluate (to_ExprName(e));
    default:
      fprintf (stderr, "Wrong expression to be evaluated %d\n", e->tag);
      exit (-1);
  }

  return 0;
}

int ExprBinop_evaluate (ExprBinop e)
{
  switch (e->optor)
  {
    case BINOP_PLUS:
      return expression_evaluate (e->left) +
             expression_evaluate (e->right);
    case BINOP_MINUS:
      return expression_evaluate (e->left) -
             expression_evaluate (e->right);
    case BINOP_TIMES:
      return expression_evaluate (e->left) *
             expression_evaluate (e->right);
    case BINOP_DIVIDE:
      return expression_evaluate (e->left) /
             expression_evaluate (e->right);
    case BINOP_EQUAL:
      return expression_evaluate (e->left) ==
             expression_evaluate (e->right);
    case BINOP_NOTEQUAL:
      return expression_evaluate (e->left) !=
             expression_evaluate (e->right);
      
    case BINOP_LESS:
      return expression_evaluate (e->left) <
             expression_evaluate (e->right);
      
    case BINOP_LESSEQUAL:
      return expression_evaluate (e->left) <=
             expression_evaluate (e->right);
      
    case BINOP_GREATER:
      return expression_evaluate (e->left) >
             expression_evaluate (e->right);
      
    case BINOP_GREATEREQUAL:
      return expression_evaluate (e->left) >=
             expression_evaluate (e->right);
    default:
      fprintf (stderr, "Wrong Binop to be evaluated %d\n", e->optor);
      exit (-1);
  }

  return 0;
}

int ExprUnop_evaluate (ExprUnop e)
{
  switch (e->optor)
  {
    case UNOP_PLUS:
      return expression_evaluate (e->operand);
    case UNOP_NEGATE:
      return -expression_evaluate (e->operand);
    default:
      fprintf (stderr, "Wrong Unop to be evaluated %d\n", e->optor);
      exit (-1);
  }

  return 0;
}

int ExprInt_evaluate (ExprInt e)
{
  return e->v;   /* default value = 0 */
}

int ExprName_evaluate (ExprName e)
{
  return (int) cardinals.get_value (e->name->sym->name);
}


int pragma_expression_evaluate (PragmaExpression e)
{
  switch (e->tag)
  {
    case TAGListPragmaExpression:
    {
      ListPragmaExpression lpe = to_ListPragmaExpression (e);
      if (lpe->l->sz == 1)
        return pragma_expression_evaluate (lpe->l->arr[0]);
      else if ((lpe->l->sz == 3) && 
	       (lpe->l->arr[0]->tag == TAGNamePragmaExpression))
      {
	NamePragmaExpression npe = to_NamePragmaExpression (lpe->l->arr[0]);
	if (strcmp (npe->name->sym->name, "sum") == 0)
	  return pragma_expression_evaluate (lpe->l->arr[1]) + 
                 pragma_expression_evaluate (lpe->l->arr[2]);
	else if (strcmp (npe->name->sym->name, "subtract") == 0)
	  return pragma_expression_evaluate (lpe->l->arr[1]) -
                 pragma_expression_evaluate (lpe->l->arr[2]);
	else if (strcmp (npe->name->sym->name, "multiply") == 0)
	  return pragma_expression_evaluate (lpe->l->arr[1]) *
                 pragma_expression_evaluate (lpe->l->arr[2]);
	else if (strcmp (npe->name->sym->name, "div") == 0)
	  return pragma_expression_evaluate (lpe->l->arr[1]) /
                 pragma_expression_evaluate (lpe->l->arr[2]);
	else if (strcmp (npe->name->sym->name, "eq") == 0)
	  return pragma_expression_evaluate (lpe->l->arr[1]) ==
                 pragma_expression_evaluate (lpe->l->arr[2]);
	else if (strcmp (npe->name->sym->name, "ne") == 0)
	  return pragma_expression_evaluate (lpe->l->arr[1]) !=
                 pragma_expression_evaluate (lpe->l->arr[2]);
	else if (strcmp (npe->name->sym->name, "le") == 0)
	  return pragma_expression_evaluate (lpe->l->arr[1]) <=
                 pragma_expression_evaluate (lpe->l->arr[2]);
	else if (strcmp (npe->name->sym->name, "lt") == 0)
	  return pragma_expression_evaluate (lpe->l->arr[1]) <
                 pragma_expression_evaluate (lpe->l->arr[2]);
	else if (strcmp (npe->name->sym->name, "ge") == 0)
	  return pragma_expression_evaluate (lpe->l->arr[1]) >=
                 pragma_expression_evaluate (lpe->l->arr[2]);
	else if (strcmp (npe->name->sym->name, "ge") == 0)
	  return pragma_expression_evaluate (lpe->l->arr[1]) >
                 pragma_expression_evaluate (lpe->l->arr[2]);
	else
	{
          cout << "Unknown operator: " << npe->name->sym->name << endl;
	  assert (false);
	}
      }
      else
      {
        cout << "Bad expression.\n";
        assert (false);
      }
      break;
    }
    case TAGNumberPragmaExpression:
      return atoi (to_NumberPragmaExpression(e)->v);
    case TAGExternalNamePragmaExpression:
    {
      ExternalNamePragmaExpression enpe = to_ExternalNamePragmaExpression (e);
      return (int) cardinals.get_value (enpe->name->sym->name);
    }
    case TAGNamePragmaExpression:
    {
      NamePragmaExpression npe = to_NamePragmaExpression (e);
      return (int) pragma_cardinals.get_value (npe->name->sym->name);
    }
    default:
      break;
  }
  assert (false);
  return 0;
}
