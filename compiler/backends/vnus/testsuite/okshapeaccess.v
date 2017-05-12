program

declarations [
    globalvariable _a0 pointer shape [] int,
    globalvariable _a1 pointer shape [dontcare] char,
    globalvariable _a2 pointer shape [dontcare,dontcare] boolean,
    globalvariable _a3 pointer shape [dontcare,dontcare,dontcare] double,
    globalvariable _a6 pointer shape [dontcare,dontcare,dontcare,dontcare,dontcare,dontcare] int,
]

statements [
    assign _a0 fillednew shape [] int 0 null,
    assign _a1 fillednew shape [10] char '?' null,
    assign _a2 fillednew shape [10,10] boolean true null,
    assign _a3 fillednew shape [10,10,10] double 2.0 null,
    assign _a6 fillednew shape [2,2,2,2,2,2] int 42 null,

    println [ 1, "a0[]:", (*_a0,[])],
    assign (*_a0,[]) 1,
    println [ 1, "a0[]:", (*_a0,[])],

    println [ 1, "a1[1]:", (*_a1,[1])],
    assign (*_a1,[1]) 'z',
    println [ 1, "a1[1]:", (*_a1,[1])],

    println [ 1, "a2[1,2]:", (*_a2,[1,2])],
    assign (*_a2,[1,2]) false,
    println [ 1, "a2[1,2]:", (*_a2,[1,2])],

    println [ 1, "a3[1,2,3]:", (*_a3,[1,2,3])],
    assign (*_a3,[1,2,3]) 21.3d,
    println [ 1, "a3[1,2,3]:", (*_a3,[1,2,3])],

    println [ 1, "a6[1,1,1,1,1,1]:", (*_a6,[1,1,1,1,1,1])],
    assign (*_a6,[1,1,1,1,1,1]) -2,
    println [ 1, "a6[1,1,1,1,1,1]:", (*_a6,[1,1,1,1,1,1])]
]
