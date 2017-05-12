program
declarations [
    externalvariable vnusout int,
    globalvariable v int,
    function min [mina, minb] int statements minscope [
        if (mina,<,minb) statements [
            return mina
        ]
        statements [
            return minb
        ]
    ],
    formalvariable mina minscope int,
    formalvariable minb minscope int
]
statements [
    println [vnusout,functioncall min [3,4]],
    println [vnusout,functioncall min [3,-4]],
    assign v 0,
    switch (v,+,1) [
        (0, statements [
            println [vnusout,"zero"],
            goto endsw
        ]),
        (1, statements [
            println [vnusout,"one"],
            goto endsw
        ]),
        (2, statements [
            println [vnusout,"two"],
            goto endsw
        ]),
	( default, statements [
	    println [vnusout,"default"]
	])
    ],
endsw:
    println [vnusout,"done"]
]
