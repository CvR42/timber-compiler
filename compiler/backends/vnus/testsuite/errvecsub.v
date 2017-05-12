// Tries to access a record field by number that is not there.

program

declarations [
    // Standard prelude starts here
    // Standard prelude end here
    procedure vectordemo_main_0 []
    statements main0 [
        println [1, field x_0 0, " ", field x_0 1, " ", field x_0 2]
        ,
        assign field x_0 0 1
        ,
        assign field x_0 1 2
        ,
        assign field x_0 -1 3
        ,
        assign field x_0 3 3
        ,
        println [1, field x_0 0, " ", field x_0 1, " ", field x_0 2]
        ,
        assign x_0 record [1, 2, 3]
        ,
        println [1, field x_0 0, " ", field x_0 1, " ", field x_0 2]
    ]

    ,
    localvariable x_0 main0 record [ field0:int, field1:int, field2:int ] record [0, 0, 0]
]

statements [
    procedurecall vectordemo_main_0 []
]
