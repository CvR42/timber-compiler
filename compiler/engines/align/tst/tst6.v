// Generated by the Spar front-end

program

declarations [
    function tst6_max_0 [a_1, b_0] int
    statements scope2 [
        return if ( a_1, >, b_0 ) a_1 b_0
    ]
    ,
    procedure tst6_main_0 []
    statements scope0 [
        foreach [i_0=0:10]
            statements scope1 [
                assign breakout1_0 functioncall tst6_max_0 [(*a_0, [( i_0, +, 1 )]), (*a_0, [( i_0, +, 2 )])]
                ,
                assign (*a_0, [i_0]) ( breakout0_0, +, breakout1_0 )
            ]
    ]
    ,
    localvariable a_0 scope0 pragma [on=(lambda (i) (at dsp2D i))] pointer shape [dontcare] int
    ,
    cardinalityvariable i_0
    ,
    localvariable breakout0_0 scope1 int (*a_0, [i_0]) const 
    ,
    formalvariable a_1 scope2 int
    ,
    formalvariable b_0 scope2 int
    ,
    localvariable breakout1_0 scope1 int const 
]

statements [
    procedurecall tst6_main_0 []
]
