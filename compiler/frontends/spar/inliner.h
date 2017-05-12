// File: inliner.h

extern Block inline_method(
 Entry_list symtab,
 const_MethodEntry e,
 const_origin org,
 const_expression_list this_parms,
 const_expression_list parms,
 tmsymbol retvar
);
