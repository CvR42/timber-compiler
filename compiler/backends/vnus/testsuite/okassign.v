program

declarations [
    externalvariable vnusout int,
    globalvariable v int
]
statements [
    assign v 7,
    assign v (v,+,2),
    println [vnusout,(v,*,v)]
]
