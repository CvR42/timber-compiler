/* File: replace.h */

extern statement replace_statement(
    statement elm,
    tmsymbol sym, expression r_e, location r_l,
    global_context gc );
extern expression replace_expression(
    expression elm,
    tmsymbol sym, expression r_e, location r_l,
    global_context gc );
extern location replace_location(
    location elm,
    tmsymbol sym, expression r_e, location r_l,
    global_context gc );
extern declaration replace_declaration(
    declaration elm,
    tmsymbol sym, expression r_e, location r_l,
    global_context gc );
extern cardinality_list replace_cardinality_list(
    cardinality_list elm,
    tmsymbol sym, expression r_e, location r_l,
    global_context gc );
extern OwnerExpr replace_OwnerExpr(
    OwnerExpr elm,
    tmsymbol sym, expression r_e, location r_l,
    global_context gc );
extern vnusprog replace_vnusprog(
    vnusprog elm,
    tmsymbol sym, expression r_e, location r_l,
    global_context gc );
extern expression replace_expression_hack(
    expression e, tmsymbol_list findSym_l, expression_list replaceExpr_l);
