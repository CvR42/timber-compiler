// This is a test program for the
// V-nus compiler.

/* */

/* This is C-style comment. */
/* This is C-style comment.
 * On multiple lines.
 * In fact,
 * quite
 * a
 * lot
 * of
 * lines,
 * to check whether line numbering after it
 * is still correct.
 */

program

pragma [
//value boundscheck false,
//flag floatshape
    declarations=true,	// This should not lead to a syntax error
    globalvariable	// ditto
]

declarations [
//    globalvariable rec0 record [],
    globalvariable rec0 record [] record [],
    globalvariable rec record [a:int, b:float, c:char] record [1,1.0,'z'],
    globalvariable rec2 record [a:int, b:float] record [1,1.0],
    globalvariable prec pointer record [a:int, b:float, c:char],
    globalvariable prec2 pointer record [a:int, b:float],
    globalvariable _x float,
    globalvariable _i int 0,
    globalvariable _bt byte 2b,
    globalvariable _shrt short,
    globalvariable _l long,
    globalvariable _chr char 'X',
    globalvariable _d double,
    globalvariable _inf double,
    globalvariable _nan double,
    globalvariable _neginf double,
    globalvariable _finf float,
    globalvariable _fnan float,
    globalvariable _fneginf float,
    localvariable ok okblock boolean,
    cardinalityvariable i,
    cardinalityvariable _card_i,
    cardinalityvariable _card_j,
    cardinalityvariable _Print1dShape_i,
    globalvariable _pp pointer procedure [],
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
    globalvariable _shp pointer shape [dontcare,dontcare] int nullednew shape [5,8] int null,
    globalvariable A pointer shape [dontcare] int nullednew shape [20] int null,
    globalvariable B pointer shape [dontcare] int nullednew shape [20] int null,
    formalvariable __l_test_dynamic_f test_dynamic int,
    formalvariable __l_test_dynamic_aout test_dynamic pointer shape [10] int pragma [floatshape],
    formalvariable __l_test_dynamic_bout test_dynamic pointer shape [dontcare] int,
    formalvariable __l_test_dynamic_ain test_dynamic pointer shape [10] int,
    formalvariable __l_test_dynamic_bin test_dynamic pointer shape [dontcare] int,
    localvariable __l_test_dynamic_tmp test_dynamic pointer shape [dontcare] int nullednew shape [getsize *__l_test_dynamic_ain 0] int null,
    localvariable __l_test_dynamic_lim test_dynamic int,
    cardinalityvariable __c_i_2,
    cardinalityvariable __c_i_3,
    formalvariable __l_run_test_dynamic_f run_test_dynamic int,
    formalvariable __l_run_test_dynamic_n run_test_dynamic int,
    localvariable __l_run_test_dynamic_ain run_test_dynamic pointer shape [dontcare] int nullednew shape [10] int null,
    localvariable __l_run_test_dynamic_bin run_test_dynamic pointer shape [dontcare] int nullednew shape [__l_run_test_dynamic_n] int null,
    localvariable __l_run_test_dynamic_cin run_test_dynamic pointer shape [dontcare] int nullednew shape [10] int null,
    localvariable __l_run_test_dynamic_din run_test_dynamic pointer shape [dontcare] int nullednew shape [( __l_run_test_dynamic_n, +, 2 )] int null,
    localvariable __l_run_test_dynamic_aout run_test_dynamic pointer shape [dontcare] int nullednew shape [10] int null,
    localvariable __l_run_test_dynamic_bout run_test_dynamic pointer shape [dontcare] int nullednew shape [__l_run_test_dynamic_n] int null,
    localvariable __l_run_test_dynamic_cout run_test_dynamic pointer shape [dontcare] int nullednew shape [10] int null,
    localvariable __l_run_test_dynamic_dout run_test_dynamic pointer shape [dontcare] int nullednew shape [( __l_run_test_dynamic_n, +, 2 )] int null,
    localvariable __l_run_test_dynamic_bad run_test_dynamic boolean,
    localvariable __l_run_test_dynamic_allok run_test_dynamic boolean,
    localvariable wherey wherexpr int 3,
    cardinalityvariable __c_i_4,
    cardinalityvariable __c_i_5,
    cardinalityvariable __c_i_6,
    cardinalityvariable __c_i_7,
    cardinalityvariable __c_i_8,
    cardinalityvariable __c_i_9,
    cardinalityvariable __c_i_a,
    cardinalityvariable __c_i_b,
    cardinalityvariable __c_i_c,
    cardinalityvariable __c_i_d,
    cardinalityvariable __c_i_e,
    cardinalityvariable __c_i_f,
    formalvariable _Print1dShape_f _Print1dShape int,
    formalvariable _Print1dShape_A _Print1dShape pointer shape [dontcare] int final pragma [location],
    formalvariable _dec_i _dec int,
    formalvariable _inc_i _inc int,
    formalvariable _testcmpint_f _testcmpint int,
    formalvariable _testcmpint_a _testcmpint int,
    formalvariable _testcmpint_b _testcmpint int,
    formalvariable _testscopes_f renamed_testscopes int,
    formalvariable _testscopes_n renamed_testscopes int,
    localvariable _testscopes_n_sub subscope int,
    localvariable ti testcasts int,
    externalfunction v__open [v__open_fnm,v__open_mode] int,
    formalvariable v__open_fnm v__open string,
    formalvariable v__open_mode v__open string,
    externalprocedure v__close [v__close_hnd],
    formalvariable v__close_hnd v__close int,
    externalprocedure v__writeFloat [v__writef_hnd,v__writef_r],
    formalvariable v__writef_hnd v__writeFloat int,
    formalvariable v__writef_r v__writeFloat float,
    externalprocedure v__writeComplex [v__writec_hnd,v__writec_c],
    formalvariable v__writec_hnd v__writeComplex int,
    formalvariable v__writec_c v__writeComplex complex,
    externalprocedure v__writeBoolean [v__writeb_hnd,v__writeb_b],
    formalvariable v__writeb_hnd v__writeBoolean int,
    formalvariable v__writeb_b v__writeBoolean boolean,
    externalprocedure v__writeInt [v__writei_hnd,v__writei_n],
    formalvariable v__writei_hnd v__writeInt int,
    formalvariable v__writei_n v__writeInt int,
    externalprocedure v__writeString [v__writes_hnd,v__writes_s],
    formalvariable v__writes_hnd v__writeString int,
    formalvariable v__writes_s v__writeString string,
    formalvariable __l_maxloc_x maxloc shape [dontcare] float,
    cardinalityvariable __maxloc_i,
    cardinalityvariable _testsec_i,
    cardinalityvariable _testsec_j,
    cardinalityvariable _testsec_k,
    procedure testcasts [] statements testcasts [
	assign ti cast int 22.3
    ],
    procedure /* intern comment test */ _testscopes [_testscopes_f, _testscopes_n]
    statements renamed_testscopes [
	pragma [
	    bla,
	    testboolean = true,
	    testin = 42,
	    teststring = "bla"
	] procedurecall
	    v__writeInt [_testscopes_f, _testscopes_n],
	procedurecall
	    v__writeString [_testscopes_f, expressionpragma [constant] " "],
	statements subscope [
	    assign _testscopes_n_sub (42,+,1),
	    procedurecall v__writeInt [_testscopes_f, _testscopes_n],
	    procedurecall v__writeString [_testscopes_f, " "],
	    procedurecall v__writeInt [_testscopes_f, _testscopes_n_sub],
	    procedurecall v__writeString [_testscopes_f, "\n"]
	]
    ],
    localvariable __l_maxloc_result maxloc int,
    function __g_maxloc [ __l_maxloc_x ] int
    statements maxloc [
        assign __l_maxloc_result 0,
        for [ __maxloc_i: ( getsize __l_maxloc_x 0, -, 1 )]
            statements pragma [independent] [
                if ( ( __l_maxloc_x, [( __maxloc_i, +, 1 )] ), >, ( __l_maxloc_x, [__maxloc_i] ) )
                    statements [
                        assign __l_maxloc_result ( __maxloc_i, +, 1 )
                    ]
                    statements [
                    ]
            ],
	return __l_maxloc_result
    ],
    procedure __g_test_dynamic [ __l_test_dynamic_f ,  __l_test_dynamic_aout ,  __l_test_dynamic_bout , __l_test_dynamic_ain ,  __l_test_dynamic_bin ]
    statements test_dynamic [
        pragma [bigdeal] assign __l_test_dynamic_lim 10,
        if ( getsize *__l_test_dynamic_ain 0, <, __l_test_dynamic_lim )
            statements [
                assign __l_test_dynamic_lim getsize *__l_test_dynamic_ain 0
            ]
            statements [],
        if ( getsize *__l_test_dynamic_ain 0, <, __l_test_dynamic_lim )
            statements [
                assign __l_test_dynamic_lim getsize *__l_test_dynamic_ain 0
            ]
            statements [],
        procedurecall v__writeString [__l_test_dynamic_f, "lim = "],
        procedurecall v__writeInt [__l_test_dynamic_f, __l_test_dynamic_lim],
        procedurecall v__writeString [__l_test_dynamic_f, "\n"],
        forall [ __c_i_2: __l_test_dynamic_lim]
            statements [
                assign
		    ( *__l_test_dynamic_tmp, [__c_i_2] )
		    (
			( *__l_test_dynamic_ain, [__c_i_2] ),
			+,
			( *__l_test_dynamic_bin, [__c_i_2] )
		    )
            ],
        forall [ __c_i_3:__l_test_dynamic_lim]
            statements [
                assign ( *__l_test_dynamic_aout, [__c_i_3] ) ( *__l_test_dynamic_tmp, [__c_i_3] ),
                assign ( *__l_test_dynamic_bout, [__c_i_3] ) ( *__l_test_dynamic_tmp, [__c_i_3] )
            ]
    ],
    procedure __g_run_test_dynamic [ __l_run_test_dynamic_f ,  __l_run_test_dynamic_n ]
    statements run_test_dynamic [
        forall [__c_i_8: 10]
            statements [
                assign ( *__l_run_test_dynamic_ain, [__c_i_8] ) 1
            ],
        forall [__c_i_9:__l_run_test_dynamic_n]
            statements [
                assign ( *__l_run_test_dynamic_bin, [__c_i_9] ) 1
            ],
        forall [__c_i_a: 10]
            statements [
                assign ( *__l_run_test_dynamic_cin, [__c_i_a] ) 1
            ],
        forall [ __c_i_b: ( __l_run_test_dynamic_n, +, 2 )]
            statements [
                assign ( *__l_run_test_dynamic_din, [__c_i_b] ) 1
            ],
        procedurecall __g_test_dynamic [__l_run_test_dynamic_f, __l_run_test_dynamic_aout, __l_run_test_dynamic_bout, __l_run_test_dynamic_ain, __l_run_test_dynamic_bin ],
        assign __l_run_test_dynamic_allok (0,=,0),
        for [__c_i_c: 10]
            statements [
                if ( ( *__l_run_test_dynamic_aout, [__c_i_c] ), <>, 4 )
                    statements [
                        procedurecall v__writeString [__l_run_test_dynamic_f, "test_dynamic: bad value for aout\n"],
                        procedurecall v__writeString [__l_run_test_dynamic_f, "aout["],
                        procedurecall v__writeInt [__l_run_test_dynamic_f, __c_i_c],
                        procedurecall v__writeString [__l_run_test_dynamic_f, "] = "],
                        procedurecall v__writeInt [__l_run_test_dynamic_f, ( *__l_run_test_dynamic_aout, [__c_i_c] )],
                        procedurecall v__writeString [__l_run_test_dynamic_f, "\n"],
                        assign __l_run_test_dynamic_allok false
                    ]
                    statements []
            ],
        assign __l_run_test_dynamic_bad (0,=,1),
        for [__c_i_d: 10]
            statements [
                assign __l_run_test_dynamic_bad ( __l_run_test_dynamic_bad,  shortor , ( ( *__l_run_test_dynamic_bout, [__c_i_d] ), <>, ( *__l_run_test_dynamic_aout, [__c_i_d] ) ) )
            ],
        if __l_run_test_dynamic_bad
            statements [
                procedurecall v__writeString [__l_run_test_dynamic_f, "test_dynamic: bad value for bout\n"],
                assign __l_run_test_dynamic_allok (0,=,1)
            ]
            statements [],
        assign __l_run_test_dynamic_bad (0,=,1),
        for [__c_i_e: 10]
            statements [
                assign __l_run_test_dynamic_bad ( __l_run_test_dynamic_bad,  shortor , ( ( *__l_run_test_dynamic_cout, [__c_i_e] ), <>, ( *__l_run_test_dynamic_bout, [__c_i_e] ) ) )
            ],
        if __l_run_test_dynamic_bad
            statements [
                procedurecall v__writeString [__l_run_test_dynamic_f, "test_dynamic: bad value for cout\n"],
                assign __l_run_test_dynamic_allok (0,=,1)
            ]
            statements [],
        assign __l_run_test_dynamic_bad (0,=,1),
        for [__c_i_f: 10]
            statements [
                assignop __l_run_test_dynamic_bad shortor ( ( *__l_run_test_dynamic_dout, [__c_i_f] ), <>, ( *__l_run_test_dynamic_cout, [__c_i_f] ) )
            ],
        if __l_run_test_dynamic_bad
            statements [
                procedurecall v__writeString [__l_run_test_dynamic_f, "test_dynamic: bad value for dout\n"],
                assign __l_run_test_dynamic_allok (0,=,1)
            ]
            statements [],
        if __l_run_test_dynamic_allok
            statements [
                procedurecall v__writeString [__l_run_test_dynamic_f, "test_dynamic: everything ok\n"]
            ]
            statements []
    ],
    procedure _Empty [] pragma [inline] statements _Empty [],
    procedure _Print1dShape [_Print1dShape_f, _Print1dShape_A]
    statements _Print1dShape [
        for [_Print1dShape_i: getsize *_Print1dShape_A 0]
	    statements [ 
		procedurecall
		    v__writeInt
		    [
		        _Print1dShape_f,
		        expressionpragma [boundscheck = true] (*_Print1dShape_A,[_Print1dShape_i])
		    ],
		procedurecall
		    v__writeString
		    [ _Print1dShape_f, " " ]
	    ],
	procedurecall v__writeString [ _Print1dShape_f, "\n" ]
    ],
    function _inc [_inc_i] int pragma [inline]
    statements _inc [
	return (_inc_i,+,1)
    ],
    function _dec [_dec_i] int
    statements _dec [
	return (_dec_i,-,1)
    ],
    procedure _testcmpint [_testcmpint_f, _testcmpint_a, _testcmpint_b]
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
	procedurecall v__writeInt [ _testcmpint_f, _testcmpint_a ],
	procedurecall v__writeString [ _testcmpint_f, "<" ],
	procedurecall
	    v__writeInt
	    [ _testcmpint_f, _testcmpint_b ],
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
	procedurecall
	    v__writeInt
	    [ _testcmpint_f, _testcmpint_a ]
	,
	procedurecall
	    v__writeString
	    [ _testcmpint_f, "<=" ]
	,
	procedurecall
	    v__writeInt
	    [ _testcmpint_f, _testcmpint_b ]
	,
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
	    ]
	,
	procedurecall
	    v__writeInt
	    [ _testcmpint_f, _testcmpint_a ]
	,
	procedurecall
	    v__writeString
	    [ _testcmpint_f, ">" ]
	,
	procedurecall
	    v__writeInt
	    [ _testcmpint_f, _testcmpint_b ]
	,
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
	procedurecall
	    v__writeInt
	    [ _testcmpint_f, _testcmpint_a ]
	,
	procedurecall
	    v__writeString
	    [ _testcmpint_f, ">=" ]
	,
	procedurecall
	    v__writeInt
	    [ _testcmpint_f, _testcmpint_b ]
	,
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
    assign _f functioncall v__open [ "runout", "w"],
    procedurecall v__writeString [ _f, "Hello world\n" ],
    println [ _f, "Hello world" ],
    println [ _f ],
    assign _bt 12b,
    assign _shrt 23s,
    assign _l -12432342l,
    assign _d -12.34e-123d,
    assign _inf 9218868437227405312u,
    assign _neginf -4503599627370496u,
    assign _nan 9221120237041090560u,
    assign _finf 2139095040t,
    assign _fneginf -8388608t,
    assign _fnan 2143289344t,
    assign _chr '?',
    assign _false false,
    assign _true (1, =, 1),
    assign _b _false,
    assign _b (not,_b),
    assign _b (_false,or,_b),
    assign _b (true,shortand,_b),
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
    assign _x 0t,
    assign _x 2.1,
    println [ _f, "initial x: ", _x ],
    assign _x (~,_x),
    println [ _f, "x after ~: ", _x ],
    assign _x (+,_x),
    println [ _f, "x after unary +: ", _x ],
    assign _x (0.7,+,_x),
    println [ _f, "after (0.7+x): ", _x ],
    assign _x (_x,-,5e2),
    println [ _f, "after (x-5e2): ", _x ],
    assign _x (_x,*,0.12),
    println [ _f, "after (x*0.12): ", _x ],
    assign _x (_x,/,0.13),
    println [ _f, "after (x/0.13): ", _x ],
    assign _x (_x,mod,0.013),
    println [ _f, "after (x mod 0.013): ", _x ],
    assign _s "test string",
    assign _c complex 2.0d 3.0d,
    assign _c2 complex 5.0d 12.0d,
    assign _c (~,_c),
    expression functioncall _inc [ functioncall _inc [ _i ] ],
    procedurecall v__writeString [ _f, "c: " ],
    procedurecall v__writeComplex [ _f, _c ],
    procedurecall v__writeString [ _f, "\n" ],
    assign _c (+,_c),
    procedurecall v__writeString [ _f, "c: " ],
    procedurecall v__writeComplex [ _f, _c ],
    procedurecall v__writeString [ _f, "\n" ],
    assign _c (_c,+,_c2),
    procedurecall v__writeString [ _f, "c: " ],
    procedurecall v__writeComplex [ _f, _c ],
    procedurecall v__writeString [ _f, "\n" ],
    assign _c (_c,*,_c2),
    procedurecall v__writeString [ _f, "c: " ],
    procedurecall v__writeComplex [ _f, _c ],
    procedurecall v__writeString [ _f, "\n" ],
    assign _c (_c,-,_c2),
    procedurecall v__writeString [ _f, "c: " ],
    procedurecall v__writeComplex [ _f, _c ],
    procedurecall v__writeString [ _f, "\n" ],
    assign _c (_c,/,_c2),
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
    procedurecall v__writeString [ _f, "\n" ],
    println [ _f, "n: ", _n ],
    println [ _f, "i: ", _i ],
    println [ _f, "b: ", _b ],
    println [ _f, "x: ", _x ],
    println [ _f, "s: ", _s ],
    assign _i 14,
    while (_i,>,0) statements [
	procedurecall v__writeString [ _f, "i = " ],
	procedurecall v__writeInt [ _f, _i ],
	procedurecall v__writeString [ _f, "\n" ],
	assign _i (_i,-,1)
    ],
    dowhile (_i,>,0) statements [
	procedurecall v__writeString [ _f, "i = " ],
	procedurecall v__writeInt [ _f, _i ],
	procedurecall v__writeString [ _f, "\n" ],
	assignop _i - 1
    ],
    for [_card_i:3,_card_j:4] statements [
	procedurecall v__writeString [ _f, "i = " ],
	procedurecall v__writeInt [ _f, _card_i ],
	procedurecall v__writeString [ _f, ", " ],
	procedurecall v__writeString [ _f, "j = " ],
	procedurecall v__writeInt [ _f, _card_j ],
	procedurecall v__writeString [ _f, "\n" ]
    ],
    procedurecall _testcmpint [_f, 1, 2],
    procedurecall _testcmpint [_f, 1, 1],
    procedurecall _testcmpint [_f, 2, 1],
    procedurecall _testcmpint [_f, 223, 1],
    forall [_card_i:5,_card_j:8] statements [
	assign (*_shp,[_card_i,_card_j]) (_card_i,+,_card_j)
    ],
    for [_card_i:5,_card_j:8] statements [
	procedurecall v__writeString [ _f, "shp[" ],
	procedurecall v__writeInt [ _f, _card_i ],
	procedurecall v__writeString [ _f, "," ],
	procedurecall v__writeInt [ _f, _card_j ],
	procedurecall v__writeString [ _f, "] = " ],
	procedurecall v__writeInt [ _f, (*_shp,[_card_i,_card_j])],
	procedurecall v__writeString [ _f, "\n" ]
    ],
    assign _n 0,
    procedurecall v__writeString [ _f, "A := 66;\n"],
    for [_card_i:getsize *A 0] statements [
	assign (*A, [_card_i]) 66
    ],
    procedurecall _Print1dShape [_f, A],

    procedurecall _Print1dShape [_f, A],

    procedurecall _Print1dShape [_f, A],
    procedurecall v__writeString 
	    [ _f, "A[i] := 0;\n"],

    forall [i: getsize *A 0] statements [
	assign (*A, [i]) 0
    ],

    procedurecall v__writeString [ _f, "A: "],

    procedurecall _Print1dShape [_f, A],

    procedurecall v__writeString 
	    [ _f, "B[i] := i;\n"],

    forall [i: getsize *B 0] statements [
	assign (*B, [i]) i
    ],

    procedurecall v__writeString [ _f, "B: "],
    procedurecall _Print1dShape [_f, B],

    procedurecall v__writeString 
	    [ _f, "A[i] := B[i];\n"],

    forall [i: getsize *A 0] statements [
	assign (*A, [i]) (*B, [i])
    ],

    procedurecall v__writeString [ _f, "A: "],
    procedurecall _Print1dShape [_f, A],
    procedurecall _testscopes [ _f, 12 ],
    procedurecall testcasts[],
    assign field rec a 12,
    assign field rec b cast float field rec a,
    assign field rec c 'x',
//    assign rec0 record [],
    assign rec record [1,1.0,'z'],
    assign prec &rec,
    assign prec2 cast pointer record [a:int, b:float] prec,
    assign rec2 cast record [a:int, b:float] rec,
    assign field *prec a field *prec2 a,
    statements okblock [
	assign ok (prec,=,cast pointer record [a:int, b:float, c:char] null),
	if ok statements [
	    println[_f,"Strange, pointer is null"]
	]
	statements [
	    println[_f,"Good, pointer is not null"]
	]
    ],
    assign prec2 new record [a:int, b:float],
    if (prec2,=,cast pointer record [a:int, b:float] null) statements [
	println[_f,"Strange, pointer is null"]
    ]
    statements [],
    delete prec2,
    garbagecollect,
    switch 3 [
	    (4, statements [
		println [ _f, "four" ]
	    ]),
	    (3, statements [
		println [ _f, "three" ]
	    ]),
	    (2, statements [
	    ]),
	    (1, statements [
		println [ _f, "one" ],
		goto next
	    ]),
	    ( default, statements [] ),
	    (0, statements [
		println [ _f, "zero" ]
	    ])
	],
next:
    procedurecall v__writeString [ _f, if (3,<,4) "yes\n" "no\n" ],
    if ("abc",<,"abcd") statements [
	println [_f,"Yes"]
    ]
    statements [
	println [_f,"No"]
    ],
    println [_f,"left shifts: int:",(1,<<,2)," byte:",(1b,<<,2)," short:",(1s,<<,2)," long:",(1l,<<,2)],
    println [_f,"right shifts: int:",(55,>>,2)," byte:",(55b,>>,2)," short:",(55s,>>,2)," long:",(55l,>>,2)],
    println [_f,"unsigned right shifts: int:",(55,>>>,2)," byte:",(55b,>>>,2)," short:",(55s,>>>,2)," long:",(55l,>>>,2)],
    println [_f,"shifts of negative numbers: left:",(-11,<<,2)," right:",(-11,>>,2)," unsigned right:",(-11,>>>,2)],
    println [_f,"bitwise operations: and:", (17,and,18), " or:",(17,or,18)," xor:",(17,xor,18)],
    println [_f,"boolean operations: and:", (true,and,false), " or:",(true,or,false)," xor:",(true,xor,false)],
    println [_f,"shortcircuit boolean operations: and:", (true,shortand,false), " or:",(true,shortor,false)],
    println[ _f,"bitwise not:", (not,12)," ",(not,-144)],
    println[ _f,"boolean not:", (not,false)," ",(not,true)],

    // Test the where expression
    println[ _f,"wherey^2:", where wherexpr (wherey,*,wherey)],

    // Test the address-of operator
    assign _pp &_Empty,

    // Optimizer fodder
    empty,

    // Test the notnullassert expression
    assign _pp notnullassert &_Empty,

    // Test the where location expression
    assign where whereloc _pp &_Empty,
    procedurecall *_pp [],

    // Test secondaries
    for [ _testsec_i=0:12:1 (_testsec_j=12:-1,_testsec_k=_i:2)] statements [
	println[ _f, "i=", _testsec_i, " j=", _testsec_j, " k=", _testsec_k ]
    ],
    // Test reduction expressions
    println[ _f, reduction + [1] ],
    println[ _f, reduction + [1,2,3,4,5,6,7,8] ],
    println[ _f, reduction - [1,2,3,4,5,6,7,8] ],
    procedurecall v__close [ _f ],
    assign _f expressionpragma [test = false] expressionpragma [pointless] 0
]
