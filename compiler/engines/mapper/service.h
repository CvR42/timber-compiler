#ifndef __MAPPER_SERVICE_H__
#define __MAPPER_SERVICE_H__

struct GlobalContextStruct
{
        // global info
    vnusprog prog;
	// stuff for new style annotations
    ProcessorType_list processorTypes;
    ProcessorDecl_list processors;
    int numberOfProcessors;
        // things to be administrated
    statement_list prelst;
    statement_list postlst;
    tmsymbol scope;
    origsymbol_list current_parms;
    SmtAssign current_ass;
    OwnerExpr_list current_owner;
    Pragma_list current_pragmas;
    VariableOwner current_varowner;
        // booleans to limit transformations
    bool add_to_prelst;
    bool do_statements;
    bool found_distribution_use;
    bool is_indirection;
    bool do_transform;
        // stuff for the search engine
    tmsymbol_list findSym_l;
    expression_list replaceExpr_l;
    location_list replaceLoc_l;
    origsymbol_list current_indices;
    origsymbol_list visited;
	// stuff for the taskpar engine
    DeclLocalVariable current_cs_f_ir;
    DeclRecord current_tir;
};
typedef GlobalContextStruct *global_context, global_context_decl;


extern void init_global_context(global_context);
extern void destroy_global_context(global_context);

extern int pragma_list_find_index_tmsymbol(Pragma_list, tmsymbol);
extern int pragma_list_contains_tmsymbol(Pragma_list, tmsymbol);
extern declaration declaration_list_find_origsymbol(declaration_list, origsymbol);
extern int declaration_list_find_pos(declaration_list, origsymbol);
extern declaration declaration_list_find_pragma(declaration_list, tmsymbol);
extern field field_list_find_origsymbol(field_list, origsymbol);

extern int find_field( const declaration_list decls, const type rectype, const origsymbol field );
extern location find_owneron_name ( const declaration_list decls, origsymbol n );
extern location find_owneron_location ( const declaration_list decls, location l );

extern void mark_declaration_list(declaration_list, unsigned);
extern declaration_list delete_marked_declaration_list(declaration_list);

extern int isIntValue(expression, const int);
extern int isBooleanValue(expression, vnus_boolean b);

extern type pierceTypeMap(type t);
extern type pierceTypePragmas(type t);
extern type pierceTypeMapAndPragmas(type t);
extern location pierceLocationWrapper(location l);
extern expression pierceExprWrapper(expression e);
extern expression pierceExprNotNullAssert(expression e);
extern int isReplicated(type t, global_context gc);
extern int hasDistribution(type t, global_context gc);
extern bool containsReplicated(OwnerExpr_list ol);
extern bool containsReplicated(OwnerExpr o);

extern cardinality_list normalise_cardinalities(cardinality_list l, global_context gc);


extern int determine_lineair(expression AIBexpr, origsymbol loopVar, expression *A, expression *B);
//extern LocSelection extract_LocSelection(expression cond, expression *procExpr);
extern expression normalise_ExprBinop(expression e, BINOP op);
extern ExprReduction split_ExprBinop(expression e, BINOP op);
extern expression form_ExprBinop(ExprReduction er);

//extern distribution_list extract_distribution(global_context gc, expression shape);
extern int count_real_distributions(DistrExpr_list distr);
extern int count_local_distributions(DistrExpr_list distr);
extern int count_iffy_distributions(DistrExpr_list distr);
extern void report_distributions(DistrExpr_list distr);
extern void extract_lineair(expression AIBexpr, cardinality card, expression *A, expression *B);

extern expression replace_expression_indices(global_context gc, expression e, cardinality_list old_l, cardinality_list new_l);
extern location replace_location_indices(global_context gc, location e, cardinality_list old_l, cardinality_list new_l);
extern cardinality_list replace_cardinality_list_indices(global_context gc, cardinality_list e, cardinality_list old_l, cardinality_list new_l);
extern OwnerExpr replace_OwnerExpr_indices(global_context gc, OwnerExpr e, cardinality_list old_l, cardinality_list new_l);
extern statement replace_statement_indices(global_context gc, statement e, cardinality_list old_l, cardinality_list new_l);
extern cardinality_list unique_rdup_cardinality_list(global_context gc, cardinality_list l);
extern void add_cardinality_declarations(global_context gc, cardinality_list l);
extern tmstring int2tmstring(const int i);
extern expression build_numberOfProcessors_expression(global_context gc);
//extern expression build_commonBlock_expression(global_context gc);
extern expression build_currentProcessor_expression(global_context gc);

extern bool stmtHasOwner(statement s);
extern bool is2owned(SmtAssign sa);
extern bool lhsHasOwner(SmtAssign sa);
extern bool rhsHasOwner(SmtAssign sa);

extern ArrayProcDecl get_processor_decl(global_context gc);
extern expression_list build_procList(expression proc, OwnerExpr on, global_context gc);

extern bool isBasicType(expression e, global_context gc);

#endif
