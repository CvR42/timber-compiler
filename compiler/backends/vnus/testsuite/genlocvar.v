program
declarations [
    externalvariable vnusout int,
    localvariable v outerscope int,
    localvariable w innerscope int,
    localvariable x outerscope shape [3] int shape [3] int [ 1,2,3 ]
]
statements [
    statements outerscope [
        statements innerscope [
            assign w 7,
            assign v (w,+,1),
            println [vnusout,"v: ",v," w: ",w]
        ],
        assign v (v,+,1),
        // `w' is not visible any more
        println [vnusout,"v: ",v]
    ]
]
