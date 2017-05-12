// File: vnustests.v
// Testing of the runtime library from a Vnus program

program

declarations [
    procedure _Print1dShape [
	_Print1dShape_f,
	_Print1dShape_A
    ]
    statements _Print1dShape [
        for
	    [_Print1dShape_i:getsize A 0]
	    statements [ 
		procedurecall
		    v__writeInt
		    [
			_Print1dShape_f,
			(_Print1dShape_A,[_Print1dShape_i])
		    ],
		procedurecall
		    v__writeString
		    [ _Print1dShape_f, " " ]
	    ],
	procedurecall v__writeString [ _Print1dShape_f, "\n" ]
    ],
    cardinalityvariable _card_i_0,
    localvariable _i forkall_scope int,
    localvariable n forkall_scope int,
    cardinalityvariable _Print1dShape_i,
    cardinalityvariable p,
    localvariable di forkall_scope int,
    localvariable i0 forkall_scope int,
    cardinalityvariable u0,
    cardinalityvariable u1,
    cardinalityvariable a0,
    localvariable a forkall_scope int,
    cardinalityvariable b,
    localvariable n_u0 forkall_scope int,
    localvariable n_u1 forkall_scope int,
    localvariable j1_low forkall_scope int,
    localvariable j0_low forkall_scope int,
    localvariable offset0 forkall_scope int,
    localvariable _f forkall_scope int,
    globalvariable A shape [35] int,
    globalvariable B shape [35] int,
    globalvariable C shape [35] int,
    formalvariable _Print1dShape_f _Print1dShape int,
    formalvariable _Print1dShape_A _Print1dShape shape [dontcare] int,
    externalvariable numberOfProcessors int,
    externalprocedure vnus_bclus1
    [
	vnus_bclus1_m,
	vnus_bclus1_dnp,
	vnus_bclus1_a,
	vnus_bclus1_b,
	vnus_bclus1_n_u1,
	vnus_bclus1_i0,
	vnus_bclus1_j1_low,
	vnus_bclus1_di,
	vnus_bclus1_p
    ],
    formalvariable vnus_bclus1_m vnus_bclus1 int,
    formalvariable vnus_bclus1_dnp vnus_bclus1 int,
    formalvariable vnus_bclus1_a vnus_bclus1 int,
    formalvariable vnus_bclus1_b vnus_bclus1 int,
    formalvariable vnus_bclus1_n_u1 vnus_bclus1 pointer int,
    formalvariable vnus_bclus1_i0 vnus_bclus1 pointer int,
    formalvariable vnus_bclus1_j1_low vnus_bclus1 pointer int,
    formalvariable vnus_bclus1_di vnus_bclus1 pointer int,
    formalvariable vnus_bclus1_p vnus_bclus1 int,
    externalprocedure vnus_bclus2
    [
	vnus_bclus2_u1,
	vnus_bclus2_i0,
	vnus_bclus2_j1_low,
	vnus_bclus2_ni,
	vnus_bclus2_di,
	vnus_bclus2_n_u0,
	vnus_bclus2_offset0,
	vnus_bclus2_p
    ],
    formalvariable vnus_bclus2_u1 vnus_bclus2 int,
    formalvariable vnus_bclus2_i0 vnus_bclus2 int,
    formalvariable vnus_bclus2_j1_low vnus_bclus2 int,
    formalvariable vnus_bclus2_ni vnus_bclus2 int,
    formalvariable vnus_bclus2_di vnus_bclus2 int,
    formalvariable vnus_bclus2_n_u0 vnus_bclus2 pointer int,
    formalvariable vnus_bclus2_offset0 vnus_bclus2 pointer int,
    formalvariable vnus_bclus2_p vnus_bclus2 int,
    externalprocedure vnus_blus
    [
	vnus_blus_m,
	vnus_blus_dnp,
	vnus_blus_a,
	vnus_blus_b,
	vnus_blus_ni,
	vnus_blus_n_j0,
	vnus_blus_j0_low,
	vnus_blus_p
    ],
    formalvariable vnus_blus_m vnus_blus int,
    formalvariable vnus_blus_dnp vnus_blus int,
    formalvariable vnus_blus_a vnus_blus int,
    formalvariable vnus_blus_b vnus_blus int,
    formalvariable vnus_blus_ni vnus_blus int,
    formalvariable vnus_blus_n_j0 vnus_blus pointer int,
    formalvariable vnus_blus_j0_low vnus_blus pointer int,
    formalvariable vnus_blus_p vnus_blus int,
    externalprocedure vnus_clus
    [
	vnus_clus_dnp,
	vnus_clus_a,
	vnus_clus_b,
	vnus_clus_ni,
	vnus_clus_di,
	vnus_clus_n_u0,
	vnus_clus_offset0,
	vnus_clus_p
    ],
    formalvariable vnus_clus_dnp vnus_clus int,
    formalvariable vnus_clus_a vnus_clus int,
    formalvariable vnus_clus_b vnus_clus int,
    formalvariable vnus_clus_ni vnus_clus int,
    formalvariable vnus_clus_di vnus_clus pointer int,
    formalvariable vnus_clus_n_u0 vnus_clus pointer int,
    formalvariable vnus_clus_offset0 vnus_clus pointer int,
    formalvariable vnus_clus_p vnus_clus int,
    externalfunction v__open [v__open_fnm,v__open_mode] int,
    formalvariable v__open_fnm v__open string,
    formalvariable v__open_mode v__open string,
    externalprocedure v__close [v__close_hnd],
    formalvariable v__close_hnd v__close int,
    externalprocedure v__writeInt [v__writeInt_hnd,v__writeInt_i],
    formalvariable v__writeInt_hnd v__writeInt int,
    formalvariable v__writeInt_i v__writeInt int,
    externalprocedure v__writeString [v__writeString_hnd,v__writeString_s],
    formalvariable v__writeString_hnd v__writeString int,
    formalvariable v__writeString_s v__writeString string
]

statements [
    forkall [p:numberOfProcessors] statements forkall_scope [
	barrier,
	assign _f functioncall v__open [ "testout", "w"],
	assign n 35,
	barrier,
	procedurecall v__writeString [ _f, "Testing run-time library\n" ],
	procedurecall v__writeString [ _f, "numberOfProcessors: " ],
	procedurecall v__writeInt [ _f, numberOfProcessors ],
	procedurecall v__writeString [ _f, "\n" ],
	println [ _f, "Testing block-cyclic distribution" ],
	for [a0:4,b:3] statements [
	    assign a (a0,+,1),
	    procedurecall v__writeString [ _f, "A := 0;\n"],
	    forall [_card_i_0:n] statements [
		barrier,
		if (((_card_i_0,/,3),mod,6),=,p)
		    statements [assign (A, [_card_i_0]) 0]
		    statements [],
		barrier
	    ],
	    procedurecall _Print1dShape [_f, A],

	    procedurecall v__writeString [ _f, "A[a*i+b] := i;\n" ],
	    procedurecall v__writeString [ _f, "a=" ],
	    procedurecall v__writeInt [ _f, a ],
	    procedurecall v__writeString [ _f, " b=" ],
	    procedurecall v__writeInt [ _f, b ],
	    procedurecall v__writeString [ _f, "\n" ],
	    // The code below simulates the results of an owner absorbtion value
	    // a loop like the one above.
	    barrier,
	    procedurecall vnus_bclus1 [
		3, numberOfProcessors, a, b, &n_u1, &i0, &j1_low, &di, p
	    ],
	    forall [u1:n_u1] statements [
		procedurecall
		    vnus_bclus2 [
			u1, i0, j1_low, ((n,-,b),/,a), di, &n_u0, &offset0, p
		    ],
		forall [u0:n_u0] statements [
		    assign _i (offset0,+,(di,*,u0)),
		    if (((a,*,_i),+,b),>=,n) statements [
			procedurecall v__writeString
			    [ _f, "array reference out of bounds: i=" ],
			procedurecall v__writeInt [ _f, _i ],
			procedurecall v__writeString [ _f, "\n" ]
		    ]
		    statements [
			assign (A, [((a,*,_i),+,b)]) _i
		    ]
		]
	    ],
	    barrier,
	    procedurecall v__writeString [ _f, "A: "],
	    procedurecall _Print1dShape [_f, A]
        ],
	barrier,
	println [ _f, "Testing block distribution" ],
	for [a0:4,b:3] statements [
	    assign a (a0,+,1),
	    procedurecall v__writeString [ _f, "B := 0;\n"],
	    forall [_card_i_0:n] statements [
		barrier,
		if (((_card_i_0,/,6),mod,6),=,p)
		    statements [assign (B, [_card_i_0]) 0]
		    statements [],
		barrier
	    ],
	    procedurecall _Print1dShape [_f, B],

	    println [ _f, "B[a*i+b] := i;" ],
	    println [ _f, "a=", a, " b=", b ],
	    // The code below simulates the results of an owner absorbtion value
	    // a loop like the one above.
	    barrier,
	    procedurecall vnus_blus [
		6, numberOfProcessors, a, b, ((n,-,b),/,a), &n_u0, &j0_low, p
	    ],
	    forall [u0:n_u0] statements [
		assign _i (j0_low,+,u0),
		if (((a,*,_i),+,b),>=,n) statements [
		    procedurecall v__writeString
			[ _f, "array reference out of bounds: i=" ],
		    procedurecall v__writeInt [ _f, _i ],
		    procedurecall v__writeString [ _f, "\n" ]
		]
		statements [
		    assign (B, [((a,*,_i),+,b)]) _i
		]
	    ],
	    barrier,
	    procedurecall v__writeString [ _f, "blus B: "],
	    procedurecall _Print1dShape [_f, B]
        ],
	barrier,
	println [ _f, "Testing cyclic distribution" ],
	for [a0:4,b:3] statements [
	    assign a (a0,+,1),
	    procedurecall v__writeString [ _f, "C := 0;\n"],
	    forall [_card_i_0:n] statements [
		barrier,
		if ((_card_i_0,mod,6),=,p)
		    statements [assign (C, [_card_i_0]) 0]
		    statements [],
		barrier
	    ],
	    procedurecall _Print1dShape [_f, C],

	    println [ _f, "C[a*i+b] := i;" ],
	    println [ _f, "a=", a, " b=", b ],
	    // The code below simulates the results of an owner absorbtion value
	    // a loop like the one above.
	    barrier,
	    procedurecall vnus_clus [
		numberOfProcessors, a, b, ((n,-,b),/,a), &di, &n_u0, &offset0, p
	    ],
	    forall [u0:n_u0] statements [
		assign _i (offset0,+,(di,*,u0)),
		if (((a,*,_i),+,b),>=,n) statements [
		    println [ _f, "array reference out of bounds: i=", _i ]
		]
		statements [
		    assign (C, [((a,*,_i),+,b)]) _i
		]
	    ],
	    barrier,
	    procedurecall v__writeString [ _f, "clus C: "],
	    procedurecall _Print1dShape [_f, C]
        ],
	barrier,
	procedurecall v__close [ _f ]
    ]
]
