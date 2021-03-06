.. File: tmadmin.t
.. Specification of the required tm functions for backend
.. administration.
..
.rename vnus_string tmstring
..
.set verbose
.set basename tmadmin
.set wantdefs
..
.append wantdefs stat_$(basename)
.append wantdefs get_balance_$(basename)
.append wantdefs flush_$(basename)
..
.append wantdefs new_origin
.append wantdefs rfre_origin
.append wantdefs rdup_origin
..
.append wantdefs new_tmstring_list
.append wantdefs rfre_tmstring_list
.append wantdefs append_tmstring_list
..
.append wantdefs new_tmsymbol_list
.append wantdefs setroom_tmsymbol_list
.append wantdefs append_tmsymbol_list
.append wantdefs rfre_tmsymbol_list
.append wantdefs rdup_tmsymbol_list
.append wantdefs print_tmsymbol_list
..
.append wantdefs new_origsymbol
.append wantdefs new_origsymbol_list
.append wantdefs rdup_origsymbol
.append wantdefs rdup_origsymbol_list
.append wantdefs append_origsymbol_list
.append wantdefs concat_origsymbol_list
.append wantdefs setroom_origsymbol_list
.append wantdefs delete_origsymbol_list
.append wantdefs rfre_origsymbol_list
.append wantdefs rfre_origsymbol
..
.append wantdefs declaration
.append wantdefs rdup_declaration
.append wantdefs rfre_declaration
.append wantdefs ${prefix new_ ${subclasses declaration}}
.append wantdefs declaration_list
.append wantdefs concat_declaration_list
.append wantdefs new_declaration_list
.append wantdefs delete_declaration_list
.append wantdefs rfre_declaration_list
.append wantdefs rdup_declaration_list
.append wantdefs insertlist_declaration_list
.append wantdefs append_declaration_list
.append wantdefs is_RoutineDeclaration 
..
..
.append wantdefs ${prefix new_ ${subclasses type}}
.append wantdefs new_type_list
.append wantdefs setroom_type_list
.append wantdefs rdup_type
.append wantdefs rdup_type_list
.append wantdefs append_type_list
.append wantdefs fre_type
.append wantdefs rfre_type
.append wantdefs rfre_type_list
..
.append wantdefs new_size_list
.append wantdefs ${prefix new_ ${subclasses size}}
.append wantdefs append_size_list
.append wantdefs setroom_size_list
.append wantdefs rdup_size_list
..
.append wantdefs new_expression_list
.append wantdefs rfre_expression_list
.append wantdefs rdup_expression_list
.append wantdefs append_expression_list
.append wantdefs insert_expression_list
.append wantdefs setroom_expression_list
.append wantdefs delete_expression_list
.append wantdefs ${prefix new_ ${subclasses expression}}
.append wantdefs rdup_expression
.append wantdefs rfre_expression
.append wantdefs fre_expression
..
.append wantdefs new_cardinality_list
.append wantdefs append_cardinality_list
.append wantdefs slice_cardinality_list
.append wantdefs rfre_cardinality_list
.append wantdefs new_cardinality
.append wantdefs rdup_cardinality
.append wantdefs rdup_cardinality_list
.append wantdefs delete_cardinality_list
.append wantdefs rfre_cardinality
.append wantdefs concat_cardinality_list
..
.append wantdefs ${prefix new_ ${subclasses location}}
.append wantdefs rfre_location
.append wantdefs rdup_location
.append wantdefs fre_location
..
.append wantdefs new_block
.append wantdefs rdup_block
.append wantdefs new_block_list
.append wantdefs append_block_list
.append wantdefs rfre_block
..
.append wantdefs ${prefix new_ ${subclasses statement}}
.append wantdefs new_statement_list
.append wantdefs append_statement_list
.append wantdefs concat_statement_list
.append wantdefs insert_statement_list
.append wantdefs delete_statement_list
.append wantdefs rfre_statement_list
.append wantdefs insertlist_statement_list
.append wantdefs rdup_statement_list
.append wantdefs rdup_statement
.append wantdefs rfre_statement
..
.append wantdefs print_vnusprog
.append wantdefs fscan_vnusprog
.append wantdefs new_vnusprog
.append wantdefs rfre_vnusprog
..
.append wantdefs new_Pragma
.append wantdefs new_Pragma_list
.append wantdefs rdup_Pragma_list
.append wantdefs rfre_Pragma_list
.append wantdefs concat_Pragma_list
.append wantdefs append_Pragma_list
.append wantdefs delete_Pragma_list
.append wantdefs rdup_FlagPragma
..
.append wantdefs ${prefix new_ ${subclasses Pragma}}
.append wantdefs ${prefix new_ ${subclasses PragmaExpression}}
..
.append wantdefs new_tmuint_list
.append wantdefs rfre_tmuint_list
.append wantdefs append_tmuint_list
..
.append wantdefs optexpression
.append wantdefs ${prefix new_ ${subclasses optexpression}}
.append wantdefs rfre_optexpression
.append wantdefs fre_optexpression
..
.append wantdefs append_field_list
.append wantdefs setroom_field_list
.append wantdefs rfre_field_list
.append wantdefs rdup_field_list
.append wantdefs new_field
.append wantdefs new_field_list
.append wantdefs concat_field_list
.append wantdefs new_switchCase_list
.append wantdefs ${prefix new_ ${subclasses switchCase}}
.append wantdefs append_switchCase_list
..
.append wantdefs fre_SmtFor
.append wantdefs rfre_ExprName
.append wantdefs rfre_LocName
.append wantdefs rfre_SmtAssign
.append wantdefs fre_DeclGlobalVariable
.append wantdefs rdup_SmtForall
.append wantdefs rfre_SmtForall
.append wantdefs new_SmtForall
.append wantdefs fre_SmtForall
.append wantdefs fre_SmtForeach
.append wantdefs rfre_SmtForeach
.append wantdefs rdup_SmtForeach
..
.append wantdefs new_OwnerExpr
.append wantdefs rdup_OwnerExpr
.append wantdefs new_OwnerExpr_list
.append wantdefs rdup_OwnerExpr_list
.append wantdefs append_OwnerExpr_list
.append wantdefs rfre_OwnerExpr_list
.append wantdefs concat_OwnerExpr_list
.append wantdefs delete_OwnerExpr_list
.append wantdefs new_NewDistAlign
.append wantdefs new_DistrExpr_list
.append wantdefs append_DistrExpr_list
.append wantdefs rfre_TypeMap
.append wantdefs new_secondary_list
.append wantdefs insert_secondary_list
.append wantdefs rdup_secondary_list
.append wantdefs new_secondary
.append wantdefs rfre_SmtIf
.append wantdefs new_NewOwnLocation

.set extradefs
.append wantextradefs snew_block
.append wantextradefs snew_SmtAssign
.append wantextradefs snew_SmtBlock
.append wantextradefs snew_SmtDelete
.append wantextradefs snew_SmtFor
.append wantextradefs snew_SmtForeach
.append wantextradefs snew_SmtIf
.append wantextradefs snew_SmtSend
.append wantextradefs snew_SmtReceive
.append wantextradefs snew_SmtBlocksend
.append wantextradefs snew_SmtBlockreceive
.append wantextradefs snew_SmtWaitPending
.append wantextradefs snew_SmtAggrSend
.append wantextradefs snew_SmtAggrReceive
.append wantextradefs snew_SmtAddTask
.append wantextradefs snew_SmtRegisterTask
.append wantextradefs snew_SmtExecuteTasks
.append wantextradefs snew_SmtTSend
.append wantextradefs snew_SmtTReceive
.append wantextradefs snew_SmtProcedureCall
