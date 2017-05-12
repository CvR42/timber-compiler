program
declarations [
    externalvariable vnusout int,
    globalvariable a pointer shape [dontcare] int nullednew shape [100] int null,
    globalvariable sum int 0,
    cardinalityvariable i,
    cardinalityvariable j
]
statements [
    foreach [i:getsize *a 0] statements [
        assign (*a,[i]) i
    ],
    foreach [j:getsize *a 0] statements [
        assign sum (sum,+,(*a,[j]))
    ],
    println [vnusout,sum]
]
