.. File: tmadmin.t
.. Specification of the required tm functions for backend
.. administration.
..
.rename vnus_string tmstring
..
.set verbose
.set basename tmadmin
.set wantdefs
.append notwantdefs isequal_origsymbol
..
.append wantdefs new_origin
..
.append wantdefs new_origsymbol
.append wantdefs rdup_origsymbol
..
.append wantdefs ${prefix new_ ${subclasses location}}
..
.append wantdefs ${prefix new_ ${subclasses expression}}
.append wantdefs rdup_expression
.append wantdefs rfre_expression
.append wantdefs fre_expression
.append wantdefs rdup_expression_list
.append wantdefs new_expression_list
.append wantdefs append_expression_list
.append wantdefs insertlist_expression_list
.append wantdefs insert_expression_list
.append wantdefs isequal_expression
.append wantdefs delete_expression_list
..
.append wantdefs rdup_Pragma_list
..
.append wantdefs rdup_OwnerExpr_list
..
.append wantdefs new_tmsymbol_list
.append wantdefs append_tmsymbol_list
..
.append wantdefs ${prefix new_ ${subclasses type}}
.append wantdefs new_type_list
.append wantdefs append_type_list
.append wantdefs setroom_type_list
.append wantdefs rdup_type
.append wantdefs rfre_type
..
.append wantdefs rdup_size_list
..
.append wantdefs append_field_list
.append wantdefs new_field_list
.append wantdefs new_field
.append wantdefs setroom_field_list
..
.append wantdefs new_declaration_list
..
.append wantdefs new_optexpression
..
.append wantdefs vnusprog
