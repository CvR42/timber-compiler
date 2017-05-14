.. File: tmadmin.t
.. Specification of the required tm functions for backend
.. administration.
..
.rename vnus_string tmstring
..
.set basename tmadmin
.set wantdefs
.set notwantdefs
.append notwantdefs isequal_origsymbol
..
.append wantdefs stat_$(basename)
.append wantdefs get_balance_$(basename)
..append wantdefs flush_$(basename)
..
.append wantdefs new_origin
.append wantdefs rdup_origin
..
.append wantdefs new_tmstring_list
.append wantdefs rfre_tmstring_list
.append wantdefs append_tmstring_list
..
.append wantdefs new_tmsymbol_list
.append wantdefs append_tmsymbol_list
.append wantdefs rfre_tmsymbol_list
.append wantdefs print_tmsymbol_list
..
.append wantdefs new_origsymbol
.append wantdefs new_origsymbol_list
.append wantdefs rdup_origsymbol
.append wantdefs rdup_origsymbol_list
.append wantdefs append_origsymbol_list
.append wantdefs concat_origsymbol_list
.append wantdefs setroom_origsymbol_list
.append wantdefs rfre_origsymbol_list
.append wantdefs rfre_origsymbol
..
.append wantdefs declaration
.append wantdefs ${prefix new_ ${subclasses declaration}}
.append wantdefs declaration_list
.append wantdefs delete_declaration_list
.append wantdefs new_declaration_list
.append wantdefs insertlist_declaration_list
.append wantdefs extractlist_declaration_list
.append wantdefs append_declaration_list
.append wantdefs is_RoutineDeclaration 
..
..
.append wantdefs ${prefix new_ ${subclasses type}}
.append wantdefs new_type_list
.append wantdefs rdup_type
.append wantdefs rdup_type_list
.append wantdefs append_type_list
.append wantdefs fre_type
.append wantdefs rfre_type
.append wantdefs rfre_type_list
.append wantdefs setroom_type_list
..
.append wantdefs new_size_list
.append wantdefs rdup_size_list
.append wantdefs ${prefix new_ ${subclasses size}}
.append wantdefs append_size_list
..
.append wantdefs new_expression_list
.append wantdefs delete_expression_list
.append wantdefs rfre_expression_list
.append wantdefs rdup_expression_list
.append wantdefs append_expression_list
.append wantdefs concat_expression_list
.append wantdefs insertlist_expression_list
.append wantdefs insert_expression_list
.append wantdefs setroom_expression_list
.append wantdefs ${prefix new_ ${subclasses expression}}
.append wantdefs rdup_expression
.append wantdefs rfre_expression
.append wantdefs fre_expression
.append wantdefs print_expression
.append wantdefs isequal_expression
..
.append wantdefs new_cardinality_list
.append wantdefs append_cardinality_list
.append wantdefs new_cardinality
..
.append wantdefs new_secondary_list
.append wantdefs new_secondary
.append wantdefs append_secondary_list
..
.append wantdefs ${prefix new_ ${subclasses location}}
.append wantdefs rfre_location
.append wantdefs fre_location
..
.append wantdefs new_block
.append wantdefs rfre_block
..
.append wantdefs ${prefix new_ ${subclasses statement}}
.append wantdefs append_statement_list
.append wantdefs concat_statement_list
.append wantdefs delete_statement_list
.append wantdefs fre_statement
.append wantdefs rfre_statement
.append wantdefs new_statement_list
.append wantdefs rfre_statement_list
.append wantdefs insertlist_statement_list
..
.append wantdefs print_vnusprog
.append wantdefs fscan_vnusprog
.append wantdefs new_vnusprog
.append wantdefs rfre_vnusprog
..
.append wantdefs new_scopeinfo
.append wantdefs new_scopeinfo_list
.append wantdefs append_scopeinfo_list
.append wantdefs delete_scopeinfo_list
.append wantdefs rfre_scopeinfo_list
..
.append wantdefs new_Pragma
.append wantdefs new_Pragma_list
.append wantdefs rdup_Pragma_list
.append wantdefs concat_Pragma_list
.append wantdefs append_Pragma_list
..
.append wantdefs ${prefix new_ ${subclasses Pragma}}
..
.append wantdefs new_tmuint_list
.append wantdefs rfre_tmuint_list
.append wantdefs append_tmuint_list
..
.append wantdefs optexpression
.append wantdefs ${prefix new_ ${subclasses optexpression}}
.append wantdefs fre_optexpression
..
.append wantdefs new_recordnode
.append wantdefs new_recordnode_list
.append wantdefs rfre_recordnode
.append wantdefs append_recordnode_list
..
.append wantdefs append_field_list
.append wantdefs setroom_field_list
.append wantdefs rfre_field_list
.append wantdefs rdup_field_list
.append wantdefs new_field
.append wantdefs field
.append wantdefs new_field_list
..
.append wantdefs new_switchCase_list
.append wantdefs ${prefix new_ ${subclasses switchCase}}
.append wantdefs append_switchCase_list
..
.append wantdefs new_waitCase_list
.append wantdefs ${prefix new_ ${subclasses waitCase}}
.append wantdefs append_waitCase_list
..
.append wantdefs append_functionnode_list
.append wantdefs new_functionnode
.append wantdefs new_functionnode_list
.append wantdefs rfre_functionnode_list
..
.append wantdefs ${prefix new_ ${subclasses PragmaExpression}}
.append wantdefs new_PragmaExpression_list
.append wantdefs append_PragmaExpression_list
..
.append wantdefs new_OwnerExpr
.append wantdefs new_OwnerExpr_list
.append wantdefs concat_OwnerExpr_list
.append wantdefs rdup_OwnerExpr_list
..
.append wantdefs new_VarState
.append wantdefs new_VarState_list
.append wantdefs rdup_VarState_list
.append wantdefs rdup_VarState
.append wantdefs delete_VarState_list
.append wantdefs append_VarState_list
.append wantdefs rfre_VarState_list
..
.append wantdefs new_ProgramState
.append wantdefs new_ProgramState_list
.append wantdefs extract_ProgramState_list
.append wantdefs delete_ProgramState_list
.append wantdefs append_ProgramState_list
.append wantdefs rfre_ProgramState_list
.append wantdefs rfre_ProgramState
..
.append wantdefs new_AbstractValue
.append wantdefs rdup_AbstractValue
.append wantdefs rfre_AbstractValue
..
.append wantdefs append_FieldVal_list
.append wantdefs delete_FieldVal_list
.append wantdefs new_FieldVal
.append wantdefs new_FieldVal_list
.append wantdefs rdup_FieldVal
.append wantdefs rfre_FieldVal_list
