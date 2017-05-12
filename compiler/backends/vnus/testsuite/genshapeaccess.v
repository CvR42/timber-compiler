program

declarations [
    globalvariable _a0 pointer shape [] int,
    globalvariable _a1 pointer shape [dontcare] char,
    globalvariable _a2 pointer shape [dontcare,dontcare] boolean,
    globalvariable _a3 pointer shape [dontcare,dontcare,dontcare] double,
    globalvariable _a6 pointer shape [dontcare,dontcare,dontcare,dontcare,dontcare,dontcare] int,
    cardinalityvariable _i,
    formalvariable a f_scope pointer shape [dontcare] char,
    procedure proc [a] statements f_scope [
	for [ _i : 12 ] statements [
	    assign (*notnullassert a,[_i]) 'a',
	    assign (*notnullassert a,[_i]) 'b'
	]
    ]
]

statements [
    assign _a0 fillednew shape [] int 0 null,
    assign _a1 fillednew shape [10] char '?' null,
    assign _a2 nullednew shape [10,10] boolean null,
    assign _a3 fillednew shape [10,10,10] double 0.0 null,
    assign _a6 nullednew shape [2,2,2,2,2,2] int null,

    println [ 1, "a0[]:", (*notnullassert _a0,[])],
    assign (*notnullassert _a0,[]) 1,
    println [ 1, "a0[]:", (*notnullassert _a0,[])],

    println [ 1, "a1[1]:", (*notnullassert _a1,[1])],
    assign (*notnullassert _a1,[1]) 'z',
    println [ 1, "a1[1]:", (*notnullassert _a1,[1])],

    println [ 1, "a2[1,2]:", (*notnullassert _a2,[1,2])],
    assign (*notnullassert _a2,[1,2]) false,
    println [ 1, "a2[1,2]:", (*notnullassert _a2,[1,2])],

    println [ 1, "a3[1,2,3]:", (*notnullassert _a3,[1,2,3])],
    assign (*notnullassert _a3,[1,2,3]) 21.3d,
    println [ 1, "a3[1,2,3]:", (*notnullassert _a3,[1,2,3])],

    println [ 1, "a6[1,1,1,1,1,1]:", (*notnullassert _a6,[1,1,1,1,1,1])],
    assign (*notnullassert _a6,[1,1,1,1,1,1]) -2,
    println [ 1, "a6[1,1,1,1,1,1]:", (*notnullassert _a6,[1,1,1,1,1,1])]
]
