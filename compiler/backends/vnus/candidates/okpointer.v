program
declarations [
    externalvariable vnusout int,
    globalvariable a int,
    globalvariable p pointer int
]
statements [
    assign a 0,
    assign p &a,
    assign *p 32,
    println [vnusout,a]
]
