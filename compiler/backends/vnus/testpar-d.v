// This is a test program for the
// V-nus compiler.

program

pragma [ processors = "6,5" ]

declarations [
    globalvariable _x float,
    globalvariable _i int,
    globalvariable _desti int,
    cardinalityvariable i,
    cardinalityvariable _card_i,
    cardinalityvariable _card_j,
    cardinalityvariable _Print1dShape_i,
    cardinalityvariable p,
    globalvariable _n int,
    globalvariable _c complex,
    globalvariable _c2 complex,
    globalvariable _tpi int,
    globalvariable _tpo int,
    globalvariable _tpio int,
    globalvariable _b boolean,
    globalvariable _true boolean,
    globalvariable _false boolean,
    globalvariable _s string,
    globalvariable _f int,
    globalvariable _shp shape [5, 8] int,
    globalvariable A shape [20] int,
    globalvariable B shape [20] int,
    formalvariable _Print1dShape_f _Print1dShape int,
    formalvariable _Print1dShape_p _Print1dShape int,
    formalvariable _Print1dShape_A _Print1dShape shape [dontcare] int,
    formalvariable _dec_i _dec int,
    formalvariable _inc_i _inc int,
    formalvariable _testcmpint_f _testcmpint int,
    formalvariable _testcmpint_p _testcmpint int,
    formalvariable _testcmpint_a _testcmpint int,
    formalvariable _testcmpint_b _testcmpint int,
    formalvariable _testscopes_f renamed_testscopes int,
    formalvariable _testscopes_p renamed_testscopes int,
    formalvariable _testscopes_n renamed_testscopes int,
    localvariable _testscopes_n_sub subscope int,
    externalfunction v__open [v__open_fnm,v__open_mode] int,
    formalvariable v__open_fnm v__open string,
    formalvariable v__open_p v__open int,
    formalvariable v__open_mode v__open string,
    externalvariable numberOfProcessors int,
    externalprocedure v__close [v__close_hnd],
    formalvariable v__close_hnd v__close int,
    formalvariable v__close_p v__close int,
    externalprocedure v__writeFloat [v__writer_hnd,v__writer_r],
    formalvariable v__writer_hnd v__writeFloat int,
    formalvariable v__writer_r v__writeFloat float,
    externalprocedure v__writeComplex [v__writec_hnd,v__writec_c],
    formalvariable v__writec_hnd v__writeComplex int,
    formalvariable v__writec_c v__writeComplex complex,
    externalprocedure v__writeBoolean [v__writeb_hnd,v__writeb_b],
    formalvariable v__writeb_hnd v__writeBoolean int,
    formalvariable v__writeb_b v__writeBoolean boolean,
    externalprocedure v__writeInt [v__writei_hnd,v__writei_i],
    formalvariable v__writei_hnd v__writeInt int,
    formalvariable v__writei_i v__writeInt int,
    externalprocedure v__writeString [v__writes_hnd,v__writes_s],
    formalvariable v__writes_hnd v__writeString int,
    formalvariable v__writes_s v__writeString string,
    procedure _testscopes [_testscopes_f, _testscopes_n,  _testscopes_p]
    statements renamed_testscopes [
	procedurecall
	    v__writeInt [_testscopes_f, _testscopes_n],
	procedurecall
	    v__writeString [_testscopes_f, " "],
	statements subscope [
	    assign _testscopes_n_sub (42,+,1),
	    procedurecall
		v__writeInt [_testscopes_f, _testscopes_n],
	    procedurecall
		v__writeString [_testscopes_f, " "],
	    procedurecall
		v__writeInt [_testscopes_f, _testscopes_n_sub],
	    procedurecall
		v__writeString [_testscopes_f, "\n"]
	]
    ],
    procedure _Empty []
    statements _Empty [],
    procedure _Print1dShape [_Print1dShape_f, _Print1dShape_A, _Print1dShape_p]
    statements _Print1dShape [
//       if ismultidimdist _Print1dShape_A statements [
	if false statements [
	    println [_Print1dShape_f, _Print1dShape_p, "A one-dim. shape uses the multidim. processor array"]
	]
	statements [],
        for [_Print1dShape_i: getsize _Print1dShape_A 0]
	    statements [ 
		procedurecall
		    v__writeInt
		    [ _Print1dShape_f, (_Print1dShape_A,[_Print1dShape_i]) ],
		procedurecall
		    v__writeString
		    [ _Print1dShape_f, " " ]
	    ],
	procedurecall v__writeString [ _Print1dShape_f, "\n" ]
    ],
    function _inc [_inc_i] int
    statements _inc [
	return (_inc_i,+,1)
    ],
    function _dec [_dec_i] int
    statements _dec [
	return (_dec_i,-,1)
    ],
    procedure _testcmpint [_testcmpint_f, _testcmpint_a, _testcmpint_b,_testcmpint_p]
    statements _testcmpint [
	procedurecall v__writeInt [ _testcmpint_f, _testcmpint_a ],
	procedurecall v__writeString [ _testcmpint_f, "=" ],
	procedurecall v__writeInt [ _testcmpint_f, _testcmpint_b ],
	if (_testcmpint_a,=,_testcmpint_b ) statements [
	    procedurecall v__writeString [ _testcmpint_f, " yes\n" ]
	]
	statements [
	    procedurecall v__writeString [ _testcmpint_f, " no\n" ]
	],
	procedurecall v__writeInt [ _testcmpint_f, _testcmpint_a ],
	procedurecall v__writeString [ _testcmpint_f, "<>" ],
	procedurecall v__writeInt [ _testcmpint_f, _testcmpint_b ],
	if (_testcmpint_a,<>,_testcmpint_b) statements [
	    procedurecall v__writeString [ _testcmpint_f, " yes\n" ]
	]
	statements [
	    procedurecall v__writeString [ _testcmpint_f, " no\n" ]
	],
	print [_testcmpint_f, _testcmpint_a, "<", _testcmpint_b ],
	if (_testcmpint_a,<,_testcmpint_b)
	    statements [
		procedurecall
		    v__writeString
		    [ _testcmpint_f, " yes\n" ]
	    ]
	    statements [
		procedurecall
		    v__writeString
		    [ _testcmpint_f, " no\n" ]
	    ]
	,
	print [_testcmpint_f, _testcmpint_a, "<=", _testcmpint_b ],
	if (_testcmpint_a,<=,_testcmpint_b)
	    statements [
		    procedurecall
			v__writeString
			[ _testcmpint_f, " yes\n" ]
	    ]
	    statements [
		    procedurecall
			v__writeString
			[ _testcmpint_f, " no\n" ]
	    ],
	print [_testcmpint_f, _testcmpint_a, ">", _testcmpint_b ],
	if (_testcmpint_a,>,_testcmpint_b)
	    statements [
		procedurecall
		    v__writeString
		    [ _testcmpint_f, " yes\n" ]
	    ]
	    statements [
		procedurecall
		    v__writeString
		    [ _testcmpint_f, " no\n" ]
	    ]
	,
	print [_testcmpint_f, _testcmpint_a, ">=", _testcmpint_b ],
	if (_testcmpint_a,>=,_testcmpint_b)
	    statements [
		procedurecall
		    v__writeString
		    [ _testcmpint_f, " yes\n" ]
	    ]
	    statements [
		procedurecall
		    v__writeString
		    [ _testcmpint_f, " no\n" ]
	    ]
    ]
]

statements [
    forkall [p:numberOfProcessors] statements [
	barrier,
	if (p,=,0) statements [
	    assign _f functioncall v__open [ "runout", "w"]
	]
	statements [],
	barrier,
	procedurecall v__writeString [ _f, "Hello world\n" ],
	barrier,
	println [ _f, "Hello world" ],
	barrier,
	if( p,=,0 ) statements [
	    assign _false false,
	    assign _true (1, =, 1),
	    assign _b _false,
	    assign _b (not,_b),
	    assign _b (_false,or,_b),
	    assign _b (true,and,_b),
	    assign _i 2,
	    assign _i (50,-,_i),
	    assign _i (5,+,_i),
	    assign _i (5,*,_i),
	    assign _i (_i,/,12),
	    assign _i (~,_i),
	    assign _i (+,_i),
	    assign _i functioncall _inc [ functioncall _inc [ _i ] ],
	    assign _i functioncall _dec [ _i ],
	    assign _n 7,
	    assign _n (+,_n),
	    assign _n (7,+,_n),
	    assign _n (_n,-,5),
	    assign _n (_n,*,12),
	    assign _n (_n,/,13),
	    assign _x 2.1,
	    assign _x (~,_x),
	    assign _x (+,_x),
	    assign _x (0.7,+,_x),
	    assign _x (_x,-,5e2),
	    assign _x (_x,*,0.12),
	    assign _x (_x,/,0.13),
	    assign _s "test string",
	    assign _c complex 2.0d 3.0d,
	    assign _c2 complex 5.0d 12.0d,
	    assign _c (~,_c)
	]
	statements [],
	barrier,
	procedurecall v__writeString [ _f, "c: " ],
	procedurecall v__writeComplex [ _f, _c ],
	procedurecall v__writeString [ _f, "\n" ],
	barrier,
	if (p,=,0) statements [
		assign _c (+,_c)
	    ]
	    statements []
	,
	barrier,
	procedurecall v__writeString [ _f, "c: " ],
	procedurecall v__writeComplex [ _f, _c ],
	procedurecall v__writeString [ _f, "\n" ],
	barrier,
	if (p,=,0) statements [
		assign _c (_c,+,_c2)
	    ]
	    statements []
	,
	barrier,
	procedurecall v__writeString [ _f, "c: " ],
	procedurecall v__writeComplex [ _f, _c ],
	procedurecall v__writeString [ _f, "\n" ],
	barrier,
	if (p,=,0) statements [
		assign _c (_c,*,_c2)
	    ]
	    statements []
	,
	barrier,
	procedurecall v__writeString [ _f, "c: " ],
	procedurecall v__writeComplex [ _f, _c ],
	procedurecall v__writeString [ _f, "\n" ],
	barrier,
	if (p,=,0) statements [
		assign _c (_c,-,_c2)
	    ]
	    statements []
	,
	barrier,
	procedurecall v__writeString [ _f, "c: " ],
	procedurecall v__writeComplex [ _f, _c ],
	procedurecall v__writeString [ _f, "\n" ],
	barrier,
	if (p,=,0) statements [
		assign _c (_c,/,_c2)
	    ]
	    statements []
	,
	barrier,
	procedurecall v__writeString [ _f, "c: " ],
	procedurecall v__writeComplex [ _f, _c ],
	procedurecall v__writeString [ _f, "\n" ],
	procedurecall v__writeString [ _f, "n: " ],
	procedurecall v__writeInt [ _f, _n ],
	procedurecall v__writeString [ _f, "\n" ],
	procedurecall v__writeString [ _f, "i: " ],
	procedurecall v__writeInt [ _f, _i ],
	procedurecall v__writeString [ _f, "\n" ],
	procedurecall v__writeString [ _f, "b: " ],
	procedurecall v__writeBoolean [ _f, _b ],
	procedurecall v__writeString [ _f, "\n" ],
	procedurecall v__writeString [ _f, "x: " ],
	procedurecall v__writeFloat [ _f, _x ],
	procedurecall v__writeString [ _f, "\n" ],
	procedurecall v__writeString [ _f, "s: " ],
	procedurecall v__writeString [ _f, _s ],
	procedurecall v__writeString [ _f, "\n" ],
	procedurecall v__writeString [ _f, "c: " ],
	procedurecall v__writeComplex [ _f, _c ],
	procedurecall v__writeString [ _f, "\n" ],
	procedurecall v__writeString [ _f, "numberOfProcessors: " ],
	procedurecall v__writeInt [ _f, numberOfProcessors ],
	procedurecall v__writeString [ _f, "\n" ],
	procedurecall v__writeString [ _f, "\n" ],
	println [ _f, "n: ", _n ],
	println [ _f, "i: ", _i ],
	println [ _f, "b: ", _b ],
	println [ _f, "x: ", _x ],
	println [ _f, "s: ", _s ],
	barrier,
	procedurecall _Empty [],
	if (p,=,0) statements [
		assign _i 14
	    ]
	    statements []
	,
	barrier,
	while (_i,>,0) statements [
	    procedurecall
		v__writeString
		[ _f, "i = " ] ,
	    procedurecall
		v__writeInt
		[ _f, _i ] ,
	    procedurecall
		v__writeString
		[ _f, "\n" ] ,
	    barrier,
	    if (p,=,0) statements [
		    assign _i (_i,-,1)
		]
		statements []
	    ,
	    barrier
	],
	for [_card_i:3,_card_j:4] statements [
	    procedurecall
		v__writeString
		[ _f, "i = " ] ,
	    procedurecall
		v__writeInt
		[ _f, _card_i ] ,
	    procedurecall
		v__writeString
		[ _f, ", " ] ,
	    procedurecall
		v__writeString
		[ _f, "j = " ] ,
	    procedurecall
		v__writeInt
		[ _f, _card_j ] ,
	    procedurecall
		v__writeString
		[ _f, "\n" ]
	],
	procedurecall _testcmpint [_f, 1, 2, p],
	procedurecall _testcmpint [_f, 1, 1, p],
	procedurecall _testcmpint [_f, 2, 1, p],
	procedurecall _testcmpint [_f, 223, 1, p],
	barrier,
	forall [_card_i:5,_card_j:8] statements [
//	    if isowner ( _shp, [_card_i,_card_j]) p statements [
	    if (((_card_i,/,5),mod,30),=,p) statements [
		assign (_shp,[_card_i,_card_j]) (_card_i,+,_card_j)
	    ]
	    statements []
	],
	barrier,
	for [_card_i:5,_card_j:8] statements [
	    procedurecall v__writeString [ _f, "shp[" ] ,
	    procedurecall v__writeInt [ _f, _card_i ] ,
	    procedurecall v__writeString [ _f, "," ] ,
	    procedurecall v__writeInt [ _f, _card_j ] ,
	    procedurecall v__writeString [ _f, "] = " ] ,
	    procedurecall v__writeInt [ _f, (_shp,[_card_i,_card_j])] ,
	    procedurecall v__writeString [ _f, "\n" ]
	],
	procedurecall v__writeString [ _f, "A := 66;\n"],
	for [_card_i:20] statements [
//	    if isowner (A, [_card_i]) p
	    if (((_card_i,/,5),mod,30),=,p)
	        statements [assign (A, [_card_i]) 66]
		statements []
	],
	barrier,
	procedurecall _Print1dShape [_f, A, p],
	barrier,

	procedurecall _Print1dShape [_f, A, p],
	barrier,
	procedurecall v__writeString [ _f, "A[i] := 0;\n"],
	forall [i:20] statements [
//	    if isowner (A, [i]) p
	    if (((i,/,5),mod,30),=,p)
		statements [ assign (A, [i]) 0]
		statements []
	],
	barrier,

	procedurecall v__writeString [ _f, "A: "],

	procedurecall _Print1dShape [_f, A, p],

	procedurecall v__writeString 
		[ _f, "B[i] := i;\n"],

	barrier,
	forall [i:20] statements [
//	    if isowner (B, [i]) p
	    if ((i,mod,30),=,p)
		statements [assign (B, [i]) i]
		statements []
	    
	],
	barrier,

	procedurecall v__writeString [ _f, "B: "],
	procedurecall _Print1dShape [_f, B, p],

	procedurecall v__writeString 
		[ _f, "A[i] := B[i];\n"],

	forall [i:20] statements [
//	    if isowner (A, [i]) p
	    if (((i,/,5),mod,30),=,p)
		statements [assign (A, [i]) (B, [i]) ]
		statements []
	],
	barrier,

	procedurecall v__writeString [ _f, "A: "],
	procedurecall _Print1dShape [_f, A, p],
	procedurecall v__writeString 
		[ _f, "A[i] := 0;\n"],

	barrier,
	forall [i:20] statements [
//	    if isowner (A, [i]) p
	    if (((i,/,5),mod,30),=,p)
		statements [ assign (A, [i]) 0]
		statements []
	],
	barrier,
	procedurecall _Print1dShape [_f, A, p],
	procedurecall v__writeString 
		[ _f, "A[19-i] := B[i];\n"],

	forall [i:20] statements [
	    assign _desti (19,-,i),
//	    if (sender (B, [i]),=,p)
	    if ((i,mod,30),=,p)
		statements [
//		    send owner (A,[_desti]) (B,[i])
		    send ((_desti,/,5),mod,30) (B,[i])
		]
		statements [],
//	    if isowner (A, [_desti]) p
	    if (((_desti,/,5),mod,30),=,p)
		statements [
//		    receive sender (B,[i]) (A, [_desti])
		    receive (i,mod,30) (A, [_desti])
		]
		statements []
	],
	barrier,
	procedurecall _Print1dShape [_f, A, p],
	barrier,
	procedurecall _testscopes [ _f, 12, p],
	barrier,
	procedurecall v__close [ _f ]

    ]
]
