/*****

$Author: leo $
$Date: 1995/10/17 13:33:59 $
$Log: ErrorStr.c,v $
Revision 1.1.1.1  1995/10/17 13:33:59  leo
Initial frontend source tree.

Revision 0.1  1995/06/23  13:05:29  joachim
initial revision derived from arnauds frontend

$Revision: 1.1.1.1 $
$Source: /home/rama/timber/CVS/compiler/frontends/toyfront/src/ErrorStr.c,v $ 

*****/

# include "ErrorStr.h"

char ErrStr[100][60] =  {"Undefined type of view",
/* 1*/                  "Invalid argument",
/* 2*/                  "Illegal type definition", 
/* 3*/                  ": Illegal designator",
/* 4*/                  ": Illegal selectorelement",
/* 5*/                  "Wrong type",
/* 6*/                  "Illegal selectorlist",
/* 7*/                  ": Not a record",
/* 8*/                  "Can't read expression",
/* 9*/                  "Illegal parameterlist",
/*10*/                  "Illegal expression",
/*11*/                  ": Illegal identifier",
/*12*/                  ": Can't write variable",
/*13*/                  "Illegal operator",
/*14*/                  "Undefined type",
/*15*/                  "Empty expressionlist",
/*16*/                  ": No recordfield of that name",
/*17*/                  ": Illegal recordtype definition",
/*18*/                  "Undefined (In-Out)-type of expression",
/*19*/                  "Mixed (In-Out)-type of expression",
/*20*/                  ": Undeclared function",
/*21*/                  ": Undeclared procedure",
/*22*/                  "Functioncall does not have enough parameters",
/*23*/                  "Functioncall has too many parameters",
/*24*/                  ": Undeclared constant",
/*25*/                  "Selectorlist too long",
/*26*/                  "Selectorelement less than zero",
/*27*/                  "Selectorelement too large",
/*28*/                  "Setexpression exceeds cardinality",
/*29*/                  ": Function not of Shape- or ViewType",
/*30*/                  "Operator not allowed on Out-type",
/*31*/                  "illegal character",
/*32*/                  "unexpected character EOF while scanning comment",
/*33*/                  "unexpected character EOF while scanning string",
/*34*/                  "Undefined escape-character",
/*35*/                  "Illegal newline",
/*36*/                  ": Wrong identifier",
/*37*/                  ": Previously declared identifier",
/*38*/                  ": Predeclared identifier",
/*39*/                  ": Can't read variable",
/*40*/                  ": Undefined identifier",
/*41*/                  "Incompatible types",
/*42*/                  "Can't write expression",
/*43*/                  ": Type of variable undefined",
/*44*/                  ": No parameters defined",
/*45*/                  ": Declaration doesn't match earlier implied declaration",
/*46*/                  ": Type of function undefined",
/*47*/                  "Illegal type",
/*48*/                  ": Illegal use of view",
/*49*/                  "Procedurecall does not have enough parameters",
/*50*/                  "Procedurecall has too many parameters",
/*51*/                  ": Illegal constant expression",
/*52*/                  "Too many identifiers",
/*53*/                  ": Modulename doesn't match filename",
/*54*/                  ": Module not found",
/*55*/                  "Expressionlist too long",
/*56*/                  ": Cyclic import definition",
/*57*/                  "Illegal cardinality specification",
/*58*/                  "Wrong value in dimension specification" ,
/*59*/                  ": Not a view"
/*60*/                  "Attempt to leave outer scope (compiler error!)"
/*61*/                  "Compiler restriction: Scoping not implemented for imports"
};

