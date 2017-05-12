program
declarations [
    externalvariable vnusout int,
    cardinalityvariable i,
    cardinalityvariable j
]
statements [
    for [i:4,j:(i,+,1)] statements [
        print [vnusout, i,",",j," "]
    ],
    println [vnusout]
]
