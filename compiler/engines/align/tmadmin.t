.. File: tmadmin.t
.. Specification of the required tm functions for backend
.. administration.
..
.rename vnus_string tmstring
..
.set verbose
.set basename tmadmin
.set wantdefs
.set notwantdefs
.append notwantdefs isequal_origsymbol
..
.append wantdefs stat_$(basename)
.append wantdefs get_balance_$(basename)
.append wantdefs flush_$(basename)
..
.append wantdefs new_origin
.append wantdefs rfre_origin
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
.append wantdefs fre_tmsymbol_list
..
.append wantdefs new_origsymbol
.append wantdefs new_origsymbol_list
.append wantdefs rdup_origsymbol
.append wantdefs rdup_origsymbol_list
.append wantdefs append_origsymbol_list
.append wantdefs concat_origsymbol_list
.append wantdefs delete_origsymbol_list
.append wantdefs setroom_origsymbol_list
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
.append wantdefs fre_expression_list
.append wantdefs rdup_expression_list
.append wantdefs append_expression_list
.append wantdefs setroom_expression_list
.append wantdefs ${prefix new_ ${subclasses expression}}
.append wantdefs rdup_expression
.append wantdefs rfre_expression
.append wantdefs fre_expression
.append wantdefs isequal_expression
..
.append wantdefs new_cardinality_list
.append wantdefs append_cardinality_list
.append wantdefs slice_cardinality_list
.append wantdefs rfre_cardinality_list
.append wantdefs new_cardinality
.append wantdefs rdup_cardinality
.append wantdefs rfre_cardinality
.append wantdefs delete_cardinality_list
.append wantdefs rdup_cardinality_list
..
.append wantdefs ${prefix new_ ${subclasses location}}
.append wantdefs rfre_location
.append wantdefs rdup_location
.append wantdefs fre_location
.append wantdefs new_location_list
.append wantdefs append_location_list
.append wantdefs fre_location_list
..
.append wantdefs new_block
.append wantdefs new_block_list
.append wantdefs append_block_list
..
.append wantdefs ${prefix new_ ${subclasses statement}}
.append wantdefs new_statement_list
.append wantdefs append_statement_list
.append wantdefs concat_statement_list
.append wantdefs insert_statement_list
.append wantdefs rfre_statement_list
.append wantdefs insertlist_statement_list
.append wantdefs rdup_statement_list
..
.append wantdefs print_vnusprog
.append wantdefs fscan_vnusprog
.append wantdefs fscan_declaration_list
.append wantdefs new_vnusprog
.append wantdefs rfre_vnusprog
..
.append wantdefs rfre_TypePragmas
.append wantdefs new_Pragma
.append wantdefs rfre_Pragma
.append wantdefs new_Pragma_list
.append wantdefs rdup_Pragma_list
.append wantdefs rfre_Pragma_list
.append wantdefs fre_Pragma_list
.append wantdefs concat_Pragma_list
.append wantdefs append_Pragma_list
.append wantdefs delete_Pragma_list
.append wantdefs rdup_FlagPragma
..
.append wantdefs ${prefix new_ ${subclasses Pragma}}
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
..
.append wantdefs new_DistrExpr
.append wantdefs isequal_DistrExpr
.append wantdefs rdup_DistrExpr
.append wantdefs rfre_DistrExpr
.append wantdefs rfre_DistrExpr_list
.append wantdefs new_DistrExpr_list
.append wantdefs rdup_DistrExpr_list
.append wantdefs append_DistrExpr_list
..
.append wantdefs rdup_OwnerExpr
.append wantdefs print_OwnerExpr
.append wantdefs rfre_OwnerExpr
.append wantdefs new_OwnerExpr_list
.append wantdefs rdup_OwnerExpr_list
.append wantdefs fre_OwnerExpr_list
.append wantdefs rfre_OwnerExpr_list
.append wantdefs append_OwnerExpr_list
.append wantdefs extract_OwnerExpr_list
.append wantdefs concat_OwnerExpr_list
.append wantdefs delete_OwnerExpr_list
..
.append wantdefs rfre_ExprWrapper
.append wantdefs rfre_LocWrapper
..
.append wantdefs ${prefix new_ ${subclasses DistrExpr}}
.append wantdefs ${prefix new_ ${subclasses OwnerExpr}}
..
.append wantdefs new_VariableOwner
.append wantdefs fre_VariableOwner
.append wantdefs print_VariableOwner
.append wantdefs new_VariableOwner_list
.append wantdefs fre_VariableOwner_list
.append wantdefs rdup_VariableOwner_list
.append wantdefs concat_VariableOwner_list
.append wantdefs append_VariableOwner_list
..
.append wantdefs rdup_secondary_list
.append wantdefs insert_secondary_list
.append wantdefs new_secondary
.append wantdefs rfre_ProcessorType_list
.append wantdefs rfre_ProcessorDecl_list
