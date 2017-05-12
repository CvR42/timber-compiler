.. File: tmadmin.t
.. Specification of the required tm functions for backend
.. administration.
..
.rename vnus_string tmstring
..
.set basename tmadmin
..
.set notwantdefs cmp_origin
.set notwantdefs isequal_origin
..
.set wantdefs
..
.append wantdefs stat_$(basename)
.append wantdefs get_balance_$(basename)
..
.append wantdefs new_origin
.append wantdefs rdup_origin
.append wantdefs rfre_origin
..
.append wantdefs append_varid_list
.append wantdefs fre_varid
.append wantdefs new_varid
.append wantdefs new_varid_list
.append wantdefs extract_varid_list
.append wantdefs rfre_varid_list
..
.append wantdefs append_origsymbol_list
.append wantdefs concat_origsymbol_list
.append wantdefs isequal_origsymbol
.append wantdefs new_origsymbol
.append wantdefs new_origsymbol_list
.append wantdefs rdup_origsymbol
.append wantdefs rdup_origsymbol_list
.append wantdefs rfre_origsymbol
.append wantdefs rfre_origsymbol_list
.append wantdefs setroom_origsymbol_list
..
.append wantdefs append_tmsymbol_list
.append wantdefs concat_tmsymbol_list
.append wantdefs delete_tmsymbol_list
.append wantdefs deletelist_tmsymbol_list
.append wantdefs new_tmsymbol_list
.append wantdefs rdup_tmsymbol_list
.append wantdefs rfre_tmsymbol_list
.append wantdefs setroom_tmsymbol_list
..
.append wantdefs append_FormalParameter_list
.append wantdefs isequal_FormalParameter_list
.append wantdefs new_FormalParameter
.append wantdefs new_FormalParameter_list
.append wantdefs rdup_FormalParameter_list
.append wantdefs rfre_FormalParameter_list
..
.append wantdefs new_HiddenParameter
.append wantdefs new_HiddenParameter_list
.append wantdefs rfre_HiddenParameter_list
.append wantdefs rdup_HiddenParameter_list
.append wantdefs append_HiddenParameter_list
.append wantdefs setroom_HiddenParameter_list
..
.append wantdefs Entry
.append wantdefs rdup_Entry
.append wantdefs print_Entry
.append wantdefs Entry_list
.append wantdefs new_Entry_list
.append wantdefs append_Entry_list
.append wantdefs delete_Entry_list
.append wantdefs ${prefix new_ ${nonvirtual ${subclasses Entry}}}
.append wantdefs is_VariableEntry
.append wantdefs is_MethodEntry
..
.append wantdefs ${prefix new_ ${nonvirtual ${subclasses type}}}
.append wantdefs rfre_type
.append wantdefs rdup_type
.append wantdefs isequal_type
.append wantdefs new_type_list
.append wantdefs append_type_list
.append wantdefs setroom_type_list
.append wantdefs rdup_type_list
.append wantdefs rfre_type_list
.append wantdefs is_ReferenceType
..
.append wantdefs ${prefix new_ ${nonvirtual ${subclasses expression}}}
.append wantdefs append_expression_list
.append wantdefs fre_expression
.append wantdefs is_LiteralExpression
.append wantdefs is_CreationExpression
.append wantdefs isequal_expression
.append wantdefs isequal_expression_list
.append wantdefs new_expression_list
.append wantdefs deletelist_expression_list
.append wantdefs print_expression
.append wantdefs rdup_expression
.append wantdefs rdup_expression_list
.append wantdefs rfre_expression
.append wantdefs rfre_expression_list
.append wantdefs setroom_expression_list
..
.append wantdefs append_Cardinality_list
.append wantdefs delete_Cardinality_list
.append wantdefs insertlist_Cardinality_list
.append wantdefs new_Cardinality
.append wantdefs new_Cardinality_list
.append wantdefs rfre_Cardinality_list
.append wantdefs slice_Cardinality_list
..
.append wantdefs ${prefix new_ ${nonvirtual ${subclasses statement}}}
.append wantdefs append_statement_list
.append wantdefs concat_statement_list
.append wantdefs delete_statement_list
.append wantdefs extractlist_statement_list
.append wantdefs fre_statement
.append wantdefs fre_statement_list
.append wantdefs insert_statement_list
.append wantdefs insertlist_statement_list
.append wantdefs is_MethodDeclaration
.append wantdefs is_TerminationStatement
.append wantdefs is_TypeDeclaration
.append wantdefs is_VariableDeclaration
.append wantdefs is_declaration
.append wantdefs new_statement_list
.append wantdefs print_statement
.append wantdefs rdup_ClassDeclaration
.append wantdefs rdup_TypeDeclaration
.append wantdefs rdup_statement_list
.append wantdefs rfre_statement
.append wantdefs rfre_statement_list
.append wantdefs setroom_statement_list
..
.append wantdefs print_SparProgram
.append wantdefs new_SparProgram
.append wantdefs rfre_SparProgram
..
.append wantdefs new_SparProgramUnit
.append wantdefs print_SparProgramUnit
.append wantdefs new_SparProgramUnit_list
.append wantdefs append_SparProgramUnit_list
..
.append wantdefs new_VarTranslation
.append wantdefs new_VarTranslation_list
.append wantdefs append_VarTranslation_list
.append wantdefs delete_VarTranslation_list
.append wantdefs rfre_VarTranslation_list
..
.append wantdefs new_VarMapping
.append wantdefs new_VarMapping_list
.append wantdefs append_VarMapping_list
.append wantdefs delete_VarMapping_list
.append wantdefs rfre_VarMapping_list
..
.append wantdefs append_NameTranslation_list
.append wantdefs new_NameTranslation
.append wantdefs new_NameTranslation_list
.append wantdefs rfre_NameTranslation_list
.append wantdefs setroom_NameTranslation_list
..
.append wantdefs append_Field_list
.append wantdefs setroom_Field_list
.append wantdefs new_Field
.append wantdefs new_Field_list
.append wantdefs rdup_Field
.append wantdefs rfre_Field_list
.append wantdefs rfre_Field
..
.append wantdefs append_Signature_list
.append wantdefs setroom_Signature_list
.append wantdefs new_Signature
.append wantdefs isequal_Signature
.append wantdefs new_Signature_list
.append wantdefs rdup_Signature
.append wantdefs rfre_Signature
.append wantdefs rfre_Signature_list
..
.append wantdefs new_Block
.append wantdefs rdup_Block
.append wantdefs rfre_Block
.append wantdefs print_Block
..
.append wantdefs new_Pragma_list
.append wantdefs append_Pragma_list
.append wantdefs rdup_Pragma_list
.append wantdefs rfre_Pragma_list
.append wantdefs delete_Pragma_list
.append wantdefs insert_Pragma_list
.append wantdefs concat_Pragma_list
..
.append wantdefs ${prefix new_ ${nonvirtual ${subclasses Pragma}}}
..
.append wantdefs ${prefix new_ ${nonvirtual ${subclasses optexpression}}}
.append wantdefs rdup_optexpression
.append wantdefs fre_optexpression
.append wantdefs rfre_optexpression
..
.append wantdefs new_SwitchCase_list
.append wantdefs ${prefix new_ ${nonvirtual ${subclasses SwitchCase}}}
.append wantdefs append_SwitchCase_list
..
.append wantdefs new_WaitCase_list
.append wantdefs ${prefix new_ ${nonvirtual ${subclasses WaitCase}}}
.append wantdefs append_WaitCase_list
..
.append wantdefs new_MethodHeader
.append wantdefs fre_MethodHeader
..
.append wantdefs new_Catch
.append wantdefs new_Catch_list
.append wantdefs append_Catch_list
..
.append wantdefs new_Import_list
.append wantdefs rfre_Import_list
.append wantdefs append_Import_list
.append wantdefs ${prefix new_ ${nonvirtual ${subclasses Import}}}
..
.append wantdefs append_FieldMapping_list
.append wantdefs concat_FieldMapping_list
.append wantdefs new_FieldMapping
.append wantdefs new_FieldMapping_list
..
.append wantdefs append_MethodMapping_list
.append wantdefs concat_MethodMapping_list
.append wantdefs new_MethodMapping
.append wantdefs rdup_MethodMapping
.append wantdefs new_MethodMapping_list
.append wantdefs rfre_MethodMapping_list
.append wantdefs delete_MethodMapping_list
..
.append wantdefs new_tmstring_list
.append wantdefs rfre_tmstring_list
.append wantdefs append_tmstring_list
.append wantdefs insert_tmstring_list
.append wantdefs concat_tmstring_list
..
.append wantdefs append_TypeBinding_list
.append wantdefs concat_TypeBinding_list
.append wantdefs new_TypeBinding
.append wantdefs new_TypeBinding_list
.append wantdefs rdup_TypeBinding_list
.append wantdefs rfre_TypeBinding_list
.append wantdefs print_TypeBinding_list
..
.append wantdefs ${prefix new_ ${nonvirtual ${subclasses TypeEntry}}}
.append wantdefs new_TypeEntry_list
.append wantdefs append_TypeEntry_list
.append wantdefs delete_TypeEntry_list
.append wantdefs print_TypeEntry_list
.append wantdefs is_ConcreteTypeEntry
.append wantdefs is_ObjectTypeEntry
..
.append wantdefs new_tmuint_list
.append wantdefs append_tmuint_list
.append wantdefs rfre_tmuint_list
.append wantdefs setroom_tmuint_list
..
.append wantdefs VtableEntry
.append wantdefs new_VtableEntry
.append wantdefs VtableEntry_list
.append wantdefs new_VtableEntry_list
.append wantdefs append_VtableEntry_list
.append wantdefs rfre_VtableEntry_list
..
.append wantdefs new_FormalDim_list
.append wantdefs new_FormalDim
.append wantdefs append_FormalDim_list
.append wantdefs rfre_FormalDim_list
.append wantdefs fre_FormalDim
.append wantdefs rdup_FormalDim
..
.append wantdefs new_ActualDim
.append wantdefs new_ActualDim_list
.append wantdefs append_ActualDim_list
.append wantdefs new_ActualDim_list
.append wantdefs rfre_ActualDim_list
.append wantdefs rdup_ActualDim_list
.append wantdefs append_ActualDim_list
.append wantdefs extractlist_ActualDim_list
..
.append wantdefs ${prefix new_ ${nonvirtual ${subclasses PragmaExpression}}}
.append wantdefs rfre_PragmaExpression
.append wantdefs new_PragmaExpression_list
.append wantdefs append_PragmaExpression_list
.append wantdefs concat_PragmaExpression_list
.append wantdefs insert_PragmaExpression_list
.append wantdefs delete_PragmaExpression_list
.append wantdefs rdup_PragmaExpression_list
..
.append wantdefs new_GenericBinding
.append wantdefs new_GenericBinding_list
.append wantdefs append_GenericBinding_list
..
.append wantdefs new_GenericMapping_list
.append wantdefs new_GenericMapping
.append wantdefs append_GenericMapping_list
..
.append wantdefs new_TypeAbstraction
.append wantdefs new_TypeAbstraction_list
.append wantdefs rdup_TypeAbstraction_list
.append wantdefs delete_TypeAbstraction_list
.append wantdefs rfre_TypeAbstraction_list
.append wantdefs append_TypeAbstraction_list
..
.append wantdefs new_ValueAbstraction
.append wantdefs new_ValueAbstraction_list
.append wantdefs rfre_ValueAbstraction_list
.append wantdefs rdup_ValueAbstraction_list
.append wantdefs delete_ValueAbstraction_list
.append wantdefs append_ValueAbstraction_list
..
.append wantdefs new_MethodInvocation
..
.append wantdefs new_AbstractValue
.append wantdefs rdup_AbstractValue
.append wantdefs rfre_AbstractValue
..
.append wantdefs new_VarState
.append wantdefs rdup_VarState
.append wantdefs rdup_VarState_list
.append wantdefs new_VarState_list
.append wantdefs rfre_VarState_list
.append wantdefs append_VarState_list
.append wantdefs delete_VarState_list
..
.append wantdefs append_ProgramState_list
.append wantdefs delete_ProgramState_list
.append wantdefs extract_ProgramState_list
.append wantdefs new_ProgramState
.append wantdefs new_ProgramState_list
.append wantdefs rdup_ProgramState
.append wantdefs rdup_ProgramState_list
.append wantdefs rfre_ProgramState
.append wantdefs rfre_ProgramState_list
..
.append wantdefs append_modflags_list
.append wantdefs new_modflags_list
.append wantdefs rfre_modflags_list
..
.append wantdefs append_arraymarker_list
.append wantdefs new_arraymarker
.append wantdefs new_arraymarker_list
.append wantdefs rfre_arraymarker_list
..
.append wantdefs append_MethodInfo_list
.append wantdefs new_MethodInfo
.append wantdefs new_MethodInfo_list
.append wantdefs rfre_MethodInfo_list
.append wantdefs delete_MethodInfo_list
