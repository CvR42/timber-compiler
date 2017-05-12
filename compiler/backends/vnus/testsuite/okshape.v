program
declarations [
    externalvariable vnusout int,
    globalvariable a pointer shape [dontcare,dontcare] int nullednew shape [3,4] int null,
    cardinalityvariable i,
    cardinalityvariable j,
    cardinalityvariable k,
    cardinalityvariable l,
    globalvariable sum int 0
]
statements [
    println [vnusout,"length of a:",getlength *a],
    for [i:3,j:4] statements [
        assign (*a,[i,j]) (i,+,j)
    ],
    for [k:4,l:3] statements [
        assign sum (sum,+,(*a,[l,k]))
    ],
    println [vnusout,"sum=",sum]
]
