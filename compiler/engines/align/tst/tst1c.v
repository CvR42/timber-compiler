// Generated by the Spar front-end

program

pragma [ProcessorType=((gpp "trimedia") (dsp "TI_C60")),Processors=((gpp gpp1) (dsp (at dsp1D 4)) (dsp (at dsp2D 2 3)))]

declarations [
    procedure tst1_main_0 []
    statements scope0 [
        pragma [on=(at dsp2D (cyclic (subtract 7 8 9 (div 10 11 12 13))) (cyclic (mod 3 4 5 i) 17))]
        assign (*a_0, [3, 4]) 7
        ,
        pragma [on=(at dsp2D _all _)]
        assign (*a_0, [5, 6]) 8
        ,
        foreach [i_0=0:10]
            statements pragma [on=(at dsp2D (at map @i_0) _)] [
                assign (*a_0, [i_0, 2]) 0
            ]
        ,
        pragma [on=_all,on=_]
        assign (*a_0, [6, 7]) (*a_0, [8, 9])
    ]
    ,
    localvariable a_0 scope0 pragma [on=(lambda (i j) (at dsp2D (block j) (block i 5)))] pointer shape [dontcare, dontcare] int fillednew pragma [on=(lambda (i j) (at dsp2D (block j) (block i 5)))] shape [50, 50] int 0
    ,
    cardinalityvariable i_0
]

statements [
    procedurecall tst1_main_0 []
]
