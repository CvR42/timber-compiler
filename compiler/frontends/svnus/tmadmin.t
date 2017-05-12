.. File: tmadmin.t
.. Specification of the required tm functions for backend
.. administration.
..
.rename vnus_byte tmstring
.rename vnus_short tmstring
.rename vnus_char tmstring
.rename vnus_float tmstring
.rename vnus_int tmstring
.rename vnus_long tmstring
.rename vnus_double tmstring
.rename vnus_string tmstring
..
.set basename tmadmin
.set wantdefs
..
.append wantdefs stat_$(basename)
.append wantdefs get_balance_$(basename)
.append wantdefs flush_$(basename)
..
.append wantdefs new_origin
.append wantdefs rdup_origin
..
.append wantdefs new_origsymbol
.append wantdefs rdup_origsymbol
.append wantdefs rfre_origsymbol
.append wantdefs new_origsymbol_list
.append wantdefs append_origsymbol_list
.append wantdefs concat_origsymbol_list
.append wantdefs rfre_origsymbol_list
.append wantdefs rdup_origsymbol_list
.append wantdefs setroom_origsymbol_list
..
..append wantdefs new_tmsymbol_list
..append wantdefs setroom_tmsymbol_list
..append wantdefs append_tmsymbol_list
..append wantdefs concat_tmsymbol_list
..append wantdefs rfre_tmsymbol_list
..append wantdefs rdup_tmsymbol_list
..append wantdefs print_tmsymbol_list
..
.append wantdefs new_formalParameter_list
.append wantdefs new_formalParameter
.append wantdefs append_formalParameter_list
.append wantdefs cmp_formalParameter_list
.append wantdefs rfre_formalParameter_list
.append wantdefs rdup_formalParameter_list
..
.append wantdefs vnusdeclaration
.append wantdefs vnusdeclaration_list
.append wantdefs new_vnusdeclaration_list
.append wantdefs rfre_vnusdeclaration_list
.append wantdefs rdup_vnusdeclaration_list
.append wantdefs append_vnusdeclaration_list
.append wantdefs new_vnusdeclaration
..
.append wantdefs ${prefix new_ ${subclasses vnusdeclarationdata}}
..
.append wantdefs ${prefix new_ ${subclasses type}}
.append wantdefs rfre_type
.append wantdefs rdup_type
.append wantdefs new_type_list
.append wantdefs append_type_list
.append wantdefs setroom_type_list
..
.append wantdefs new_size_list
.append wantdefs ${prefix new_ ${subclasses size}}
.append wantdefs append_size_list
.append wantdefs setroom_size_list
..
.append wantdefs new_expression_list
.append wantdefs rfre_expression_list
.append wantdefs append_expression_list
.append wantdefs rdup_expression_list
.append wantdefs setroom_expression_list
.append wantdefs ${prefix new_ ${subclasses expression}}
.append wantdefs fre_expression
.append wantdefs rdup_expression
.append wantdefs rfre_expression
..
.append wantdefs new_cardinality_list
.append wantdefs append_cardinality_list
.append wantdefs setroom_cardinality_list
.append wantdefs rfre_cardinality_list
.append wantdefs new_cardinality
..
.append wantdefs new_functioncall
.append wantdefs rfre_functioncall
..
.append wantdefs new_view
.append wantdefs rfre_view
..
.append wantdefs new_statement_list
.append wantdefs append_statement_list
.append wantdefs concat_statement_list
.append wantdefs ${prefix new_ ${subclasses statement}}
.append wantdefs rfre_statement
..
.append wantdefs ${prefix new_ ${subclasses declaration}}
.append wantdefs new_declaration_list
.append wantdefs append_declaration_list
.append wantdefs rfre_declaration_list
..
.append wantdefs print_vnusprog
.append wantdefs new_vnusprog
.append wantdefs rfre_vnusprog
..
.append wantdefs new_translation
.append wantdefs new_translation_list
.append wantdefs append_translation_list
.append wantdefs delete_translation_list
.append wantdefs rfre_translation_list
..
.append wantdefs new_block
.append wantdefs rdup_block
..
.append wantdefs new_pragma_list
.append wantdefs append_pragma_list
.append wantdefs rdup_pragma_list
.append wantdefs concat_pragma_list
..
.append wantdefs ${prefix new_ ${subclasses pragma}}
..
.append wantdefs ${prefix new_ ${subclasses distribution}}
.append wantdefs new_distribution_list
.append wantdefs append_distribution_list
..
.append wantdefs ${prefix new_ ${subclasses optexpression}}
.append wantdefs rdup_optexpression
..
.append wantdefs new_field
.append wantdefs new_field_list
.append wantdefs setroom_field_list
.append wantdefs append_field_list
..
.append wantdefs new_switchCase_list
.append wantdefs ${prefix new_ ${subclasses switchCase}}
.append wantdefs append_switchCase_list
