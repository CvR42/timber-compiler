// Generated by sugar Vnus front-end

program

pragma [value boundscheck true]

declarations [
    globalvariable x_0 float,
    globalvariable i_0 int,
    globalvariable desti_0 int,
    globalvariable V_0 view 1 int,
    globalvariable W_0 view 1 int,
    globalvariable n_0 int,
    globalvariable c_0 complex,
    globalvariable c2_0 complex,
    globalvariable tpi_0 int,
    globalvariable tpo_0 int,
    globalvariable tpio_0 int,
    globalvariable b_0 boolean,
    globalvariable s_0 string,
    globalvariable f_0 int,
    globalvariable shp_0 shape [5, 8] [blockcyclic 5, collapsed] int,
    globalvariable A_0 shape [20] [blockcyclic 5] int,
    globalvariable B_0 shape [20]  int,
    globalvariable aview_0 view 2 int,
    formalvariable f_1 v__writeString int,
    formalvariable s_1 v__writeString string,
    formalvariable p_0 v__writeString int,
    externalprocedure v__writeString [f_1, s_1, p_0],
    formalvariable f_2 v__writeInt int,
    formalvariable n_1 v__writeInt int,
    formalvariable p_1 v__writeInt int,
    externalprocedure v__writeInt [f_2, n_1, p_1],
    formalvariable f_3 v__writeComplex int,
    formalvariable c_1 v__writeComplex complex,
    formalvariable p_2 v__writeComplex int,
    externalprocedure v__writeComplex [f_3, c_1, p_2],
    formalvariable f_4 v__writeBoolean int,
    formalvariable b_1 v__writeBoolean boolean,
    formalvariable p_3 v__writeBoolean int,
    externalprocedure v__writeBoolean [f_4, b_1, p_3],
    formalvariable f_5 v__writeFloat int,
    formalvariable r_0 v__writeFloat float,
    formalvariable p_4 v__writeFloat int,
    externalprocedure v__writeFloat [f_5, r_0, p_4],
    formalvariable nm_0 v__open string,
    formalvariable mode_0 v__open string,
    formalvariable p_5 v__open int,
    externalfunction v__open [nm_0, mode_0, p_5] int,
    formalvariable f_6 v__close int,
    formalvariable p_6 v__close int,
    externalprocedure v__close [f_6, p_6],
    externalvariable numberOfProcessors int,
    formalvariable f_7 test_scopes int,
    formalvariable n_2 test_scopes int,
    formalvariable p_7 test_scopes int,
    localvariable nloc_0 test_scopes int,
    localvariable nsub_0 scope0 int,
    localvariable n_3 scope1 int,
    localvariable n_4 scope2 int,
    cardinalityvariable i_1,
    localvariable A_1 scope3 shape [23]  int,
    cardinalityvariable i1_0,
    localvariable z_0 scope4 int,
    procedure test_scopes_0 [f_7, n_2, p_7]
    statements test_scopes [
        assign nloc_0 ( n_2, *, 2 )
        ,
        pragma [flag constant]
        procedurecall v__writeInt [f_7, n_2, p_7]
        ,
        procedurecall v__writeString [f_7, " ", p_7]
        ,
        procedurecall v__writeInt [f_7, nloc_0, p_7]
        ,
        procedurecall v__writeString [f_7, "\n", p_7]
        ,
        statements scope0 [
            assign nsub_0 ( nloc_0, *, 3 )
            ,
            procedurecall v__writeInt [f_7, n_2, p_7]
            ,
            procedurecall v__writeString [f_7, " ", p_7]
            ,
            procedurecall v__writeInt [f_7, nloc_0, p_7]
            ,
            procedurecall v__writeString [f_7, " ", p_7]
            ,
            procedurecall v__writeInt [f_7, nsub_0, p_7]
            ,
            procedurecall v__writeString [f_7, "\n", p_7]
            ,
            if ( nsub_0, >, nloc_0 )
                statements scope1 [
                    assign n_3 3
                    ,
                    while ( n_3, >, 0 )
                        statements [
                            assign n_3 ( n_3, -, 1 )
                            ,
                            assign nsub_0 ( nsub_0, +, n_3 )
                        ]
                    ,
                    procedurecall v__writeInt [f_7, nsub_0, p_7]
                    ,
                    procedurecall v__writeString [f_7, "\n", p_7]
                ]
                statements scope2 [
                    assign n_4 6
                    ,
                    while ( n_4, >, 0 )
                        statements [
                            assign n_4 ( n_4, -, 1 )
                            ,
                            assign nsub_0 ( nsub_0, +, n_4 )
                        ]
                    ,
                    procedurecall v__writeInt [f_7, nsub_0, p_7]
                    ,
                    procedurecall v__writeString [f_7, "\n", p_7]
                ]
        ]
        ,
        for [i_1:nloc_0]
            statements scope3 [
                forall [i1_0:getsize A_1 0]
                    statements scope4 [
                        assign z_0 ( i1_0, +, 2 )
                        ,
                        assign (A_1, [i1_0]) z_0
                    ]
            ]
    ]
    ,
    procedure empty_proc_0 []
    statements empty_proc [
    ]
    ,
    formalvariable v_0 empty_proc1 pointer shape [dontcare]  int,
    procedure empty_proc1_0 [v_0]
    statements empty_proc1 [
        l:
        assign (*v_0, [0]) 42
    ]
    ,
    formalvariable f_8 Print1dView int,
    formalvariable V_1 Print1dView view 1 int,
    formalvariable p_8 Print1dView int,
    cardinalityvariable i_2,
    procedure Print1dView_0 [f_8, V_1, p_8]
    statements Print1dView [
        for [i_2:getsize V_1 0]
            statements [
                print [f_8, p_8, (V_1, [i_2]), " "]
            ]
        ,
        println [f_8, p_8, ""]
    ]
    ,
    formalvariable f_9 Print1dShape int,
    formalvariable A_2 Print1dShape pointer shape [dontcare]  int,
    formalvariable p_9 Print1dShape int,
    cardinalityvariable i_3,
    procedure Print1dShape_0 [f_9, A_2, p_9]
    statements Print1dShape [
        for [i_3:getsize *A_2 0]
            statements [
                procedurecall v__writeInt [f_9, (*A_2, [i_3]), p_9]
                ,
                procedurecall v__writeString [f_9, " ", p_9]
            ]
        ,
        procedurecall v__writeString [f_9, "\n", p_9]
        ,
        if ismultidimdist *A_2
            statements [
                println [f_9, p_9, "Distributed over the multi-dimensional processor array"]
            ]
            statements [
                println [f_9, p_9, "Distributed over the one-dimensional processor array"]
            ]
    ]
    ,
    formalvariable i_4 inc int,
    returnvariable res_0 inc int,
    function inc_0 [i_4] res_0 int 
    statements inc [
        assign res_0 ( i_4, +, 1 )
    ]
    ,
    formalvariable i_5 dec int,
    returnvariable res_1 dec int,
    function dec_0 [i_5] res_1 int 
    statements dec [
        assign res_1 ( i_5, -, 1 )
    ]
    ,
    formalvariable f_a testcmpinteger int,
    formalvariable a_0 testcmpinteger int,
    formalvariable b_2 testcmpinteger int,
    formalvariable p_a testcmpinteger int,
    procedure testcmpinteger_0 [f_a, a_0, b_2, p_a]
    statements testcmpinteger [
        procedurecall v__writeInt [f_a, a_0, p_a]
        ,
        procedurecall v__writeString [f_a, "=", p_a]
        ,
        procedurecall v__writeInt [f_a, b_2, p_a]
        ,
        if ( a_0, =, b_2 )
            statements [
                procedurecall v__writeString [f_a, " yes\n", p_a]
            ]
            statements [
                procedurecall v__writeString [f_a, " no\n", p_a]
            ]
        ,
        procedurecall v__writeInt [f_a, a_0, p_a]
        ,
        procedurecall v__writeString [f_a, "<>", p_a]
        ,
        procedurecall v__writeInt [f_a, b_2, p_a]
        ,
        if ( a_0, <>, b_2 )
            statements [
                procedurecall v__writeString [f_a, " yes\n", p_a]
            ]
            statements [
                procedurecall v__writeString [f_a, " no\n", p_a]
            ]
        ,
        procedurecall v__writeInt [f_a, a_0, p_a]
        ,
        procedurecall v__writeString [f_a, "<", p_a]
        ,
        procedurecall v__writeInt [f_a, b_2, p_a]
        ,
        if ( a_0, <, b_2 )
            statements [
                procedurecall v__writeString [f_a, " yes\n", p_a]
            ]
            statements [
                procedurecall v__writeString [f_a, " no\n", p_a]
            ]
        ,
        procedurecall v__writeInt [f_a, a_0, p_a]
        ,
        procedurecall v__writeString [f_a, "<=", p_a]
        ,
        procedurecall v__writeInt [f_a, b_2, p_a]
        ,
        if ( a_0, <=, b_2 )
            statements [
                procedurecall v__writeString [f_a, " yes\n", p_a]
            ]
            statements [
                procedurecall v__writeString [f_a, " no\n", p_a]
            ]
        ,
        procedurecall v__writeInt [f_a, a_0, p_a]
        ,
        procedurecall v__writeString [f_a, ">", p_a]
        ,
        procedurecall v__writeInt [f_a, b_2, p_a]
        ,
        if ( a_0, >, b_2 )
            statements [
                procedurecall v__writeString [f_a, " yes\n", p_a]
            ]
            statements [
                procedurecall v__writeString [f_a, " no\n", p_a]
            ]
        ,
        procedurecall v__writeInt [f_a, a_0, p_a]
        ,
        procedurecall v__writeString [f_a, ">=", p_a]
        ,
        procedurecall v__writeInt [f_a, b_2, p_a]
        ,
        if ( a_0, >=, b_2 )
            statements [
                procedurecall v__writeString [f_a, " yes\n", p_a]
            ]
            statements [
                procedurecall v__writeString [f_a, " no\n", p_a]
            ]
    ]
    ,
    cardinalityvariable p_b,
    localvariable V0_0 scope5 view 0 int,
    localvariable s0_0 scope5 shape [1]  int,
    cardinalityvariable i_6,
    cardinalityvariable j_0,
    cardinalityvariable i_7,
    cardinalityvariable j_1,
    cardinalityvariable i_8,
    cardinalityvariable j_2,
    cardinalityvariable i_9,
    cardinalityvariable j_3,
    cardinalityvariable i_a,
    cardinalityvariable j_4,
    cardinalityvariable i_b,
    cardinalityvariable i_c,
    cardinalityvariable j_5,
    cardinalityvariable i_d,
    cardinalityvariable j_6,
    cardinalityvariable i_e,
    cardinalityvariable i_f,
    cardinalityvariable i_10,
    cardinalityvariable i_11,
    cardinalityvariable i_12,
    cardinalityvariable i_13

]

statements [
    forkall [p_b:numberOfProcessors]
        statements scope5 [
            barrier
            ,
            if ( p_b, =, 0 )
                statements [
                    assign f_0 functioncall v__open ["runout", "w", p_b]
                ]
                statements [
                ]
            ,
            barrier
            ,
            procedurecall v__writeString [f_0, "Hello world\n", p_b]
            ,
            barrier
            ,
            if true
                statements [
                    procedurecall v__writeString [f_0, "Yep, true is true\n", p_b]
                ]
                statements [
                    procedurecall v__writeString [f_0, "Huh, true is false?\n", p_b]
                ]
            ,
            if false
                statements [
                    procedurecall v__writeString [f_0, "Huh, false is true?\n", p_b]
                ]
                statements [
                    procedurecall v__writeString [f_0, "Yep, false is false\n", p_b]
                ]
            ,
            barrier
            ,
            if ( p_b, =, 0 )
                statements [
                    assign b_0 false
                    ,
                    assign b_0 (  not , b_0 )
                    ,
                    assign b_0 ( false,  or , b_0 )
                    ,
                    assign b_0 ( true,  and , b_0 )
                    ,
                    assign i_0 2
                    ,
                    assign i_0 ( 50, -, i_0 )
                    ,
                    assign i_0 ( 5, +, i_0 )
                    ,
                    assign i_0 ( 5, *, i_0 )
                    ,
                    assign i_0 ( i_0, /, 12 )
                    ,
                    assign i_0 ( ~, i_0 )
                    ,
                    assign i_0 ( +, i_0 )
                    ,
                    assign i_0 functioncall inc_0 [functioncall inc_0 [i_0]]
                    ,
                    assign i_0 functioncall dec_0 [i_0]
                    ,
                    assign n_0 7
                    ,
                    assign n_0 ( +, n_0 )
                    ,
                    assign n_0 ( 7, +, n_0 )
                    ,
                    assign n_0 ( n_0, -, 5 )
                    ,
                    assign n_0 ( n_0, *, 12 )
                    ,
                    assign n_0 ( n_0, /, 13 )
                    ,
                    assign n_0 ( n_0, *, 6 )
                    ,
                    assign n_0 ( n_0, /, 3 )
                    ,
                    assign x_0 2.1
                    ,
                    assign x_0 ( ~, x_0 )
                    ,
                    assign x_0 ( +, x_0 )
                    ,
                    assign x_0 ( 0.7, +, x_0 )
                    ,
                    assign x_0 ( x_0, -, 5e2 )
                    ,
                    assign x_0 ( x_0, *, 0.12 )
                    ,
                    assign x_0 ( x_0, /, 0.13 )
                    ,
                    assign s_0 "test string"
                    ,
                    assign c_0 complex cast double 2 cast double 3
                    ,
                    assign c2_0 complex cast double 5 cast double 12
                    ,
                    assign c_0 ( ~, c_0 )
                ]
                statements [
                ]
            ,
            empty
            ,
            barrier
            ,
            procedurecall v__writeString [f_0, "c: ", p_b]
            ,
            procedurecall v__writeComplex [f_0, c_0, p_b]
            ,
            procedurecall v__writeString [f_0, "\n", p_b]
            ,
            barrier
            ,
            if ( p_b, =, 0 )
                statements [
                    assign c_0 ( +, c_0 )
                ]
                statements [
                ]
            ,
            barrier
            ,
            procedurecall v__writeString [f_0, "c: ", p_b]
            ,
            procedurecall v__writeComplex [f_0, c_0, p_b]
            ,
            procedurecall v__writeString [f_0, "\n", p_b]
            ,
            barrier
            ,
            if ( p_b, =, 0 )
                statements [
                    assign c_0 ( c_0, +, c2_0 )
                ]
                statements [
                ]
            ,
            barrier
            ,
            procedurecall v__writeString [f_0, "c: ", p_b]
            ,
            procedurecall v__writeComplex [f_0, c_0, p_b]
            ,
            procedurecall v__writeString [f_0, "\n", p_b]
            ,
            barrier
            ,
            if ( p_b, =, 0 )
                statements [
                    assign c_0 ( c_0, *, c2_0 )
                ]
                statements [
                ]
            ,
            barrier
            ,
            procedurecall v__writeString [f_0, "c: ", p_b]
            ,
            procedurecall v__writeComplex [f_0, c_0, p_b]
            ,
            procedurecall v__writeString [f_0, "\n", p_b]
            ,
            barrier
            ,
            if ( p_b, =, 0 )
                statements [
                    assign c_0 ( c_0, -, c2_0 )
                ]
                statements [
                ]
            ,
            barrier
            ,
            procedurecall v__writeString [f_0, "c: ", p_b]
            ,
            procedurecall v__writeComplex [f_0, c_0, p_b]
            ,
            procedurecall v__writeString [f_0, "\n", p_b]
            ,
            barrier
            ,
            if ( p_b, =, 0 )
                statements [
                    assign c_0 ( c_0, /, c2_0 )
                ]
                statements [
                ]
            ,
            barrier
            ,
            procedurecall v__writeString [f_0, "c: ", p_b]
            ,
            procedurecall v__writeComplex [f_0, c_0, p_b]
            ,
            procedurecall v__writeString [f_0, "\n", p_b]
            ,
            procedurecall v__writeString [f_0, "n: ", p_b]
            ,
            procedurecall v__writeInt [f_0, n_0, p_b]
            ,
            procedurecall v__writeString [f_0, "\n", p_b]
            ,
            procedurecall v__writeString [f_0, "i: ", p_b]
            ,
            procedurecall v__writeInt [f_0, i_0, p_b]
            ,
            procedurecall v__writeString [f_0, "\n", p_b]
            ,
            procedurecall v__writeString [f_0, "b: ", p_b]
            ,
            procedurecall v__writeBoolean [f_0, b_0, p_b]
            ,
            procedurecall v__writeString [f_0, "\n", p_b]
            ,
            procedurecall v__writeString [f_0, "x: ", p_b]
            ,
            procedurecall v__writeFloat [f_0, x_0, p_b]
            ,
            procedurecall v__writeString [f_0, "\n", p_b]
            ,
            procedurecall v__writeString [f_0, "s: ", p_b]
            ,
            procedurecall v__writeString [f_0, s_0, p_b]
            ,
            procedurecall v__writeString [f_0, "\n", p_b]
            ,
            procedurecall v__writeString [f_0, "c: ", p_b]
            ,
            procedurecall v__writeComplex [f_0, c_0, p_b]
            ,
            procedurecall v__writeString [f_0, "\n", p_b]
            ,
            procedurecall v__writeString [f_0, "numberOfProcessors: ", p_b]
            ,
            procedurecall v__writeInt [f_0, numberOfProcessors, p_b]
            ,
            procedurecall v__writeString [f_0, "\n", p_b]
            ,
            procedurecall v__writeString [f_0, "\n", p_b]
            ,
            barrier
            ,
            if ( p_b, =, 0 )
                statements [
                    assign i_0 14
                ]
                statements [
                ]
            ,
            barrier
            ,
            while ( i_0, >, 0 )
                statements [
                    procedurecall v__writeString [f_0, "i = ", p_b]
                    ,
                    procedurecall v__writeInt [f_0, i_0, p_b]
                    ,
                    procedurecall v__writeString [f_0, "\n", p_b]
                    ,
                    barrier
                    ,
                    if ( p_b, =, 0 )
                        statements [
                            assign i_0 ( i_0, -, 1 )
                        ]
                        statements [
                        ]
                    ,
                    barrier
                ]
            ,
            for [i_6:3, j_0:4]
                statements [
                    procedurecall v__writeString [f_0, "i = ", p_b]
                    ,
                    procedurecall v__writeInt [f_0, i_6, p_b]
                    ,
                    procedurecall v__writeString [f_0, ", ", p_b]
                    ,
                    procedurecall v__writeString [f_0, "j = ", p_b]
                    ,
                    procedurecall v__writeInt [f_0, j_0, p_b]
                    ,
                    procedurecall v__writeString [f_0, "\n", p_b]
                ]
            ,
            procedurecall testcmpinteger_0 [f_0, 1, 2, p_b]
            ,
            procedurecall testcmpinteger_0 [f_0, 1, 1, p_b]
            ,
            procedurecall testcmpinteger_0 [f_0, 2, 1, p_b]
            ,
            procedurecall testcmpinteger_0 [f_0, 223, 1, p_b]
            ,
            barrier
            ,
            forall [i_7:5, j_1:8]
                statements [
                    if isowner (shp_0, [i_7, j_1]) p_b
                        statements [
                            assign (shp_0, [i_7, j_1]) ( i_7, +, j_1 )
                        ]
                        statements [
                        ]
                ]
            ,
            barrier
            ,
            for [i_8:5, j_2:8]
                statements [
                    procedurecall v__writeString [f_0, "shp[", p_b]
                    ,
                    procedurecall v__writeInt [f_0, i_8, p_b]
                    ,
                    procedurecall v__writeString [f_0, ",", p_b]
                    ,
                    procedurecall v__writeInt [f_0, j_2, p_b]
                    ,
                    procedurecall v__writeString [f_0, "] = ", p_b]
                    ,
                    procedurecall v__writeInt [f_0, (shp_0, [i_8, j_2]), p_b]
                    ,
                    procedurecall v__writeString [f_0, "\n", p_b]
                ]
            ,
            assign aview_0 view [i_9:8, j_3:5] (shp_0, [j_3, i_9])
            ,
            for [i_a:8, j_4:5]
                statements [
                    procedurecall v__writeString [f_0, "aview[", p_b]
                    ,
                    procedurecall v__writeInt [f_0, i_a, p_b]
                    ,
                    procedurecall v__writeString [f_0, ",", p_b]
                    ,
                    procedurecall v__writeInt [f_0, j_4, p_b]
                    ,
                    procedurecall v__writeString [f_0, "] = ", p_b]
                    ,
                    procedurecall v__writeInt [f_0, (aview_0, [i_a, j_4]), p_b]
                    ,
                    procedurecall v__writeString [f_0, "\n", p_b]
                ]
            ,
            procedurecall v__writeString [f_0, "A := 66;\n", p_b]
            ,
            barrier
            ,
            for [i_b:getsize A_0 0]
                statements [
                    if isowner (A_0, [i_b]) p_b
                        statements [
                            assign (A_0, [i_b]) 66
                        ]
                        statements [
                        ]
                ]
            ,
            barrier
            ,
            procedurecall Print1dView_0 [f_0, view [i_c:getsize A_0 0] (A_0, [i_c]), p_b]
            ,
            procedurecall v__writeString [f_0, "V <- A;\nV := 2112;\n", p_b]
            ,
            barrier
            ,
            assign V_0 view [j_5:10] (A_0, [j_5])
            ,
            for [i_d:getsize V_0 0]
                statements [
                    assign (V_0, [i_d]) 2112
                ]
            ,
            procedurecall Print1dView_0 [f_0, V_0, p_b]
            ,
            procedurecall Print1dShape_0 [f_0, &A_0, p_b]
            ,
            assign W_0 view [j_6:getsize V_0 0] (A_0, [( ( ( 3, *, j_6 ), +, 2 ),  mod , 10 )])
            ,
            for [i_e:getsize V_0 0]
                statements [
                    barrier
                    ,
                    assign (W_0, [i_e]) i_e
                    ,
                    barrier
                ]
            ,
            procedurecall Print1dShape_0 [f_0, &A_0, p_b]
            ,
            procedurecall v__writeString [f_0, "A[i] := 0;\n", p_b]
            ,
            barrier
            ,
            forall [i_f:getsize A_0 0]
                statements [
                    if isowner (A_0, [i_f]) p_b
                        statements [
                            assign (A_0, [i_f]) 0
                        ]
                        statements [
                        ]
                ]
            ,
            barrier
            ,
            procedurecall v__writeString [f_0, "A: ", p_b]
            ,
            procedurecall Print1dShape_0 [f_0, &A_0, p_b]
            ,
            procedurecall v__writeString [f_0, "B[i] := i;\n", p_b]
            ,
            forall [i_10:getsize A_0 0]
                statements [
                    if isowner (B_0, [i_10]) p_b
                        statements [
                            assign (B_0, [i_10]) i_10
                        ]
                        statements [
                        ]
                ]
            ,
            barrier
            ,
            procedurecall v__writeString [f_0, "B: ", p_b]
            ,
            procedurecall Print1dShape_0 [f_0, &B_0, p_b]
            ,
            procedurecall v__writeString [f_0, "A[i] := B[i];\n", p_b]
            ,
            forall [i_11:getsize A_0 0]
                statements [
                    if isowner (A_0, [i_11]) p_b
                        statements [
                            assign (A_0, [i_11]) (B_0, [i_11])
                        ]
                        statements [
                        ]
                ]
            ,
            barrier
            ,
            procedurecall v__writeString [f_0, "A: ", p_b]
            ,
            procedurecall Print1dShape_0 [f_0, &A_0, p_b]
            ,
            procedurecall v__writeString [f_0, "A[i] := 0;\n", p_b]
            ,
            barrier
            ,
            forall [i_12:getsize A_0 0]
                statements [
                    if isowner (A_0, [i_12]) p_b
                        statements [
                            assign (A_0, [i_12]) 0
                        ]
                        statements [
                        ]
                ]
            ,
            barrier
            ,
            procedurecall v__writeString [f_0, "A: ", p_b]
            ,
            procedurecall Print1dShape_0 [f_0, &A_0, p_b]
            ,
            procedurecall v__writeString [f_0, "A[i] := B[i];\n", p_b]
            ,
            barrier
            ,
            forall [i_13:getsize A_0 0]
                statements [
                    assign desti_0 ( 19, -, i_13 )
                    ,
                    if ( sender (B_0, [i_13]), =, p_b )
                        statements [
                            send p_b owner (A_0, [desti_0]) (B_0, [i_13])
                        ]
                        statements [
                        ]
                    ,
                    if isowner (A_0, [desti_0]) p_b
                        statements [
                            receive p_b sender (B_0, [i_13]) (A_0, [desti_0])
                        ]
                        statements [
                        ]
                ]
            ,
            barrier
            ,
            procedurecall v__writeString [f_0, "A: ", p_b]
            ,
            procedurecall Print1dShape_0 [f_0, &A_0, p_b]
            ,
            println [f_0, p_b, "Block size of A: ", getblocksize A_0 0]
            ,
            procedurecall test_scopes_0 [f_0, 12, p_b]
            ,
            procedurecall v__close [f_0, p_b]
            ,
            procedurecall empty_proc_0 []
            ,
            procedurecall empty_proc1_0 [&s0_0]
            ,
            assign V0_0 view [] (s0_0, [0])
        ]
]