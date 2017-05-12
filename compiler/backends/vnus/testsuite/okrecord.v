program
declarations [
    externalvariable vnusout int,
    globalvariable a record [a:int, b:int, c:char],
    globalvariable b record [a:int, b:int]
]
statements [
    assign a record [1,2,'x'],
    println [vnusout,"a:",field a a, " ", field a b, " ", field a c],
    assign field b a 42,
    assign field b b -1,
    println [vnusout,"b:",field b a, " ", field b b],
    assign field b 0 (~,field b 0),
    assign field b 1 (~,field b 1),
    println [vnusout,"b:",field b a, " ", field b b],
    assign b cast record [a:int, b:int] a,
    println [vnusout,"b:",field b a, " ", field b b]
]
