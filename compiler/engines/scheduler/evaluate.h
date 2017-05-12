#ifndef _a_EVALUATE_H
#define _a_EVALUATE_H

#include <tmc.h>
#include "vnusbasetypes.h"
#include "tmadmin.h"


int cardinality_evaluate (cardinality e);

int expression_evaluate (expression e);
int ExprInt_evaluate (ExprInt e);
int ExprBinop_evaluate (ExprBinop e);
int ExprUnop_evaluate (ExprUnop e);
int ExprName_evaluate (ExprName e);

int pragma_expression_evaluate (PragmaExpression e);

#endif
