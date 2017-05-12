#ifndef __PARSE_PRAGMAS_H__
#define __PARSE_PRAGMAS_H__

extern void parse_global_pragmas(global_context gc);
extern TaskId parse_task_pragma(Pragma p);
extern int parse_taskid_pragma(Pragma p);
extern Arc_list parse_arc_pragmas(Pragma_list pl, tmsymbol key);
extern expression parse_expression(PragmaExpression x);

extern Pragma_list extract_simple_on_pragmas(Pragma_list p_l, OwnerExpr_list *o_l);
extern Pragma_list extract_abstract_on_pragmas(Pragma_list p_l, OwnerExpr_list *o_l);

#endif
