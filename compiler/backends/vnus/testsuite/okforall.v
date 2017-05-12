program
declarations [
    externalvariable vnusout int,
    globalvariable a pointer shape [dontcare] int nullednew shape [10] int null,
    cardinalityvariable i,
    cardinalityvariable j,
    cardinalityvariable k
]
statements [
    forall [i:getsize *a 0] statements [
        assign (*a,[i]) i
    ],
    for [k:getsize *a 0] statements [
        print [vnusout,(*a,[k])," "]
    ],
    println [vnusout],
    forall [j:(getsize *a 0,-,1)] statements [
        assign (*a,[j]) (*a,[(j,+,1)])
    ],
    for [k:getsize *a 0] statements [
        print [vnusout,(*a,[k])," "]
    ],
    println [vnusout]
]
