program
declarations [
    externalvariable vnusout int,
    cardinalityvariable i,
    cardinalityvariable j
]
statements [
    pragma [independent] for [i:4,j:3] statements [
        println [vnusout,expressionpragma [range = (0 4)] i," ",j]
    ]
]
