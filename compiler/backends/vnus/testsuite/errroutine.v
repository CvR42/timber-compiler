program
declarations [
    externalvariable vnusout int,
    procedure min4 [a, b, c, d] statements min4scope [
        assign mn functioncall min [
            functioncall min [a, b, c],
            functioncall min [c]
        ],
	assign mn functioncall min [ a, false ],
        println [vnusout,"Minimum: ",mn]
    ],
    localvariable mn min4scope int,
    formalvariable a min4scope int,
    formalvariable b min4scope int,
    formalvariable c min4scope int,
    formalvariable d min4scope int,
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
    procedurecall min4 [1, 2, 3, 4, 5],
    procedurecall min4 [1, 44, -3]
]
