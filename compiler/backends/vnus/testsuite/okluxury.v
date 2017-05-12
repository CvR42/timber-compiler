program
declarations [
    externalvariable vnusout int,
    cardinalityvariable i
]
statements [
    for [i:5] statements [
        print [vnusout,i,":"]
    ],
    println [vnusout],
    println [vnusout,"done"]
]
