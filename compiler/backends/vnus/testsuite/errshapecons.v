program
declarations [
    externalvariable vnusout int,
    globalvariable a shape [3,3] int,
    cardinalityvariable i,
    cardinalityvariable j,
    cardinalityvariable k,
    cardinalityvariable l,
    globalvariable sum int 0
]
statements [
    assign a shape [3,3] int [1,2,3,4,5,6,7,8,9],
    for [k:4,l:3] statements [
        assign sum (sum,+,(a,[l,k]))
    ],
    println [vnusout,sum]
]
