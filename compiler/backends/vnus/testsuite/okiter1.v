program
declarations [
    externalvariable vnusout int,
    cardinalityvariable i,
    cardinalityvariable j
]
statements [
    for [i:4] statements [
        for [j:3] statements [
            print [vnusout,i,",",j," "]
        ]
    ],
    println [vnusout]
]
